#include "BLE_Client_Setup_Header.h"
#include "WiFiMesh_Header.h"
#include "Button_Header.h"
#include "LEDLighting_Header.h"
#include "WaterPump_Headter.h"

#define waitingTime_switchConnect 60000

static bool tryConnectBLE = false;

static unsigned long nextTime_switchConnect = 0;

void OnClickCallback()
{
  if (isConnectedBLEService())
  {
    DisconnectDevice();
    nextTime_switchConnect = millis() + waitingTime_switchConnect;
    tryConnectBLE = false;
  }
  
  if (isConnectedMeshNetwork)
  {
    DisconnectedWifiMesh();
    nextTime_switchConnect = millis() + waitingTime_switchConnect;
    tryConnectBLE = true;
  }
}

void OnClickRelayCallback()
{
  isRelayOn = !isRelayOn;
  digitalWrite(relay_pin, !isRelayOn);
}

void setup()
{
  Serial.begin(115200);
  
  SetUpLED();
  SetUpWaterPump();
  SetUpConnectedBLECallback(&BLEConnectCallback);
  SetUpDisconnectedBLECallback(&BLEDisconnectCallback);
  SetUpConnectedWiFiMeshCallback(&WiFiMeshConnectCallback);
  SetUpDisconnectedWiFiMeshCallback(&WiFiMeshBLEDisconnectCallback);
  SetUpCallback(0, &OnClickCallback);
  SetUpCallback(1, &OnClickRelayCallback);

  SetUpBLE();
}

void loop()
{
  CheckButtonOnClick(0, pin_button);
  CheckButtonOnClick(1, active_relay_button);
      
  if (millis() > nextTime_switchConnect)
  {
    nextTime_switchConnect = millis() + waitingTime_switchConnect;
    tryConnectBLE = !tryConnectBLE;
  }

  if(!isConnectedMeshNetwork && !isConnectedBLEService())
  {
    if (tryConnectBLE)
    {
      ScanBLEAndConnect();
    }
    else
    {
      SetUpWifiMesh();
    }
  }

  if(isConnectedMeshNetwork)
  {
    UpdateWifiMesh();
    SendoutWifiMesh("Hello!");
    delay(1000);
  }
  delay(50);
}
