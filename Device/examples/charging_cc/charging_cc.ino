#include "Device.h"

Device device;

bool isCharging = false;

void setup()
{
  Serial.begin(115200);

  /* initialize the device */
  device.begin();
  Serial.println(F("Device initialized"));

  Serial.println(F("\n\nEnter current between [0 - 16000] mA to start charging with constant current"));
  Serial.println(F("Enter 0 to stop charging and disconnect the battery"));
}

void loop()
{
  if (Serial.available())
  {
    String str = Serial.readStringUntil('\n');
    float value = str.toFloat();

    if (value > 0 && value <= 16000)
    {
      /* set the charging voltage */
      device.setChargingCurrent(value);
      isCharging = true;
    }
    else
    {
      /* disconnect the battery from the circuit */
      device.disconnectBattery();
      isCharging = false;
    }
  }

  /* record sensor data if charging */
  if (isCharging)
  {
    device.logSensors();
  }

  delay(1000);
}
