#ifndef __MODBUS_H
#define __MODBUS_H

#include <ModbusRtu.h>
#include <vector>

// Hardware timer 5 need add in cores/maple/HardwareTimer.h

#define MODBUS_NONE                     0   /*!< null operator */
#define MODBUS_READ_COILS               1	/*!< FCT=1 -> read coils or digital outputs */
#define MODBUS_READ_DISCRETE_INPUT      2	/*!< FCT=2 -> read digital inputs */
#define MODBUS_READ_REGISTERS           3	/*!< FCT=3 -> read registers or analog outputs */
#define MODBUS_READ_INPUT_REGISTER      4	/*!< FCT=4 -> read analog inputs */
#define MODBUS_WRITE_COIL               5	/*!< FCT=5 -> write single coil or output */
#define MODBUS_WRITE_REGISTER           6	/*!< FCT=6 -> write single register */
#define MODBUS_WRITE_MULTIPLE_COILS     15	/*!< FCT=15 -> write multiple coils or outputs */
#define MODBUS_WRITE_MULTIPLE_REGISTERS 16	/*!< FCT=16 -> write multiple registers */

enum ModbusChannel {
#ifdef STM32_HIGH_DENSITY
	MODBUS1 = MODBUS_SERIAL3, // UART4
	MODBUS2 = MODBUS_SERIAL4, // UART5
	MODBUS3 = MODBUS_SERIAL1  // USART2
#else
	MODBUS1 = MODBUS_SERIAL1
#endif
};

enum ModbusRole {
	SLAVE,
	MASTER
};

enum ModbusStatus {
	STA_IDLE = 0,
	STA_INIT,
	STA_CONFIGURE,
	STA_PAUSE,
	STA_RUN
};

class CModbus: public ModbusRtu
{
	public:
		CModbus(ModbusChannel channel) : ModbusRtu()
		{
			c_txen_pin = PB5;
			if (channel == MODBUS1) {
				c_txen_pin = PB5;
			}
#ifdef STM32_HIGH_DENSITY
			else if (channel == MODBUS2) {
				c_txen_pin = PB2;
			}
			else if (channel == MODBUS3) {
				c_txen_pin = PB3;
			}
#endif
			c_channel = channel;
			c_sta = STA_IDLE;
		}
		void begin(ModbusRole role);
		void begin(ModbusRole role, uint8_t modbus_id);
		void polla();
		void masterQueryClear(void);
		void masterQueryAdd(uint8_t slave_id, uint8_t func, uint16_t start_addr, uint16_t *data, uint8_t count);
		void slaveConfigure(uint16_t *data, uint8_t count);
		void start(uint32_t boudrate, uint16_t period_ms);
		void start(uint32_t boudrate);
		bool busy();
		void resume(void);
		void pause(void);
		std::vector<modbus_t> c_telegram;
		uint16_t *c_data;
		uint8_t c_data_count;
		uint8_t c_channel;
		uint8_t c_role;
		uint16_t c_period_ms;
		ModbusStatus c_sta;
	private:
		uint32_t c_boudrate;
		uint16_t c_ident;
		uint16_t c_txen_pin;
};

extern CModbus Modbus1;
#ifdef STM32_HIGH_DENSITY
extern CModbus Modbus2;
extern CModbus Modbus3;
#endif

#endif // #ifndef __MODBUS_H
