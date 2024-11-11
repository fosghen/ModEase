# ModEase

ModEase — это проект для управления Modbus-устройствами, предоставляющий удобный графический интерфейс для настройки, подключения, мониторинга и записи/чтения данных с устройств через протокол Modbus RTU.

## Содержание

- [Описание](#описание)
- [Функциональность](#функциональность)
- [Установка](#установка)
- [Запуск проекта](#запуск-проекта)
- [Использование](#использование)
- [Архитектура](#архитектура)
- [Автор](#автор)

## Описание

Проект разработан на C++ с использованием Qt для создания графического интерфейса и управления устройствами Modbus. Он позволяет пользователям подключаться к Modbus-устройствам, настраивать параметры подключения, читать и записывать значения регистров, а также отображать данные в таблицах с использованием JSON для конфигурации.

## Функциональность

- **Подключение к Modbus-устройствам** через COM-порт, настройка параметров соединения (скорость, паритет, количество бит данных и стоп-битов).
- **Чтение и запись регистров** Modbus.
- **Отображение данных** из регистров в удобном табличном виде.
- **Загрузка и настройка конфигураций** через JSON для инициализации таблиц и интерфейса.
- **Поддержка обработки ошибок** и отображение статуса подключения.
- **Управление через GUI** с удобными кнопками и параметрами для подключения и отключения устройства.

## Установка

### Требования

- Qt 6
- Компилятор, поддерживающий C++11 и выше
- CMake (если вы собираете проект с использованием CMake)

### Шаги для установки

1. Склонируйте репозиторий:

    ```bash
    git clone https://github.com/fosghen/ModEase.git
    cd ModEase
    ```

2. Соберите проект:

   **Если используете CMake:**

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

   **Если используете Qt Creator:**

    - Откройте проект в Qt Creator, настройте и запустите сборку.

## Запуск проекта

После успешной сборки выполните следующие шаги для запуска:

	```bash
	./ModEase
	```
## Использование
### Основные шаги

   1. Настройка параметров соединения: Выберите COM-порт, скорость передачи данных, паритет, бит данных и стоп-биты в интерфейсе.
   2. Подключение к устройству: Нажмите кнопку "Подключиться". Статус подключения отобразится на панели статуса.
   3. Для чтения регистров используйте кнопку "Считать".
   4. Для записи в таблице двойным кликом (или с помощью Enter) выберите необходимый регистр и укажите значение. Если вы хотите записать сразу несколько, то установите галочку "Запись нескольких регистров".
   5. Отключение устройства: Нажмите кнопку "Отключиться", чтобы завершить соединение с Modbus-устройством.

## Примеры JSON-конфигурации для таблиц

Файл JSON для настройки таблиц инициализирует параметры, такие как адреса и множители регистров:
```json
{
    "Основные": {
        "0": {
            "meaning": "gName",
            "units": "",
            "address": 0,
            "multy": 1,
            "digits": 0,
            "description": "Название устройства",
            "access": 0,
            "dtype": 0
        },
        "1": {
            "meaning": "gName",
            "units": "",
            "address": 1,
            "multy": 1,
            "digits": 0,
            "description": "gName",
            "access": 0,
            "dtype": 0
        }
	}
	"ИИ": {
        "0": {
            "meaning": "iiStatus",
            "units": "",
            "address": 100,
            "multy": 1,
            "digits": 0,
            "description": "Статусы работы",
            "access": 0,
            "dtype": 0
        },
        "1": {
            "meaning": "iiEnable",
            "units": "",
            "address": 101,
            "multy": 1,
            "digits": 0,
            "description": "Включить/выключить ИИ",
            "access": 2,
            "dtype": 0
        }
	}
}
```

## Архитектура
Проект состоит из нескольких основных компонентов:

   - MainWindow: Основное окно GUI, обеспечивает взаимодействие пользователя с программой.
   - ConnectionManager: Управляет подключением и отключением от Modbus-устройства, настройкой параметров соединения и состоянием соединения.
   - RegisterManager: Обрабатывает чтение и запись регистров устройства.
   - TableManager: Инициализирует и управляет отображением таблиц в UI, загружает данные из JSON для конфигурации.
   - SettingsDialog: Диалог для настройки параметров подключения.
   

## Автор

[Роман] - [r.efremov2@g.nsu.ru]