#include "AWS_IoTCore_Header.h"
#include "AWSConnecter_Header.h"
#include "Info_Header.h"
#include "SoftwareSerial_Header.h"
#include "WiFi_MultiConnect_Header.h"
#include "WiFi_NTPTimer_Header.h"
#include <ArduinoJson.h>

#define wifi_ssid "Cabala"
#define wifi_password "21424861"
#define wifi_ssid2 "KikoChan"
#define wifi_password2 "Sam123789"
#define wifi_ssid3 "Galaxy A33"
#define wifi_password3 "64854678"

String userName = "testingtesting@gmail.com";
String password = "1234567890";

void setup()
{
  SetUpSerial();
  Serial.begin(115200);
  
  SetUpLED();

  AddWiFiAddress(wifi_ssid, wifi_password);
  AddWiFiAddress(wifi_ssid2, wifi_password2);
  AddWiFiAddress(wifi_ssid3, wifi_password3);
}

void loop()
{
  CheckWiFiLED(WiFi.isConnected());
  CheckAWSLED(IsConnectedAWS());

  TryConnection();
  
  String json = HandleReceivedMessageFromSoftwareSerial();
  
  if (!WiFi.isConnected() || !IsConnectedAWS())
  {
    delay(100);
    return;
  }
    
  AWSCheckReceivedMessage();

  // check the format is json
  if (!json.equals("") && json.indexOf("{") != -1 && json.indexOf("}") != -1 && json.indexOf(":") != -1 )
  {
    AWSSendoutJson(json);
  }
  delay(100);
}

void TryConnection()
{
  if (!WiFi.isConnected())
  {
    ConnectWiFi(30);

    if (WiFi.isConnected())
    {
      InitNTPTimer();
      SoftwareSerialSendout("Connected WiFi");
    }
  }
  else
  {
    if (!IsConnectedAWS())
    {
      TryConnectAWSService(10);
      client.setCallback(AWSReceivedMessageLog);
      SoftwareSerialSendout("Connected AWS");
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

  printf("DeviceTpye is [%d]\nDeviceMac is  [%s]\n", doc["DeviceTpye"].as<int>(), doc["DeviceMac"].as<String>().c_str());
 
  if (doc["Register"].is<int>())
  {
    printf("Register is   [%d]\n", doc["Register"].as<int>());
  }
  else
  {
    printf("value is      [%d]\n", doc["Value"].as<int>());
    doc["Time"] = (unsigned long) time(NULL);
  }
  
  doc["UserName"] = userName;
  doc["Password"] = password;  

  String str;
  serializeJsonPretty(doc, str);
  return str;
}

void SoftwareSerialSendout(String str)
{
  StaticJsonDocument<jsonSerializeRegisterSize> doc;

  doc["message"] = str;
  Serial.println("Send out to SoftwareSerial :");
  serializeJsonPretty(doc, Serial);
  Serial.println();
  
  // SoftwareSerialSendout
  serializeJsonPretty(doc, mySerial);
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
  
  Serial.println("Send out to SoftwareSerial");
  serializeJsonPretty(doc, Serial);
  Serial.println();

  // SoftwareSerialSendout
  serializeJsonPretty(doc, mySerial);
}
