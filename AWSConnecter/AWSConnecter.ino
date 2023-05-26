// Version 0.1.0, Release at 20230523
#include "AWS_IoTCore_Header.h"
#include "AWSConnecter_Header.h"
#include "Info_Header.h"
#include "SoftwareSerial_Header.h"
#include "WiFi_MultiConnect_Header.h"
#include "WiFi_NTPTimer_Header.h"
#include <ArduinoJson.h>

#define wifi_ssid "Cabala"
#define wifi_password "21424861"
#define wifi_ssid2 "Galaxy A33"
#define wifi_password2 "64854678"

void setup()
{
  SetUpSerial();
  Serial.begin(115200);
  
  SetUpLED();

  AddWiFiAddress(wifi_ssid, wifi_password);
  AddWiFiAddress(wifi_ssid2, wifi_password2);
}

void loop()
{
  CheckWiFiLED(WiFi.isConnected());
  CheckAWSLED(IsConnectedAWS());

  TryConnection();
  
  String json = HandleReceivedMessageFromSoftwareSerial();
  
  if (!WiFi.isConnected() || IsConnectedAWS())
  {
    delay(100);
    return;
  }
    
  AWSCheckReceivedMessage();
  if (!json.equals(""))
  {
    AWSSendoutJson(json);
  }
}

void TryConnection()
{
  if (!WiFi.isConnected())
  {
    ConnectWiFi(30);
  }
  else
  {
    if (!IsConnectedAWS())
    {
      TryConnectAWSService(10);
      client.setCallback(AWSReceivedMessageLog);
    }
  }
}

String HandleReceivedMessageFromSoftwareSerial()
{
  if(!mySerial.available()) return "";
  
  StaticJsonDocument<jsonSerializeAWSDataSize> doc;

  DeserializationError error = deserializeJson(doc, mySerial);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return "";
  }
  
  bool isOn = doc["Register"].is<int>();
  int deviceTpye = doc["DeviceTpye"];
  const char* mac = doc["DeviceMAC"];
  
  Serial.print("]DeviceTpye is [");
  Serial.print(deviceTpye);
  Serial.print("]\nDeviceMAC is  [");
  Serial.print(mac);
  
  if (isOn)
  {
    int onOff = doc["Register"];
    Serial.print("]\nOnOff is : ");
    Serial.print(onOff);
  }
  else
  {
    int value = doc["Value"];
    Serial.print("Value is [");
    Serial.print(value);
    Serial.println("]");
    doc["Time"] = (unsigned long) time(NULL);
  }
  
  doc["UserName"] = userName;
  doc["Password"] = password;  

  String str;
  serializeJsonPretty(doc, str);
  return str;
}

/* Received message from IoT Core */
void AWSReceivedMessageLog(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Received message from : ");
  Serial.print(topic);
  Serial.print(" (length : ");
  Serial.print(length);
  Serial.println(")");

  StaticJsonDocument<jsonSerializeAWSDataSize> doc;
  
  deserializeJson(doc, payload);
  
  // SoftwareSerialSendout
  serializeJsonPretty(doc, mySerial);
  
  Serial.println("Send out to SoftwareSerial");
  serializeJsonPretty(doc, Serial);
  Serial.println();
}
