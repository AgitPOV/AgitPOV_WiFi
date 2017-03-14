#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Chrono.h>


/*
const char* ssid = "********";
const char* password = "********";
*/
IPAddress myIP(192, 168, 4, 1);

IPAddress remoteIP(192, 168, 4, 100);
unsigned int remotePort = 4210;

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "Hi there! Got the message :-)";  // a reply string to send back
char udpMessageBuffer[64];

Chrono udpSendChrono;


/******************************************************************************/
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"

// Begin using the library by creating an instance of the MMA8452Q
//  class. We'll call it "accel". That's what we'll reference from
//  here on out.
MMA8452Q accel;

void setup()
{

  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(myIP, myIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("UDP_Accelerometer");


  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);


  accel.init(SCALE_8G);
}


void loop()
{

   if (accel.available()) {
    // First, use accel.read() to read the new variables:
    accel.read();

    //printCalculatedAccels();
    //printAccels(); // Uncomment to print digital readings
    String data = "data ";
    data+= String(accel.cx,3);
    data+= " ";
    data+= String(accel.cy,3);
    data+= " ";
    data+= String(accel.cz,3);
    data.toCharArray(udpMessageBuffer, 64) ;
    
    Udp.beginPacket(remoteIP, remotePort);
    Udp.write(udpMessageBuffer);
    Udp.endPacket();
    
     // accel.read() will update two sets of variables. 
    // * int's x, y, and z will store the signed 12-bit values 
    //   read out of the accelerometer.
    // * floats cx, cy, and cz will store the calculated 
    //   acceleration from those 12-bit values. These variables 
    //   are in units of g's.
    // Check the two function declarations below for an example
    // of how to use these variables.

  }

  // SEND DATA
/*
  if ( udpSendChrono.hasPassed(500) ) {
    udpSendChrono.restart();
    
    udpMessage[0] = 1;
    udpMessage[1] = 2;
    udpMessage[2] = 3;
    udpMessage[3] = 4;
    
     Udp.beginPacket(remoteIP, remotePort);
    Udp.write(udpMessage);
    Udp.endPacket();
  }

  */
  
  /*
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacekt);
    Udp.endPacket();
  }
  */
}


// The function demonstrates how to use the accel.x, accel.y and
//  accel.z variables.
// Before using these variables you must call the accel.read()
//  function!
void printAccels()
{
  Serial.print("accels ");
  Serial.print(accel.x, 3);
  Serial.print("\t");
  Serial.print(accel.y, 3);
  Serial.print("\t");
  Serial.print(accel.z, 3);
  Serial.print("\t");
  Serial.println();
}

// This function demonstrates how to use the accel.cx, accel.cy,
//  and accel.cz variables.
// Before using these variables you must call the accel.read()
//  function!
void printCalculatedAccels()
{ 
  Serial.print("calculated ");
  Serial.print(accel.cx, 3);
  Serial.print("\t");
  Serial.print(accel.cy, 3);
  Serial.print("\t");
  Serial.print(accel.cz, 3);
  Serial.print("\t");
  Serial.println();
}
