#define maximum_device_count 32

typedef struct DeviceInfo DeviceInfo;
struct DeviceInfo
{
  String deviceMAC;
  int deviceTpye;
  int onOff;
  int value;
  unsigned int wifiMeshNodeId;
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

bool IsExistedDevice(unsigned int wifiMeshNodeId)
{
  return (GetExistedDeviceInt(wifiMeshNodeId) != -1);
}

unsigned int GetWiFiMeshNodeIdByMacAddress(String mac)
{
  for(int i = 0; i < maximum_device_count; i++)
  {
    if (deviceInfo[i].deviceMAC == mac)
      return deviceInfo[i].wifiMeshNodeId;
  }
  return 0;
}

bool IsExistedDevice(String mac)
{
  return (GetWiFiMeshNodeIdByMacAddress(mac) != 0);
}
