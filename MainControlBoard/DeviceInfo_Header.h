#define maximum_device_count 32

#define pump_mac_address_1 "176:178:28:167:24:192"
#define pump_mac_address_2 "120:33:132:225:22:76"
#define soil_mac_address_1 "176:178:28:166:252:228"
#define soil_mac_address_2 "120:33:132:224:254:84"

typedef struct DeviceInfo DeviceInfo;
struct DeviceInfo
{
  String deviceMac;
  int deviceTpye;
  int bleChannel;
  unsigned int wifiMeshNodeId;
  int onOff;
  int value;
  String listenDevice;
  int activeValue;
  int activeState;
};

static DeviceInfo deviceInfo[maximum_device_count];
static int currentRegistedDeviceCount = 0;

void PrintDeviceInfo(int index)
{
  printf("DeviceInfo     : [%d]\n", index);
  printf("deviceTpye     : [%d]\ndeviceMac      : [%s]\n", deviceInfo[index].deviceTpye, deviceInfo[index].deviceMac.c_str());
  printf("onOff          : [%d]\nvalue          : [%d]\n", deviceInfo[index].onOff, deviceInfo[index].value);
  printf("wifiMeshNodeId : [%u]\nbleChannel     : [%d]\n", deviceInfo[index].wifiMeshNodeId, deviceInfo[index].bleChannel);
  printf("listen Device  : [%s]\n", deviceInfo[index].listenDevice.c_str());
  printf("active Value   : [%d]\nactive State   : [%d]\n", deviceInfo[index].activeValue, deviceInfo[index].activeState);
}

// This is hard code
String GetListenDevice(String mac)
{
  if (mac.equals(pump_mac_address_1)) return soil_mac_address_1;
  if (mac.equals(pump_mac_address_2)) return soil_mac_address_2;
  
  return "";
}

int GetActiveValue(int deviceType)
{
  switch(deviceType)
  {
    // water pump
    case 2:
      return 2500;
   
    default:
      return -1;
  }
}

void CreateNewDevice(String mac, int deviceType, int onOff, unsigned int nodeId, int channel)
{
  DeviceInfo newDevice = {
      .deviceMac = mac,
      .deviceTpye = deviceType,
      .bleChannel = channel,
      .wifiMeshNodeId = nodeId,
      .onOff = onOff,
      .value = -1,
      .listenDevice = GetListenDevice(mac),
      .activeValue = GetActiveValue(deviceType),
      .activeState = 0,
  };
  deviceInfo[currentRegistedDeviceCount] = newDevice;

  printf("Register a new device :\n");
  PrintDeviceInfo(currentRegistedDeviceCount);
  printf("Current Registered Decive Count is [%d]\n", currentRegistedDeviceCount+1);
  currentRegistedDeviceCount++;
}

int GetExistedDeviceIndex(unsigned int wifiMeshNodeId)
{
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].wifiMeshNodeId == wifiMeshNodeId)
      return i;
  }
  return -1;
}

int GetExistedDeviceIndex(String mac)
{
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].deviceMac == mac)
      return i;
  }
  return -1;
}

bool IsExistedDevice(unsigned int wifiMeshNodeId)
{
  return (GetExistedDeviceIndex(wifiMeshNodeId) != -1);
}

unsigned int GetWiFiMeshNodeIdByMacAddress(String mac)
{
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].deviceMac == mac)
      return deviceInfo[i].wifiMeshNodeId;
  }
  return 0;
}

bool IsExistedDevice(String mac)
{
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].deviceMac == mac)
      return true;
  }
  return false;
}

void SetNodeIdByDeviceMac(String mac, unsigned int nodeId)
{
  int index = GetExistedDeviceIndex(mac);
  deviceInfo[index].wifiMeshNodeId = nodeId;
  printf("Decive [%s] had been existed, resend success message :\nReset wifiMeshNodeId to [%u]\n", mac.c_str(), deviceInfo[index].wifiMeshNodeId);
}

int CheckDeviceListener()
{
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].deviceTpye == 2)
    {
      bool isExistedListenDevice = IsExistedDevice(deviceInfo[i].listenDevice);
      if (!isExistedListenDevice) continue;

      int listenDeviceIndex = GetExistedDeviceIndex(deviceInfo[i].listenDevice);
      
      if (deviceInfo[listenDeviceIndex].value == -1)                       continue;
      if (deviceInfo[listenDeviceIndex].value < deviceInfo[i].activeValue) continue;
      if (deviceInfo[i].activeState == 1)                                  continue;

      deviceInfo[i].activeState = 1;
      printf("Device [%s] trigger active.\nListen Device value [%d] >= active Value [%d]\n", deviceInfo[i].deviceMac.c_str(), deviceInfo[listenDeviceIndex].value, deviceInfo[i].activeValue);
      return i;
    }
  }
  return -1;
}
