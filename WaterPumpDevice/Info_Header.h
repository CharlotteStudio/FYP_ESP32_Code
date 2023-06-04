#define jsonSerializeRegisterSize 192
#define jsonSerializeDataSize 32
#define jsonDeserializeSize 192
#define jsonSerializeAWSDataSize 256

/* DeviceTpye
0 : Main Device
1 : Soli Sensor
2 : WaterPump
3 : Temp Sensor
4 : Fan
5 : Lighting
*/

// Pump mac address-1 : 176:178:28:167:24:192
// Pump mac address-2 : 120:33:132:225:22:76
// Soil mac address-1 : 176:178:28:166:252:228
// Soil mac address-2 : 120:33:132:224:254:84

// Device ⇒ Main Control Board Register
/*
{
  "To":1234567890,
  "DeviceType":1,
  "DeviceMac":"255:255:255:255:255:255",
  "Register":1
}
*/

// Device ⇒ Main Control Board Value
/*
{
  "To":1234567890,
  "Value":1000
}
*/

// Main Control Board ⇒ AWS Register
/*
{
  "UserName":"TestingTesting@gmail.com",
  "Password":"1234567890123456",
  "DeviceType":1,
  "DeviceMac":"255:255:255:255:255:255",
  "Register":0
}
*/


// AWS ⇒ Main Control Board (Set Active)
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
// Soil mac address-1 : 176:178:28:166:252:228
// Soil mac address-2 : 120:33:132:224:254:84
/*
{
  "DeviceMac":"176:178:28:166:252:228",
  "SetUpdateSpeed":60
}
*/

// Main Control Board ⇒ Device
/*
{
  "To":1234567890,
  "ActiveState":1
}
{
  "To":1234567890,
  "SetUpdateSpeed":60
}
*/
