#include "Indicator.h"
#include "Watchdog.h"

CIndicator::CIndicator() {
}

/**
 * @brief
 * Инициализация Модуля индикации.
 *
 * Устанавливается яркость дисплея.
 * Если не указывать параметр яркости, устанавливается яркость по-умолчанию равная DEFAULT. 
 *
 * @param brightness   значение яркости дисплея 1..7
 * @ingroup indic_setup
 */
void CIndicator::begin(int brightness) {
	if (!Watchdog.ready()) return;
	if (brightness == 0) brightness = 1;
	tm1650 = TM1650();
	Wire.begin();
	tm1650.init();
	tm1650.setBrightness(brightness);
}

/**
 * @brief Включение дисплея.
 * 
 * @ingroup indic_display
 */
void CIndicator::displayOn() {
	tm1650.displayOn();
}

/**
 * @brief Выключение дисплея.
 * 
 * @ingroup indic_display
 */
void CIndicator::displayOff() {
	tm1650.displayOff();
}

/**
 * @brief Запуск теста дисплея.
 * 
 * Зажигание всех сегментов дисплея на 1 секунду.
 * 
 * @ingroup indic_display
 */
void CIndicator::displayTest() {
	Watchdog.reset();
	tm1650.displayString((char*)"8888");
	tm1650.setDot(N1, true);
	tm1650.setDot(N2, true);
	tm1650.setDot(N3, true);
	tm1650.setDot(N4, true);
	delay(1000);
	tm1650.displayString((char*)"    ");
	tm1650.setDot(N1, false);
	tm1650.setDot(N2, false);
	tm1650.setDot(N3, false);
	tm1650.setDot(N4, false);
	Watchdog.reset();
}

/**
 * @brief Очиcтка дисплея.
 * 
 * Отключает все сегменты.
 * 
 * @ingroup indic_display
 */
void CIndicator::displayClear() {
	tm1650.clear();
}

/**
 * @brief Вывод строки на дисплей.
 * 
 * @param str   указатель на строку, которую нужно вывести.
 * 
 * Например:
 * @code
 *   Indicator.displayString("HELP");
 * @endcode
 * или
 * @code
 *   const char *str_help = "HELP";
 *   Indicator.displayString(str_help);
 * @endcode
 * @ingroup indic_display
 */
void CIndicator::displayString(const char *str) {
	byte slen = 0;
	bool dot;
	char ch;
	char *s = (char *)str;
	while (*s && slen < 4) {
		if (slen > 0 && *s == '.' && *(s-1) != '.') slen--;
		slen++;
		s++;
	}
	s--;
 	for (uint8_t i = 0; i < 4; i++) {
		if (i < slen) {
			dot = false;
			if (*s == '.') {
				dot = true;
				s--;
			}
			if (*s == '.') ch = ' ';
			else ch = *s--;
			tm1650.displayChar(3-i, ch, false);
			if (dot) tm1650.setDot(3-i, true);
		}
		else tm1650.displayChar(3-i, ' ', false);
	}
}

/**
 * @brief Вывод числа с указанием дисплея и необходимости вывода точки.
 * 
 * @param display  определение Display для указания используемого дисплея.
 * @param digit    число, которое нужно вывести, 0..9
 * @param dot      флаг вывода точки на дисплее. Выводить точку, если dot = true.
 * @ingroup indic_display
 */
void CIndicator::displayDigit(Display display, uint8_t digit, boolean dot) {
	uint8_t offset = char_offset(digit);
	if (offset == -1) return;
	tm1650.displayChar(display, (digit & 0xF) + offset, dot);
}

/**
 * @brief Вывод числа на дисплей.
 * 
 * @param digit  число типа int16, которое нужно вывести, -999..9999
 * @ingroup indic_display
 */
void CIndicator::displayDigit(int16_t digit) {
	char *s = ___itoa(digit);
	displayString(s);
}

/**
 * @brief Вывод числа типа float с указанием точности.
 * 
 * @param digit    число с плавающей запятой, которое нужно вывести.
 * @param dplaces  точность числа, определяет количество отображаемых знаков после запятой.
 * @ingroup indic_display
 */
void CIndicator::displayDigit(float digit, uint8_t dplaces) {
	char *s = ___ftoa(digit, dplaces);
	displayString(s);
}

/**
 * @brief Управление точкой указанного дисплея.
 * 
 * Позволяет сделать вывод точки на указанном дисплее.
 * 
 * @param display  определение Display для указания используемого дисплея.
 * @param dot      флаг вывода точки на дисплее. Выводить точку, если dot = true.
 * @ingroup indic_display
 */
void CIndicator::displayDot(Display display, boolean dot) {
	tm1650.setDot(display, dot);
}

/**
 * @brief Управление яркостью дисплея.
 * 
 * Позволяет изменять яркость дисплея на заданную.
 * 
 * @param brightness  значение заданной яркости, 1..7 либо DEFAULT,LEVEL1..LEVEL7.
 * @ingroup indic_display
 */
void CIndicator::displayBrightness(int brightness) {
	tm1650.setBrightness(brightness);
}

/**
 * @brief Определение нажатия указанной кнопки (K1, K2 или K3).
 * 
 * @ingroup indic_button
 */
boolean CIndicator::buttonPress(Button button) {
	uint8_t keys = tm1650.getButtons();
	Button key = NONE;
	switch (keys) {
		case 0x44:
			key = K1;
			break;
		case 0x4C:
			key = K2;
			break;
		case 0x54:
			key = K3;
			break;
		default:
			key = NONE;
			break;
	}
	return button == key;
}

/**
 * @brief Определение нажатой кнопки, возвращает K1, K2, K3 или NONE в случае, если не нажата ни одна кнопка.
 * 
 * @ingroup indic_button
 */
Button CIndicator::buttonPress() {
	uint8_t keys = tm1650.getButtons();
	Button key = NONE;
	switch (keys) {
		case 0x44:
			key = K1;
			break;
		case 0x4C:
			key = K2;
			break;
		case 0x54:
			key = K3;
			break;
		default:
			key = NONE;
			break;
	}
	return key;
}

int CIndicator::char_offset(uint8_t digit) {
	uint8_t offset = -1;
	if (digit <= 9) offset = '0';
	else if (digit >= 0xA && digit <= 0xF) offset = 'A' - 0xA;
	return offset;
}

char *CIndicator::___itoa(int i) {
	char *pos = xtoa_buf + sizeof (xtoa_buf) - 1;
	uint32_t u;
	int negative = 0;
	if (i < 0) {
		negative = 1;
		u = ((uint32_t) (-(1 + i))) + 1;
	}
	else u = i;
	*pos = 0;
	do {
		*--pos = '0' + (u % 10);
		u /= 10;
	} while (u);
	if (negative) *--pos = '-';
	return pos;
}

char *CIndicator::___ftoa(float f, int dplaces) {
	int pos = 0, ix, dp, num, loop;
	if (f < 0) {
		xtoa_buf[pos++] = '-';
		f = -f;
	}
	dp = 0;
	loop = dplaces + 2;
	while (f >= 10.0) {
		f = f / 10.0;
		dp++;
		loop++;
	}
	for (ix = 1; ix < loop; ix++) {
			num = f;
			f=f - num;
			if (num > 9)
				xtoa_buf[pos++] = '#';
			else
				xtoa_buf[pos++] = '0' + num;
			if (dp == 0) xtoa_buf[pos++] = '.';
			f = f * 10.0;
			dp--;
	}
	xtoa_buf[pos] = '\0'; // null term
	return xtoa_buf;
}
