#include "Truckduino.h"

#ifndef BV
	#define BV(bit)			(1<<(bit))
#endif
#ifndef cbi
	#define cbi(reg,bit)	reg &= ~(BV(bit))
#endif
#ifndef sbi
	#define sbi(reg,bit)	reg |= (BV(bit))
#endif

//
// все регистры 16-битные
//                        w w w w r     r     r r r r r r r r r r r r r r r r r r
//                        0 1 2 3 4     5     6 7 8 9 1011121314151617181920212223
uint16_t data_slave[] = { 0,0,0,0,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
//                        ^ ^ ^ ^ ^     ^     ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
//                        | | | | |     |     | | | | | | | | | | | | | | | | | +-----  Temperature DS18x20
//                        | | | | |     |     | | | | | | | | | | | | | | | | +-----  Discrete Input [1-8]
//                        | | | | |     |     | | | | | | | | +-+-+-+-+-+-+-+
//                        | | | | |     |     +-+-+-+-+-+-+-+               +---  Analog Input Current [1-8]
//                        | | | | |     |                   +---------  Analog Input Voltage [1-8]
//                        | | | | |     +---------------------------  Valve[17-18] status and Keys register
//                        | | | | +-------------------------------  Valve[1-16] status register
//                        | | | +-------------------------------  Relay control register
//                        | | +-------------------------------  Valve[17-18] control register
//                        | +-------------------------------  Valve[9-16] control register
//                        +-------------------------------  Valve[1-8] control register
//
// 0 - Valve[1-8] control register:
//   bit0              bit1              bit2              bit3              bit4
// +-----------------+-----------------+-----------------+-----------------+-----
// | VALVE L1 ON/OFF | VALVE R1 ON/OFF | VALVE L2 ON/OFF | VALVE R2 ON/OFF | ...
// +-----------------+-----------------+-----------------+-----------------+-----
//
// 1 - Valve[9-16] control register:
//   bit0              bit1              bit2               bit3               bit4
// +-----------------+-----------------+------------------+------------------+-----
// | VALVE L9 ON/OFF | VALVE R9 ON/OFF | VALVE L10 ON/OFF | VALVE R10 ON/OFF | ...
// +-----------------+-----------------+------------------+------------------+-----
//
// 2 - Valve[17-18] control register:
//   bit0               bit1               bit2               bit3
// +------------------+------------------+------------------+------------------+
// | VALVE L17 ON/OFF | VALVE R17 ON/OFF | VALVE L18 ON/OFF | VALVE R18 ON/OFF |
// +------------------+------------------+------------------+------------------+
//
// 3 - Relay control register:
//   bit0            bit1            bit2            bit3
// +---------------+---------------+---------------+-----
// | RELAY1 ON/OFF | RELAY2 ON/OFF | RELAY3 ON/OFF | ...
// +---------------+---------------+---------------+-----
//
// 4 - Valve[1-16] status register:
//   bit0            bit1            bit2            bit3            bit4
// +---------------+---------------+---------------+---------------+-----
// | VALVE1 STATUS | VALVE2 STATUS | VALVE3 STATUS | VALVE4 STATUS | ...
// +---------------+---------------+---------------+---------------+-----
//
// 5 - Valve[17-18] status and Keys register:
//   bit0             bit1             bit2          bit3          bit4
// +----------------+----------------+-------------+-------------+-------------+
// | VALVE17 STATUS | VALVE18 STATUS | KEY1 ON/OFF | KEY2 ON/OFF | KEY3 ON/OFF |
// +----------------+----------------+-------------+-------------+-------------+
//
// 6-13 - Analog Input Voltage [1-8]:
//   byte0  byte1  byte2  byte3  byte4  byte5  byte6  byte7
// +------+------+------+------+------+------+------+------+
// |  U1  |  U2  |  U3  |  U4  |  U5  |  U6  |  U7  |  U8  |
// +------+------+------+------+------+------+------+------+
//
// 14-21 - Analog Input Current [1-8]:
//   byte0  byte1  byte2  byte3  byte4  byte5  byte6  byte7
// +------+------+------+------+------+------+------+------+
// |  I1  |  I2  |  I3  |  I4  |  I5  |  I6  |  I7  |  I8  |
// +------+------+------+------+------+------+------+------+
//
// 22 - Discrete Input [1-8]:
//   bit0   bit1   bit2   bit3   bit4   bit5   bit6   bit7
// +------+------+------+------+------+------+------+------+
// |  D1  |  D2  |  D3  |  D4  |  D5  |  D6  |  D7  |  D8  |
// +------+------+------+------+------+------+------+------+
//
// 23 - Temperature DS18x20:
//   byte0
// +----------------------------------+
// |  1WIRE TEMPERATURE FROM DS18x20  |
// +----------------------------------+
//

OneWire ds(ONEWIRE_PIN);

bool key1 = false, key2 = false, key3 = false;
float temperature = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("Hello from modbus-control test program!");

	Indicator.begin();
	Indicator.displayTest();

	Control.begin();

	Input.begin();
	for (int i = 0; i < 8; i++) Input.filterConfig(i, 4);

	Modbus1.begin(SLAVE, 2);
	Modbus1.slaveConfigure(data_slave, 24);
	Modbus1.start(115200);

	// Modbus2.begin(SLAVE, 2);
	// Modbus2.slaveConfigure(data_slave, 24);
	// Modbus2.start(115200);

	// Modbus3.begin(SLAVE, 2);
	// Modbus3.slaveConfigure(data_slave, 24);
	// Modbus3.start(115200);

	Indicator.begin();
	Indicator.displayString("S2A2");
}

