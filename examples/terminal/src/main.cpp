#include "Truckduino.h"

void setup() {
	Serial.begin(9600);
	Serial.println("Hello from terminal test program.");
    Serial.println("");
    Serial.println("#######  Программа суммирования двух чисел  #######");
    Serial.println("Введите положительное или отрицательное число и нажмите Enter");
    Serial.println("");
}

int a, b;
char buffer[10];
int index;

void loop() {
    while (Serial.available()) Serial.read();

    Serial.print("Первое число: ");
    while (!Serial.available()) {
		delay(100);
		Watchdog.reset();
	}
    index = 0;
	while (true) {
        if (Serial.available()) {
			buffer[index] = Serial.read();
			if (buffer[index] == '\r' || buffer[index] == '\n') {
				buffer[index] = '\0';
				Serial.println("");
				break;
			}
			Serial.print(buffer[index]);
			index++;
        	if (index == 6) {
				Serial.println("");
				break;
			}
		}
    }
    a = atoi(buffer);

	delay(100);
    while (Serial.available()) Serial.read();

    Serial.print("Второе число: ");
	while (!Serial.available()) {
		delay(100);
		Watchdog.reset();
	}
    index = 0;
    while (true) {
        if (Serial.available()) {
			buffer[index] = Serial.read();
			if (buffer[index] == '\r' || buffer[index] == '\n') {
				buffer[index] = '\0';
				Serial.println("");
				break;
			}
			Serial.print(buffer[index]);
			index++;
			if (index == 6) {
				Serial.println("");
				break;
			}
		}
    }
    b = atoi(buffer);

    Serial.print("Результат: (");
    Serial.print(a);
    Serial.print(") + (");
    Serial.print(b);
    Serial.print(") = ");
    Serial.print(a + b);
    Serial.println("");
    Serial.println("");
	delay(100);
	Watchdog.reset();
}
