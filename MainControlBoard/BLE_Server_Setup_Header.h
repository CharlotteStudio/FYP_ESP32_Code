#include "BLE_Server_Header.h"

#define specialChannelCount 2
#define maxCharacteristicUUIDChannelCount maxCharacteristicCount-specialChannelCount

static char* serviceUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
static char* deviceName = "ESP32-BLE-Server";

static char* characteristicUUID_To      = "beb5483e-36e1-4688-b7f5-ea00361b26a0";  // for check target
static char* characteristicUUID_Message = "beb5483e-36e1-4688-b7f5-ea00361b26a1";  // for check json

static char* characteristicUUID_channel[maxCharacteristicUUIDChannelCount] = {
  "beb5483e-36e1-4688-b7f5-ea10121b26a1",
  "beb5483e-36e1-4688-b7f5-ea10121b26a2",
  "beb5483e-36e1-4688-b7f5-ea10121b26a3",
  "beb5483e-36e1-4688-b7f5-ea10121b26a4",
  "beb5483e-36e1-4688-b7f5-ea10121b26a5",
  "beb5483e-36e1-4688-b7f5-ea10121b26a6",
};

static String ble_empty = "_";

static int currentChannelIndex = 0;

void changedConnectionCallback()
{
  Serial.printf("changed connections\n");
}

void BLEConnectedCallback()
{
  //Serial.println("A BLE Device Connected.");
}

void BLEDisconnectedCallback()
{
  //Serial.println("A BLE Device Disconnected.");
}

void SetUpBLE()
{
  SetUpDeviceConnectedCallback(BLEConnectedCallback);
  SetUpDeviceDisconnectedCallback(BLEDisconnectedCallback);

  SetUpBLEServer(deviceName);
  
  SetUpBLEService(serviceUID);

  SetUpBLECharacteristic(characteristicUUID_To);
  SetUpBLECharacteristic(characteristicUUID_Message);
  for(int i = 0; i < maxCharacteristicUUIDChannelCount; i++)
  {
    SetUpBLECharacteristic(characteristicUUID_channel[i]);
  }
  
  SetCharacteristicMessage(characteristicUUID_To, ble_empty);
  SetCharacteristicMessage(characteristicUUID_Message, ble_empty);
  for(int i = 0; i < maxCharacteristicUUIDChannelCount; i++)
  {
    SetCharacteristicMessage(characteristicUUID_channel[i], ble_empty);
  }

  StartService();
  SetUpAdvertising();
}
