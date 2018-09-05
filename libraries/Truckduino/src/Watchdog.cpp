#include "Watchdog.h"

#define WATCHDOG_PIN	PB15
#define WATCHDOG_INIT	pinMode(WATCHDOG_PIN, OUTPUT)
#define WATCHDOG_HI		digitalWrite(WATCHDOG_PIN, HIGH)
#define WATCHDOG_LO		digitalWrite(WATCHDOG_PIN, LOW)


CWatchdog::CWatchdog() {
	Watchdog.begin();
	wdt_flag = false;
}

void CWatchdog::begin(void) {
	AFIO_BASE->MAPR |= AFIO_MAPR_SWJ_CFG_NO_JTAG_NO_SW;
	WATCHDOG_INIT;
	ready_flag = true;
}

bool CWatchdog::ready(void) {
	return ready_flag;
}

void CWatchdog::reset(void) {
	if (ready()) {
		if (wdt_flag) {
			wdt_flag = false;
			WATCHDOG_HI;
		}
		else {
			wdt_flag = true;
			WATCHDOG_LO;
		}
	}
}
