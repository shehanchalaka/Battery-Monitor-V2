#include "Device.h"

Device device;

void setup() {
  Serial.begin(115200);

  device.begin();
  Serial.println(F("Device initialized"));

  /* connect the battery to the circuit */
  // device.connectBattery();
  /* disconnects the battery from the circuit */
  // device.disconnectBattery();
}

void loop() {
  /* read voltage */
  float voltage = device.readVoltage();
  /* read current */
  float current = device.readCurrent();
  /* read power */
  float power = device.readPower();

  char buffer[100];
  sprintf(buffer, "Voltage: %.2f V Current: %.2f mA Power: %.2f mW", voltage, current, power);
  Serial.println(buffer);

  delay(500);
}
