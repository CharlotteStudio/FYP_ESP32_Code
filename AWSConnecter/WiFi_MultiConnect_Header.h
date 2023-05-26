/* Version 0.1.0
 *
 * Compatible ESP32 & ESP8266
 * 
 * Set Connect Multi WiFi
 * The Detail in the Google Notes => ESP32 : WiFi
 *
 * Reference Link:
 * https://youyouyou.pixnet.net/blog/post/120275911-%E7%AC%AC%E5%8D%81%E7%AF%87-esp32-wifi%E7%B6%B2%E8%B7%AF%E9%80%A3%E7%B7%9A(pm2.5%E9%A1%AF%E7%A4%BA%E5%99%A8)
 *
 * GitHub :
 * https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiMulti.h
 */

#if defined(ESP32)

#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

#endif

static int addressCount = 0;

void AddWiFiAddress(char* ssid, char* password)
{
  wifiMulti.addAP(ssid, password);
  addressCount++;
}

void AddWiFiAddress(String ssid, String password)
{
  // String convert to char*
  int ssid_len = ssid.length() + 1; 
  char ssidBuf[ssid_len] ;
  ssid.toCharArray(ssidBuf, ssid_len);
  
  int password_len = password.length() + 1; 
  char passwordBuf[password_len] ;
  password.toCharArray(passwordBuf, password_len);

  AddWiFiAddress(ssidBuf, passwordBuf);
}

void ConnectWiFi(float timeout)
{
  if (addressCount == 0)
  {
    Serial.println("Haven't assigned any WiFi Address.");
    return;
  }

  float currentTime = 0;

  wifiMulti.run();

  Serial.print("Connecting Wifi");

  while(!WiFi.isConnected() || currentTime < timeout)
  {
    delay(500);
    currentTime += 0.5f;
    Serial.print(".");
  }

  if (WiFi.isConnected())
  {
    /* Check the device IP Address */
    Serial.println();
    Serial.println("Connected, IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("Timeout, Fail Connection");
  }
}
