#include "BLE_Server_Setup_Header.h"
#include "DeviceInfo_Header.h"
#include "Info_Header.h"
#include "SoftwareSerial_Header.h"
#include "WiFiMesh_Header.h"
#include <ArduinoJson.h>

void ReceivedMessageFormWiFiMesh(unsigned int, String&);
void SendoutRegisteredSuccessMessage(unsigned int);

static bool isConnectedAWS = false;

void OnTargetChange(String str)
{
  Serial.print("Message Target is [");
  Serial.print(str);
  
  if (!str.equals("0"))
  {
    Serial.println("], is not me");
    return;
  }
  Serial.println("], is me");

  String json = GetCharacteristicMessage(characteristicUUID_Message);
  ReceivedMessageFormBLE(json);
}


void setup()
{
  SetUpSerial();
  Serial.begin(115200);
  
  SetUpBLE();
  SetUpReceivedMessageCallback(&ReceivedMessageFormWiFiMesh);
  SetUpWifiMesh();

  SetUpOnCharacteristicChangeCallback(OnTargetChange, characteristicUUID_To);
}

void loop()
{
  SoftwareSerialReceiveAndSendout();

  if (!isConnectedAWS) { delay(50); return; }
  
  UpdateWifiMesh();
  /*
  String currentSoilValue = GetCharacteristicMessage(characteristicUUID_SoilSensor);
  
  if (currentSoilValue != lastSoilString)
  {
    lastSoilString = currentSoilValue;
    SoftwareSerialSendout();
  }
  */
  delay(50);
}

void ReceivedMessageFormBLE(String &json)
{
  if(currentChannelIndex >= maxCharacteristicCount)
  {
    Serial.println("BLE Channel is full, please using WiFi-Mesh.");
    // ToDo send stop message
    return;
  }
  
  StaticJsonDocument<jsonDeserializeSize> doc;

  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  String mac = doc["DeviceMac"].as<String>();
  
  if (!IsExistedDevice(mac))
  {
    DeviceInfo newDevice = {
        .deviceMac = mac,
        .deviceTpye = doc["DeviceTpye"].as<int>(),
        .onOff = doc["Register"].as<int>(),
        .value = 0,
        .wifiMeshNodeId = 0,
        .bleChannel = currentChannelIndex
    };
    deviceInfo[currentRegistedDeviceCount++] = newDevice;
    
    printf("Register a new device :\nDeviceTpye is [%d]\nDeviceMac is  [%s]\nRegistered is [%d]\nWifi Mesh NodeId is [%u]\nbleChannel is [%d]\n", newDevice.deviceTpye, newDevice.deviceMac.c_str(), newDevice.onOff, newDevice.wifiMeshNodeId, newDevice.bleChannel);
    printf("Current Registered Decive Count is [%d]\n", currentRegistedDeviceCount);

    SendoutRegisteredSuccessMessage_BLE(mac, newDevice.bleChannel);
    currentChannelIndex++;
  }
  else
  {
    printf("Decive [%s] had been existed, resend success message :\n", mac.c_str());
    SendoutRegisteredSuccessMessage_BLE(mac, deviceInfo[GetExistedDeviceInt(mac)].bleChannel);
  }
  
  // Software Serial send out register
  doc.remove("To");

  serializeJsonPretty(doc, mySerial);
  Serial.println("Software Serial send out json : ");
  serializeJsonPretty(doc, Serial);
  Serial.println("");
}

void SendoutRegisteredSuccessMessage_BLE(String mac, int channelNumber)
{
  Serial.println("Send out registered success at BLE");
  SetCharacteristicMessage(characteristicUUID_To, mac);
  SetCharacteristicMessage(characteristicUUID_Message, String(currentChannelIndex));
}

