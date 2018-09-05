#include "Modbus.h"
#include "Watchdog.h"

// Hardware timer 5 need add in HardwareTimer.h

uint32_t m1_counter = 0;
uint32_t m1_tindex = 0;

void m1_handler(void) {
	if (Modbus1.c_role == MASTER) {
		if (m1_counter % Modbus1.c_period_ms == 0) {
			if (Modbus1.c_telegram.size() > 0) {
				if (!Modbus1.busy()) {
					Modbus1.query(Modbus1.c_telegram[m1_tindex]);
					m1_tindex++;
					if (m1_tindex == Modbus1.c_telegram.size()) m1_tindex = 0;
				}
			}
		}
		Modbus1.poll();
	}
	else {
		Modbus1.poll(Modbus1.c_data, Modbus1.c_data_count);
	}
	m1_counter++;
}

#ifdef STM32_HIGH_DENSITY
uint32_t m2_counter = 0;
uint32_t m2_tindex = 0;

uint32_t m3_counter = 0;
uint32_t m3_tindex = 0;

void m2_handler(void) {
	if (Modbus2.c_role == MASTER) {
		if (m2_counter % Modbus2.c_period_ms == 0) {
			if (Modbus2.c_telegram.size() > 0) {
				if (!Modbus2.busy()) {
					Modbus2.query(Modbus2.c_telegram[m2_tindex]);
					m2_tindex++;
					if (m2_tindex == Modbus2.c_telegram.size()) m2_tindex = 0;
				}
			}
		}
		Modbus2.poll();
	}
	else {
		Modbus2.poll(Modbus2.c_data, Modbus2.c_data_count);
	}
	m2_counter++;
}

void m3_handler(void) {
	if (Modbus3.c_role == MASTER) {
		if (m3_counter % Modbus3.c_period_ms == 0) {
			if (Modbus3.c_telegram.size() > 0) {
				if (!Modbus3.busy()) {
					Modbus3.query(Modbus3.c_telegram[m3_tindex]);
					m3_tindex++;
					if (m3_tindex == Modbus3.c_telegram.size()) m3_tindex = 0;
				}
			}
		}
		Modbus3.poll();
	}
	else {
		Modbus3.poll(Modbus3.c_data, Modbus3.c_data_count);
	}
	m3_counter++;
}
#endif

/// Modbus class implementation

void CModbus::begin(ModbusRole role, uint8_t modbus_id) {
	if (!Watchdog.ready()) return;
	if (c_sta > STA_IDLE) return;
	c_role = role;
	c_ident = 0;
	if (role == SLAVE) {
		c_ident = modbus_id;
		if (c_ident == 0) c_ident = 1;
	}
	c_sta = STA_INIT;
	ModbusRtu::init(c_ident, c_channel, c_txen_pin);
}

void CModbus::begin(ModbusRole role) {
	if (!Watchdog.ready()) return;
	if (c_sta > STA_IDLE) return;
	c_role = role;
	c_ident = 0;
	if (role == SLAVE) c_ident = 1;
	c_sta = STA_INIT;
	ModbusRtu::init(c_ident, c_channel, c_txen_pin);
}

void CModbus::start(uint32_t boudrate, uint16_t period_ms) {
	if (c_sta < STA_CONFIGURE) return;
	c_boudrate = boudrate;
	c_period_ms = period_ms;
	ModbusRtu::begin(c_boudrate);

	if (c_channel == MODBUS1) {
		Timer4.pause();
		Timer4.setPeriod(1000);
		Timer4.attachCompare1Interrupt(m1_handler);
		Timer4.resume();
	}
#ifdef STM32_HIGH_DENSITY
	else if (c_channel == MODBUS2) {
		Timer5.pause();
		Timer5.setPeriod(1000);
		Timer5.attachCompare1Interrupt(m2_handler);
		Timer5.resume();
	}
	else if (c_channel == MODBUS3) {
		Timer1.pause();
		Timer1.setPeriod(1000);
		Timer1.attachCompare1Interrupt(m3_handler);
		Timer1.resume();
	}
#endif
	c_sta = STA_RUN;
}

void CModbus::start(uint32_t boudrate) {
	start(boudrate, 0);
}

bool CModbus::busy() {
	return getState() != COM_IDLE;
}

void CModbus::resume(void) {
	if (c_sta != STA_PAUSE) return;
	if (c_channel == MODBUS1) {
		Timer4.resume();
	}
#ifdef STM32_HIGH_DENSITY
	else if (c_channel == MODBUS2) {
		Timer5.resume();
	}
	else if (c_channel == MODBUS3) {
		Timer1.resume();
	}
#endif
}

void CModbus::pause(void) {
	if (c_sta != STA_RUN) return;
	if (c_channel == MODBUS1) {
		Timer4.pause();
	}
#ifdef STM32_HIGH_DENSITY
	else if (c_channel == MODBUS2) {
		Timer5.pause();
	}
	else if (c_channel == MODBUS3) {
		Timer1.pause();
	}
#endif
}

void CModbus::masterQueryClear(void) {
	c_telegram.erase(c_telegram.begin(), c_telegram.end());
}

void CModbus::masterQueryAdd(uint8_t slave_id, uint8_t func, uint16_t start_addr, uint16_t *data, uint8_t count) {
	if (c_sta < STA_INIT) return;
	modbus_t t;
	t.u8id = slave_id;
	t.u8fct = func;
	t.u16RegAdd = start_addr;
	t.u16CoilsNo = count;
	t.au16reg = data;
	c_telegram.push_back(t);
	c_sta = STA_CONFIGURE;
}

void CModbus::slaveConfigure(uint16_t *data, uint8_t count) {
	if (c_sta < STA_INIT) return;
	c_data = data;
	c_data_count = count;
	c_sta = STA_CONFIGURE;
}
