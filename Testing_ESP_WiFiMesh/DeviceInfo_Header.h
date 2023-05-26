#define maximum_device_count 32

typedef struct DeviceInfo DeviceInfo;
struct DeviceInfo
{
    String deviceMAC;
    int deviceTpye;
    int onOff;
    int value;
    unsigned int wifiMeshDeviceId;
};

static DeviceInfo deviceInfo[maximum_device_count];
static int currentRegistedDeviceCount = 0;

int GetExistedDeviceInt(unsigned int wifiMeshDeviceId)
{
    for(int i = 0; i < maximum_device_count; i++)
    {
      if (deviceInfo[i].wifiMeshDeviceId == wifiMeshDeviceId)
        return i;
    }
    return -1;
}

bool IsExistedDevice(unsigned int wifiMeshDeviceId)
{
  return (GetExistedDeviceInt(wifiMeshDeviceId) != -1);
}
