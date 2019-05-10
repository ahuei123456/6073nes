#include "apu.hpp"

APU::APU(std::shared_ptr<Mem> memory) {
	this->memory = memory;
}


uint8_t APU::length_lookup(uint8_t index) {
      if (index % 2 == 1) {
	  uint8_t lin_length_index = (0x1F - index) / 2;
	  return lin_length_table[lin_length_index]; 
      }

      else if (index <= 0xE) {
	  uint8_t length_10_index = (0xE - index) / 2;
	  return length_10_table[length_10_index];
      }

      else {
	  //We know the length must be from the base length 12 table
	  uint8_t length_12_index = (0x1E - index) / 2;
	  return length_12_table[length_12_index];
      }

	
}

uint16_t APU::noise_period_lookup(uint8_t index) {
	return noise_period_table[index];
}

uint16_t APU::dmc_period_lookup(uint8_t index) {
	return dmc_period_table[index];
}

void APU::status_reg_changes() {
//Handles actions that occur after the status register is written to.
	uint8_t d_bit = (status_reg >> 4) % 2;
	uint8_t n_bit = (status_reg >> 3) % 2;
	uint8_t t_bit = (status_reg >> 2) % 2;
	uint8_t p2_bit = (status_reg >> 1) % 2;
	uint8_t p1_bit = (status_reg) % 2;


	if (d_bit == 0) {
		dmc_length_counter = 0;
		dmc_bytes_remaining = 0;
        }	

	else {
		if (dmc_bytes_remaining == 0) {
			dmc_restart = true;		
		}	
	}

	if (n_bit == 0) {
		noise_length_counter = 0;
		noise_silence = true;
        }

	if (t_bit == 0) {
		triangle_length_counter = 0;
		triangle_silence = true;
        }

	if (p2_bit == 0) {
		pulse_length_counters[1] = 0;
		pulse_silence[1] = 0;
	}		

	if (p1_bit == 0) {
		pulse_length_counters[0] = 0;
		pulse_silence[0] = 0;
	}
}

void APU::frame_counter_changes() {
//Handles actions that occur after frame counter is written to.
	uint8_t mode = (frame_counter >> 7) % 2;
	uint8_t interrupt_inh = (frame_counter >> 6) % 2;
	if (interrupt_inh == 1) {
		frame_interrupt_flag = false;
	}

	frame_divider = 2;
	if (mode == 1) {
		//This clocks all units the frame counter controls.
		frame_clock();
		frame_divider = 1;
	}
	
	else {
		frame_divider = 1;
	}
}

void APU::reg_write(uint64_t index, uint8_t value) {
       if (IS_PULSE_REG(index)) {
	    uint8_t wave_num = (index - 0x4000) / 4;
	    pulse_regs[wave_num][index % 4] = value;
	    if (index % 4 == 3) {
		  uint8_t new_length_index = (pulse_regs[wave_num][3] >> 3) % 32;
		  pulse_length_counters[wave_num] = length_lookup(new_length_index);
	    	  duty_counters[wave_num] = 0; 
	    }
       }

       else if (IS_TRIANGLE_REG(index)) {
	    triangle_regs[index % 4] = value;

	    if (index % 4 == 0) {
		  triangle_reload_flag = true;
	    }

	    if (index % 4 == 3) {
		  uint8_t new_length_index = (triangle_regs[3] >> 3) % 32;
		  triangle_length_counter = length_lookup(new_length_index);
            	  triangle_reload_flag = true;
	    }
       }

       else if (IS_NOISE_REG(index)) {
	    noise_regs[index % 4] = value;

	    
	    if (index % 4 == 3) {
		  uint8_t new_length_index = (noise_regs[3] >> 3) % 32;
		  noise_length_counter = length_lookup(new_length_index);
       	    }
       }

       else if (IS_DMC_REG(index)) {
	    dmc_regs[index % 4] = value;
	    if (index % 4 == 3) {
		  uint8_t new_length_index = (noise_regs[3] >> 3) % 32;
		  dmc_length_counter = length_lookup(new_length_index);
            }
       }

       else if (index == APUSTATUS) {
	    status_reg = value;
       	    status_reg_changes();
       }

       else if (index == FRAME_COUNTER) {
	    frame_counter = value;
	    frame_counter_changes();
       }
       	
}


