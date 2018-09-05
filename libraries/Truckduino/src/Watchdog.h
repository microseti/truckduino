#ifndef __WAITUP_H
#define __WAITUP_H

#include <stdint.h>
#include <Arduino.h>

// Watchdog - системный модуль, используется для реализации вотчдогтаймера.
// Для использования необходимо вызвать Watchdog.reset() в цикле программы, время которого не должно превышать 2 секунды.
// Если цикл занимает время больше 2 секунд, необходимо разбить участки задержек в цикле и выполнить их с использованием функции Watchdog.reset().
// Если указанная функция не вызвана за период менее 2 секунд, то произойдет перезагрузка программы Trackduino.
// Использование Watchdog.reset() обязательно в любой программе.

class CWatchdog
{
	public:
		CWatchdog();
		void begin(void);
		bool ready(void);
		void reset(void);

	private:
		bool ready_flag;
		bool wdt_flag;
};

extern CWatchdog Watchdog;

#endif // #ifndef __WAITUP_H
