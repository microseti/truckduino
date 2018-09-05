#include "Truckduino.h"

int valve_index;
int valve_state[18];
bool key1, key2, key3;

void setup() {
	Serial.begin(9600);
	Serial.println("Valve control test program.");
	Indicator.begin();
	Indicator.displayTest();
	Indicator.displayBrightness(LEVEL3);

	Control.begin();

	valve_index = 0;
	for (int i = 0; i < 18; i++) valve_state[i] = 0;

	key1 = false;
	key2 = false;
	key3 = false;

	Indicator.displayString("0  0");
	delay(1000);
	Watchdog.reset();
}

void loop() {
	if (Indicator.buttonPress(K1)) {
		if (!key1) {
			key1 = true;
			if (valve_index == 0) valve_index = 18;
			valve_index--;
			Indicator.displayDigit(valve_index+1);
			delay(1000);
			Watchdog.reset();
		}
	}
	else key1 = false;
	if (Indicator.buttonPress(K3)) {
		if (!key3) {
			key3 = true;
			valve_index++;
			if (valve_index == 18) valve_index = 0;
			Indicator.displayDigit(valve_index+1);
			delay(1000);
			Watchdog.reset();
		}
	}
	else key3 = false;
	if (Indicator.buttonPress(K2)) {
		if (!key2) {
			key2 = true;
			if (valve_state[valve_index] == 0) valve_state[valve_index] = 1;
			else if (valve_state[valve_index] == 1) valve_state[valve_index] = 2;
			else if (valve_state[valve_index] == 2) valve_state[valve_index] = 0;

			if (valve_state[valve_index] == 0) {
				Control.valveOff(valve_index);
				Indicator.displayString("0  0");
			}
			else if (valve_state[valve_index] == 1) {
				Control.valveLeft(valve_index);
				Indicator.displayString("1  0");
			}
			else if (valve_state[valve_index] == 2) {
				Control.valveRight(valve_index);
				Indicator.displayString("0  1");
			}
			delay(1000);
			Watchdog.reset();
		}
	}
	else key2 = false;

	if (Control.valveStateGet(valve_index) == FAILURE) Indicator.displayString("Err1");
	else Indicator.displayString("Err0");

	delay(100);
	Watchdog.reset();
}
