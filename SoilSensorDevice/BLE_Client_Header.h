/* Version 0.1.1
 *  
 * BLE Client ESP32 only
 * Resource : https://www.electronicshub.org/esp32-ble-tutorial/
 * 
 * The Git Hub of BLE
 * https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEDevice.h
 * https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEScan.h
 * https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEUUID.h
 * https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLERemoteCharacteristic.h
 * https://github.com/pcbreflux/espressif/blob/master/esp32/app/ESP32_ble_notify/main/GreatNeilKolbanLib/BLEAdvertisedDeviceCallbacks.h
 * 
 * Can use will BLE_Client_Setup_Header.h & ESP32_BLE_Client.ino
 * This is a simple set up demo
 * 
 */

#include "BLEDevice.h"

#define maxCharacteristicCount 8
#define maxScannedDeviceCount 64

static BLEUUID                  targetServiceUUID;
static BLEUUID                  targetCharacteristicArray[maxCharacteristicCount];

static BLEScan                  *bleScaner;
static BLEAdvertisedDevice      *targetDevice;
static BLEAdvertisedDevice      scannedDeviceArray[maxScannedDeviceCount];
static BLEClient                *bleClient;
static BLERemoteService         *remoteService;
static BLERemoteCharacteristic  *remoteCharacteristicArray[maxCharacteristicCount];

static int registeredCharacteristicCount = 0;
static int currentScannedDeviceCount = 0;

bool isFoundTargetDevice()
{
  return targetDevice != NULL;
}

bool isConnectedBLEService()
{
  if (bleClient == nullptr) return false;
  return bleClient->isConnected();
}

bool IsMatchAnyScannedDevice(std::string address)
{
  for (int i = 0; i < maxScannedDeviceCount; i++)
  {
    if (address.compare(scannedDeviceArray[i].getAddress().toString()) == 0) return true;
  }
  return false;
}

void ClearDeviceArray()
{
  for(int i = 0; i < maxScannedDeviceCount; i++)
    scannedDeviceArray[i] = BLEAdvertisedDevice();
}

typedef void (*ConnectedBLECallback)();
static ConnectedBLECallback connectedBLECallback;

typedef void (*DisconnectedBLECallback)();
static DisconnectedBLECallback disconnectedBLECallback;

void SetUpConnectedBLECallback(ConnectedBLECallback cb)
{
  connectedBLECallback = cb;
  Serial.println("set up Connected BLE Callback.");
}

void SetUpDisconnectedBLECallback(DisconnectedBLECallback cb)
{
  disconnectedBLECallback = cb;
  Serial.println("set up Disconnected BLE Callback.");
}

void DisconnectDevice();
int GetMatchCharacteristicIntex(char*);

// The callback class of Scan for BLE servers
class ScannedDeviceCallback: public BLEAdvertisedDeviceCallbacks
{
  // Callback for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    if (!IsMatchAnyScannedDevice(advertisedDevice.getAddress().toString()))
    {
      if (currentScannedDeviceCount < maxScannedDeviceCount)
      {
        scannedDeviceArray[currentScannedDeviceCount] = advertisedDevice;
        currentScannedDeviceCount++;      
      }
      
      Serial.print("BLE Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
    }
  }
};

class ConnectedCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient* bleClient)
  {
    Serial.println("Connected");
    ClearDeviceArray();
    
    if (connectedBLECallback == NULL) return;
    connectedBLECallback();
  }

  void onDisconnect(BLEClient* bleClient)
  {
    Serial.println("Disconnected");
    DisconnectDevice();

    if (disconnectedBLECallback == NULL) return;
    disconnectedBLECallback();
  }
};

void SetUpBLEDevices(char* deviceName)
{
  Serial.println("Start BLE Client.");
  
  BLEDevice::init(deviceName);
}

void RegisterCharacteristic(char* characteristic)
{
  targetCharacteristicArray[registeredCharacteristicCount] = BLEUUID(characteristic);
  printf("Register Characteristic Success : [%s]\n", characteristic);
  registeredCharacteristicCount++;
}

void ScanBLEDevices(float scanDuration)
{
  Serial.println("Start scan BLE Devices.");
  if (bleScaner == NULL)
  {
    // get the BLE Scaner
    bleScaner = BLEDevice::getScan();
    
    // set up scan successful callback
    bleScaner->setAdvertisedDeviceCallbacks(new ScannedDeviceCallback());
    
    bleScaner->setInterval(1349);
    bleScaner->setWindow(449);
    bleScaner->setActiveScan(true);
    bleScaner->start(scanDuration, false);
  }
  
  BLEDevice::getScan()->start(scanDuration);
}

void StopScanBLEDevices()
{
  Serial.println("Stop scan BLE Devices.");
  BLEDevice::getScan()->stop();
}

