/* Version 0.1.0
 *  
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

#define maxCharacteristicCount 8

static BLEUUID           serviceUUID;
static BLEUUID           characteristicUUIDArray[maxCharacteristicCount];
static BLECharacteristic *myCharacteristicArray[maxCharacteristicCount];

static BLEServer         *myServer;
static BLEService        *myService;
static BLEAdvertising    *myAdvertising;

static int registeredCharacteristicCount = 0;

int GetMatchCharacteristicIntex(char*);

typedef void (*OnDeviceConnectedCallback)();
typedef void (*OnDeviceDisconnectedCallback)();
typedef void (*OnCharacteristicChangeCallback)(String);

static OnDeviceConnectedCallback    onDeviceConnectedCallback;
static OnDeviceDisconnectedCallback onDeviceDisconnectedCallback;
static OnCharacteristicChangeCallback onCharacteristicChangeCallback[maxCharacteristicCount];

void SetUpDeviceConnectedCallback(OnDeviceConnectedCallback cb) { onDeviceConnectedCallback = cb; }
void SetUpDeviceDisconnectedCallback(OnDeviceDisconnectedCallback cb) { onDeviceDisconnectedCallback = cb; }
void SetUpOnCharacteristicChangeCallback(OnCharacteristicChangeCallback cb, char *characteristic)
{
  int index = GetMatchCharacteristicIntex(characteristic);
  if (index == -1) return;

  onCharacteristicChangeCallback[index] = cb;
}

class OnServerCallbacks: public BLEServerCallbacks
{
    void onConnect(BLEServer* server)
    {
      BLEDevice::startAdvertising();
      printf("A device connnected.   Total Device is : [%d]\n", server->getConnectedCount()+1);
      onDeviceConnectedCallback();
    };

    void onDisconnect(BLEServer* server)
    {
      printf("A device disconnected. Total Device is : [%d]\n", server->getConnectedCount()-1);
      onDeviceDisconnectedCallback();
    }
};

class OnCharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic* pCharacteristic)
  {
    String uuid = pCharacteristic->getUUID().toString().c_str();
    
    int uuid_len = uuid.length() + 1; 
    char uuidBuf[uuid_len] ;
    uuid.toCharArray(uuidBuf, uuid_len);
  
    int index = GetMatchCharacteristicIntex(uuidBuf);
    String value = pCharacteristic->getValue().c_str();

    printf("On Characteristic Callbacks for [%s], value:\n%s\n", uuid.c_str(), value);
    
    if (index == -1)                                    return;
    if (onCharacteristicChangeCallback[index] == NULL)  return;
    
    onCharacteristicChangeCallback[index](value);
  }
};

void SetUpBLEServer(char* deviceName)
{
  BLEDevice::init(deviceName);
  myServer = BLEDevice::createServer();
  myServer->setCallbacks(new OnServerCallbacks());
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
  myCharacteristicArray[registeredCharacteristicCount]->setCallbacks(new OnCharacteristicCallbacks());
  printf("Create Characteristic successful : [%s]\n", myCharacteristicArray[registeredCharacteristicCount]->getUUID().toString().c_str());
  
  registeredCharacteristicCount++;
}

void StartService()
{
  myService->start();
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

void SetCharacteristicMessage(char* characteristicUUID, String str)
{
  if (myServer == NULL || myService == NULL) return;
  int matchIndex = GetMatchCharacteristicIntex(characteristicUUID);
  if (matchIndex == -1)
  {
    Serial.println("Do not find match BLECharacteristic.");
    return;
  }

  printf("[%s] set the Value : [%s]\n", characteristicUUID, str.c_str());
  myCharacteristicArray[matchIndex]->setValue(str.c_str());
}

String GetCharacteristicMessage(char* characteristicUUID)
{
  if (myServer == NULL || myService == NULL) return "";
  int matchIndex = GetMatchCharacteristicIntex(characteristicUUID);
  if (matchIndex == -1)
  {
    Serial.println("Do not find match BLECharacteristic.");
    return "";
  }

  std::string value = myCharacteristicArray[matchIndex]->getValue();
  printf("[%s] get the Value : [%s]\n", characteristicUUID, value.c_str());
  return value.c_str();
}

int GetMatchCharacteristicIntex(char *characteristic)
{
  for (int i = 0; i < maxCharacteristicCount; i++)
  {
    if(strcmp(characteristicUUIDArray[i].toString().c_str(), characteristic) == 0) return i;
  }
  printf("Can not find characteristic [%s]\n", characteristic);
  return -1;
}