uint8_t APU::reg_read(uint64_t index) {
       if (IS_PULSE_REG(index)) {
	    return pulse_regs[(index - 0x4000) / 4][index % 4];
       }

       else if (IS_TRIANGLE_REG(index)) {
	    return triangle_regs[index % 4];
       }

       else if (IS_NOISE_REG(index)) {
	    return noise_regs[index % 4];
       }

       else if (IS_DMC_REG(index)) {
	    return dmc_regs[index % 4];

       }

       else if (index == APUSTATUS) {
	    uint8_t return_reg = 0;
	    if (dmc_bytes_remaining > 0) {
		 return_reg |= 0x10;
	    }	    

	    if (noise_length_counter > 0) {
		 return_reg |= 0x8;
	    }

	    if (triangle_length_counter > 0) {
		 return_reg |= 0x4;
	    }

	    if (pulse_length_counters[1] > 0) {
		 return_reg |= 0x2;
	    }

	    if (pulse_length_counters[0] > 0) {
		 return_reg |= 0x1;
            }

	    frame_interrupt_flag = false;
	    return return_reg;
       }

       else if (index == FRAME_COUNTER) {
	    return frame_counter;
       }
}



uint8_t APU::mix_waves() {
//This calculates the audio signal per APU cycle. The analog signal is calculated based off of the channel signals and ranges from 0 - 1.
//The digital signal is proportional to the analog signal, and right now it is 1 byte. If you want to change the resolution level, change the proportionate factor it is multiplied by.
	uint8_t digital_output;
	double analog_output;
	double pulse_out, tnd_out;
	double pulse_wave_sum = pulse_signals[0] + pulse_signals[1];
	double tnd_sum = 3 * triangle_signal + 2 * noise_signal + dmc_signal;

	if (pulse_wave_sum == 0) {
		pulse_out = 0;
	}

	else {
		 pulse_out = 95.52 / ( (8128.0 / pulse_wave_sum) + 100);
	}

	if (tnd_sum == 0) {
		 tnd_out = 0;	
	}	

	else {
 		 tnd_out = 163.67 / ((24329.0 / tnd_sum) + 100);
	}


	analog_output = tnd_sum + pulse_wave_sum;
	digital_output = (uint8_t) (256 * analog_output);

	return digital_output; 
}

void APU::clock_linear_counter() {
	if (triangle_reload_flag) {
	     uint8_t reload_value = triangle_regs[0] % 128;
	     triangle_linear_counter = reload_value;
	}

	else if (triangle_linear_counter > 0) {
	     triangle_linear_counter--;
	}

	bool control_flag = ((triangle_regs[0] >> 7) % 2 == 1);
	if (!control_flag) {
	     triangle_reload_flag = false;
	}

	     
}

void APU::clock_length_counter(uint8_t& counter, bool halt) {
//Clocking the length counter just decrements the length counter if there is no halt flag and the counter value is not 0.
	if (!halt && counter != 0) {
		counter--;
	}
}

void APU::clock_envelope(struct Envelope& envelope, uint8_t& info_reg) {
	if (envelope.start_flag) {
	     if (envelope.divider == 0) {
		  uint8_t period = info_reg % 0x10;
		  envelope.divider = period;
	     	  if (envelope.decay_counter == 0) {
			uint8_t loop = (info_reg >> 5) % 2;
			if (loop) {
			      envelope.decay_counter = 15;
		        }
	          }

		  else {
			envelope.decay_counter--;
	          }
	     }

	     else {
		  envelope.divider--;
	     }		
        }

	else { 
	      uint8_t period = info_reg % 0x10;
      	      envelope.divider = period;
              envelope.decay_counter = 15;			      
	}
}

void APU::clock_sweep(struct Sweep& sweep, uint8_t& info_reg) {
//This clocks the sweep module, which controls the period of the pulse and noise wave timers.
	
	if (sweep.divider == 0) {
	//When the sweep clock reaches 0, this stuff happens.
	
	//Sweep units are enabled, sweep chaneel is unmuted, and the pulse period is adjusted.
	     info_reg |= 0x80;
             sweep.mute = false;
	     uint8_t num_periods = (info_reg >> 4) % 8;
	     sweep.divider = num_periods;
	     sweep.reload_flag = false;
	}

	else if (sweep.reload_flag) {
	//If reload flag is true or divider is 0, divider is reset to P and the reload flag becomes false.
	     uint8_t num_periods = (info_reg >> 4) % 8;
	     sweep.reload_flag = false;
	     sweep.divider = num_periods;
	}

	else {
	//Decrement the clock.
	     sweep.divider--;
	}
}

