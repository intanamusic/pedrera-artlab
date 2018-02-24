
/**
* Receive OSC messages at NodeMCU from any OSC speaking sender.
* Case: Switch an LED (connected to NodeMCU) on or off via Smartphone
* Created by Fabian Fiess in November 2016
* Inspired by Oscuino Library Examples, Make Magazine 12/2015, https://trippylighting.com/teensy-arduino-ect/touchosc-and-arduino-oscuino/
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h> // for receiving OSC messages
#include <OSCMessage.h>                // for sending OSC messages

char ssid[] = "Pisound"; // your network SSID (name)
char pass[] = "blokaslabs"; // your network password

//char ssid[] = "event@upf"; // your network SSID (name)
//char pass[] = "faunpa18"; // your network password

//char ssid[] = "TP-LINK_B5A8"; // your network SSID (name)
//char pass[] = "16653481"; // your network password

const int numReadings = 40;
#define node 0
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;    

int SonarPin=A0;
int sensorValue;
// Button Input + LED Output
const int ledPin = D0; // D5 pin at NodeMCU
const int boardLed = LED_BUILTIN; // Builtin LED

//destination IP
IPAddress outIp(255, 255, 255, 255);
const unsigned int outPort = 2345;

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP


void setup() {
Serial.begin(9600);

// Specify a static IP address for NodeMCU
// If you erase this line, your ESP8266 will get a dynamic IP address
WiFi.config(IPAddress(192,168,0,123),IPAddress(192,168,0,1), IPAddress(255,255,255,0));

// Connect to WiFi network
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, pass);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

Serial.println("Starting UDP");
Udp.begin(1234);
Serial.print("Local port: ");
Serial.println(Udp.localPort());
for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

int get_average()
{
 // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(SonarPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;  
  if(average<80){   
    Serial.println(average);
    return average;
  }
  
}


void SendVDMXMessage(int sensorValue)
{
  
   OSCMessage msg("/vdmx/0");
   msg.add((int32_t)sensorValue);
   Udp.beginPacket(outIp, 1235);
   msg.send(Udp); // send the bytes to the SLIP stream
   Udp.endPacket(); // mark the end of the OSC Packet
   msg.empty(); 
   delay(10);
}


void SendSonicPiMessage(int sensorValue)
{
 
     Serial.println(sensorValue); 
  
  if (40 <= sensorValue && sensorValue <= 80)
  {
     Serial.println(sensorValue);
     OSCMessage msg("/pisound/0");
     msg.add((int32_t)sensorValue);
     Udp.beginPacket(outIp, 4559);
     msg.send(Udp); // send the bytes to the SLIP stream
     Udp.endPacket(); // mark the end of the OSC Packet
     msg.empty(); 
     delay(10);
    
  }
/*
  if (90 <= sensorValue && sensorValue <= 160)
  {
     OSCMessage msg("/pisound/0");
     msg.add(2);
     Udp.beginPacket(outIp, 4559);
     msg.send(Udp); // send the bytes to the SLIP stream
     Udp.endPacket(); // mark the end of the OSC Packet
     msg.empty(); 
     delay(10);
    
  }*/

  
}


void SendProtopixelMessage(int sensorValue)
{
  
   OSCMessage msg("/protopixel/0");
   msg.add((int32_t)sensorValue);
   Udp.beginPacket(outIp, outPort);
   msg.send(Udp); // send the bytes to the SLIP stream
   Udp.endPacket(); // mark the end of the OSC Packet
   msg.empty(); 
   delay(10);
}

void loop() {

   sensorValue=get_average();
   //sensorValue=analogRead(SonarPin);
   SendProtopixelMessage(sensorValue);    
   SendVDMXMessage(sensorValue);
   SendSonicPiMessage(sensorValue);
}



