/**
 * @file         Truckduino.h
 * @version      1.0
 * @date         2018.09.07
 * @author       Волошин Алексей
 * @contact      microseti@yandex.ru
 * @contribution ООО "Микросети"
 *
 * @example "01. Модуль индикации. Дисплей"
 * @include display/src/main.cpp

 * @example "02. Модуль индикации. Кнопки"
 * @include buttons/src/main.cpp

 * @example "03. Модуль расширения"
 * @include extension/src/main.cpp

 * @example "04. Модуль аналоговых входов"
 * @include inputs/src/main.cpp

 * @example "05. Модуль Modbus. Мастер и Слейв"
 * @include modbus/src/main.cpp

 * @example "06. Модуль Modbus. Черный ящик"
 * @include modbus-blackbox/src/main.cpp

 * @example "07. Модуль Modbus. Управление"
 * @include modbus-control/src/main.cpp

 * @example "08. Модуль релейных выходов"
 * @include relays/src/main.cpp

 * @example "09. Модуль клапанов"
 * @include valves/src/main.cpp

 * @example "10. Эмулятор системы измерения веса"
 * @include weight-emulator/src/main.cpp

 * @example "11. Пример работы с OneWire"
 * @include onewire/src/main.cpp

 * @example "12. Пример работы с терминалом"
 * @include terminal/src/main.cpp

 * @example "13. Пример работы с прерыванием таймера"
 * @include timer-interrupt/src/main.cpp
 */

#ifndef __TRUCKDUINO_H
#define __TRUCKDUINO_H

#include <Watchdog.h>
#include <Input.h>
#include <Control.h>
#include <Indicator.h>
#include <Extension.h>
#include <Modbus.h>
#include "OneWireSTM.h"

#define ONEWIRE_PIN PC13

HardwareTimer Timer(8);
CWatchdog Watchdog;
CInput Input;
CControl Control;
CIndicator Indicator;
CExtension Ext1(0);
CExtension Ext2(1);
CModbus Modbus1(MODBUS1);
#ifdef STM32_HIGH_DENSITY
CModbus Modbus2(MODBUS2);
CModbus Modbus3(MODBUS3);
#endif

#endif // #ifndef __TRUCKDUINO_H
