/*
 * Compatible ESP32 & ESP8266
 */
#include "AWS_IoTCore_Header.h"
#include "Button_Header.h"
#include "Info_Header.h"
#include "WiFi_MultiConnect_Header.h"
#include "WiFi_NTPTimer_Header.h"
#include <ArduinoJson.h>

#define wifi_ssid "Cabala"
#define wifi_password "21424861"
#define wifi_ssid3 "Galaxy A33"
#define wifi_password3 "64854678"
#define wifi_ssid2 "KikoChan"
#define wifi_password2 "Sam123789"

#define buttonPin 2

uint8_t mac_Address[6];

void OnClickCallback()
{
  String json = CreateJson();
  AWSSendoutJson(json);
}

void setup()
{
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  SetUpCallback(0, &OnClickCallback);

  AddWiFiAddress(wifi_ssid, wifi_password);
  AddWiFiAddress(wifi_ssid2, wifi_password2);
  AddWiFiAddress(wifi_ssid3, wifi_password3);

  ConnectWiFi(30);
  
  InitNTPTimer();

  TryConnectAWSService(10);

  /* Create a message handler */
  client.setCallback(AWSReceivedMessageLog);
  
  WiFi.macAddress(mac_Address);
  randomSeed(time(NULL));
}

void loop()
{
  delay(50);
  
  if (!isConnectedAWS) return;
  
  CheckButtonOnClick(0, buttonPin);
  AWSCheckReceivedMessage();
}

bool isRegister = false;

String CreateJson()
{
  String str;
  String str_mac = String(mac_Address[0]);
  for(int i = 1; i < 6; i++)
  {
    str_mac = String(str_mac + ":");
    str_mac = String(str_mac + mac_Address[i]);
  }
  
  StaticJsonDocument<jsonSerializeAWSDataSize> doc;

  if (isRegister)
  {
    doc["UserName"] = "TestingTesting@gmail.com";
    doc["Password"] = "123456";
    doc["DeviceType"] = 1;
    doc["DeviceMac"] = str_mac;
    doc["Register"] = 1;
    serializeJsonPretty(doc, str);
  }
  else
  {
    doc["UserName"] = "TestingTesting@gmail.com";
    doc["Password"] = "123456";
    doc["DeviceType"] = 1;
    doc["DeviceMac"] = str_mac;
    doc["Time"] = (unsigned long) time(NULL);
    doc["Value"] = random(2700);
    serializeJsonPretty(doc, str);
  }
  isRegister = !isRegister;
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

  StaticJsonDocument<jsonDeserializeSize> doc;
  
  deserializeJson(doc, payload);
  
  serializeJsonPretty(doc, Serial);
  Serial.println();
}
