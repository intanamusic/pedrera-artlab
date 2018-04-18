/*---------------------------------------------------------------------------------------------

  Simple example how to scan for X Air devices on the network with ESP8266 devices.
  You need to install the OSC library by Adrian Freed and Yotam Mann available either
  throught he Arduino library manager or at https://github.com/CNMAT/OSC

  This is based on:

  Open Sound Control (OSC) library for the ESP8266

  Example for sending messages from the ESP8266 to a remote computer
  The example is sending "hello, osc." to the address "/test".

  This example code is in the public domain.

  --------------------------------------------------------------------------------------------- */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

const unsigned long BROADCAST_MILLIS = 1000; // Send broadcast messages every 1000 milliseconds
unsigned long lastBroadcastMillis = 0;

char ssid[] = "ONO806D";          // fill in your network SSID (name)
char pass[] = "@T3t4l0g1=0";                    // fill in your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
IPAddress broadcastIp;
const unsigned int outPort = 10024;          // OSC Port of the X Air devices
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

typedef enum {
  WIFI_CONNECTED = 0,
  WIFI_DISCONNECTED
} ConnectionState_t;

ConnectionState_t connectionState = WIFI_DISCONNECTED;
OSCErrorCode error;


// Track changes of the connection state
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case WIFI_EVENT_STAMODE_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectionState = WIFI_CONNECTED;
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connectionState = WIFI_DISCONNECTED;
      break;
  }
}

void setup() {
  Serial.begin(115200);
  connect();  
}

void connect() {
  WiFi.disconnect(true);
  delay(1000);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, pass);

  // Just wait ... 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // COnnection established.
  // Print some connection info.
  Serial.println("");

  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  
  // Get broadcast address to scan for X Air devices
  broadcastIp = ~WiFi.subnetMask() | WiFi.gatewayIP();
}

// Handle UDP parsing errors
void errorHandler(OSCMessage msg) {
  error = msg.getError();
  Serial.print("error: ");
  Serial.println(error);
}

// Handler for X Air /info messages
void infoHandler(OSCMessage &msg) {
  Serial.println("\n/info message received!");
  for (int i = 0; i < msg.size(); ++i) {
    if (msg.isString(i)) {
      char str[255];
      msg.getString(i, str, 255);
      Serial.println(str);
    }
  }
}

// Handler for X Air /xinfo messages
void xinfoHandler(OSCMessage &msg, IPAddress fromIp) {
  Serial.print("\n/info message received from ");
  Serial.print(fromIp);
  Serial.print("\n");
  for (int i = 0; i < msg.size(); ++i) {
    if (msg.isString(i)) {
      char str[255];
      msg.getString(i, str, 255);
      Serial.println(str);
    }
  }
}

// Send broadcast messages to the /info or /xinfo path 
void sendBroadcastMsg() {
  OSCMessage msg("/xinfo");
  //OSCMessage msg("/info");
  Udp.beginPacketMulticast(broadcastIp, outPort, WiFi.localIP());
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

// Process incoming messages
void processIncoming() {
  OSCMessage inMsg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      inMsg.fill(Udp.read());
    }
    if (inMsg.hasError()) {
      errorHandler(inMsg);
    } else if (inMsg.fullMatch("/xinfo")) {
      xinfoHandler(inMsg, Udp.remoteIP());
    } else {
      inMsg.dispatch("/info", infoHandler);
    }
  }
}

// Main loop
void loop() {
  if (connectionState == WIFI_DISCONNECTED) {
    connect();
  }
  // Send broadcast message if necessary
  const unsigned long now = millis();
  if (now >= lastBroadcastMillis + BROADCAST_MILLIS) {
    sendBroadcastMsg();
    lastBroadcastMillis = now;
  }

  processIncoming();
}



