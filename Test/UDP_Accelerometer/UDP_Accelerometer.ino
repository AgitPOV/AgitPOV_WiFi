#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Chrono.h>
#include <Math.h>

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
// DOTSTAR /////////////////////////////
#include <Adafruit_DotStar.h>
#include <SPI.h>    // pour les dotstars
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6 
#define CLOCKPIN   D5
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
// uint32_t color = 0x121212;  // couleur pour l'affichage d'un mot
// uint32_t color = 0xCC3300; // naranja
//uint32_t color = 0x991100; // rouge pas trop clair
uint32_t color = 0xFFFFFF; // test blanc


#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"

// Begin using the library by creating an instance of the MMA8452Q
//  class. We'll call it "accel". That's what we'll reference from
//  here on out.
MMA8452Q accel;

int AgitAngle = 0;

const int nmbrLectures = 5;  // 10 semble fonctionner
int lectures[nmbrLectures];      // the readings from the analog input
int lectureIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float moyenne = 0;                // the average
float derniereValeur = 1;

void setup()
{

  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(myIP, myIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("UDP_Accelerometer");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  accel.init(SCALE_8G, ODR_800);
  strip.begin(); // DOTSTAR Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}


void loop()
{

   if (accel.available()) {
    // First, use accel.read() to read the new variables:
    accel.read();

    total = total - lectures[lectureIndex];  // subtract the last reading:
    lectures[lectureIndex] = accel.cy;
    total = total + lectures[lectureIndex];  // add the reading to the total:
    lectureIndex = lectureIndex + 1; // advance to the next position in the array:
    
    if (lectureIndex >= nmbrLectures) {   // if we're at the end of the array...
      lectureIndex = 0;  // ...wrap around to the beginning:
      }

    moyenne = total / nmbrLectures;

    // calculer le laps de temps entre les deux points subséquents les plus hauts vitesseTour
    // 
    
   
    //printCalculatedAccels();
    //printAccels(); // Uncomment to print digital readings
    String data = "data ";
    data+= String(accel.cx,3);
    data+= " ";
    data+= String(accel.cy,3);
    data+= " ";
    data+= String(accel.cz,3);
    data+= " ";
    data+= String(moyenne,3); // un moyenne de Y pour faire du lissage des données 
    
    // Calcul de l'angle...
    // AgitAngle = round(atan2 (accel.cx,accel.cy) * -180/3.14159265 ); // ne fonctionne pas avec le X au plancher (+ de '8G') 
    // AgitAngle = round(atan2 (1,accel.cy) * -180/3.14159265 ); // X remplacé par '1' // radians to degrees and rounding
    // AgitAngle = round(atan2 (1,moyenne) * -180/3.14159265 ); // radians to degrees and rounding
    // data+= " ";
    // data+= String(AgitAngle);
    
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

if(derniereValeur == 0 && moyenne > 0){  // on sort de la série de 0 et on augmente le compte
     dotInit();
    }
      derniereValeur = moyenne;
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


 void dotStop(){ // éteint toutes les lumières
    
   for(byte i = 0;i<=23;i++){
      strip.setBrightness(0); 
      strip.setPixelColor(i, 0); 
      strip.show(); 
      }
  }

   void dotInit(){ // séquence de départ
    
    dotStop();
    
  for(byte i = 0;i<=12;i++){ /// UP!!
     
      strip.setBrightness(200); 
      strip.setPixelColor(i, color); 
      strip.show(); 
      yield(); // donne un peu de temps au wifi
      delay(1);
      yield();
   
      
    /*  for(int k = 120;k>=0;k=k-50){ /// Down
        strip.setBrightness(k); 
        strip.setPixelColor(i, color); 
        strip.show(); 
        yield(); // donne un peu de temps au wifi
        delay(1);
        yield();
      }*/
     //  dotStop(); // Une seule LED allumée à la fois

    } 
     dotStop();
      
 } // fin dotInit
