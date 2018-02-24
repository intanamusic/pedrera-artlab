
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
#define TRIGGER 5
#define ECHO    4


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
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

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

int get_average(int distance)
{
 // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = distance;
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
  return average;
}



int ReadSensor()
{
  long duration, distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  if(distance<400){   
    Serial.println(distance);
    return  get_average(distance);
  }
  
  delay(10);  
  
  
}


void SendVDMXMessage(int sensorValue)
{
   Serial.println(sensorValue);
   OSCMessage msg("/vdmx/2");
   msg.add((int32_t)sensorValue);
   Udp.beginPacket(outIp, 1235);
   msg.send(Udp); // send the bytes to the SLIP stream
   Udp.endPacket(); // mark the end of the OSC Packet
   msg.empty(); 
   delay(10);
}




void SendSonicPiMessage(int sensorValue)
{
 
  
  if (20 <= sensorValue && sensorValue <= 60)
  {

     Serial.println(sensorValue);
     OSCMessage msg("/pisound/2");
     msg.add((int32_t)sensorValue);
     Udp.beginPacket(outIp, 4559);
     msg.send(Udp); // send the bytes to the SLIP stream
     Udp.endPacket(); // mark the end of the OSC Packet
     msg.empty(); 
     delay(20);
    
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
  
   OSCMessage msg("/protopixel/2");
   msg.add((int32_t)sensorValue);
   Udp.beginPacket(outIp, outPort);
   msg.send(Udp); // send the bytes to the SLIP stream
   Udp.endPacket(); // mark the end of the OSC Packet
   msg.empty(); 
   delay(10);
}

void loop() {

//   sensorValue=get_average();
   sensorValue=ReadSensor();
   SendProtopixelMessage(sensorValue);    
   SendVDMXMessage(sensorValue);
   SendSonicPiMessage(sensorValue);
}



