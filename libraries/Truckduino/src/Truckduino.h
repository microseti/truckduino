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
