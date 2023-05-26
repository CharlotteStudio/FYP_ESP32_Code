sssss#include "BLE_Server_Header.h"
#include "painlessMesh.h"
#include "MyVariable_Header.h"
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Rx Tx
#define rxPin 13
#define txPin 12

#define jsonSerializeSize 64
#define jsonDeserializeSize 96

SoftwareSerial mySerial(rxPin, txPin);

// BLE
static char* serviceUID          = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
static char* deviceName          = "ESP32-BLE-Server";
static char* characteristicUUID_SoilSensor  = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
static char* characteristicUUID_Irrigation  = "beb5483e-36e1-4688-b7f5-ea07361b26a7";

// Wifi Mesh
#define   MESH_PREFIX     "TestingMeshWifi"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
uint32_t myNodeId;
painlessMesh mesh;

// Data
String name = "Testing Device";
String lastSoilString = "";
String toTriggerIrrigation = "";
int lastSoil = 0;
int triggerIrrigation = 2700;

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

void receivedCallback(uint32_t from, String &msg )
{
  Serial.printf("Received message from Node ID [%u] msg = [%s]\n", from, msg.c_str());
}

void changedConnectionCallback()
{
  Serial.printf("changed connections\n");
}

void BLEConnectedCallback()
{
  Serial.println("A BLE Device Connected.");
}

void BLEDisconnectedCallback()
{
  Serial.println("A BLE Device Disconnected.");
}

void setup()
{
  Serial.begin(115200);
  
  SetUpWifiSerial();
  SetUpBLE();
  SetUpWifiMesh();
}

void loop()
{
  SoftwareSerialReceive();
  
  String currentSoilValue = GetCharacteristicMessage(characteristicUUID_SoilSensor);
  
  if (currentSoilValue != lastSoilString)
  {
    lastSoilString = currentSoilValue;
    SoftwareSerialSendout();
  }

  CheckIrrigation();
  
  mesh.update();
}

void SetUpWifiSerial()
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(4800);
}

void SetUpBLE()
{
  SetUpDeviceConnectedCallback(BLEConnectedCallback);
  SetUpDeviceDisconnectedCallback(BLEDisconnectedCallback);

  SetUpBLEServer(deviceName);
  
  SetUpBLEService(serviceUID);

  SetUpBLECharacteristic(characteristicUUID_SoilSensor);
  SetUpBLECharacteristic(characteristicUUID_Irrigation);
  
  SetCharacteristicMessage(characteristicUUID_SoilSensor, lastSoilString);
  SetCharacteristicMessage(characteristicUUID_Irrigation, toTriggerIrrigation);

  SetUpAdvertising();
}

void SetUpWifiMesh()
{
  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(receivedCallback);
  mesh.onChangedConnections(changedConnectionCallback);

  Serial.print("Device Node ID : ");
  myNodeId = mesh.getNodeId();
  Serial.println(myNodeId);
}

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
