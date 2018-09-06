/**
 * @file         Indicator.h
 * @version      1.0
 * @date         2018.08.25
 * @author       Волошин Алексей
 * @contact      microseti@yandex.ru
 * @contribution ООО "Микросети"
 *
 * @description
 *  Truckduino библиотека работы с Модулем индикации, выполняет обработку нажатий кнопок и вывод текстовой информации на дисплей.
 *  Для работы модуля необходимо предварительно создать объект watchdog-таймера Watchdog и запустить его методом begin().
 *  Модуль индикации содержит 3 тактовые кнопки и 4 семисегментных дисплея. Модуль построен на микросхеме индикатора TM1650.
 *
 *  Описание (Datasheet) микросхемы индикатора TM1650:
 *  <ul>
 *    <li><a href='../../Truckduino/info/tm1650-rus.pdf'>на русском языке</a>
 *    <li><a href='../../Truckduino/info/tm1650-rus.pdf'>на китайском языке (оригинал)</a>
 *  </ul>
 * 
 * @defgroup indic_setup    Модуль индикации. Инициализация
 * @defgroup indic_display  Модуль индикации. Дисплей
 * @defgroup indic_button   Модуль индикации. Кнопки
 *
 */


#ifndef __INDICATOR_H
#define __INDICATOR_H

#include <TM1650.h>

/**
 * @enum Button
 * @brief
 * Перечисление "Кнопка" служит для указания используемой кнопки на Модуле индикации.
 */
enum Button {
	NONE, //!< Ничего.
	K1, //!< Кнопка 1.
	K2, //!< Кнопка 2.
	K3 //!< Кнопка 3.
};

/**
 * @enum Display
 * @brief
 * Перечисление "Дисплей" служит для указания используемого семисегментного индикатора на Модуле индикации.
 */
enum Display {
	N1, //!< Дисплей 1.
	N2, //!< Дисплей 2.
	N3, //!< Дисплей 3.
	N4 //!< Дисплей 4.
};

#define DEFAULT	3 //!< Уровень яркости дисплея по умолчанию.
#define LEVEL1	1 //!< Уровень 1 - для задания яркости дисплея (минимальная яркость).
#define LEVEL2	2 //!< Уровень 2 - для задания яркости дисплея.
#define LEVEL3	3 //!< Уровень 3 - для задания яркости дисплея.
#define LEVEL4	4 //!< Уровень 4 - для задания яркости дисплея.
#define LEVEL5	5 //!< Уровень 5 - для задания яркости дисплея.
#define LEVEL6	6 //!< Уровень 6 - для задания яркости дисплея.
#define LEVEL7	7 //!< Уровень 7 - для задания яркости дисплея (максимальная яркость).

/**
 * @class CIndicator
 * @brief
 * Класс Truckduino для работы с Модулем индикации.
 */
class CIndicator
{
	public:
		CIndicator(); //!< Конструктор класса
		void begin(int brightness = DEFAULT); //!< Инициализация Модуля индикации.
		void displayOn(); //!< Включение дисплея.
		void displayOff(); //!< Выключение дисплея.
		void displayTest(); //!< Запуск теста дисплея.
		void displayClear(); //!< Очиcтка дисплея.
		void displayString(const char *string); //!< Вывод строки на дисплей.
		void displayDigit(Display display, uint8_t digit, boolean dot); //!< Вывод числа с указанием дисплея и необходимости вывода точки.
		void displayDigit(int16_t digit); //!< Вывод числа на дисплей.
		void displayDigit(float digit, uint8_t dplaces); //!< Вывод числа типа float с указанием точности.
		void displayDot(Display display, boolean dot); //!< Управление точкой указанного дисплея.
		void displayBrightness(int brightness); //!< Управление яркостью дисплея.
		boolean buttonPress(Button button); //!< Определение нажатия указанной кнопки (K1, K2 или K3).
		Button buttonPress(); //!< Определение нажатой кнопки, возвращает K1, K2, K3 или NONE в случае, если не нажата ни одна кнопка.
	private:
		TM1650 tm1650; //!< Объект класса драйвера микросхемы TM1650
		char xtoa_buf[15]; //!< Буфер для преобразования числа в строку.
		int char_offset(uint8_t digit); //!< Расчет сдвига индикации для заданного отображаемого числа.
		char *___itoa (int i); //!< Преобразование числа типа integer в строку.
		char *___ftoa (float f, int dplaces); //!< Преобразование числа типа float в строку.
};

extern CIndicator Indicator; //!< Объявление объекта Indicator, используется для работы с индикатором.

#endif // __INDICATOR_H
