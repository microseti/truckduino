#include "Control.h"
#include "Watchdog.h"
#include <HardwareTimer.h>

uint32_t t1_counter = 0;

CControl::CControl() {
}

void CControl::begin() {
	if (!Watchdog.ready()) return;
	// 000: valves-l-r * 8
	// 001: valves-l-r * 8
	// 010: valves-l-r * 2, relay *4
	// 011: inputs * 16 - error valve state
	// 100: inputs, pin (0,1) - error valve (17, 18) state

	expander0.begin(0x20);
	expander1.begin(0x21);
	expander2.begin(0x22);
	expander3.begin(0x23);
	// expander4.begin(0x24);
	for(uint8_t i = 0; i < 16; i++) {
		expander0.pinMode(i, OUTPUT);
		expander0.digitalWrite(i, HIGH);
	}
	for(uint8_t i = 0; i < 16; i++) {
		expander1.pinMode(i, OUTPUT);
		expander1.digitalWrite(i, HIGH);
	}
	for(uint8_t i = 0; i < 8; i++) {
		expander2.pinMode(i, OUTPUT);
		expander2.digitalWrite(i, HIGH);
	}
	for(uint8_t i = 0; i < 16; i++) {
		// expander3.pinMode(i, INPUT);
		// expander3.digitalWrite(i, LOW);
	}
	// for(uint8_t i = 0; i < 16; i++) {
		// expander4.pinMode(i, INPUT);
	// 	expander4.digitalWrite(i, LOW);
	// }

	for(uint8_t i = 0; i < VALVE_MAX; i++) valves[i] = OFF;
	for(uint8_t i = 0; i < 4; i++) relays[i] = OFF;

	for(uint8_t i = 0; i < 16; i++) valves_state[i] = expander3.digitalRead(i) ? NORMAL : FAILURE;
	// for(uint8_t i = 0; i < 2; i++) valves_state[16 + i] = expander4.digitalRead(i) ? NORMAL : FAILURE;

	t1_setup();
}

void CControl::valveLeft(int valve) {
	if (valve < VALVE_MAX) {
		valves[valve] = LEFT;
		uint8_t outp = valve * 2;
		// left on
		if (outp < 16) expander0.digitalWrite(outp, LOW);
		else if (outp < 32) expander1.digitalWrite(outp - 16, LOW);
		else expander2.digitalWrite(outp - 32, LOW);
		// right off
		if (outp < 16) expander0.digitalWrite(outp + 1, HIGH);
		else if (outp < 32) expander1.digitalWrite(outp - 16 + 1, HIGH);
		else expander2.digitalWrite(outp - 32 + 1, HIGH);

	}
}

void CControl::valveRight(int valve) {
	if (valve < VALVE_MAX) {
		valves[valve] = RIGHT;
		uint8_t outp = valve * 2;
		// left off
		if (outp < 16) expander0.digitalWrite(outp, HIGH);
		else if (outp < 32) expander1.digitalWrite(outp - 16, HIGH);
		else expander2.digitalWrite(outp - 32, HIGH);
		// right on
		if (outp < 16) expander0.digitalWrite(outp + 1, LOW);
		else if (outp < 32) expander1.digitalWrite(outp - 16 + 1, LOW);
		else expander2.digitalWrite(outp - 32 + 1, LOW);
	}
}

void CControl::valveLeftIndependentOn(int valve) {
	if (valve < VALVE_MAX) {
		valves[valve] = INDEPENDENT;
		uint8_t outp = valve * 2;
		// left on
		if (outp < 16) expander0.digitalWrite(outp, LOW);
		else if (outp < 32) expander1.digitalWrite(outp - 16, LOW);
		else expander2.digitalWrite(outp - 32, LOW);
	}
}

void CControl::valveLeftIndependentOff(int valve) {
	if (valve < VALVE_MAX) {
		valves[valve] = INDEPENDENT;
		uint8_t outp = valve * 2;
		// left on
		if (outp < 16) expander0.digitalWrite(outp, HIGH);
		else if (outp < 32) expander1.digitalWrite(outp - 16, HIGH);
		else expander2.digitalWrite(outp - 32, HIGH);
	}
}

void CControl::valveRightIndependentOn(int valve) {
	if (valve < VALVE_MAX) {
		valves[valve] = INDEPENDENT;
		uint8_t outp = valve * 2;
		// right on
		if (outp < 16) expander0.digitalWrite(outp + 1, LOW);
		else if (outp < 32) expander1.digitalWrite(outp - 16 + 1, LOW);
		else expander2.digitalWrite(outp - 32 + 1, LOW);
	}
}

