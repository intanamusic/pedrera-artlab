#include <UIPEthernet.h> // Used for Ethernet
#include <UIPUdp.h>
#include <OSCBundle.h> // for receiving OSC messages
#include <OSCMessage.h>                // for sending OSC messages
#include <CapacitiveSensor.h>

// **** ETHERNET SETTING ****
byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x31 };                                       
IPAddress ip(192, 168, 133, 111);                        
IPAddress Sendip(192, 168, 133, 100);                        

EthernetServer server(80);
const unsigned int outPortProtopixel = 2345; 
const unsigned int outPortSonicPi = 4559;
#include <UIPEthernet.h>
EthernetUDP Udp;

CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,6);        // 1M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire to the capacitive foil


void setup() {
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Udp.begin(1234);
  Serial.print("Local port: ");

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
     cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example

}

int SendMessageP(char* message,int port,bool sensorValue) 
{
  
   OSCMessage msg(message);
   msg.add((int16_t)sensorValue);
   Udp.beginPacket(Sendip, port);
   msg.send(Udp); // send the bytes to the SLIP stream
   Udp.endPacket(); // mark the end of the OSC Packet
   msg.empty(); 
   delay(10);
}

void ReadCapacitiveSensor()
{
  
      long start = millis();
      long total1 =  cs_4_2.capacitiveSensor(30);
      Serial.println(total1);                  // print sensor output 1
      if(total1>1500)
      {
        SendMessageP("/column1P/1",outPortProtopixel,true);
        SendMessageP("/column1S/1",outPortSonicPi,true);
      }  
}


void loop() {

 // SendMessage(10);
  // listen for incoming clients
  EthernetClient client = server.available();
    ReadCapacitiveSensor();

  if (client) 
  {  
    Serial.println("-> New Connection");

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();

         if (c == '\n' && currentLineIsBlank) 
        {
          client.println("<html><title>Hello World!</title><body><h3>Sensor n.1 ready!</h3> <br><a href=\"http://192.168.133.112\">click here for sensor n.2</a> <br><a href=\"http://192.168.133.113\">click here for sensor n.3</a></body>");
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      } 
    }

    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    Serial.println("   Disconnected\n");
  }
}
