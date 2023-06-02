#define maximum_device_count 32

typedef struct DeviceInfo DeviceInfo;
struct DeviceInfo
{
  String deviceMac;
  int deviceTpye;
  int onOff;
  int value;
  unsigned int wifiMeshNodeId;
  int bleChannel;
};

static DeviceInfo deviceInfo[maximum_device_count];
static int currentRegistedDeviceCount = 0;

int GetExistedDeviceInt(unsigned int wifiMeshNodeId)
{
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].wifiMeshNodeId == wifiMeshNodeId)
      return i;
  }
  return -1;
}

int GetExistedDeviceInt(String mac)
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
  return (GetExistedDeviceInt(wifiMeshNodeId) != -1);
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

void PrintDeviceInfo(int index)
{
  printf("DeviceInfo     : [%d]\n", index);
  printf("deviceTpye     : [%d]\ndeviceMac      : [%s]\n", deviceInfo[index].deviceTpye, deviceInfo[index].deviceMac.c_str());
  printf("onOff          : [%d]\nvalue          : [%d]\n", deviceInfo[index].onOff, deviceInfo[index].value);
  printf("wifiMeshNodeId : [%u]\nbleChannel     : [%d]\n", deviceInfo[index].wifiMeshNodeId, deviceInfo[index].bleChannel);
}
