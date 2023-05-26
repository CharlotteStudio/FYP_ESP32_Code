/* Version 0.1.2
 * Wifi Mesh Sample
 * Compatible ESP32 & ESP8266
 * 
 * Sample for connection & disconnect
 */
#include "painlessMesh.h"

#define MESH_PREFIX   "TestingMeshWifi"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT     5555

static painlessMesh mesh;
static bool isConnectedMeshNetwork = false;

unsigned int GetMyNodeId() { return mesh.getNodeId(); }

typedef void (*ConnectedWiFiMeshCallback)();
static ConnectedWiFiMeshCallback connectedWiFiMeshCallback;

typedef void (*DisconnectedWiFiMeshCallback)();
static DisconnectedWiFiMeshCallback disconnectedWiFiMeshCallback;

typedef void (*ReceivedMessageCallback)(unsigned int, String&);
static ReceivedMessageCallback receivedMessageCallback;

void SetUpConnectedWiFiMeshCallback(ConnectedWiFiMeshCallback cb)
{
  connectedWiFiMeshCallback = cb;
  Serial.println("set up Connected WiFiMesh Callback.");
}

void SetUpDisconnectedWiFiMeshCallback(DisconnectedWiFiMeshCallback cb)
{
  disconnectedWiFiMeshCallback = cb;
  Serial.println("set up Disconnected WiFiMesh Callback.");
}

void SetUpReceivedMessageCallback(ReceivedMessageCallback cb)
{
  receivedMessageCallback = cb;
  Serial.println("set up Received Message Callback.");
}

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("Received message from Node ID [%u]\nmsg : [%s]\n", from, msg.c_str());
  
  if (receivedMessageCallback != NULL)
  {
    receivedMessageCallback(from, msg);
  }
}

void SetUpWifiMesh()
{
  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(receivedCallback);
  isConnectedMeshNetwork = true;

  if (connectedWiFiMeshCallback != NULL)
  {
    connectedWiFiMeshCallback();
  }

  Serial.print("Device Node ID : ");
  Serial.println(mesh.getNodeId());
}

void DisconnectedWifiMesh()
{
  if(!isConnectedMeshNetwork) return;
  mesh.stop();
  isConnectedMeshNetwork = false;
  
  if (disconnectedWiFiMeshCallback != NULL)
  {
    disconnectedWiFiMeshCallback();
  }
  Serial.println("Disconnected Wifi Mesh.");
}

void UpdateWifiMesh()
{
  if (!isConnectedMeshNetwork) return;
  mesh.update();
}

void SendoutWifiMesh(String str)
{
  if (!isConnectedMeshNetwork) return;
  Serial.println("Sendout message :");
  Serial.println(str);
  mesh.sendBroadcast(str);
}
