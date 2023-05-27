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
  
  if (!IsConnected()) { TryConnection(); return; }

  if (isConnectedMeshNetwork) UpdateWifiMesh();
  
  if (!isRegistered) { TrySendRegisterMessage(); return; }

  delay(50);
}
