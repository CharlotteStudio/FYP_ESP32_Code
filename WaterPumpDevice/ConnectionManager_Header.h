#include "BLE_Client_Setup_Header.h"
#include "WiFiMesh_Header.h"
#include "LEDLighting_Header.h"
#include "Button_Header.h"
#include "Info_Header.h"
#include "ArduinoJson.h"
#include "MacAddress_Header.h"
#include "WaterPump_Header.h"

#define waitingTime_switchConnect 60000
#define waitingTime_sendRegisteredMessage 5000

static unsigned long nextTime_switchConnect = 0;
static unsigned long nextTime_sendRegisteredMessage = 0;

static bool tryConnectBLE = true;
static bool isRegistered = false;

bool IsConnected() { return isConnectedMeshNetwork || isConnectedBLEService(); }

void SwitchConnection();
String CreateRegisteredMessage();
String CreateValueDataMessage(int);
void OnClickCallback();
void ReceivedWiFiMeshCallback(unsigned int, String&);

void SetUpConnection()
{
  SetUpConnectedBLECallback(&BLEConnectCallback);
  SetUpDisconnectedBLECallback(&BLEDisconnectCallback);

  SetUpConnectedWiFiMeshCallback(&WiFiMeshConnectCallback);
  SetUpReceivedMessageCallback(&ReceivedWiFiMeshCallback);
  SetUpDisconnectedWiFiMeshCallback(&WiFiMeshBLEDisconnectCallback);

  SetUpBLE();
  SetUpCallback(0, &OnClickCallback);
}

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

void TryConnection()
{
  if (IsConnected()) return;
  SwitchConnection();

  if (tryConnectBLE)
  {
    Serial.println("Try connect BLE ...");
    //ScanBLEAndConnect();
  } 
  else
  {
    Serial.println("Try connect WiFi Mesh ...");
    SetUpWifiMesh();
  }
}

void SwitchConnection()
{
  if (millis() > nextTime_switchConnect)
  {
    nextTime_switchConnect = millis() + waitingTime_switchConnect;
    tryConnectBLE = !tryConnectBLE;
  }
}

void TrySendRegisterMessage()
{
  if (!IsConnected() || isRegistered) return;

  if (isConnectedBLEService())
  {
    Serial.println("To be set up");
  }

  if (isConnectedMeshNetwork)
  {
    if (millis() < nextTime_sendRegisteredMessage) return;
  
    nextTime_sendRegisteredMessage = millis() + waitingTime_sendRegisteredMessage;

    String json = CreateRegisteredMessage();
    SendoutWifiMesh(json);
  }
}

String CreateRegisteredMessage()
{
  StaticJsonDocument<jsonSerializeRegisterSize> doc;
  doc["To"] = 0;
  doc["DeviceTpye"] = DeviceTpye;
  doc["DeviceMac"] = GetMacAddressString();
  doc["Register"] = 1;
  String str;
  serializeJsonPretty(doc, str);
  return str;
}

void ReceivedWiFiMeshCallback(unsigned int from, String& json)
{
  StaticJsonDocument<jsonDeserializeSize> doc;

  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  unsigned int targetDevice = doc["To"].as<unsigned int>();
  Serial.print("Target is [");
  Serial.print(targetDevice);
  if (targetDevice == GetMyNodeId())
  {
    Serial.println("], is me");
    bool isRegisterMessage = doc["Register"].is<int>();
    if (isRegisterMessage)
    {
      int registered = doc["Register"];
      if (registered > 0)
      {
        Serial.println("Received register success.");
        isRegistered = true;
      }
      else
      {
        Serial.println("Received register fail.");
        isRegistered = false;
      }
    }

    bool isSetActiveWaterPump = doc["ActiveState"].is<int>();
    if (isSetActiveWaterPump)
    {
      int activeCode = doc["ActiveState"].as<int>();
      
      if (activeCode != 1 && activeCode != 0)
      {
        printf("[%d] is not command code.\n", activeCode);
        return;
      }
      
      printf("command code is [%d]\n", activeCode);
      if (activeCode == 1) ActiveWaterPump();
      if (activeCode == 0) CloseWaterPump();
    }
  }
  else
  {
    Serial.println("], is not me");
  }
}