void APU::pulse_update(uint8_t index) {
//Index indicates which pulse wave is being updated.
	uint8_t const_vol = (pulse_regs[index][0] >> 4) % 2;
	uint8_t length_counter_halt = (pulse_regs[index][0] >> 4) & 0x1;
	uint16_t timer_period = pulse_regs[index][2] + ((pulse_regs[index][3] % 8) << 8);
	uint8_t duty_index = (pulse_regs[index][0] >> 6) % 4;
	bool sweep_mute = pulse_sweeps[index].mute;
	uint8_t reg_volume = pulse_regs[index][0] % 16;

	if (duty_info[duty_index][duty_counters[index]] == 1 && !sweep_mute && pulse_length_counters[index] != 0 && timer_period >= 8)
	{//All of these conditions must be met to output the envelope volume.
	 //1. Duty sequence is not currently 0.
	 //2. Sweep module hasn't muted the wave
	 //3. Length counter is not 0
	 //4. Period of timer is greater than or equal to 8.
		if (const_vol) {
		//If constant volume flag is set, than volume specified in register is output.
			pulse_signals[index] = reg_volume;
		}

		else {
		//Use the decay volume in the pulse sweep module.
			pulse_signals[index] = pulse_envelopes[index].decay_counter; 
		}
	}	

	else {
	//If conditions above not met, output = 0.
		pulse_signals[index] = 0;
	}	

	
	
	if (pulse_dividers[index] == 0) {
		 duty_counters[index] = (duty_counters[index] + 1) % 8;
		 pulse_dividers[index] = timer_period;
	}

	else {
		 pulse_dividers[index]--;
	}
}

void APU::triangle_update() {
	uint8_t timer_period = triangle_regs[2] + ((triangle_regs[3] % 8) << 8);
	if (triangle_timer == 0) {
		triangle_timer = timer_period;
		if (triangle_linear_counter != 0 && triangle_length_counter != 0) {
			triangle_divider = (triangle_divider + 1) % 32;
		}
	}

	if (triangle_divider <= 15) {
		triangle_signal = 15 - triangle_divider;
	}

	else {
		triangle_signal = triangle_divider - 16;
	}
}

void APU::noise_update() {
	uint8_t const_vol = (noise_regs[0] >> 4) % 2;
	uint8_t mode_bit = (noise_regs[2] >> 7) % 2;
	uint8_t xor_bit_index = (mode_bit == 1) ? 6 : 1;
	uint8_t feedback;
	uint8_t reg_volume = noise_regs[0] % 16;
	uint8_t timer_period = noise_period_lookup(noise_regs[2] % 16);

	if (noise_timer == 0) {
		feedback = (noise_shift_register % 2) ^ ((noise_shift_register >> xor_bit_index) % 2);
		noise_shift_register = noise_shift_register >> 1;
		noise_shift_register |= feedback << 14;
		noise_timer = timer_period;
	}

	else {
		noise_timer--;
	}

	if ((noise_shift_register % 2 == 0) && noise_length_counter != 0) {
		if (const_vol == 1) {
			noise_signal = reg_volume;
		}

		else {
			noise_signal = noise_envelope.decay_counter;
		}
	}

	else {
		noise_signal = 0;
	}
}

void APU::dmc_update() {
	uint8_t timer_period = dmc_period_lookup(dmc_regs[0] % 16);

	if (dmc_restart && dmc_bits_remaining == 0) {
		dmc_bytes_remaining = dmc_regs[3];
		dmc_restart = false;
		dmc_current_address = dmc_regs[2];
	}

	if (dmc_bytes_remaining != 0 && dmc_empty) {
		uint8_t loop_flag = (dmc_regs[0] >> 6) % 2;
		uint8_t irq_flag = (dmc_regs[0] >> 7) % 2;

		dmc_empty = false;
		dmc_buffer = memory->mem_read(dmc_current_address);
		if (dmc_current_address == 0xFFFF) {
			dmc_current_address = 0x8000;
		}

		else {
			dmc_current_address++;
		}

		dmc_bytes_remaining--;
		if (dmc_bytes_remaining == 0 && loop_flag == 1) {
			dmc_restart = true;
		}

		else if (dmc_bytes_remaining == 0 && irq_flag == 1) {
		//Process interrupt request here
		}
		
	}	



	if (dmc_timer == 0) {
		dmc_timer = timer_period;
		if (!dmc_silence) {
			uint8_t next_bit = dmc_shift_reg % 2;
			if (next_bit == 1 && dmc_signal <= 125) {
				dmc_signal += 2;
			}

			else if (next_bit == 0 && dmc_signal >= 2) {
				dmc_signal -= 2;
			}
		}
		dmc_bits_remaining--;
		dmc_shift_reg = dmc_shift_reg >> 2;
		if (dmc_bits_remaining == 0) {
			dmc_bits_remaining = 8;
			if (dmc_empty) {
				dmc_silence = true;
			}

			else {
				dmc_silence = false;
				dmc_shift_reg = dmc_buffer;
				dmc_empty = true;

			}
		}

	}

	else {
		dmc_timer--;
	}
}

