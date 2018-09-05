#include "Truckduino.h"

bool relay_state[4];
int channel = 0;	// 0 - RELAY1, 1 - RELAY2, 2 - RELAY3, 3 - RELAY4
bool key1 = false, key2 = false, key3 = false;

void display_channel() {
    Indicator.displayString("Ch.  ");
    Indicator.displayDigit(N4, channel + 1, false);
    delay(50);
	Watchdog.reset();
}

void display_state() {
    if (relay_state[channel]) Indicator.displayString("On");
    else Indicator.displayString("Off");
}

void setup () {
    Indicator.begin();
    Indicator.displayTest();

    Control.begin();

    relay_state[0] = false;
    relay_state[1] = false;
    relay_state[2] = false;
    relay_state[3] = false;

    display_channel();
}

void loop() {
    if (Indicator.buttonPress(K1)) {
        if (!key1) {
            key1 = true;
            if (channel > 0) channel--;
            display_channel();
        }
    }
    else key1 = false;

    if (Indicator.buttonPress(K2)) {
        if (!key2) {
            key2 = true;
            if (relay_state[channel]) {
                relay_state[channel] = false;
                Control.relayOff(channel);
            }
            else {
                relay_state[channel] = true;
                Control.relayOn(channel);
            }
            display_state();
        }
    }
    else key2 = false;

    if (Indicator.buttonPress(K3)) {
        if (!key3) {
            key3 = true;
            if (channel < 3) channel++;
            display_channel();
        }
    }
    else key3 = false;

    delay(50);
	Watchdog.reset();
}