void ReceivedMessageFormWiFiMesh(unsigned int wifiMeshNodeId, String &json)
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
  
  if (targetDevice != 0)
  {
    Serial.println("], is not me");
    return;
  }
  Serial.println("], is me");
  
  bool isRegisterMessage = doc["Register"].is<int>();
  int index = GetExistedDeviceInt(wifiMeshNodeId);

  // Software Serial send out value
  if (!isRegisterMessage && index != -1)
  {
    deviceInfo[index].value = doc["Value"].as<int>();
    printf("Update value is [%d]\n", deviceInfo[index].value);
    
    doc["DeviceTpye"] = deviceInfo[index].deviceTpye;
    doc["DeviceMac"] = deviceInfo[index].deviceMac;
    
    serializeJsonPretty(doc, mySerial);
    Serial.println("Software Serial send out json : ");
    serializeJsonPretty(doc, Serial);
    Serial.println("");
    return;
  }

  // Create a new Device
  if (index == -1)
  {
    DeviceInfo newDevice = {
        .deviceMac = doc["DeviceMac"].as<String>(),
        .deviceTpye = doc["DeviceTpye"].as<int>(),
        .onOff = doc["Register"].as<int>(),
        .value = 0,
        .wifiMeshNodeId = wifiMeshNodeId,
        .bleChannel = -1
    };
    deviceInfo[currentRegistedDeviceCount++] = newDevice;
    
    printf("Register a new device :\nDeviceTpye is [%d]\nDeviceMac is  [%s]\nRegistered is [%d]\nWifi Mesh NodeId is [%u]\n", newDevice.deviceTpye, newDevice.deviceMac.c_str(), newDevice.onOff, newDevice.wifiMeshNodeId);
    printf("Current Registered Decive Count is [%d]\n", currentRegistedDeviceCount);

    SendoutRegisteredSuccessMessage(wifiMeshNodeId);
  }
  else
  {
    printf("Decive [%u] had been existed, resend success message :\n", wifiMeshNodeId);
    SendoutRegisteredSuccessMessage(wifiMeshNodeId);
  }
  
  // Software Serial send out register
  doc.remove("To");

  serializeJsonPretty(doc, mySerial);
  Serial.println("Software Serial send out json : ");
  serializeJsonPretty(doc, Serial);
  Serial.println("");
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

void SoftwareSerialReceiveAndSendout()
{
  if(!mySerial.available()) return;
  
  StaticJsonDocument<jsonSerializeAWSDataSize> doc;

  DeserializationError error = deserializeJson(doc, mySerial);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  bool isMessage = doc["message"].is<String>();
  if (isMessage)
  {
    Serial.println("Get a message :");
    serializeJsonPretty(doc, Serial);
    Serial.println("");
    String command = doc["message"].as<String>();

    if (command.equalsIgnoreCase("Connected AWS"))
    {
      isConnectedAWS = true;
      Serial.println("Is connected AWS Server.");
    }
    return;
  }
  
  String mac_address = doc["DeviceMac"].as<String>();

  if (!IsExistedDevice(mac_address))
  {
    printf("Don't find the device by address [%s]\n", mac_address.c_str());
    return;
  }

  // Send out to WiFi Mesh
  StaticJsonDocument<jsonSerializeDataSize> sendoutDoc;

  sendoutDoc["To"] = GetWiFiMeshNodeIdByMacAddress(mac_address);
  
  if (doc["ActiveState"].is<int>())    sendoutDoc["ActiveState"]    = doc["ActiveState"];
  if (doc["SetUpdateSpeed"].is<int>()) sendoutDoc["SetUpdateSpeed"] = doc["SetUpdateSpeed"];
  if (doc["OwnerDevice"].is<int>())    sendoutDoc["OwnerDevice"]    = doc["OwnerDevice"];
  if (doc["ActiveValue"].is<int>())    sendoutDoc["ActiveValue"]    = doc["ActiveValue"];

  String str;
  serializeJsonPretty(sendoutDoc, str);
  SendoutWifiMesh(str);
}