void APU::frame_clock() {
//The frame counter has two sequences: a 4-step sequence and a 5-step sequence. Each step = 1 frame clock.
	uint8_t mode = (frame_counter >> 7) % 2;
	if (mode == 0) { //4-step
		if (frame_divider % 2 == 0) {
			//Length counters and sweeps clock every 2 frame counter clocks, or half-frame clock..
			clock_length_counter(pulse_length_counters[0], (pulse_regs[0][0] >> 4) % 2);
			clock_length_counter(pulse_length_counters[1], (pulse_regs[1][0] >> 4) % 2);
			clock_length_counter(triangle_length_counter, (triangle_regs[0] >> 7) % 2);
			clock_length_counter(noise_length_counter, (noise_regs[0] >> 4) % 2);

			clock_sweep(pulse_sweeps[0], pulse_regs[0][1]);
			clock_sweep(pulse_sweeps[1], pulse_regs[1][1]);			
		}
		//Linear counter and envelopes clock every frame counter clock, or quarter-frame clock..
		clock_linear_counter();
		clock_envelope(pulse_envelopes[0], pulse_regs[0][0]);
		clock_envelope(pulse_envelopes[1], pulse_regs[1][0]);
		clock_envelope(noise_envelope, noise_regs[0]);
		
		uint8_t interrupt_inh = (frame_counter >> 6) % 2;
		if (!interrupt_inh && frame_divider == 4) {
		//During the 4th cycle, if the interrupt inhibit flag is false, frame interrupt is true.
			frame_interrupt_flag = true;
		}

		frame_divider = (frame_divider % 4) + 1;
	}

	else { //5-step
	        if (frame_divider == 2 || frame_divider == 5) {
		//Half-frame clocks. Note frame clock != frame counter clock
			clock_length_counter(pulse_length_counters[0], (pulse_regs[0][0] >> 4) % 2);
			clock_length_counter(pulse_length_counters[1], (pulse_regs[1][0] >> 4) % 2);
			clock_length_counter(triangle_length_counter, (triangle_regs[0] >> 7) % 2);
			clock_length_counter(noise_length_counter, (noise_regs[0] >> 4) % 2);
		}

		if (frame_divider != 4) {
		//Quarter-frame clocks.
			clock_linear_counter();
			clock_envelope(pulse_envelopes[0], pulse_regs[0][0]);
			clock_envelope(pulse_envelopes[1], pulse_regs[1][0]);
			clock_envelope(noise_envelope, noise_regs[0]);
		}

		frame_divider = (frame_divider % 5) + 1;
	}
}

/*

SDL_AudioCallback callback(void* userdata, Uint8* stream, int len) {

}

*/


void APU::initialize_SDL() {

	SDL_AudioSpec want, have;
	want.freq = 11025;
	want.format = AUDIO_U8;
	want.channels = 1;
	want.callback = NULL;

	//SDL_AudioInit("sndio");
	//std::cout << SDL_GetNumAudioDrivers() << "\n";
	//std::cout << SDL_GetCurrentAudioDriver() << "\n";
	std::cout << SDL_GetNumAudioDevices(0) << "\n";
	for (int i = 0; i < 6; i++) {
		std::cout << SDL_GetAudioDriver(i) << "\n";
	//	std::cout << SDL_GetNumAudioDevices(0) << "\n";
//		std::cout << SDL_GetAudioDeviceName(0, 0) << "\n";
	}

	device = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, 0), 0, &want, &have, 0);
	if (device == 0) {
		std::cout << "Error in opening audio device " << SDL_GetError() << "\n";
	}

	SDL_PauseAudioDevice(device, 0);	

}

void APU::send_byte_to_SDL() {
	SDL_QueueAudio(device, &current_signal, 1);
}


void APU::execute() {
      pulse_update(0);
      pulse_update(1);
      //Note triangle updates twice in 1 APU cycle as specified in the documentation.
      triangle_update();
      triangle_update();

      noise_update();

      dmc_update();

      //initialize_SDL();
      current_signal = mix_waves();
      send_byte_to_SDL();
}
