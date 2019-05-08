#include <SDL.h>
#include "mem.h"

#define WAVE_REGS 4
#define NUM_PULSE_WAVES 2

class Mem;

class APU {

private:
	//Registers used by APU.
	std::array<std::array<uint8_t, WAVE_REGS>, NUM_PULSE_WAVES> pulse_regs;
	std::array<uint8_t, NUM_PULSE_WAVES> pulse_signals;
	std::array<bool, NUM_PULSE_WAVES> pulse_silence;

	std::array<uint8_t, WAVE_REGS> triangle_regs;
	uint8_t triangle_signal;
	bool triangle_silence = true;

	std::array<uint8_t, WAVE_REGS> noise_regs;
	uint8_t noise_signal;
	bool noise_silence = true;

	std::array<uint8_t, WAVE_REGS> dmc_regs;
	uint8_t dmc_signal;
	bool dmc_silence = true;
	
	uint8_t status_reg;
	uint8_t frame_counter;

	uint8_t current_signal = 0;
public:
	void pulse_update(uint8_t index);
	void triangle_update();
	void noise_update();
	void dmc_update();

	uint8_t mix_waves();
	void execute();

}
