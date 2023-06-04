#include "BLE_Client_Setup_Header.h"
#include "WiFiMesh_Header.h"
#include "LEDLighting_Header.h"
#include "Button_Header.h"
#include "Info_Header.h"
#include "ArduinoJson.h"
#include "MacAddress_Header.h"
#include "WaterPump_Header.h"

#define DeviceTpye 2

#define waitingTime_switchConnect  30000
#define waitingTime_sendRegisteredMessage 5000
#define waitingTime_autoDisconnect 60000

static unsigned long nextTime_switchConnect = 0;
static unsigned long nextTime_sendRegisteredMessage = 0;
static unsigned long nextTime_autoDisconnected = 0;

static bool tryConnectBLE  = true;
static bool isRegistered   = false;
static bool autoDisconnect = false;

static int valueChannel = -1;

bool IsConnected() { return isConnectedMeshNetwork || isConnectedBLEService(); }

void SwitchConnection();
String CreateRegisteredMessage();
String CreateValueDataMessage(int);
void OnClickCallback();
void ReceivedBLECallback(String&);
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
    tryConnectBLE = false;
  }
  
  if (isConnectedMeshNetwork)
  {
    DisconnectedWifiMesh();
    tryConnectBLE = true;
  }
  isRegistered = false;
  delay(2000);
  nextTime_switchConnect = millis() + waitingTime_switchConnect;
}

void TryConnection()
{
  if (IsConnected()) return;
  SwitchConnection();

  if (tryConnectBLE)
  {
    ScanBLEAndConnect();
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
  if (!IsConnected() || isRegistered)
  {
    if (autoDisconnect)
    {
      autoDisconnect = false;      
    }
    return;
  }

  if (millis() < nextTime_sendRegisteredMessage)  return;

  // check auto disconnect
  if (!autoDisconnect)
  {
    autoDisconnect = true;
    nextTime_autoDisconnected = millis() + waitingTime_autoDisconnect;
  }
  if (autoDisconnect && millis() > nextTime_autoDisconnected)
  {
    Serial.println("Time out try auto Disconnect.");
    autoDisconnect = false;
    OnClickCallback();
    return;
  }
  
  nextTime_sendRegisteredMessage = millis() + waitingTime_sendRegisteredMessage;

  if (isConnectedBLEService())
  {
    String toTarget = ReceivedBLEMessage(characteristicUUID_To);
    String message  = ReceivedBLEMessage(characteristicUUID_Message);
    if (!toTarget.equals(ble_empty) || !message.equals(ble_empty)) return;
    
    String json = CreateRegisteredMessage();
    printf("Send out Register message by BLE.\n%s\n", json.c_str());
    SendoutBLEMessage(characteristicUUID_Message, json);
    SendoutBLEMessage(characteristicUUID_To, "0");
  }

  if (isConnectedMeshNetwork)
  {
    Serial.println("Send out Register message by WiFi-Mesh.");
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

void UpdateBLE()
{
  String target = ReceivedBLEMessage(characteristicUUID_To);
  if (target.equals(ble_empty) || target.equals("0")) return;
  
  if (target.equals(mac_address_str))
  {
    printf("Message Target is [%s], is me\n", target.c_str());
    String message = ReceivedBLEMessage(characteristicUUID_Message);
    if (!message.equals(ble_empty))
    {
      ReceivedBLECallback(message);
    }
    else
    {
      printf("Message json is empty...\n");
    }
    target  = ble_empty;
    message = ble_empty;
    SendoutBLEMessage(characteristicUUID_To, ble_empty);
    SendoutBLEMessage(characteristicUUID_Message, ble_empty);
  }
  else
  {
    printf("Message Target is [%s], is not me\n", target.c_str());
    if (target.equals(""))
    {
      printf("Main Control Device is disconnected, try disconnected that.\n");
      DisconnectDevice();
    }
  }
}


void ReceivedBLECallback(String& json)
{
  StaticJsonDocument<jsonDeserializeSize> doc;

  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  bool isRegisterMessage = doc["Register"].is<int>();
  if (isRegisterMessage)
  {
    int registered = doc["Register"];
    if (registered > 0)
    {
      valueChannel = doc["Channel"].as<int>();
      printf("Received register success. channel is [%d]\n", valueChannel);
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

void TrySendDataMessage(int value)
{
  if (!IsConnected() || !isRegistered) return;

  if (isConnectedBLEService())
  {
    if (valueChannel == -1) return;
    SendoutBLEMessage(characteristicUUID_channel[valueChannel], String(value));
  }

  if (isConnectedMeshNetwork)
  {
    String json = CreateValueDataMessage(value);
    SendoutWifiMesh(json);
  }
}

String CreateValueDataMessage(int value)
{
  StaticJsonDocument<jsonSerializeDataSize> doc;
  doc["Value"] = value;
  String str;
  serializeJsonPretty(doc, str);
  return str;
}
