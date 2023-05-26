/* Version 0.1.0
 *
 * Compatible ESP32 & ESP8266
 *
 * Functions:
 * void ConnectWiFi(char* ssid, char* password)
 * void ConnectWiFi(String ssid, String password)
 *
 */
#if defined(ESP32)

#include <WiFi.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>

#endif

// The Main Connect WiFi code
void ConnectWiFi(char* ssid, char* password)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting Wifi");

  // Waiting the state is connect Wifi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Check the device IP Address
  Serial.println();
  Serial.println("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

// ESP32 的 WiFi.begin() 唔可以用 String !!! 所以用 String 要先 convert
void ConnectWiFi(String ssid, String password)
{
  int ssid_len = ssid.length() + 1; 
  char ssidBuf[ssid_len] ;
  ssid.toCharArray(ssidBuf, ssid_len);
  
  int password_len = password.length() + 1; 
  char passwordBuf[password_len] ;
  password.toCharArray(passwordBuf, password_len);

  // 把 String convert to char* 再 connect
  ConnectWiFi(ssidBuf, passwordBuf);
}