void loop () {
	for (int i = 0; i < 8; i++) {
		if ((data_slave[0] >> i*2) & 1) Control.valveLeftIndependentOn(i);
		else Control.valveLeftIndependentOff(i);
		if ((data_slave[0] >> (i*2 + 1)) & 1) Control.valveRightIndependentOn(i);
		else Control.valveRightIndependentOff(i);
	}
	for (int i = 0; i < 8; i++) {
		if ((data_slave[1] >> i*2) & 1) Control.valveLeftIndependentOn(8+i);
		else Control.valveLeftIndependentOff(8+i);
		if ((data_slave[1] >> (i*2 + 1)) & 1) Control.valveRightIndependentOn(8+i);
		else Control.valveRightIndependentOff(8+i);
	}
	for (int i = 0; i < 2; i++) {
		if ((data_slave[2] >> i*2) & 1) Control.valveLeftIndependentOn(16+i);
		else Control.valveLeftIndependentOff(16+i);
		if ((data_slave[2] >> (i*2 + 1)) & 1) Control.valveRightIndependentOn(16+i);
		else Control.valveRightIndependentOff(16+i);
	}
	for (int i = 0; i < 4; i++) {
		if ((data_slave[3] >> i) & 1) Control.relayOn(i);
		else Control.relayOff(i);
	}

	for (int i = 0; i < 16; i++) {
		if (Control.valveStateGet(i) == NORMAL) sbi(data_slave[4], i);
		else cbi(data_slave[4], i);
	}
	// for (int i = 0; i < 2; i++) {
	// 	if (Control.valveStateGet(16+i) == NORMAL) sbi(data_slave[5], i);
	// 	else cbi(data_slave[5], i);
	// }

	if (Indicator.buttonPress(K1)) {
		if (!key1) {
			key1 = true;
			sbi(data_slave[5], 2);
		}
	}
	else {
		key1 = false;
		cbi(data_slave[5], 2);
	}
	if (Indicator.buttonPress(K2)) {
		if (!key2) {
			key2 = true;
			sbi(data_slave[5], 3);
		}
	}
	else {
		key2 = false;
		cbi(data_slave[5], 3);
	}
	if (Indicator.buttonPress(K3)) {
		if (!key3) {
			key3 = true;
			sbi(data_slave[5], 4);
		}
	}
	else {
		key3 = false;
		cbi(data_slave[5], 4);
	}

	for (int i = 0; i < 8; i++) {
		data_slave[6+i] = (uint16_t) (100 * Input.voltageRead(i));
	}
	for (int i = 0; i < 8; i++) {
		data_slave[14+i] = (uint16_t) (100 * Input.currentRead(i));
	}
	for (int i = 0; i < 8; i++) {
		if (Input.discreteRead(i)) sbi(data_slave[22], i);
		else cbi(data_slave[22], i);
	}

	// temperature = 1.23;
	data_slave[23] = (uint16_t) (100 * temperature);

	temperature_process();

	delay(20);
	Watchdog.reset();
}

uint32_t temperature_process_timer = 0;
bool search_complete = false;
bool conversion_flag = false;
uint32_t conversion_time = 0;
byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];

void temperature_process() {
	temperature_process_timer++;
	if (temperature_process_timer % 10 == 0) {
		// 1 time in second
		if (search_complete) {
			if (conversion_flag) {
				conversion_flag = false;

				present = ds.reset();
				ds.select(addr);
				ds.write(0xBE);         // Read Scratchpad

				for ( i = 0; i < 9; i++) {           // we need 9 bytes
				  data[i] = ds.read();
				}

				int16_t raw = (data[1] << 8) | data[0];
				if (type_s) {
				  raw = raw << 3; // 9 bit resolution default
				  if (data[7] == 0x10) {
					// "count remain" gives full 12 bit resolution
					raw = (raw & 0xFFF0) + 12 - data[6];
				  }
				} else {
				  byte cfg = (data[4] & 0x60);
				  // at lower res, the low bits are undefined, so let's zero them
				  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
				  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
				  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
				  //// default is 12 bit resolution, 750 ms conversion time
				}
				temperature = (float)raw / 16.0;
			}
			else {
				conversion_flag = true;
				ds.reset();
				ds.select(addr);
				ds.write(0x44, 1);        // start conversion, with parasite power on at the end
			}
		}
		else {
			// search devices
			if (!ds.search(addr)) {
			  ds.reset_search();
			  return;
			}
			if (OneWire::crc8(addr, 7) != addr[7]) {
				return;
			}
			switch (addr[0]) {
			  case 0x10:
				// Chip = DS18S20
				type_s = 1;
				break;
			  case 0x28:
				// Chip = DS18B20
				type_s = 0;
				break;
			  case 0x22:
				// Chip = DS1822
				type_s = 0;
				break;
			  default:
				// Device is not a DS18x20 family device.
				return;
			}
			search_complete = true;
			conversion_flag = false;
		}
	}
}
