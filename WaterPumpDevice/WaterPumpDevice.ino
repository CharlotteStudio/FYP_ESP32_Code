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
  AutoCloseWaterPump();
  
  if (!IsConnected()) { TryConnection(); return; }

  if (isConnectedMeshNetwork) UpdateWifiMesh();
  
  if (!isRegistered) { TrySendRegisterMessage(); return; }
  
  delay(50);
}
