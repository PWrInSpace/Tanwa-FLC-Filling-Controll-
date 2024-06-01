# ESP-Now-Slave

## 1. Diagram blokowy

![Alt text](./docs/ESP-Now-Slave.drawio.png?raw=true "Block diagram")

## 2. Instrukcja implementacji na frameworku Arduino

### A. Przygotowanie repo i plików
 
 1. Upewnij się, że masz wszystko zaktualizowane na repo na serwerze oraz u siebie lokalnie. Usuń niepotrzebne branche,
 2. Stwórz w repozytorium swojego projektu nowy branch związany z nowym konceptem. Skopiuj tam folder `ESP-Now-Slave-Ardu` i nie zapomnij zrobić jakiegoś wstępnego commita,
 3. Otwórz projekt `ESP-Now-Slave-Ardu` w platformio,

### B. Konfiguracja projektu

 4. Przejdź do pliku `include/pinout.h` i dodaj potrzebny pinout,
 5. W pliku `include/dataStruct.h` ustaw ile ma trwać okres zbierania i przesyłania danych w danym sta'cie odebranym od OBC (w ms). Jeżeli ESP ma w danym sta'cie spać, ustaw `SLEEP_TIME_MS` np:
 ```C++
// IMPORTANT! To set for each module and state:
enum Periods {

    INIT_PERIOD             = SLEEP_TIME_MS,
    IDLE_PERIOD             = SLEEP_TIME_MS,
    RECOVERY_ARM_PERIOD     = SLEEP_TIME_MS,
    FUELING_PERIOD          = 500,
    ARMED_TO_LAUNCH_PERIOD  = 1000,
    RDY_TO_LAUNCH_PERIOD    = 1000,
    COUNTDOWN_PERIOD        = 500,
    FLIGHT_PERIOD           = 10,
    FIRST_STAGE_REC_PERIOD  = 250,
    SECOND_STAGE_REC_PERIOD = 500,
    ON_GROUND_PERIOD        = SLEEP_TIME_MS,
    HOLD_PERIOD             = 1000,
    ABORT_PERIOD            = SLEEP_TIME_MS
};

// IMPORTANT END!
 ```
 6. W pliku `include/dataStruct.h` w strukturze `DataToObc` dodaj wszelkie zmienne, które mają być wysyłane do OBC (ta struktura dotyczy jedynie wysyłania, nie zapisu). Pamiętaj, że identyczna struktura musi znajdować się w kodzie OBC, stąd o każdej zmianie tutaj na etapie integracji ele należy poinformować kogoś od OBC. Do tych zmiennych odwołasz się potem używając `moduleData.dataToObc.nazwaZmiennej`. np:
 ```C++
struct DataToObc {

    bool wakenUp : 1;
    // IMPORTANT! To implementation of each module:
    uint16_t test;
    float pressure;
    float temperature;
    float humidity;
    // IMPORTANT END!
};
 ```
 7. W pliku `include/dataStruct.h` w strukturze `ModuleData` dodaj wszelkie zmienne globalne. Potem na poziomie całego projektu odwołasz się do nich poprzez `moduleData.nazwaZmiennej` (pamiętaj, te wartości nie będą ani wysyłane do OBC ani nigdzie trwale zapisywane). np:
 ```C++
// IMPORTANT! Add other global variables in this struct:
float initialPressure;
float initialTemperature;
float initialHumidity;
// IMPORTANT END!
 ```

### C. Zapis na flash i SD

 8. W pliku `include/dataStruct.h` w strukturze `DataToSave` dodaj wszelkie zmienne do zapisu na flashu i / lub SD. Potem na poziomie całego projektu odwołasz się do nich poprzez `moduleData.dataToSave.nazwaZmiennej` (jeżeli potrzebna ci jakaś zmienna globalnie, ale nie ma potrzeby jej zapisywać, wróć do pkt 7). Nie zapomnij uzupełnić metody `toString()`. np:
 ```C++
struct DataToSave {

    // IMPORTANT! To implementation of each module:
    float pressure;
    float humidity;

    String toString() {

        char outStr[50];
        sprintf(outStr, "%0.2f;%0.2f", pressure, humidity);
        return String(outStr);
    }
    // IMPORTANT END!
};
 ```
 9. W pliku `include/dataStruct.h` w strukturze `ModuleData` ustaw, czy chcesz używać zapisu na flasha lub sd. np:
 ```C++
// IMPORTANT! Set the flash or sd logger:
    const bool flashSave = true;
    const bool sdSave = false;
// IMPORTANT END!
 ```
 **Pamiętaj o sprawdzeniu, czy pinout SPI i SD jest odpowiedni w przypadku używania karty SD.**
 Flash czyszczony jest automatycznie przy przechodzeniu na pierwszy "nieśpiący" state.

### D. Dodanie implementacji własnych funkcji
 
 10. W pliku `src/functions.cpp` dodaj odpowiednią implementację każdej z funkcji (usuwając zbędne `#warning`). Pamiętaj, że funkcje:
  - `initPeripherals` wykona się tylko raz na obudzenie,
  - `measure` co każdy okres wysyłania danych,
  - `rxNowHandler` przy każdym przyjściu danych z OBC innych niż nr state'u (który aktualizowany jest automatycznie).

## 3. Instrukcja implementacji na frameworku IDF

### A. Przygotowanie repo i plików

 1. Upewnij się, że masz wszystko zaktualizowane na repo na serwerze oraz u siebie lokalnie. Usuń niepotrzebne branche,
 2. Stwórz w repozytorium swojego projektu nowy branch związany z nowym konceptem. Skopiuj tam folder `ESP-Now-Slave-Idf` i nie zapomnij zrobić jakiegoś wstępnego commita,
 3. Otwórz projekt `ESP-Now-Slave-Idf` w platformio,

