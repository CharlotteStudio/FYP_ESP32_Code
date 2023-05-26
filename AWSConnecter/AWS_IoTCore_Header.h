/* Version 0.1.1
 *
 * Compatible ESP32 & ESP8266
 */
#include "AWS_IoTCore_MySecret.h"

#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

#ifdef ESP8266
BearSSL::X509List cert(AWS_CERT_CA);
BearSSL::X509List client_crt(AWS_CERT_CRT);
BearSSL::PrivateKey key(AWS_CERT_PRIVATE);
#endif

bool IsConnectedAWS() { return client.connected(); }

void AWSSendoutJson(String json);

bool TryConnectAWSService(int timeout/* seconds */)
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
#if defined(ESP32)
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
#elif defined(ESP8266)
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
#endif

  /* Connect to the MQTT broker on the AWS endpoint we defined earlier */
  client.setServer(AWS_IOT_ENDPOINT, 8883);
  
  Serial.println("Connecting AWS IoT Core");

  float runningTime = 0;
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
    
    runningTime++;
    if (runningTime > timeout) break;
  }

  if (IsConnectedAWS())
  {
    Serial.println("\nAWS IoT Core Connected!");
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  }
  else
  {
    Serial.println("\nFail Connect, Timeout.");
  }
  return IsConnectedAWS();
}

void AWSSendoutJson(String json)
{
  if (!IsConnectedAWS())
  {
    Serial.println("Have not connect to AWS Service, please check.");
    return;
  }
  
  char* jsonChar = new char[json.length() + 1];
  strcpy(jsonChar, json.c_str());

  Serial.println("Completed send out Json : ");
  Serial.println(jsonChar);
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonChar);
}

void AWSCheckReceivedMessage()
{
  if (!IsConnectedAWS()) return;
  
  client.loop();
}
