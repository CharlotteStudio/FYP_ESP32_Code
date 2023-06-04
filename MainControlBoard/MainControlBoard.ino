#include "BLE_Server_Setup_Header.h"
#include "DeviceInfo_Header.h"
#include "Info_Header.h"
#include "SoftwareSerial_Header.h"
#include "WiFiMesh_Header.h"
#include <ArduinoJson.h>

#define ble_message_reset_time 10

static unsigned long nextTime_bleMessageReset = 0;
static bool isLockedBLETarget = false;
static bool isConnectedAWS = false;

static bool needWifi = true;

void ReceivedMessageFormWiFiMesh(unsigned int, String&);
void SendoutRegisteredSuccessMessage(unsigned int);

void OnTargetChange(int index, String str)
{
  if (!isConnectedAWS && needWifi)
  {
    Serial.println("Not connected AWS, reset message ...");
    SetCharacteristicMessage(characteristicUUID_To, ble_empty);
    SetCharacteristicMessage(characteristicUUID_Message, ble_empty);
    return;
  }
  Serial.print("Message Target is [");
  Serial.print(str);
  
  if (!str.equals("0"))
  {
    Serial.println("], is not me");
    if (str.equals(ble_empty))
    {
      Serial.println("Release BLE Locker.");
      isLockedBLETarget = false;
    }
    return;
  }
  Serial.println("], is me");

  String json = GetCharacteristicMessage(characteristicUUID_Message);
  ReceivedMessageFormBLE(json);
}

void OnValueChannelChangeCallback(int index, String str)
{
  int channelIndex = index - specialChannelCount;
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].bleChannel == channelIndex)
    {
      StaticJsonDocument<jsonDeserializeRegisterSize> doc;
      
      deviceInfo[i].value = str.toInt();
      printf("Update value is [%d]\n", deviceInfo[i].value);
      
      doc["Value"]      = deviceInfo[i].value;
      doc["DeviceTpye"] = deviceInfo[i].deviceTpye;
      doc["DeviceMac"]  = deviceInfo[i].deviceMac;
      
      serializeJsonPretty(doc, mySerial);
      Serial.println("Software Serial send out json : ");
      serializeJsonPretty(doc, Serial);
      Serial.println("");
      return;
    }
  }
  printf("Unregister device [%d] message : [%s]\n", channelIndex, str.c_str());
}

void setup()
{
  SetUpSerial();
  Serial.begin(115200);
  
  SetUpBLE();
  SetUpReceivedMessageCallback(&ReceivedMessageFormWiFiMesh);
  SetUpWifiMesh();

  // The call back function will case BLE Deserialize json error, therefore closed that
  /*
  SetUpOnCharacteristicChangeCallback(OnTargetChange, characteristicUUID_To);
  for(int i = 0; i < maxCharacteristicUUIDChannelCount; i++)
  {
    SetUpOnCharacteristicChangeCallback(OnValueChannelChangeCallback, characteristicUUID_channel[i]);
  }
  */

  SoftwareSerialSendout("Check State");
}

void loop()
{
  SoftwareSerialReceiveAndSendout();

  if (!isConnectedAWS && needWifi) { delay(50); return; }
  
  UpdateBLECallback();
  UpdateWifiMesh();

  int triggerActiveDevice = CheckDeviceListener();
  if (triggerActiveDevice != -1)
  {
    AutoSendActiveMessage(triggerActiveDevice);
  }

  if (isLockedBLETarget && millis() > nextTime_bleMessageReset)
  {
    isLockedBLETarget = false;
    SetCharacteristicMessage(characteristicUUID_To, ble_empty);
    SetCharacteristicMessage(characteristicUUID_Message, ble_empty);
  }

  delay(50);
}

void UpdateBLECallback()
{
  String toStr = GetCharacteristicMessage(characteristicUUID_To);
  if (!toStr.equals(ble_empty))
  {
    OnTargetChange(0, toStr);
  }

  for(int i = 0; i < maxCharacteristicUUIDChannelCount; i++)
  {
    int channel = i+specialChannelCount;
    String str = GetCharacteristicMessage(characteristicUUID_channel[i]);

    if (str.equals(ble_empty)) continue;
    
    OnValueChannelChangeCallback(channel, str);
    SetCharacteristicMessage(characteristicUUID_channel[i], ble_empty);
  }
}

void SoftwareSerialSendout(String str)
{
  StaticJsonDocument<jsonSerializeRegisterSize> doc;

  doc["message"] = str;
  Serial.println("Send out to SoftwareSerial :");
  serializeJsonPretty(doc, Serial);
  Serial.println();
  
  // SoftwareSerialSendout
  serializeJsonPretty(doc, mySerial);
}

