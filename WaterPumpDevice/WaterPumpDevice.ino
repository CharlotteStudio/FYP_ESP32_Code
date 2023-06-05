#include "ConnectionManager_Header.h"

void setup()
{
  Serial.begin(115200);

  SetUpWaterPump();
  SetUpLED();
  SetUpConnection();
}

void loop()
{
  CheckButtonOnClick(0, pin_button);
  bool cloesedPump = IsCloseWaterPump();
  
  if (!IsConnected()) { TryConnection(); return; }

  if (isConnectedBLEService())UpdateBLE();
  if (isConnectedMeshNetwork) UpdateWifiMesh();
  
  if (!isRegistered) { TrySendRegisterMessage(); return; }

  if (cloesedPump) SendoutClosedPumpMessage();

  CheckingBLEChannel();
  
  delay(50);
}
