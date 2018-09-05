#include "Truckduino.h"

void setup() {
	Serial.begin(9600);
	Serial.println("Indicator module. Display test program.");
	Indicator.begin();
	Indicator.displayTest();
}

void loop() {
	Indicator.displayString("test");
	delay(1000);
	Watchdog.reset();

	Indicator.displayBrightness(LEVEL7);
	delay(1000);
	Watchdog.reset();

	Indicator.displayBrightness(LEVEL1);
	delay(1000);
	Watchdog.reset();

	Indicator.displayBrightness(LEVEL3);
	Indicator.displayString("0123");
	delay(500);
	Watchdog.reset();

	Indicator.displayString("0123");
	Indicator.displayDot(N1, true);
	delay(500);
	Watchdog.reset();

	Indicator.displayString("0123");
	Indicator.displayDot(N1, false);
	Indicator.displayDot(N2, true);
	delay(500);
	Watchdog.reset();

	Indicator.displayString("0123");
	Indicator.displayDot(N2, false);
	Indicator.displayDot(N3, true);
	delay(500);
	Watchdog.reset();

	Indicator.displayDigit(N1, 0, false);
	Indicator.displayDigit(N2, 1, false);
	Indicator.displayDigit(N3, 2, false);
	Indicator.displayDigit(N4, 3, true);
	delay(500);
	Watchdog.reset();

	Indicator.displayDigit(N1, 0xA, false);
	Indicator.displayDigit(N2, 0xB, false);
	Indicator.displayDigit(N3, 0xC, false);
	Indicator.displayDigit(N4, 0xD, false);
	delay(1000);
	Watchdog.reset();

	Indicator.displayDigit(0);
	delay(1000);
	Watchdog.reset();

	Indicator.displayDigit(1);
	delay(500);
	Watchdog.reset();

	Indicator.displayDigit(12);
	delay(500);
	Watchdog.reset();

	Indicator.displayDigit(123);
	delay(500);
	Watchdog.reset();

	Indicator.displayDigit(1234);
	delay(1000);
	Watchdog.reset();

	Indicator.displayDigit(9.876, 2);
	delay(1000);
	Watchdog.reset();

	Indicator.displayDigit(0.567, 1);
	delay(1000);
	Watchdog.reset();

	Indicator.displayClear();
	delay(500);
	Watchdog.reset();

	Indicator.displayString("-   "); delay(100);
	Indicator.displayString("--  "); delay(100);
	Indicator.displayString("--- "); delay(100);
	Indicator.displayString("----"); delay(100);
	Indicator.displayString(" ---"); delay(100);
	Indicator.displayString("  --"); delay(100);
	Indicator.displayString("   -"); delay(100);
	Watchdog.reset();
	
	Indicator.displayOff();
	delay(500);
	Indicator.displayOn();
	
	Watchdog.reset();
}
