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


float cy;
float cyLop;
float cyLopSlow;

float cyPrevious;
float cyLopPrevious;

//float cyV;
//float cyVLop;

float cyCentered;

float threshold = 0.5;
bool triggered = false;


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

  updateAccelerometer();

  if ( cyCentered > threshold ) {
    if ( triggered == false ) {
      triggered = true;
      dotInit();
    }
  } else {
    triggered = false;
  }

}

void updateAccelerometer() {

  if (accel.available()) {
    // First, use accel.read() to read the new variables:
    accel.read();

    cy = accel.cy;
    cyLop = lop(cy, cyLop, 0.1); 

    cyLopSlow = lop(cy, cyLopSlow, 0.01); 

    cyCentered = cyLop - cyLopSlow;

    cyLopPrevious = cyLop;
    cyPrevious = cy;

  
    String data = "data ";
    data += String(cy, 3);
   // data += " ";
    //data += String(cyLop, 3);
    //data += " ";
   // data += String(cyLopSlow, 3);
   // data += " ";
   // data += String(cyCentered, 3); // un moyenne de Y pour faire du lissage des données
   
    data.toCharArray(udpMessageBuffer, 32) ;

    Udp.beginPacket(remoteIP, remotePort);
    Udp.write(udpMessageBuffer);
    Udp.endPacket();
  }

}


float lop(float target, float current , float amount) {

  return (target - current) * amount + current;

}



void dotStop() { // éteint toutes les lumières

  for (byte i = 0; i <= 23; i++) {
    strip.setBrightness(0);
    strip.setPixelColor(i, 0);
    strip.show();
  }
}

void dotInit() { // séquence de départ

  dotStop();
  strip.setBrightness(200);
  
  for (byte i = 0; i <= 12; i++) { /// UP!!

    strip.setPixelColor(i, color);
    
    
  


    /*  for(int k = 120;k>=0;k=k-50){ /// Down
        strip.setBrightness(k);
        strip.setPixelColor(i, color);
        strip.show();
        yield(); // donne un peu de temps au wifi
        delay(1);
        yield();
      }*/
    

  }
  strip.show();
  updateAccelerometer();
  dotStop();

} // fin dotInit
