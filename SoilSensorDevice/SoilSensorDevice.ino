#include "ConnectionManager_Header.h"
#include "SoilSensor_Header.h"

void setup()
{
  Serial.begin(115200);
  
  SetUpLED();
  SetUpConnection();
}

void loop()
{
  CheckButtonOnClick(0, pin_button);
  TryGetSoilSensorValue();

  if (!IsConnected()) { TryConnection(); return; }

  if (!isRegistered) { TrySendRegisterMessage(); return; }

  if (currentSoilSensorValue != lastSoilSensorValue)
  {
    TrySendDataMessage(currentSoilSensorValue);
    lastSoilSensorValue = currentSoilSensorValue;
  }

  UpdateWifiMesh();

  delay(50);
}
