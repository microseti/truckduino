#include "Truckduino.h"

//
// все регистры 16-битные
//                        r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r
//                        0 1 2 3 4 5 6 7 8 9 101112131415161718192021222324252627282930313233343536373839404142434445464748495051
uint16_t data_slave[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
//                        ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
//                        | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                        | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                        | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                        | | | | +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                        | | | | |
//                        | | | | +-----  Registers 0 - 47
//                        | | | +-----  Temperature
//                        | | +-----  Current
//                        | +-----  Internal Power
//                        +-----  External Power
//

OneWire ds(ONEWIRE_PIN);
float temperature = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Hello from modbus-bbox test program!");

	Modbus2.begin(SLAVE, 1);
	Modbus2.slaveConfigure(data_slave, 52);
	Modbus2.start(115200);

	for (uint8_t i = 0; i < 52; i++) data_slave[i] = 0;

	Input.init();
}

uint32_t counter = 0;

void loop () {
	data_slave[0] = 1234;	// External Power
	data_slave[1] = 2222;	// Internal Power
	data_slave[2] = 3333;	// Current
	data_slave[3] = (uint16_t) (100 * temperature);	// Temperature
	data_slave[4] = (uint16_t) (100 * Input.voltageRead(INPUT1));		// Inpit voltage
	data_slave[5] = 1;		// Reg1
	data_slave[6] = 2;		// Reg2
	data_slave[7] = 3;		// Reg3
	data_slave[8] = 4;		// Reg4
	data_slave[9] = 5;		// Reg5
	data_slave[10] = 6;		// Reg6
	data_slave[11] = 7;		// Reg7
	data_slave[12] = 8;		// Reg8
	data_slave[13] = 9;		// Reg9
	data_slave[14] = 10;	// Reg10
	data_slave[15] = 0;		// Reg11
	data_slave[16] = 0;		// Reg12
	data_slave[17] = 0;		// Reg13
	data_slave[18] = 0;		// Reg14
	data_slave[19] = 0;		// Reg15
	data_slave[20] = 0;		// Reg16
	data_slave[21] = 0;		// Reg17
	data_slave[22] = 0;		// Reg18
	data_slave[23] = 0;		// Reg19
	data_slave[24] = 20;	// Reg20
	data_slave[25] = 0;		// Reg21
	data_slave[26] = 0;		// Reg22
	data_slave[27] = 0;		// Reg23
	data_slave[28] = 0;		// Reg24
	data_slave[29] = 0;		// Reg25
	data_slave[30] = 0;		// Reg26
	data_slave[31] = 0;		// Reg27
	data_slave[32] = 0;		// Reg28
	data_slave[33] = 0;		// Reg29
	data_slave[34] = 30;	// Reg30
	data_slave[35] = 0;		// Reg31
	data_slave[36] = 0;		// Reg32
	data_slave[37] = 0;		// Reg33
	data_slave[38] = 0;		// Reg34
	data_slave[39] = 0;		// Reg35
	data_slave[40] = 0;		// Reg36
	data_slave[41] = 0;		// Reg37
	data_slave[42] = 0;		// Reg38
	data_slave[43] = 0;		// Reg39
	data_slave[44] = 32767;	// Reg40
	data_slave[45] = 0;		// Reg41
	data_slave[46] = 0;		// Reg42
	data_slave[47] = 0;		// Reg43
	data_slave[48] = 0;		// Reg44
	data_slave[49] = 0;		// Reg45
	data_slave[50] = 0;		// Reg46
	data_slave[51] = 65535;	// Reg47

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
				  // default is 12 bit resolution, 750 ms conversion time
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