### B. Konfiguracja projektu

 4. Przejdź do pliku `include/pinout.h` i dodaj potrzebny pinout,
 5. W pliku `include/dataStruct.h` ustaw ile ma trwać okres zbierania i przesyłania danych w danym sta'cie odebranym od OBC (w ms). Jeżeli ESP ma w danym sta'cie spać, ustaw `SLEEP_TIME_MS` np:
 ```C
// IMPORTANT! To set for each module and state:
typedef enum {

    INIT_PERIOD             = SLEEP_TIME_MS,
    IDLE_PERIOD             = SLEEP_TIME_MS,
    RECOVERY_ARM_PERIOD     = SLEEP_TIME_MS,
    FUELING_PERIOD          = 500,
    ARMED_TO_LAUNCH_PERIOD  = 1000,
    RDY_TO_LAUNCH_PERIOD    = 1000,
    COUNTDOWN_PERIOD        = 500,
    FLIGHT_PERIOD           = 10,
    FIRST_STAGE_REC_PERIOD  = 250,
    SECOND_STAGE_REC_PERIOD = 500,
    ON_GROUND_PERIOD        = SLEEP_TIME_MS,
    HOLD_PERIOD             = 1000,
    ABORT_PERIOD            = SLEEP_TIME_MS
} Periods;

// IMPORTANT END!
 ```
 6. W pliku `include/dataStruct.h` dodaj wszelkie zmienne, które mają być wysyłane do OBC (ta struktura dotyczy jedynie wysyłania, nie zapisu). Pamiętaj, że identyczna struktura musi znajdować się w kodzie OBC, stąd o każdej zmianie tutaj na etapie integracji ele należy poinformować kogoś od OBC. np:
 ```C
typedef struct DataToObc {
    bool wakenUp : 1;
    uint32_t uptime;
    // IMPORTANT! To implementation of each module:
    uint16_t test;
    float pressure;
    float temperature;
    float humidity;
    // IMPORTANT END!
} DataToObc;
 ```
 7. W pliku `include/dataStruct.h` w strukturze `ModuleData` dodaj wszelkie zmienne globalne. Potem na poziomie całego projektu odwołasz się do nich poprzez `moduleData.nazwaZmiennej` (pamiętaj, te wartości nie będą ani wysyłane do OBC ani nigdzie trwale zapisywane). np:
 ```C
// IMPORTANT! Add other global variables in this struct:
float initialPressure;
float initialTemperature;
float initialHumidity;
// IMPORTANT END!
 ```

### C. Zapis na flash i SD

 8. W pliku `include/dataStruct.h` w strukturze `DataToSave` dodaj wszelkie zmienne do zapisu na flashu i / lub SD. Potem na poziomie całego projektu odwołasz się do nich poprzez `moduleData.dataToSave.nazwaZmiennej` (jeżeli potrzebna ci jakaś zmienna globalnie, ale nie ma potrzeby jej zapisywać, wróć do pkt 7). np:
 ```C
typedef struct DataToSave {

    uint32_t uptime;
    // IMPORTANT! To implementation of each module:

    // IMPORTANT END!
} DataToSave;
 ```
 9. W pliku `src/functions.c` zedytuj funkcję `create_sd_string`. np:
 ```C
bool create_sd_string(DataToSave *data, char *buffer, size_t len) {
    snprintf(buffer, len, "%0.2f\n", data->dupa);
    return true;
}
 ```
 10. Nie zapomnij o odpowiednich flagach w `platformio.ini`. Usuń niepotrzebne:
 ```ini
build_flags = -Wall -DTEMPLATE_CMD -DTEMPLATE_FLASH -DTEMPLATE_SD
 ```
 11. W pliku `include/dataStructs.h` ustaw co jaki czas ma wykonywać się zapis (lepiej nie przedobrzyć :D). np:
 ```C
// CONFIGURE MEMORY LOGGING
#define DATA_LOG_PERIOD 100  // ms // IMPORTANT!
 ```

### D. Dodanie implementacji własnych funkcji
 
 12. W pliku `src/functions.c` dodaj odpowiednią implementację każdej z funkcji (usuwając zbędne `#warning`). Pamiętaj, że funkcje:
  - `initPeripherals` wykona się tylko raz na obudzenie,
  - `measure` co każdy okres wysyłania danych,
  - `rxNowHandler` przy każdym przyjściu danych z OBC innych niż nr state'u (który aktualizowany jest automatycznie).

## 4. Odczyt danych z flasha
 
 ### A. Arduino

 1. Pamiętaj by sprawdzić, czy ustawiono poprawny pin przycisku boota w `include/pinout.h`,
 2. Podczas uruchamiania ESP, otwórz monitor portu szeregowego i kliknij przycisk boot od razu po wyświetleniu `Press the boot button to read flash...`. Na UART zostanie wysłany ciąg ramek odzyskanych z flasha. Jeśli nie zdążysz wcisnąć przycisku na czaso, program wyświetli `Flash reading skipped / finished.`.
 **Spokojnie, nienaciśnięcie przycisku nie spowoduje skasowania danych na flashu. Za to spowoduje je przejście w state, który nie implementuje sleepa.**

 ### B. IDF

 1. Pamiętaj by sprawdzić, czy ustawiono flagę `-DTEMPLATE_CMD` w pliku `platformio.ini`,
 2. Podczas uruchamiania ESP, otwórz monitor portu szeregowego i kliknij przycisk boot od razu po wyświetleniu logów inicjalizacji wpisz `flash-read`. Czasu jest mało (bo potem idzie spać), więc możesz wykorzystać sktypt pythonowy, który to ułatwi (powstanie niedługo).