void CControl::valveRightIndependentOff(int valve) {
	if (valve < VALVE_MAX) {
		valves[valve] = INDEPENDENT;
		uint8_t outp = valve * 2;
		// right on
		if (outp < 16) expander0.digitalWrite(outp + 1, HIGH);
		else if (outp < 32) expander1.digitalWrite(outp - 16 + 1, HIGH);
		else expander2.digitalWrite(outp - 32 + 1, HIGH);
	}
}

void CControl::valveOff(int valve) {
	if (valve < VALVE_MAX) {
		valves[valve] = OFF;
		uint8_t outp = valve * 2;
		// left and right off
		if (outp < 16) {
			expander0.digitalWrite(outp, HIGH);
			expander0.digitalWrite(outp + 1, HIGH);
		}
		else if (outp < 32) {
			expander1.digitalWrite(outp - 16, HIGH);
			expander1.digitalWrite(outp - 16 + 1, HIGH);
		}
		else {
			expander2.digitalWrite(outp - 32, HIGH);
			expander2.digitalWrite(outp - 32 + 1, HIGH);
		}
	}
}

CtrlState CControl::valveGet(int valve) {
	CtrlState ret_val = OVERRANGE;
	if (valve < VALVE_MAX) {
		ret_val = valves[valve];
	}
	return ret_val;
}

CtrlState CControl::valveStateGet(int valve) {
	CtrlState ret_val = OVERRANGE;
	if (valve < VALVE_MAX) {
		if (valve < 16) valves_state[valve] = expander3.digitalRead(valve) ? NORMAL : FAILURE;
		// else valves_state[valve] = expander4.digitalRead(valve - 16) ? NORMAL : FAILURE;
		ret_val = valves_state[valve];
	}
	return ret_val;
}

void CControl::relayOn(int relay) {
	if (relay < RELAY_MAX) {
		relays[relay] = ON;
		expander2.digitalWrite(4 + relay, LOW);
		// if (relay == 0) expander2.digitalWrite(5, LOW);
		// else if (relay == 1) expander2.digitalWrite(4, LOW);
		// else if (relay == 2) expander2.digitalWrite(7, LOW);
		// else if (relay == 3) expander2.digitalWrite(6, LOW);
	}
}

void CControl::relayOff(int relay) {
	if (relay < RELAY_MAX) {
		relays[relay] = OFF;
		expander2.digitalWrite(4 + relay, HIGH);
		// if (relay == 0) expander2.digitalWrite(5, HIGH);
		// else if (relay == 1) expander2.digitalWrite(4, HIGH);
		// else if (relay == 2) expander2.digitalWrite(7, HIGH);
		// else if (relay == 3) expander2.digitalWrite(6, HIGH);
	}
}

CtrlState CControl::relayGet(int relay) {
	CtrlState ret_val = OVERRANGE;
	if (relay < RELAY_MAX) {
		ret_val = relays[relay];
	}
	return ret_val;
}


void t1_handler(void) {
	// if (int1_flag || digitalRead(CONTROL_INT1) == LOW) {
	// if (digitalRead(CONTROL_INT1) == LOW) {
		// if (int1_flag) int1_flag = false;
		// else int1_flag = true;
		// Serial.println("x");
		// for (uint8_t i = 0; i < 16; i++) Control.valves_state[i] = Control.expander1.digitalRead(i) ? NORMAL : FAILURE;
	// }
	// else if (int2_flag || digitalRead(CONTROL_INT2) == LOW) {
	// if (digitalRead(CONTROL_INT2) == LOW) {
		// if (int2_flag) int2_flag = false;
		// else int2_flag = true;
		// Serial.println("y");
		// for (uint8_t i = 0; i < 2; i++) Control.valves_state[16 + i] = Control.expander3.digitalRead(i + 4) ? NORMAL : FAILURE;
	// }
	// t1_counter++;
}

void CControl::t1_setup(void) {
	// Serial.println("timer1 setup start");
	// Timer1.pause();
	// Timer1.setPeriod(100000);
	// Timer1.setCount(0);
	// Timer1.attachCompare1Interrupt(t1_handler);
	// Timer1.resume();
	// Serial.println("timer1 setup end");
}
