#include "Arduino.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Adafruit_INA260.h"
#include "Adafruit_DS3502.h"
#include "Adafruit_Si7021.h"
#include <TinyPICO.h>

#ifndef DEVICE_h
#define DEVICE_h

#define RELAY_1_PIN 25
#define RELAY_2_PIN 27
#define RELAY_3_PIN 26

typedef enum _device_mode
{
    DISCONNECTED = 0,
    CHARGING_CV = 1,
    CHARGING_CC = 2,
    DISCHARGING = 3,
} DeviceMode;

class Device
{
public:
    bool begin();
    void appendFile(const char *path, const char *message);
    void logSensors();

    float readVoltage();
    float readCurrent();
    float readPower();
    float readTemperature();
    float readHumidity();

    void connectBattery();
    void disconnectBattery();
    void setChargingVoltage(float voltage);
    void setChargingCurrent(float current);
    void setDischargeCurrent(float current);

private:
    uint8_t WIPER_MAX = 127;
    char buffer[100];
    DeviceMode deviceMode = DISCONNECTED;

    void setWiper(uint8_t new_wiper_value);
    void errorBlink();
    void successBlink();
};

#endif // DEVICE_h