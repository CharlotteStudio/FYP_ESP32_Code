#define jsonSerializeRegisterSize   128
#define jsonDeserializeRegisterSize 192
#define jsonSerializeDataSize       32
#define jsonDeserializeDataSize     64
#define jsonSerializeAWSDataSize    192
#define jsonDeserializeAWSDataSize  256

//#define pump_mac_address_1 "176:178:28:167:24:192"
//#define pump_mac_address_2 "120:33:132:225:22:76"
//#define soil_mac_address_1 "176:178:28:166:252:228"
//#define soil_mac_address_2 "120:33:132:224:254:84"

/* DeviceTpye
0 : Main Device
1 : Soli Sensor
2 : WaterPump
3 : Temp Sensor
4 : Fan
5 : Lighting
*/

// Device ⇒ Main Control Board (Register)
// jsonSerializeRegisterSize
/*
{
  "To":4294967295,
  "DeviceType":1,
  "DeviceMac":"255:255:255:255:255:255",
  "Register":1
}
*/

// Main Control Board ⇒ Device (Register)(BLE)
// jsonSerializeDataSize
/*
{
  "Register": 1,
  "Channel": 0
}
{
  "To": 4294967295,
  "Register": 1
}
*/


// Device ⇒ Main Control Board (Value)
// jsonSerializeDataSize
/*
{
  "To":4294967295,
  "Value":1000
}
*/

// Main Control Board ⇒ AWS (Register)
// jsonSerializeAWSDataSize
/*
{
  "UserName":"TestingTesting@gmail.com",
  "Password":"1234567890123456",
  "DeviceType":1,
  "DeviceMac":"255:255:255:255:255:255",
  "Register":0
}
*/

// Main Control Board ⇒ AWS (Data)
// jsonSerializeAWSDataSize
/*
{
  "UserName":"TestingTesting@gmail.com",
  "Password":"1234567890123456",
  "DeviceType":1,
  "DeviceMac":"255:255:255:255:255:255",
  "Time":1685896349,
  "Vlaue":0
}
*/



// AWS ⇒ Main Control Board (Set Active)
// jsonSerializeAWSDataSize 256
// Pump mac address-1 : 176:178:28:167:24:192
// Pump mac address-2 : 120:33:132:225:22:76
/*
{
  "DeviceMac":String
  "OwnerDevice":string
  "ActiveValue":int
  "ActiveState":int
}
{
  "DeviceMac":"120:33:132:225:22:76",
  "ActiveState":1
}
*/

// AWS ⇒ Main Control Board (Set Update speed)
// jsonSerializeAWSDataSize 256
// Soil mac address-1 : 176:178:28:166:252:228
// Soil mac address-2 : 120:33:132:224:254:84
/*
{
  "DeviceMac":"176:178:28:166:252:228",
  "SetUpdateSpeed":60
}
*/

// Main Control Board ⇒ Device
// jsonSerializeDataSize
/*
{
  "To": 4294967295,
  "ActiveState": 1
}
{
  "To": 4294967295,
  "SetUpdateSpeed": 60
}
*/
