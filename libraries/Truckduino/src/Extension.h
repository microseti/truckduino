#ifndef __EXTENSION_H
#define __EXTENSION_H

#include <Arduino.h>
#include <Wire.h>

#define COUNTER1	0
#define COUNTER2	1

#define PWM1	0
#define PWM2	1

#define PIO1	0
#define PIO2	1

class CExtension
{
	public:
		CExtension(uint8_t channel = 0);
		void begin(void);
		void counterConfig(uint16_t freq_hz);
		void counterReset(int counter);
		float counterFreqGet(int counter);
		uint32_t counterCountGet(int counter);
		void counterCountSet(int counter, uint32_t count);
		void pwmWrite(int pwm, uint16_t value);
		void pioConfig(int pio, WiringPinMode mode);
		void pioWrite(int pio, uint8_t state);
		uint8_t pioRead(int pio);
		uint16_t adcRead();
		TwoWire *i2c;
		HardwareSerial *serial;
		volatile uint32_t p_counter1;
		volatile uint32_t p_counter2;
		volatile uint32_t counter1;
		volatile uint32_t counter2;
		volatile float freq1;
		volatile float freq2;
		uint8_t c_freq;
	private:
		uint8_t c_channel;
};

extern CExtension Ext1;
extern CExtension Ext2;

#endif // __EXTENSION_H
