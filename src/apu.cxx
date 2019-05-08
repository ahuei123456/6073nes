#include "apu.hpp"
uint8_t PPU::mix_waves() {
//This calculates the audio signal per APU cycle. The analog signal is calculated based off of the channel signals and ranges from 0 - 1.
//The digital signal is proportional to the analog signal, and right now it is 1 byte. If you want to change the resolution level, change the proportionate factor it is multiplied by.
	uint8_t digital_output;
	double analog_output;
	double pulse_out, tnd_out;
	double pulse_wave_sum = pulse_signals[0] + pulse_signals[1];
	double tnd_sum = 3 * triangle_signal + 2 * noise_signal + dmc_signal;

	pulse_out = 95.52 / ( (8128.0 / pulse_wave_sum) + 100);
	tnd_out = 163.67 / ((24329.0 / tnd_sum) + 100);
	
	analog_output = tnd_sum + pulse_wave_sum;
	digital_output = (uint8_t) (256 * analog_signal);

	return 
}

void pulse_update(uint8_t index) {
	uint8_t const_vol = pulse_regs[index][0] % 0x10;
	uint8_t length_counter = 
		
}

void triangle_update() {

}

void noise_update() {

}

void dmc_update() {

}


void execute() {
      
      mix_waves();
}
