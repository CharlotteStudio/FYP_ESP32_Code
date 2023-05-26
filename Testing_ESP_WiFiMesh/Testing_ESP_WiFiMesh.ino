// Version 0.1.0, Release at 20230523
#include "WiFiMesh_Header.h"
#include "Button_Header.h"
#include "Info_Header.h"
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <ArduinoJson.h>

#define maximum_device_count 32
#define buttonPin 2

String mac_Address_str;

typedef struct DeviceInfo DeviceInfo;
struct DeviceInfo
{
    String deviceMAC;
    int deviceTpye;
    int onOff;
    int value;
    unsigned int wiFiMeshDeviceId;
};

DeviceInfo deviceInfo[maximum_device_count];
static int currentRegistedDeviceCount = 0;

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
    doc["DeviceMAC"] = mac_Address_str;
    doc["Register"] = 1;
    serializeJsonPretty(doc, str);
  }
  else
  {
    StaticJsonDocument<jsonSerializeDataSize> doc;
    doc["To"] = 0;
    doc["Value"] = 1000;
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

void DeserializateJson(unsigned int wiFiMeshDeviceId, String json)
{
  StaticJsonDocument<jsonDeserializeSize> doc;

  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  unsigned int targetDevice = doc["To"].is<unsigned int>();
  Serial.print("Target is [");
  Serial.print(targetDevice);
  if (targetDevice == GetMyNodeId())
  {
    Serial.println("], is me");
  }
  else
  {
    Serial.println("], is not me");
  }

  bool isRegisterMessage = doc["Register"].is<int>();
  int index = GetExistedDeviceInt(wiFiMeshDeviceId);
  if (isRegisterMessage)
  {
    if (index == -1)
    {
      int deviceTpye = doc["DeviceTpye"];
      const char* mac = doc["DeviceMAC"];
      int onOff = doc["Register"];

      Serial.println("Register a new device");
      Serial.print("DeviceTpye is [");
      Serial.print(deviceTpye);
      Serial.print("]\nDeviceMAC is  [");
      Serial.print(mac);
      Serial.print("]\nRegister : ");
      Serial.println(onOff);

      DeviceInfo newDevie = {
          .deviceMAC = String(mac),
          .deviceTpye = deviceTpye,
          .onOff = onOff,
          .value = 0,
          .wiFiMeshDeviceId = wiFiMeshDeviceId
      };
      
      deviceInfo[currentRegistedDeviceCount++] = newDevie;

      Serial.print("Current Registered Decive Count is ");
      Serial.println(currentRegistedDeviceCount);
      SendoutRegisteredSuccessMessage(wiFiMeshDeviceId);
    }
    else
    {
      Serial.print("Decive [");
      Serial.print(wiFiMeshDeviceId);
      Serial.println("] had been existed");
    }
  }
  else
  {
    deviceInfo[index].value = doc["Value"];
    Serial.print("Update value is [");
    Serial.print(deviceInfo[index].value);
    Serial.println("]");
  }
}

bool IsExistedDevice(unsigned int wiFiMeshDeviceId) { return (GetExistedDeviceInt(wiFiMeshDeviceId) != -1); }

int GetExistedDeviceInt(unsigned int wiFiMeshDeviceId)
{
    for(int i = 0; i < maximum_device_count; i++)
    {
      if (deviceInfo[i].wiFiMeshDeviceId == wiFiMeshDeviceId)
        return i;
    }
    return -1;
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
}

void loop()
{
  CheckButtonOnClick(0, buttonPin);
  delay(50);

  if (!isConnectedMeshNetwork) return;
  UpdateWifiMesh();
}
