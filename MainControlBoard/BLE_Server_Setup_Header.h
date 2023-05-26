#include "BLE_Server_Header.h"

static char* serviceUID          = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
static char* deviceName          = "ESP32-BLE-Server";
static char* characteristicUUID_SoilSensor  = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
static char* characteristicUUID_Irrigation  = "beb5483e-36e1-4688-b7f5-ea07361b26a7";

String lastSoilString = "";
String toTriggerIrrigation = "";

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
