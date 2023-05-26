/*
 * BLE Server, ESP32 only
 * Resource : https://www.electronicshub.org/esp32-ble-tutorial/
 * 
 * The Git Hub of BLE
 * https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLECharacteristic.h
 *
 * Functions :
 * void SetUpBLEServer(char* deviceName)
 * void SetUpBLEService(char* serviceUID)
 * void SetUpBLECharacteristic(char* characteristicUUID)
 * void SetUpAdvertising()
 * void SetCharacteristicMessage(char* characteristicUUID, String str)
 * String GetCharacteristicMessage(char* characteristicUUID)
 * int GetMatchCharacteristicIntex(char *characteristic)
 * 
 */
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define characteristicSize 8

typedef void (*OnDeviceConnectedCallback)();
typedef void (*OnDeviceDisconnectedCallback)();

OnDeviceConnectedCallback    onDeviceConnectedCallback;
OnDeviceDisconnectedCallback onDeviceDisconnectedCallback;

static BLEUUID           serviceUUID;
static BLEUUID           characteristicUUIDArray[characteristicSize];
static BLECharacteristic *myCharacteristicArray[characteristicSize];

static BLEServer         *myServer;
static BLEService        *myService;
static BLEAdvertising    *myAdvertising;

int registeredCharacteristicCount = 0;

void SetUpDeviceConnectedCallback(OnDeviceConnectedCallback cb)
{
  onDeviceConnectedCallback = cb;
}

void SetUpDeviceDisconnectedCallback(OnDeviceDisconnectedCallback cb)
{
  onDeviceDisconnectedCallback = cb;
}

class MyServerCallbacks: public BLEServerCallbacks
{
    void onConnect(BLEServer* server)
    {
      Serial.print("A device connnected. Total Device is : ");
      Serial.println(server->getConnectedCount());
      onDeviceConnectedCallback();
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* server)
    {
      Serial.print("A device disconnected. Total Device is : ");
      Serial.println(server->getConnectedCount());
      onDeviceDisconnectedCallback();
    }
};

void SetUpBLEServer(char* deviceName)
{
  BLEDevice::init(deviceName);
  myServer = BLEDevice::createServer();
  myServer->setCallbacks(new MyServerCallbacks());
  Serial.println("Set up Server successful.");
}

void SetUpBLEService(char* serviceUID)
{
  if(myServer == NULL)
  {
    Serial.println("Do not set up BLE Server");
    return;
  }
  
  serviceUUID = BLEUUID(serviceUID);
  myService = myServer->createService(serviceUUID);
  myService->start();
  
  Serial.println("Set up Service successful.");
}

void SetUpBLECharacteristic(char* characteristicUUID)
{
  if(myServer == NULL)
  {
    Serial.println("Do not set up BLE Server");
    return;
  }
  if(myService == NULL)
  {
    Serial.println("Do not set up BLE Service");
    return;
  }
  
  characteristicUUIDArray[registeredCharacteristicCount] = BLEUUID(characteristicUUID);

  myCharacteristicArray[registeredCharacteristicCount] = myService->createCharacteristic(
      characteristicUUIDArray[registeredCharacteristicCount],
      BLECharacteristic::PROPERTY_READ | 
      BLECharacteristic::PROPERTY_WRITE
      );
  myService->start();
  registeredCharacteristicCount++;
}

void SetUpAdvertising()
{
  if(myServer == NULL)
  {
    Serial.println("Do not set up BLE Server");
    return;
  }
  if(myService == NULL)
  {
    Serial.println("Do not set up BLE Service");
    return;
  }
  
  myAdvertising = BLEDevice::getAdvertising();
  
  myAdvertising->addServiceUUID(serviceUUID);
  myAdvertising->setScanResponse(true);
  myAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  myAdvertising->setMinPreferred(0x12);
  
  BLEDevice::startAdvertising();
  
  Serial.println("Set up Advertising successful.");
}

int GetMatchCharacteristicIntex(char *characteristic)
{
  for (int i = 0; i < characteristicSize; i++)
  {
    if(strcmp(characteristicUUIDArray[i].toString().c_str(), characteristic) == 0) return i;
  }
  return -1;
}

void SetCharacteristicMessage(char* characteristicUUID, String str)
{
  if (myServer == NULL || myService == NULL) return;
  int matchIndex = GetMatchCharacteristicIntex(characteristicUUID);
  if (matchIndex == -1)
  {
    Serial.println("Do not find match BLECharacteristic.");
    return;
  }

  Serial.print("Set the Value : ");
  Serial.println(str);
  myCharacteristicArray[matchIndex]->setValue(str.c_str());
}

String GetCharacteristicMessage(char* characteristicUUID)
{
  if (myServer == NULL || myService == NULL) return "";
  int matchIndex = GetMatchCharacteristicIntex(characteristicUUID);
  if (matchIndex == -1)
  {
    //Serial.println("Do not find match BLECharacteristic.");
    return "";
  }

  std::string value = myCharacteristicArray[matchIndex]->getValue();
  //Serial.print("Get the Value : ");
  //Serial.println(value.c_str());
  return value.c_str();
}
