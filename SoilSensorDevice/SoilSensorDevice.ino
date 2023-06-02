#include "ConnectionManager_Header.h"

void setup()
{
  Serial.begin(115200);
  
  SetUpLED();
  SetUpConnection();
}

void loop()
{
  CheckButtonOnClick(0, pin_button);
  bool isGotValue = TryGetSoilSensorValue();

  if (!IsConnected()) { TryConnection(); return; }

  if (isConnectedBLEService())UpdateBLE();
  if (isConnectedMeshNetwork) UpdateWifiMesh();
  
  if (!isRegistered) { TrySendRegisterMessage(); return; }

  if (isGotValue) TrySendDataMessage(currentSoilSensorValue);

  delay(50);
}
