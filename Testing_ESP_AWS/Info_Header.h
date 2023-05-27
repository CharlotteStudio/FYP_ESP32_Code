#define jsonSerializeRegisterSize 192
#define jsonSerializeDataSize 32
#define jsonDeserializeSize 192
#define jsonSerializeAWSDataSize 256

#define DeviceTpye 1

/* DeviceTpye
0 : Main Device
1 : Soli Sensor
2 : WaterPump
3 : Temp Sensor
4 : Fan
5 : Lighting
*/

// Device ⇒ Main Control Board Register
/*
{
  "To":1234567890,
  "DeviceType":1,
  "DeviceMac":[255,255,255,255,255,255],
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
  "DeviceMac":[255,255,255,255,255,255],
  "Register":0
}
*/


// AWS ⇒ Main Control Board
/*
{
  "DeviceMac":String
  "OwnerDevice":string
  "ActiveValue":int
  "ActiveState":bool
}
*/

// Main Control Board ⇒ Device
/*
{
  "To":1234567890,
  "ActiveState":bool
}
*/
