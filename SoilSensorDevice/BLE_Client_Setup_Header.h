#include "BLE_Client_Header.h"

#define maxCharacteristicUUIDChannelCount maxCharacteristicCount-2

static char* serviceUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
static char* deviceName = "Soil_Sensor";

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

#define scanDuration 3               // scan BLE time
#define waitingTime_bleScaner 5000   // scan BLE and try connect will use this time

static unsigned long nextTime_bleScaner = 0;

void SetUpBLE()
{
  SetUpBLEDevices(deviceName);
  RegisterCharacteristic(characteristicUUID_To);
  RegisterCharacteristic(characteristicUUID_Message);

  for(int i = 0; i < maxCharacteristicUUIDChannelCount; i++)
  {
    RegisterCharacteristic(characteristicUUID_channel[i]);
  }
}

void ScanBLEAndConnect()
{
  if(millis() > nextTime_bleScaner && !isFoundTargetDevice())
  {
    Serial.println("Scan BLE ...");
    nextTime_bleScaner = millis() + waitingTime_bleScaner;

    // must !!! complete the scan time then keep going
    ScanBLEDevices(scanDuration);
    FindTargetDevice(serviceUID);
  }

  if (isFoundTargetDevice() && !isConnectedBLEService() && millis() > nextTime_bleScaner)
  {
    nextTime_bleScaner = millis() + waitingTime_bleScaner;
    
    if (TryConnectTargetServer())
      Serial.println("Completed connect to BLE Server.");
    else
      Serial.println("Failed connect to BLE server.");
  }
}
