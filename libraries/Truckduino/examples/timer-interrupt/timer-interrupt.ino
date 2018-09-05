#include "Truckduino.h"

uint8_t timer1_counter = 0;
uint8_t timer2_counter = 0;
bool key1 = false, key2 = false, key3 = false;
bool display_flag = false;
bool timer_flag = true;

void timer1_handler() {
	timer1_counter++;
}

void timer2_handler() {
	timer2_counter--;
}

void timer3_handler() {
}

void timer4_handler() {
}

void setup() {
	Serial.begin(9600);
	Serial.println("Hello from timer-interrupt test program.");

	Indicator.begin();
	Indicator.displayTest();

	Timer.pause();
	Timer.setPeriod(100000);	// 100 ms
	Timer.attachCompare1Interrupt(timer1_handler);
	Timer.attachCompare2Interrupt(timer2_handler);
	Timer.attachCompare3Interrupt(timer3_handler);
	Timer.attachCompare4Interrupt(timer4_handler);
	Timer.resume();

	Indicator.displayDigit(0);
	delay(500);
}

void loop() {
	if (Indicator.buttonPress(K1)) {
		if (!key1) {
			key1 = true;
			display_flag = !display_flag;
		}
	}
	else key1 = false;
	if (Indicator.buttonPress(K2)) {
		if (!key2) {
			key2 = true;
			timer_flag = !timer_flag;
			if (timer_flag) Timer.resume();
			else Timer.pause();
		}
	}
	else key2 = false;
	if (Indicator.buttonPress(K3)) {
		if (!key3) {
			key3 = true;
			timer1_counter = 0;
			timer2_counter = 0;
		}
	}
	else key3 = false;

	if (display_flag) Indicator.displayDigit(timer2_counter);
	else Indicator.displayDigit(timer1_counter);

	delay(50);
	Watchdog.reset();
}
