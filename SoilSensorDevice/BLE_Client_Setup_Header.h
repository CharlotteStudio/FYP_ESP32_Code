#include "BLE_Client_Header.h"

static char* serviceUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
static char* characteristicUUID_SoilSensor = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
static char* characteristicUUID_WaterPump  = "beb5483e-36e1-4688-b7f5-ea07361b26a7";
static char* deviceName = "Soil_Sensor";

#define scanDuration 3               // scan BLE time
#define waitingTime_bleScaner 5000   // scan BLE and try connect will use this time

static unsigned long nextTime_bleScaner = 0;

void SetUpBLE()
{
  SetUpBLEDevices(deviceName);
  RegisterCharacteristic(characteristicUUID_SoilSensor);
}

void ScanBLEAndConnect()
{
  if(millis() > nextTime_bleScaner && !isFoundTargetDevice())
  {
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
