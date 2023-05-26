// Version 0.1.0, Release at 20230526
#include "BLE_Server_Setup_Header.h"
#include "DeviceInfo_Header.h"
#include "Info_Header.h"
#include "SoftwareSerial_Header.h"
#include "WiFiMesh_Header.h"
#include <ArduinoJson.h>

//void SoftwareSerialReceive();
void ReceivedMessageFormWiFiMesh(unsigned int, String&);
void SendoutRegisteredSuccessMessage(unsigned int);

void setup()
{
  SetUpSerial();
  Serial.begin(115200);
  
  SetUpBLE();
  SetUpReceivedMessageCallback(&ReceivedMessageFormWiFiMesh);
  SetUpWifiMesh();
}

void loop()
{
  //SoftwareSerialReceive();
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

void ReceivedMessageFormWiFiMesh(unsigned int wifiMeshDeviceId, String &json)
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
  int index = GetExistedDeviceInt(wifiMeshDeviceId);

  // Software Serial send out value
  if (!isRegisterMessage && index != -1)
  {
    deviceInfo[index].value = doc["Value"].as<int>();
    printf("Update value is [%d]\n", deviceInfo[index].value);
    
    doc["DeviceTpye"] = deviceInfo[index].deviceTpye;
    doc["DeviceMAC"] = deviceInfo[index].deviceMAC;
    
    serializeJsonPretty(doc, mySerial);
    Serial.println("Software Serial send out json : ");
    serializeJsonPretty(doc, Serial);
    return;
  }

  // Create a new Device
  if (index == -1)
  {
    DeviceInfo newDevice = {
        .deviceMAC = doc["DeviceMAC"].as<String>(),
        .deviceTpye = doc["DeviceTpye"].as<int>(),
        .onOff = doc["Register"].as<int>(),
        .value = 0,
        .wifiMeshDeviceId = wifiMeshDeviceId
    };
    deviceInfo[currentRegistedDeviceCount++] = newDevice;
    
    printf("Register a new device :\nDeviceTpye is [%d]\nDeviceMAC is  [%s]\nRegistered is [%d]\nWifi Mesh NodeId is [%u]\n", newDevice.deviceTpye, newDevice.deviceMAC.c_str(), newDevice.onOff, newDevice.wifiMeshDeviceId);
    printf("Current Registered Decive Count is [%d]\n", currentRegistedDeviceCount);

    SendoutRegisteredSuccessMessage(wifiMeshDeviceId);
  }
  else
  {
    printf("Decive [%u] had been existed, resend success message :\n", wifiMeshDeviceId);
    SendoutRegisteredSuccessMessage(wifiMeshDeviceId);
  }
  
  // Software Serial send out register
  doc.remove("To");

  serializeJsonPretty(doc, mySerial);
  Serial.println("Software Serial send out json : ");
  serializeJsonPretty(doc, Serial);
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

/*
void SoftwareSerialReceive()
{
  if(!mySerial.available()) return;
  
  StaticJsonDocument<jsonDeserializeSize> doc;

  DeserializationError error = deserializeJson(doc, mySerial);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  String targetName = String(doc["DeviceName"].as<const char*>());
  Serial.print("Received Json, ");
  Serial.print("Target Device is : ");
  Serial.println(targetName);

  int setHumidity = -1;
  int setActive   = -1;
  
  if(doc["SetHumidity"].is<int16_t>())
  {
    setHumidity = doc["SetHumidity"];
  }
  
  if(doc["SetActive"].is<int16_t>())
  {
    setActive = doc["SetActive"];
  }

  if(setHumidity != -1)
  {
    Serial.print("Set Humidity of Trigger Irrigation to ");
    Serial.println(setHumidity);
    triggerIrrigation = setHumidity;
  }

  if(setActive != -1)
  {
    Serial.print("Set Irrigation : ");
    if (setActive == 0)
    {
      Serial.println("Off");
      SetCharacteristicMessage(characteristicUUID_Irrigation, "Off");
    }
      
    if (setActive == 1)
    {
      SetCharacteristicMessage(characteristicUUID_Irrigation, "On");
      Serial.println("On");
    }
  }
}
*/
