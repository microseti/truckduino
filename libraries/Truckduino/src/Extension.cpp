#include "Extension.h"
#include "Watchdog.h"
#include <HardwareTimer.h>
#include <HardwareSerial.h>

CExtension::CExtension(uint8_t channel) {
	if (channel) c_channel = 1;
	else c_channel = 0;
}

void c1_e1_handler(void) {
	Ext1.p_counter1++;
	Ext1.counter1++;
}

void c1_e2_handler(void) {
	Ext2.p_counter1++;
	Ext2.counter1++;
}

void c2_e1_handler(void) {
	Ext1.p_counter2++;
	Ext1.counter2++;
}

void c2_e2_handler(void) {
	Ext2.p_counter2++;
	Ext2.counter2++;
}

void CExtension::begin(void) {
	if (!Watchdog.ready()) return;
	if (c_channel == 0) {
		digitalWrite(PB4, LOW);			// pio1
		digitalWrite(PA8, LOW);			// pio2
		pinMode(PC8, OUTPUT);			// pwm1
		pinMode(PC9, OUTPUT);			// pwm2
		pinMode(PA0, INPUT);			// adc
		pinMode(PA15, INPUT_PULLUP);	// counter1
		attachInterrupt(PA15, c1_e1_handler, FALLING);
		pinMode(PA12, INPUT_PULLUP);	// counter2
		attachInterrupt(PA12, c2_e1_handler, FALLING);

		// jtdi ext2.int1, need disable jtag
		AFIO_BASE->MAPR |= AFIO_MAPR_SWJ_CFG_NO_JTAG_NO_SW;
	}
	else {
		digitalWrite(PB9, LOW);			// pio1
		digitalWrite(PB8, LOW);			// pio2
		pinMode(PC6, OUTPUT);			// pwm1
		pinMode(PC7, OUTPUT);			// pwm2
		pinMode(PA1, INPUT);			// adc
		pinMode(PA7, INPUT_PULLUP);		// counter1
		attachInterrupt(PA7, c1_e2_handler, FALLING);
		pinMode(PA6, INPUT_PULLUP);		// counter2
		attachInterrupt(PA6, c2_e2_handler, FALLING);
	}
	serial = &Serial2;
	i2c = &Wire;
	p_counter1 = 0;
	p_counter2 = 0;
	counter1 = 0;
	counter2 = 0;
	freq1 = 0;
	freq2 = 0;
	c_freq = 1;
}

void t2_e1_handler(void) {
	Ext1.freq1 = Ext1.counter1 / Ext1.c_freq;
	Ext1.counter1 = 0;
	Ext1.freq2 = Ext1.counter2 / Ext1.c_freq;
	Ext1.counter2 = 0;
}

void t3_e2_handler(void) {
	Ext2.freq1 = Ext2.counter1 / Ext2.c_freq;
	Ext2.counter1 = 0;
	Ext2.freq2 = Ext2.counter2 / Ext2.c_freq;
	Ext2.counter2 = 0;
}

void CExtension::counterConfig(uint16_t freq_hz) {
	if (freq_hz > 1000) freq_hz = 1000;
	else if (freq_hz == 0) freq_hz = 1;

	if (c_channel == 0) {
		Timer2.pause();
		Timer2.setPeriod((1000 / freq_hz) * 1000);	// in us
		Timer2.attachCompare1Interrupt(t2_e1_handler);
		Timer2.resume();
	}
	else {
		Timer3.pause();
		Timer3.setPeriod((1000 / freq_hz) * 1000);	// in us
		Timer3.attachCompare1Interrupt(t3_e2_handler);
		Timer3.resume();
	}

	c_freq = freq_hz;
}

void CExtension::counterReset(int counter) {
	if (counter == COUNTER1) {
		p_counter1 = 0;
		counter1 = 0;
		freq1 = 0;
	}
	else if (counter == COUNTER2) {
		p_counter2 = 0;
		counter2 = 0;
		freq2 = 0;
	}
}

float CExtension::counterFreqGet(int counter) {
	float ret_val = 0;
	if (counter == COUNTER1) {
		ret_val = freq1;
	}
	else if (counter == COUNTER2) {
		ret_val = freq2;
	}
	return ret_val;
}

uint32_t CExtension::counterCountGet(int counter) {
	uint32_t ret_val = 0;
	if (counter == COUNTER1) {
		ret_val = p_counter1;
	}
	else if (counter == COUNTER2) {
		ret_val = p_counter2;
	}
	return ret_val;
}

void CExtension::counterCountSet(int counter, uint32_t count) {
	if (counter == COUNTER1) {
		p_counter1 = count;
	}
	else if (counter == COUNTER2) {
		p_counter2 = count;
	}
}

void CExtension::pwmWrite(int pwm, uint16_t value) {
	if (c_channel == 0) {
		if (pwm == PWM1) {
			analogWrite(PC8, value);
		}
		else if (pwm == PWM2) {
			analogWrite(PC9, value);
		}
	}
	else {
		if (pwm== PWM1) {
			analogWrite(PC6, value);
		}
		else if (pwm == PWM2) {
			analogWrite(PC7, value);
		}
	}
}

void CExtension::pioConfig(int pwm, WiringPinMode mode) {
	if (c_channel == 0) {
		if (pwm == PIO1) {
			pinMode(PB4, mode);
		}
		else if (pwm == PIO2) {
			pinMode(PA8, mode);
		}
	}
	else {
		if (pwm == PIO1) {
			pinMode(PB9, mode);
		}
		else if (pwm == PIO2) {
			pinMode(PB8, mode);
		}
	}
}

void CExtension::pioWrite(int pio, uint8_t state) {
	if (c_channel == 0) {
		if (pio == PIO1) {
			digitalWrite(PB4, state);
		}
		else if (pio == PIO2) {
			digitalWrite(PA8, state);
		}
	}
	else {
		if (pio == PIO1) {
			digitalWrite(PB9, state);
		}
		else if (pio == PIO2) {
			digitalWrite(PB8, state);
		}
	}
}

uint8_t CExtension::pioRead(int pio) {
	uint8_t ret_val = LOW;
	if (c_channel == 0) {
		if (pio == PIO1) {
			ret_val = digitalRead(PB4);
		}
		else if (pio == PIO2) {
			ret_val = digitalRead(PA8);
		}
	}
	else {
		if (pio == PIO1) {
			ret_val = digitalRead(PB9);
		}
		else if (pio == PIO2) {
			ret_val = digitalRead(PB8);
		}
	}
	return ret_val;
}

uint16_t CExtension::adcRead() {
	uint16_t ret_val = 0;
	if (c_channel == 0) {
		ret_val = analogRead(PA0);
	}
	else {
		ret_val = analogRead(PA1);
	}
	return ret_val;
}
