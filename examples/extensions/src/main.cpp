#include "Truckduino.h"

bool pio_flag;

void setup() {
	Serial.begin(9600);
	Serial.println("Extension modules test program.");

	// Инициализация модулей расширения 1 и 2
	Ext1.begin();
	Ext2.begin();

	// Настройка счетчиков на частоту 1Гц
	Ext1.counterConfig(1);
	Ext2.counterConfig(1);

	// Настройка портов ввода-вывода на выход
	Ext1.pioConfig(PIO1, OUTPUT);
	Ext1.pioConfig(PIO2, OUTPUT);
	Ext2.pioConfig(PIO1, OUTPUT);
	Ext2.pioConfig(PIO2, OUTPUT);

	// Выходы широтно-импульсной модуляции (PWM)
	Ext1.pwmWrite(PWM1, 50);
	Ext1.pwmWrite(PWM2, 100);
	Ext2.pwmWrite(PWM1, 150);
	Ext2.pwmWrite(PWM2, 200);

	// Последовательный порт USART
	Ext1.serial->begin(9600);
	Ext1.serial->println("Extension modules test program.");

	// Последовательный порт I2C (Возможно использовать только одно: USART или I2C)
	// Ext1.i2c->beginTransmission(0x22);
	// Ext1.i2c->write(0xFF);
	// Ext1.i2c->endTransmission();

	pio_flag = false;
	delay(500);
	Watchdog.reset();
}

void loop() {
	if (pio_flag) {
		pio_flag = false;
		Ext1.pioWrite(PIO1, HIGH);
		Ext1.pioWrite(PIO2, HIGH);
		Ext2.pioWrite(PIO1, HIGH);
		Ext2.pioWrite(PIO2, HIGH);
		Ext1.pwmWrite(PWM1, 50);
	}
	else {
		pio_flag = true;
		Ext1.pioWrite(PIO1, LOW);
		Ext1.pioWrite(PIO2, LOW);
		Ext2.pioWrite(PIO1, LOW);
		Ext2.pioWrite(PIO2, LOW);
		Ext1.pwmWrite(PWM1, 200);
	}

	Ext1.serial->println("");

	Serial.print("Counter: ");
	Serial.print(Ext1.counterCountGet(COUNTER1));Serial.print(" ");
	Serial.print(Ext1.counterCountGet(COUNTER2));Serial.print(" ");
	Serial.print(Ext2.counterCountGet(COUNTER1));Serial.print(" ");
	Serial.print(Ext2.counterCountGet(COUNTER2));Serial.print(" ");
	Serial.print("| Freq: ");
	Serial.print(Ext1.counterFreqGet(COUNTER1));Serial.print(" ");
	Serial.print(Ext1.counterFreqGet(COUNTER2));Serial.print(" ");
	Serial.print(Ext2.counterFreqGet(COUNTER1));Serial.print(" ");
	Serial.print(Ext2.counterFreqGet(COUNTER2));Serial.print(" ");
	Serial.print("| Adc: ");
	Serial.print(Ext1.adcRead());Serial.print(" ");
	Serial.print(Ext2.adcRead());Serial.print(" ");
	Serial.println("");

	Ext1.serial->println("Extension modules test program.");

	delay(500);
	Watchdog.reset();
}
