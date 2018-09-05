#include "Truckduino.h"

bool key1 = false, key2 = false, key3 = false;
bool update_flag = false;
uint32_t value = 1000;
uint8_t data[50];

void indic_update() {
	Indicator.displayDigit(value);
}

void setup() {
	Serial.begin(9600);
	Serial.println("Hello from weight emulator program.");
	Indicator.begin();
	Indicator.displayTest();
	Indicator.displayBrightness(LEVEL3);
	Control.begin();
	indic_update();
}

void loop() {
	if (Indicator.buttonPress(K1)) {
		if (!key1) {
			Serial.println("K1 pressed");
			key1 = true;
			if (value > 0) {
				if (value > 1100 || value <= 900) value -= 100;
				else value -= 10;
			}
			update_flag = true;
		}
	}
	else key1 = false;

	if (Indicator.buttonPress(K2)) {
		if (!key2) {
			Serial.println("K2 pressed");
			key2 = true;
			value = 1000;
			update_flag = true;
		}
	}
	else key2 = false;

	if (Indicator.buttonPress(K3)) {
		if (!key3) {
			Serial.println("K3 pressed");
			key3 = true;
			if (value < 9900) {
				if (value >= 1100 || value < 900) value += 100;
				else value += 10;
			}
			update_flag = true;
		}
	}
	else key3 = false;

	if (update_flag) {
		update_flag = false;
		indic_update();
	}

	if (Serial.available()) {
		delay(10);
		uint8_t len = 0;
		while (Serial.available()) {
			data[len++] = Serial.read();
			if (len == 50) break;
			delay(10);
		}
		if (len >= 7) {
			if (data[0] == 0x07 && data[1] == 0x00 && data[2] == 0x00 && data[3] == 0x00 && data[4] == 0x00 && data[5] == 0x00 && data[6] == 0x85) {
				data[1] = 0x80;
				data[2] = (uint8_t) (value);
				data[3] = (uint8_t) (value >> 8);
				data[4] = (uint8_t) (value >> 16);
				data[5] = 0x14;
				data[6] = 0xFF;
				for (uint8_t i = 0; i < 7; i++) Serial.write(data[i]);
				delay(10);
				Serial.flush();
			}
		}
		len = 0;
	}
	delay(50);
	Watchdog.reset();
}
