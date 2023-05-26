// Version 0.1.0, Release at 20230526
#include "BLE_Server_Setup_Header.h"
#include "DeviceInfo_Header.h"
#include "Info_Header.h"
#include "SoftwareSerial_Header.h"
#include "WiFiMesh_Header.h"
#include <ArduinoJson.h>

//void SoftwareSerialSendout();
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

  CheckIrrigation();
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

  unsigned int targetDevice = doc["To"].is<unsigned int>();
  Serial.print("Target is [");
  Serial.print(targetDevice);
  
  if (targetDevice != 1)
  {
    Serial.println("], is not me");
    return;
  }
  Serial.println("], is me");
  
  bool isRegisterMessage = doc["Register"].is<int>();
  int index = GetExistedDeviceInt(wifiMeshDeviceId);

  if (!isRegisterMessage && index != -1)
  {
    deviceInfo[index].value = doc["Value"];
    printf("Update value is [%d]", deviceInfo[index].value);
    // SEND OUT !
    return;
  }

  // Create a new  Device
  if (index == -1)
  {
    int deviceTpye = doc["DeviceTpye"];
    const char* mac = doc["DeviceMAC"];
    int onOff = doc["Register"];

    printf("Register a new device\nDeviceTpye is [%d]\nDeviceMAC is [%s]\nRegistered : [%d]\n", deviceTpye, mac, onOff);

    DeviceInfo newDevice = {
        .deviceMAC = String(mac),
        .deviceTpye = deviceTpye,
        .onOff = onOff,
        .value = 0,
        .wifiMeshDeviceId = wifiMeshDeviceId
    };
    deviceInfo[currentRegistedDeviceCount++] = newDevice;
    
    printf("Current Registered Decive Count is [%d]\n", currentRegistedDeviceCount);
    
    SendoutRegisteredSuccessMessage(wifiMeshDeviceId);
  }
  else
  {
    printf("Decive [%d] had been existed", wifiMeshDeviceId);
    SendoutRegisteredSuccessMessage(wifiMeshDeviceId);
  }
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
void CheckIrrigation()
{
  if (lastSoil > triggerIrrigation)
  {
    String str = GetCharacteristicMessage(characteristicUUID_Irrigation);
    
    if(str == "Off" || str == "")
    {
      Serial.println("Set Irrigation : On");
      SetCharacteristicMessage(characteristicUUID_Irrigation, "On");
    }
  }
}
*/
/*
void SoftwareSerialSendout()
{
  StaticJsonDocument<48> doc;
  
  doc["DeviceName"] = name;
  lastSoil = lastSoilString.toInt();
  doc["SoilHumidity"] = lastSoil;
   
  serializeJsonPretty(doc, mySerial);

  Serial.print("Send out json : ");
  serializeJsonPretty(doc, Serial);
}

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
