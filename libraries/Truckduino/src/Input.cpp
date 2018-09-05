#include "Input.h"
#include "Arduino.h"
#include "HardwareTimer.h"
#include "Watchdog.h"

volatile uint32_t t8_counter = 0;
int input_pins[INPUT_MAX] = { PA5, PB1, PC0, PC1, PC2, PC3, PC4, PC5 };

CInput::CInput() {
}

void CInput::begin() {
	if (!Watchdog.ready()) return;
	for (int i = 0; i < INPUT_MAX; i++) {
		pinMode(input_pins[i], INPUT);
		input_values[i] = analogRead(input_pins[i]);
		filter_levels[i] = 0;
		percent_modes[i] = 0;
		ranges_lo[i] = 0;
		ranges_hi[i] = 4095;
		tholds[i] = INPUT_DISCRETE_THOLD;
		hysts[i] = INPUT_DISCRETE_HYST;
		discrete_states[i] = false;
	}

	t8_setup();
}

void CInput::filterConfig(int channel, int level) {
	if (level < 0) level = 0;
	else if (level > INPUT_FILTER_MAX) level = INPUT_FILTER_MAX;
	if (channel < INPUT_MAX) {
		filter_levels[channel] = level;
		for (int i = 0; i < level; i++) {
			filter_values[channel][i] = input_values[channel];
		}
	}
}

void CInput::discreteConfig(int channel, int one_thold, int hyst) {
	if (channel < INPUT_MAX) {
		tholds[channel] = one_thold;
		hysts[channel] = hyst;
	}
}

void CInput::valuePercentConfig(int channel, int lo, int hi) {
	if (channel < INPUT_MAX) {
		percent_modes[channel] = 0;
		ranges_lo[channel] = lo;
		ranges_hi[channel] = hi;
	}
}

void CInput::voltagePercentConfig(int channel, float lo, float hi) {
	if (channel < INPUT_MAX) {
		percent_modes[channel] = 1;
		ranges_lo[channel] = lo;
		ranges_hi[channel] = hi;
	}
}

void CInput::currentPercentConfig(int channel, float lo, float hi) {
	if (channel < INPUT_MAX) {
		percent_modes[channel] = 2;
		ranges_lo[channel] = lo;
		ranges_hi[channel] = hi;
	}
}

int CInput::valueRead(int channel) {
	int ret_val = 0;
	if (channel < INPUT_MAX) {
		ret_val = input_values[channel];
	}
	return ret_val;
}

float CInput::voltageRead(int channel) {
	float ret_val = 0.0;
	if (channel < INPUT_MAX) {
		ret_val = 31.39 * input_values[channel] / 4095;
	}
	return ret_val;
}

float CInput::currentRead(int channel) {
	float ret_val = 0.0;
	if (channel < INPUT_MAX) {
		ret_val = 21.997 * input_values[channel] / 4095;
	}
	return ret_val;
}

bool CInput::discreteRead(int channel) {
	bool ret_val = false;
	if (channel < INPUT_MAX) {
		ret_val = discrete_states[channel];
		if (input_values[channel] >= tholds[channel]) ret_val = true;
		else if (input_values[channel] < tholds[channel] - hysts[channel]) ret_val = false;
	}
	discrete_states[channel] = ret_val;
	return ret_val;
}

float CInput::percentRead(int channel) {
	float ret_val = 0;
	if (channel < INPUT_MAX) {
		float value = 0;
		if (percent_modes[channel] == 0) value = valueRead(channel);
		else if (percent_modes[channel] == 1) value = voltageRead(channel);
		else if (percent_modes[channel] == 2) value = currentRead(channel);
		ret_val = ((value - ranges_lo[channel]) * 100.0 / (ranges_hi[channel] - ranges_lo[channel]));
	}
	return ret_val;
}

static void t8_handler(void) {
	// 50 раз в секунду
	t8_counter++;
	for (int i = 0; i < INPUT_MAX; i++) {
		if (Input.filter_levels[i] > 0) {
			for (int j = 1; j < Input.filter_levels[i]; j++) {
				Input.filter_values[i][j-1] = (Input.filter_values[i][j-1] + Input.filter_values[i][j]) / 2;
			}
			Input.filter_values[i][Input.filter_levels[i]-1] = (Input.filter_values[i][Input.filter_levels[i]-1] + analogRead(input_pins[i])) / 2;
			Input.input_values[i] = Input.filter_values[i][0];
		}
		else Input.input_values[i] = analogRead(input_pins[i]);
	}

}

void CInput::t8_setup(void) {
	Timer8.pause();
	Timer8.setPeriod(20000);
	Timer8.setCount(0);
	Timer8.attachCompare1Interrupt(t8_handler);
	t8_counter = 0;
	Timer8.resume();
}