void ReceivedMessageFormBLE(String &json)
{
  if(currentChannelIndex >= maxCharacteristicCount)
  {
    Serial.println("BLE Channel is full, please using WiFi-Mesh.");
    // ToDo send stop message
    return;
  }
  
  StaticJsonDocument<jsonDeserializeRegisterSize> doc;

  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Software Serial send out register
  serializeJsonPretty(doc, mySerial);
  Serial.println("Software Serial send out json : ");
  serializeJsonPretty(doc, Serial);
  Serial.println("");

  String mac = doc["DeviceMac"].as<String>();

  if (!IsExistedDevice(mac))
  {
    CreateNewDevice(mac, doc["DeviceTpye"].as<int>(), doc["Register"].as<int>(), 0, currentChannelIndex);
    SendoutRegisteredSuccessMessage_BLE(mac, currentChannelIndex);
    currentChannelIndex++;
  }
  else
  {
    int index = GetExistedDeviceIndex(mac);
    int channel = deviceInfo[index].bleChannel;
    deviceInfo[index].wifiMeshNodeId = 0;
    
    printf("Decive [%s] had been existed, resend success message :\nReset wifiMeshNodeId to [%u]\n", mac.c_str(), deviceInfo[index].wifiMeshNodeId);
    if (channel == -1)
    {
      printf("have not assign channel, assign a new channel [%d]n", currentChannelIndex);
      deviceInfo[index].bleChannel = currentChannelIndex;
      currentChannelIndex++;
    }

    SendoutRegisteredSuccessMessage_BLE(mac, deviceInfo[index].bleChannel);
  }
}

void SendoutRegisteredSuccessMessage_BLE(String mac, int channelNumber)
{
  Serial.println("Send out registered success at BLE");
  
  StaticJsonDocument<jsonSerializeDataSize> doc;
  doc["Register"] = 1;
  doc["Channel"] = channelNumber;
  String str;
  serializeJsonPretty(doc, str);
  serializeJsonPretty(doc, Serial);
  Serial.println("");
  
  SetCharacteristicMessage(characteristicUUID_To, mac);
  SetCharacteristicMessage(characteristicUUID_Message, str);
  nextTime_bleMessageReset = millis() + ble_message_reset_time;
}

void ReceivedMessageFormWiFiMesh(unsigned int wifiMeshNodeId, String &json)
{
  StaticJsonDocument<jsonDeserializeRegisterSize> doc;

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
  int index = GetExistedDeviceIndex(wifiMeshNodeId);

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
    String mac = doc["DeviceMac"].as<String>();

    // CHeck the device is registered by BLE
    if(IsExistedDevice(mac))
    {
      SetNodeIdByDeviceMac(mac, wifiMeshNodeId);
    }
    else
    {
      CreateNewDevice(mac, doc["DeviceTpye"].as<int>(), doc["Register"].as<int>(), wifiMeshNodeId, -1);
    }
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
  StaticJsonDocument<jsonSerializeDataSize> doc;
  doc["To"] = target;
  doc["Register"] = 1;
  String str;
  serializeJsonPretty(doc, str);
  SendoutWifiMesh(str);
}

void SoftwareSerialReceiveAndSendout()
{
  if(!mySerial.available()) return;
  
  StaticJsonDocument<jsonDeserializeAWSDataSize> doc;

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

// Checking the device is existed.
  if (!IsExistedDevice(mac_address))
  {
    printf("Don't find the device by address [%s]\n", mac_address.c_str());
    return;
  }

// Send out to WiFi Mesh
  StaticJsonDocument<jsonSerializeDataSize> sendoutDoc;

  if (doc["ActiveState"].is<int>())    sendoutDoc["ActiveState"]    = doc["ActiveState"];
  if (doc["SetUpdateSpeed"].is<int>()) sendoutDoc["SetUpdateSpeed"] = doc["SetUpdateSpeed"];
  if (doc["OwnerDevice"].is<int>())    sendoutDoc["OwnerDevice"]    = doc["OwnerDevice"];
  if (doc["ActiveValue"].is<int>())    sendoutDoc["ActiveValue"]    = doc["ActiveValue"];

  unsigned int nodeId = GetWiFiMeshNodeIdByMacAddress(mac_address);
  String str;

  if (nodeId == 0)
  {
    printf("The message target mac is [%s], but no nodeId, send by BLE.\n", mac_address.c_str());
    serializeJsonPretty(sendoutDoc, str);
    SetCharacteristicMessage(characteristicUUID_To, mac_address);
    SetCharacteristicMessage(characteristicUUID_Message, str);
  }
  else
  {
    printf("The message target mac is [%s], nodeId is [%u], send by Wifi-mesh.\n", mac_address.c_str(), nodeId);
    sendoutDoc["To"] = nodeId;

    serializeJsonPretty(sendoutDoc, str);
    SendoutWifiMesh(str);
  }
}

void AutoSendActiveMessage(int index)
{
  StaticJsonDocument<jsonSerializeDataSize> doc;

  doc["ActiveState"] = 1;
  String str;
  
  if (deviceInfo[index].wifiMeshNodeId == 0)
  {
    printf("Send out active message to [%s] by BLE\n", deviceInfo[index].deviceMac );
    serializeJsonPretty(doc, str);
    SetCharacteristicMessage(characteristicUUID_To, deviceInfo[index].deviceMac);
    SetCharacteristicMessage(characteristicUUID_Message, str);
  }
  else
  {
    printf("Send out active message to [%s] by Wifi\n", deviceInfo[index].deviceMac );
    
    doc["To"] = deviceInfo[index].wifiMeshNodeId;
    serializeJsonPretty(doc, str);
    SendoutWifiMesh(str);
  }
}
