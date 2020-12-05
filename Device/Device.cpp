#include "Device.h"

Adafruit_INA260 ina260 = Adafruit_INA260();
Adafruit_DS3502 ds3502 = Adafruit_DS3502();
Adafruit_Si7021 si7021 = Adafruit_Si7021();
TinyPICO tp = TinyPICO();

bool Device::begin()
{
    bool result = true;

    pinMode(RELAY_1_PIN, OUTPUT);
    pinMode(RELAY_2_PIN, OUTPUT);
    pinMode(RELAY_3_PIN, OUTPUT);

    if (!SD.begin())
    {
        Serial.println(F("SD: Failed"));
        result = false;
    }
    else
    {
        Serial.println(F("SD: Ok"));
    }

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println(F("SD Card: Failed"));
        result = false;
    }
    else
    {
        Serial.println(F("SD Card: Ok"));
    }

    if (!ina260.begin(0x41))
    {
        Serial.println(F("INA260: Failed"));
        result = false;
    }
    else
    {
        Serial.println(F("INA260: Ok"));
    }

    if (!ds3502.begin())
    {
        Serial.println("DS3502: Failed");
        result = false;
    }
    else
    {
        Serial.println(F("DS3502: Ok"));
    }

    if (!si7021.begin())
    {
        Serial.println("Si7021: Failed");
        result = false;
    }
    else
    {
        Serial.println(F("Si7021: Ok"));
    }

    if (result)
    {
        successBlink();
    }
    else
    {
        errorBlink();
    }

    return result;
}

void Device::appendFile(const char *path, const char *message)
{
    File file = SD.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println(F("Failed to open file for appending"));
        return;
    }
    if (!file.print(message))
    {
        Serial.println("Append failed");
    }
    file.close();
}

void Device::logSensors()
{
    unsigned long t = millis();
    float voltage = readVoltage();
    float current = readCurrent();
    float power = readPower();
    float temperature = readTemperature();
    float humidity = readHumidity();
    String _mode = "";
    if (deviceMode == DISCONNECTED)
    {
        _mode = "DISCONNECTED";
    }
    else if (deviceMode == CHARGING_CV)
    {
        _mode = "CHARGING_CV";
    }
    else if (deviceMode == CHARGING_CC)
    {
        _mode = "CHARGING_CC";
    }
    else if (deviceMode == DISCHARGING)
    {
        _mode = "DISCHARGING";
    }

    sprintf(buffer, "%lu,%s,%.3f,%.3f,%.3f,%.1f,%.1f\n", t, _mode.c_str(), voltage, current, power, temperature, humidity);
    appendFile("/output.txt", buffer);

    Serial.printf("Time: %lu Mode: %s V: %.3fV I: %.3fmA P: %.3fmW T: %.1fC H: %.1f\n", t, _mode.c_str(), voltage, current, power, temperature, humidity);
}

float Device::readVoltage()
{
    // returns voltage in V
    return ina260.readBusVoltage() / 1000;
}

float Device::readCurrent()
{
    // returns current in mA
    return ina260.readCurrent();
}

float Device::readPower()
{
    // returns power consumption in mW
    return ina260.readPower();
}

float Device::readTemperature()
{
    // returns temperature in C
    return si7021.readTemperature();
}

float Device::readHumidity()
{
    // returns humidity
    return si7021.readHumidity();
}

void Device::connectBattery()
{
    // connects the battery to the circuit
    digitalWrite(RELAY_1_PIN, HIGH);
}

void Device::disconnectBattery()
{
    // disconnects the battery from the circuit
    digitalWrite(RELAY_1_PIN, LOW);
    deviceMode = DISCONNECTED;
}

void Device::setChargingVoltage(float voltage)
{
    // set the output to constant voltage charging mode
    // Output range 0-5V -> 0-127
    disconnectBattery();

    uint8_t wiper = voltage * 127 / 5;
    setWiper(wiper);

    digitalWrite(RELAY_2_PIN, HIGH);
    digitalWrite(RELAY_3_PIN, LOW);
    connectBattery();
    deviceMode = CHARGING_CV;
}

void Device::setChargingCurrent(float current)
{
    // set the output to constant current charging mode
    // Output range 0-16A -> 0-127
    disconnectBattery();

    uint8_t wiper = current * 127 / 16000;
    setWiper(wiper);

    digitalWrite(RELAY_2_PIN, LOW);
    digitalWrite(RELAY_3_PIN, HIGH);
    connectBattery();
    deviceMode = CHARGING_CC;
}

void Device::setDischargeCurrent(float current)
{
    // set the output to constant current discharging mode
    // Output range 0-8A -> 0-127
    disconnectBattery();

    uint8_t wiper = current * 127 / 8000;
    setWiper(wiper);

    digitalWrite(RELAY_2_PIN, LOW);
    digitalWrite(RELAY_3_PIN, LOW);
    connectBattery();
    deviceMode = DISCHARGING;
}

void Device::setWiper(uint8_t new_wiper_value)
{
    if (new_wiper_value <= WIPER_MAX)
    {
        ds3502.setWiper(new_wiper_value);
    }
    else
    {
        ds3502.setWiper(WIPER_MAX);
    }
}

void Device::errorBlink()
{
    tp.DotStar_SetPower(true);
    tp.DotStar_Clear();
    while (1)
    {
        tp.DotStar_SetBrightness(100);
        tp.DotStar_SetPixelColor(255, 0, 0);
        delay(500);
        tp.DotStar_Clear();
        delay(500);
    }
}

void Device::successBlink()
{
    tp.DotStar_SetPower(true);
    tp.DotStar_Clear();
    for (int x = 0; x < 2; x++)
    {
        tp.DotStar_SetBrightness(100);
        tp.DotStar_SetPixelColor(0, 255, 0);
        delay(100);
        tp.DotStar_Clear();
        delay(200);
    }
    tp.DotStar_Clear();
}