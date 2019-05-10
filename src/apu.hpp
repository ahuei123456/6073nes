#ifndef apu_hpp
#define apu_hpp

#include <SDL.h>
#include "mem.hpp"
#include <iostream>
#include <sndio.h>

#define WAVE_REGS 4
#define NUM_PULSE_WAVES 2
#define IS_PULSE_REG(index) (index >= 0x4000 && index <= 0x4007)
#define IS_TRIANGLE_REG(index) (index >= 0x4008 && index <= 0x400B)
#define IS_NOISE_REG(index) (index >= 0x400C && index <= 0x400F)
#define IS_DMC_REG(index) (index >= 0x4010 && index <= 0x4013)
#define APUSTATUS 0x4015
#define FRAME_COUNTER 0x4017
#define DUTY_CYCLE_LENGTH 8
#define NUM_DUTY_TYPES 4
#define NUM_LIN_LENGTH 16
#define LIN_LENGTH_HIGHEST_BYTE 0x1F
#define NUM_BASE_LENGTH_12 8
#define LENGTH_12_HIGHEST_BYTE 0x1E
#define NUM_BASE_LENGTH_10 8
#define LENGTH_10_HIGHEST_BYTE 0x0E
#define NUM_NOISE_PERIODS 16
#define NUM_DMC_PERIODS 16

class Mem;

struct Envelope {
	bool start_flag;
	uint8_t divider;
	uint8_t decay_counter;
	uint8_t volume;
};

struct Sweep {
	uint8_t divider;
	bool reload_flag;
	bool mute = false;
};


class APU {
//IMPORTANT: Execution of the frame clock takes place at a different rate than executing the APU cycle. Moreover, wave generators are clocked at different rates than the components that feed values
//to the waves, such as the sweep module and the envelope generator. 1 frame clock = 3728 APU cycles, so keep this in mind when deciding to execute the frame_clock function.
//The execute function executes a single APU cycle. All the wave timers are clocked every APU cycle(except for triangle wave, which is clocked twice per APU cycle).
//Envelopes and length counters are clocked at a half-frame rate(every 2 frame clocks) and linear counters and sweeps are clocked at a quarter-frame rate(every frame clock).

private:
	//Registers used by APU.
	std::shared_ptr<Mem> memory;
	//
	//
	std::array<std::array<uint8_t, DUTY_CYCLE_LENGTH>, NUM_DUTY_TYPES> duty_info = {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1};  
	std::array<uint8_t, NUM_LIN_LENGTH> lin_length_table = {30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 254};
        std::array<uint8_t, NUM_BASE_LENGTH_12> length_12_table = {32, 16, 72, 192, 96, 48, 24, 12};
	std::array<uint8_t, NUM_BASE_LENGTH_10> length_10_table = {26, 14, 60, 160, 80, 40, 20, 10};	
	std::array<uint16_t, NUM_NOISE_PERIODS> noise_period_table = {4, 8, 16, 32, 64, 96, 128, 160, 202,254, 380, 508, 762, 1016, 2034, 4068};
	std::array<uint16_t, NUM_DMC_PERIODS> dmc_period_table = {428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54};

	std::array<std::array<uint8_t, WAVE_REGS>, NUM_PULSE_WAVES> pulse_regs;
	std::array<uint8_t, NUM_PULSE_WAVES> pulse_signals = {0, 0};
	std::array<bool, NUM_PULSE_WAVES> pulse_silence;
	std::array<uint8_t, NUM_PULSE_WAVES> pulse_length_counters;
	std::array<uint8_t, NUM_PULSE_WAVES> duty_counters = {0, 0};
	std::array<struct Envelope, NUM_PULSE_WAVES> pulse_envelopes;
	std::array<struct Sweep, NUM_PULSE_WAVES> pulse_sweeps;
	std::array<uint8_t, NUM_PULSE_WAVES> pulse_dividers = {0, 0};

	std::array<uint8_t, WAVE_REGS> triangle_regs;
	uint8_t triangle_signal = 0;
	bool triangle_silence = true;
	uint8_t triangle_length_counter;
	uint8_t triangle_envelope;
	uint8_t triangle_linear_counter;
	bool triangle_reload_flag = false;
	uint8_t triangle_timer = 0;
	uint8_t triangle_divider = 0;

	std::array<uint8_t, WAVE_REGS> noise_regs;
	uint8_t noise_signal = 0;
	bool noise_silence = true;
	uint8_t noise_length_counter;
	uint16_t noise_shift_register;
	struct Envelope noise_envelope;
	uint8_t noise_timer = 0;

	std::array<uint8_t, WAVE_REGS> dmc_regs;
	uint8_t dmc_signal = 0;
	bool dmc_silence = true;
	uint8_t dmc_length_counter;
	uint8_t dmc_buffer;
	uint8_t dmc_bytes_remaining = 0;
	uint16_t dmc_current_address;
	uint8_t dmc_timer = 0;
	uint8_t dmc_bits_remaining = 0;
	uint8_t dmc_shift_reg;
	bool dmc_restart = false;
	bool dmc_empty = true;

	uint8_t status_reg;
	uint8_t frame_counter;
	bool frame_interrupt_flag = false;
	uint8_t frame_divider;

	uint8_t current_clock;

	uint8_t current_signal = 128;


	SDL_AudioDeviceID device;

public:
	APU(std::shared_ptr<Mem> memory); 
	uint8_t length_lookup(uint8_t index);
	uint16_t noise_period_lookup(uint8_t index);
	uint16_t dmc_period_lookup(uint8_t index);

	void clock_linear_counter();
	void clock_length_counter(uint8_t& counter, bool halt);
	void clock_envelope(struct Envelope& envelope, uint8_t& info_reg);
	void clock_sweep(struct Sweep& sweep, uint8_t& info_reg);

	void pulse_update(uint8_t index);
	void triangle_update();
	void noise_update();
	void dmc_update();

	void status_reg_changes();
	void frame_counter_changes();

	void reg_write(uint64_t index, uint8_t value);
	uint8_t reg_read(uint64_t index);

	uint8_t mix_waves();
	void frame_clock();
	void execute();

	SDL_AudioCallback callback(void* userdata, Uint8* stream, int len);
	void initialize_SDL();
	void send_byte_to_SDL();

};

#endif
