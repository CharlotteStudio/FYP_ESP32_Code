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


// AWS ⇒ Main Control Board (Set Active)
/*
{
  "DeviceMac":String
  "OwnerDevice":string
  "ActiveValue":int
  "ActiveState":int
}
*/

// AWS ⇒ Main Control Board (Set Update speed)
// Soil mac address : 120:33:132:224:254:84
/*
{
  "DeviceMac":String,
  "SetUpdateSpeed":int
}
*/

// Main Control Board ⇒ Device
/*
{
  "To":1234567890,
  "ActiveState":int
}
{
  "To":1234567890,
  "SetUpdateSpeed":int
}
*/
