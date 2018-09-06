# Truckduino: платформа для разработчиков, развернутая на [PlatformIO](http://platformio.org)

Truckduino board build on STM32 microcontroller. The STM32 family of 32-bit Flash MCUs based on the ARM Cortex-M processor is designed to offer new degrees of freedom to MCU users. It offers a 32-bit product range that combines very high performance, real-time capabilities, digital signal processing, and low-power, low-voltage operation, while maintaining full integration and ease of development.

* [Домашняя страница](http://truckduino.ru)

# Быстрый старт
Здесь приводится описание установки текстового редактора **Atom** и окружения **PlatformIO** с настройкой платформы и библиотеки **Truckduino** на ОС Windows. Также приводится пример сборки тестового проекта и загрузки программы в плату **Truckduino**.
> Подробнее по установке **PlatformIO** можно прочитать [здесь](https://docs.platformio.org/en/stable/ide/atom.html).
## 1. Установка PlatformIO IDE:
* Скачайте **Atom** по [ссылке](https://atom.io/) и установите его.
* Запустите **Atom** и откройте **Package Manager** (`Menu: File > Settings > Install`).
* Выполните поиск официального [пакета](https://atom.io/packages/platformio-ide) **"platformio-ide"**.
* Установите **PlatformIO IDE**.
## 2. Установка Clang (Intelligent Code Completion):
* Скачайте [Clang 3.9.1 для Windows](http://llvm.org/releases/download.html).
* Установите **Clang**, в диалоговом окне установщика выберите пункт **"Add LLVM to the system PATH"**.
## 3. Установка платформы для разработчиков Truckduino:
* Скачайте программу **Git** по [ссылке](https://git-scm.com/download/win) и установите ее.
* Запустите терминал в **PlatformIO** (`Menu: PlatformIO > Terminal > New Terminal`) и выполните следующую команду:
```bash
> pio platform install https://github.com/microseti/truckduino
```
## 4. Установка библиотеки ядра Truckduino:
* В терминале **PlatformIO** выполните следующую команду:
```bash
> pio lib -g install Truckduino-Core
```
## 5. Запуск тестовой программы:
* Создайте директорию `truckduino-test` и перейдите в нее.
* Создайте файл `platformio.ini` со следующим содержанием:
```ini
[env:truckduino]
platform = truckduino
framework = arduino
board = truckduino
```
* Создайте директорию `src` и перейдите в нее.
* Создайте файл `main.cpp` со следующим содержанием:
```cpp
#include "Truckduino.h"

void setup() {
    Indicator.begin();
    Indicator.displayTest();
}

void loop() {
    Indicator.displayString("test");
    delay(500);
    Indicator.displayClear();
    delay(500);
    Watchdog.reset();
}
```
* У вас должна получиться такая структура файлов:
```
|--truckduino-test
|  |- platformio.ini
|  |--src
|     |- main.cpp
```
* Откройте директорию `truckduino-test` правым кликом мыши и выбором пункта из меню **"открыть с помощью Atom"**.
* В редакторе **Atom** запустите сборку проекта командой `Menu: PlatformIO > Build`.
* Загрузите полученную прошивку в плату **Truckduino** командой `Menu: PlatformIO > Upload` и проверьте работу платы с новой загруженной программой.

