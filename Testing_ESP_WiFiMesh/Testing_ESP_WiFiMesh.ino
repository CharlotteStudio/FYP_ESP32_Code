// Version 0.1.0, Release at 20230523
#include "WiFiMesh_Header.h"
#include "Button_Header.h"
#include "Info_Header.h"
#include "DeviceInfo_Header.h"
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <ArduinoJson.h>

#define maximum_device_count 32
#define buttonPin 2

String mac_Address_str;

bool isRegister = false;

void DeserializateJson(String json);
void OnWiFiMeshConnectCallback() { Serial.println("Connect Callback."); }
void OnWiFiMeshDisconnectCallback() { Serial.println("Disconnect Callback."); }
void OnReceivedMessageCallback(unsigned int from, String &msg) { DeserializateJson(from, msg); }

void OnClickCallback()
{
  String json = CreateJson();
  SendoutWifiMesh(json);
}

String CreateJson()
{
  String str;
  if (isRegister)
  {
    StaticJsonDocument<jsonSerializeRegisterSize> doc;
    doc["To"] = 0;
    doc["DeviceTpye"] = DeviceTpye;
    doc["DeviceMac"] = mac_Address_str;
    doc["Register"] = 1;
    serializeJsonPretty(doc, str);
  }
  else
  {
    StaticJsonDocument<jsonSerializeDataSize> doc;
    doc["Value"] = random(2700);
    serializeJsonPretty(doc, str);
  }
  isRegister = !isRegister;
  return str;
}

void SendoutRegisteredSuccessMessage(unsigned int target)
{
  StaticJsonDocument<jsonSerializeRegisterSize> doc;
  doc["To"] = target;
  doc["Register"] = 1;
  String str;
  serializeJsonPretty(doc, str);
  SendoutWifiMesh(str);
}

void DeserializateJson(unsigned int wifiMeshNodeId, String json)
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
  Serial.print("Message Target is [");
  Serial.print(targetDevice);
  
  if (targetDevice != GetMyNodeId())
  {
    Serial.println("], is not me");
    return;
  }
  Serial.println("], is me");

  bool isRegisterMessage = doc["Register"].is<int>();
  int index = GetExistedDeviceInt(wifiMeshNodeId);

  if (!isRegisterMessage && index != -1)
  {
    deviceInfo[index].value = doc["Value"].as<int>();
    printf("Update value is [%d]", deviceInfo[index].value);
    return;
  }

  if (index == -1)
  {
    DeviceInfo newDevice = {
        .deviceMac = doc["DeviceMac"].as<String>(),
        .deviceTpye = doc["DeviceTpye"].as<int>(),
        .onOff = doc["Register"].as<int>(),
        .value = 0,
        .wifiMeshNodeId = wifiMeshNodeId
    };
    deviceInfo[currentRegistedDeviceCount++] = newDevice;
    
    printf("Register a new device :\nDeviceTpye is [%d]\nDeviceMac is  [%s]\nRegistered is [%d]\nWifi Mesh NodeId is [%u]\n", newDevice.deviceTpye, newDevice.deviceMac.c_str(), newDevice.onOff, newDevice.wifiMeshNodeId);
    printf("Current Registered Decive Count is [%d]\n", currentRegistedDeviceCount);
    
    SendoutRegisteredSuccessMessage(wifiMeshNodeId);
  }
  else
  {
    printf("Decive [%u] had been existed", wifiMeshNodeId);
    //SendoutRegisteredSuccessMessage(wifiMeshNodeId);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  
  SetUpConnectedWiFiMeshCallback(&OnWiFiMeshConnectCallback);
  SetUpDisconnectedWiFiMeshCallback(&OnWiFiMeshDisconnectCallback);
  SetUpReceivedMessageCallback(&OnReceivedMessageCallback);

  SetUpWifiMesh();

  SetUpCallback(0, OnClickCallback);

  uint8_t mac_Address[6];
  WiFi.macAddress(mac_Address);
  mac_Address_str = String(mac_Address[0]);
  for(int i = 1; i < 6; i++)
  {
    mac_Address_str = String(mac_Address_str + ":");
    mac_Address_str = String(mac_Address_str + mac_Address[i]);
  }
  randomSeed(analogRead(0));
}

void loop()
{
  CheckButtonOnClick(0, buttonPin);
  delay(50);

  if (!isConnectedMeshNetwork) return;
  UpdateWifiMesh();
}
