#include "Truckduino.h"

byte dig = 5;
byte bright = LEVEL1;
bool key1 = false, key2 = false, key3 = false;
bool update_flag = false;

void indic_update() {
	Indicator.displayDigit(N1, dig, false);
	Indicator.displayDigit(N2, dig, false);
	Indicator.displayDigit(N3, dig, false);
	Indicator.displayDigit(N4, dig, false);
}

void setup() {
	Serial.begin(9600);
	Serial.println("Indicator module. Buttons test program.");
	Indicator.begin(LEVEL1);
	Indicator.displayTest();
	indic_update();
}

void loop() {
	if (Indicator.buttonPress(K1)) {
		if (!key1) {
			key1 = true;
			if (dig > 0) dig--;
			update_flag = true;
		}
	}
	else key1 = false;
	if (Indicator.buttonPress(K3)) {
		if (!key3) {
			key3 = true;
			if (dig < 9) dig++;
			update_flag = true;
		}
	}
	else key3 = false;
	if (Indicator.buttonPress(K2)) {
		if (!key2) {
			key2 = true;
			bright++;
			if (bright > LEVEL7) bright = LEVEL1;
			Indicator.displayBrightness(bright);
		}
	}
	else key2 = false;
	if (update_flag) {
		update_flag = false;
		indic_update();
	}
	delay(50);
	Watchdog.reset();
}
