#include "Truckduino.h"

int input_channel = INPUT1;
int channel_index = 0;	// 0 - INPUT1, 1 - INPUT2, 2 - INPUT3, ... , 7 - INPUT8
int mode_index = 0;		// 0 - value, 1 - voltage, 2 - current, 3 - discrete, 4 - percent
bool key1 = false, key2 = false, key3 = false;

void display_channel() {
    Indicator.displayString("Ch.  ");
    Indicator.displayDigit(N4, channel_index + 1, false);
    delay(500);
    Watchdog.reset();
}

void display_mode() {
    if (mode_index == 0) Indicator.displayString("UALu");
    else if (mode_index == 1) Indicator.displayString("Uolt");
    else if (mode_index == 2) Indicator.displayString("Curr");
    else if (mode_index == 3) Indicator.displayString("dEsc");
    else if (mode_index == 4) Indicator.displayString("pErc");
    delay(500);
    Watchdog.reset();
}

void setup () {
    Indicator.begin();
    Indicator.displayTest();
    display_channel();
    delay(500);
    display_mode();
    delay(500);
    Watchdog.reset();
    
    Input.begin();
    Input.voltagePercentConfig(INPUT1, 0, 10);
}

void loop() {
    if (Indicator.buttonPress(K1)) {
        if (!key1) {
            key1 = true;
            if (channel_index > 0) channel_index--;
            display_channel();
        }
    }
    else key1 = false;

    if (Indicator.buttonPress(K2)) {
        if (!key2) {
            key2 = true;
            mode_index++;
            if (mode_index == 5) mode_index = 0;
            display_mode();
        }
    }
    else key2 = false;

    if (Indicator.buttonPress(K3)) {
        if (!key3) {
            key3 = true;
            if (channel_index < 7) channel_index++;
            display_channel();
        }
    }
    else key3 = false;

    if (mode_index == 0) Indicator.displayDigit(Input.valueRead(input_channel));
    else if (mode_index == 1) Indicator.displayDigit(Input.voltageRead(input_channel),2);
    else if (mode_index == 2) Indicator.displayDigit(Input.currentRead(input_channel),2);
    else if (mode_index == 3) Indicator.displayDigit(Input.discreteRead(input_channel));
    else if (mode_index == 4) Indicator.displayDigit(Input.percentRead(input_channel),2);

    delay(100);
    Watchdog.reset();
}