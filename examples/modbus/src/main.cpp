#include "Truckduino.h"

// Для теста необходимо соединить клеммы A1 с A2, B1 с B2 перемычками.
// Слэйв записывает в свой буфер по адресу 1 число, которое можно изменить кнопками K1 и K3
// Мастер запрашивает (функция 03) у Слэйва регистры с адресами 0, 1, 2
// Мастер из своего буфера по адресу 2 берет число и выводит его на индикатор

uint16_t data_slave[3] = { 1000, 1000, 1000 };
uint16_t data_master[3] = { 0, 0, 0 };
bool key1 = false, key2 = false, key3 = false;

void setup() {
    Serial.begin(9600);
    Serial.println("Hello from modbus test program!");

	CModbus *master = &Modbus2;
	CModbus *slave = &Modbus1;

	slave->begin(SLAVE, 1);
	slave->slaveConfigure(data_slave, 3);
	slave->start(115200);
	//
	master->begin(MASTER);
	master->masterQueryClear();
	master->masterQueryAdd(1, MODBUS_READ_REGISTERS, 0, data_master, 3);	// read
	// master->masterQueryAdd(1, MODBUS_WRITE_REGISTER, 4, mdata + 4, 1);	// write example
	// master->masterQueryAdd(1, MODBUS_WRITE_REGISTER, 5, mdata + 5, 1);	// write example
	master->start(115200, 50);

	Indicator.begin();
	Indicator.displayTest();
}

void loop () {
	Indicator.displayDigit(data_master[1]);

	if (Indicator.buttonPress(K1)) {
		if (!key1) {
			key1 = true;
			if (data_slave[1] > 0) {
				if (data_slave[1] > 1100 || data_slave[1] <= 900) data_slave[1] -= 100;
				else data_slave[1] -= 10;
			}
			Indicator.displayString("b1  ");
		}
	}
	else key1 = false;

	if (Indicator.buttonPress(K2)) {
		if (!key2) {
			key2 = true;
			data_slave[1] = 1000;
			Indicator.displayString("b2  ");
		}
	}
	else key2 = false;

	if (Indicator.buttonPress(K3)) {
		if (!key3) {
			key3 = true;
			if (data_slave[1] < 9900) {
				if (data_slave[1] >= 1100 || data_slave[1] < 900) data_slave[1] += 100;
				else data_slave[1] += 10;
			}
			Indicator.displayString("b3  ");
		}
	}
	else key3 = false;

	delay(200);
	Watchdog.reset();
}