void FindTargetDevice(char* serverUID)
{
  targetServiceUUID = BLEUUID(serverUID);

  Serial.print("Try find server device : ");
  Serial.println(serverUID);

  for (int i = 0; i < maxScannedDeviceCount; i++)
  {
    if (!scannedDeviceArray[i].haveServiceUUID()) continue;
    if (!scannedDeviceArray[i].isAdvertisingService(targetServiceUUID)) continue;
    
    targetDevice = new BLEAdvertisedDevice(scannedDeviceArray[i]);
    Serial.println("Found Target Device.");
    StopScanBLEDevices();
    return;
  }
  Serial.println("Not Found Target Device.");
}

void NotifyCallback(BLERemoteCharacteristic* remoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
  Serial.print("Notify callback for characteristic ");
  Serial.print(remoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.println((char*)pData);
}

bool GetRemoteService()
{
  // Obtain a reference to the service we are after in the remote BLE server.
  remoteService = bleClient->getService(targetServiceUUID);
  if (remoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(targetServiceUUID.toString().c_str());
    DisconnectDevice();
    return false;
  }
  Serial.println("Found Service successful.");

  std::map<std::string, BLERemoteCharacteristic*>* characteristicMap = remoteService->getCharacteristics();
  int characteristicCount = characteristicMap->size();
  printf("The Remote Service Characteristic Count is [%d]\n", characteristicCount);

  for (auto it = characteristicMap->begin(); it != characteristicMap->end(); ++it)
  {
    const std::string& key = it->first;
    printf("The Characteristic is [%s]\n", key.c_str());
  }
  printf("\n");
  
  return true;
}

void RegisterRemoteCharacteristic()
{
  for (int i = 0; i < maxCharacteristicCount; i++)
  {
    printf("Try get the remote Service : [%s]\n", targetCharacteristicArray[i].toString().c_str());
    remoteCharacteristicArray[i] = remoteService->getCharacteristic(targetCharacteristicArray[i]);

    if (remoteCharacteristicArray[i] == nullptr)
    {
      printf("The Target Characteristic Array [%d] is null !\n", i);
    } 
    else
    {
      Serial.println("Found Device Characteristic successful.");
      if(remoteCharacteristicArray[i]->canNotify())
      {
        remoteCharacteristicArray[i]->registerForNotify(NotifyCallback);
        Serial.println("Register For Notify Listener.");
      }
    }
  }
  Serial.println("Register Characteristic successful.");
}

bool TryConnectTargetServer()
{
  if (!isFoundTargetDevice)
  {
    Serial.println("Have not find Target Device.");
    return false;
  }

  Serial.print("Forming a connection to ");
  Serial.println(targetDevice->getAddress().toString().c_str());

  bleClient = BLEDevice::createClient();
  Serial.println("Created Client");

  bleClient->setClientCallbacks(new ConnectedCallback());

  // Connect to BLE Server.
  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  bleClient->connect(targetDevice);

  if(!GetRemoteService()) return false;
  RegisterRemoteCharacteristic();
  
  return true;
}

void DisconnectDevice()
{
  if (bleClient->isConnected()) bleClient->disconnect();
  
  // Release BLEClient member
  delete bleClient;
  bleClient = nullptr;

  targetServiceUUID  = BLEUUID("");
  targetDevice       = NULL;
  currentScannedDeviceCount = 0;

  if (disconnectedBLECallback == NULL) return;
  disconnectedBLECallback();
}

void SendoutBLEMessage(char* characteristicUUID, String str)
{
  if (!isConnectedBLEService()) return;
  int matchIndex = GetMatchCharacteristicIntex(characteristicUUID);
  if (matchIndex == -1)
  {
    Serial.println("Do not find match BLECharacteristic.");
    return;
  }

  // Set the characteristic's value to be the array of bytes that is actually a string.
  remoteCharacteristicArray[matchIndex]->writeValue(str.c_str(), str.length());
}

String ReceivedBLEMessage(char* characteristicUUID)
{
  if (!isConnectedBLEService()) return "";
  int matchIndex = GetMatchCharacteristicIntex(characteristicUUID);
  if (matchIndex == -1)
  {
    Serial.println("Do not find match BLECharacteristic.");
    return "";
  }
  if (!remoteCharacteristicArray[matchIndex]->canRead()) return "";

  // Read the value of the characteristic.
  std::string value = remoteCharacteristicArray[matchIndex]->readValue();
  return value.c_str();
}

int GetMatchCharacteristicIntex(char *characteristic)
{
  for (int i = 0; i < maxCharacteristicCount; i++)
  {
    if(strcmp(targetCharacteristicArray[i].toString().c_str(), characteristic) == 0) return i;
  }
  return -1;
}
