
#include <Chrono.h>
#include <Math.h>

#define UDP_DEBUG  // Comment to turn off WiFi + UDP

#ifdef UDP_DEBUG
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

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
#endif

//int povArray[] = { 0 , 0 , 2040 , 2176 , 2176 , 2176 , 2040 , 0 , 2032 , 2056 , 2056 , 2120 , 1136 , 0 , 3064 , 0 , 2048 , 2048 , 4088 , 2048 , 2048 , 0 , 0 , 4088 , 2176 , 2176 , 2176 , 3968 , 0 , 2032 , 2056 , 2056 , 2056 , 2032 , 0 , 4064 , 16 , 8 , 16 , 4064 , 0 , 0 };
// int povArray[] = {252,510,771,771,771,510,252,0,512,516,518,1023,1023,512,512,0,774,899,705,609,561,543,526,0,385,785,561,569,621,967,387,0,30,31,16,16,1020,1022,16,0,399,783,521,521,521,1017,497,0,510,995,529,529,529,1011,486,0,3,897,961,97,49,31,15};
// int povArray[] = {1008,2040,3084,3084,3084,2040,1008,0,4,516,1540,4092,4092,4,4,0,1548,3100,2100,2148,2244,3972,1796,0,2072,2188,2244,2500,2916,3644,3096,0,1920,3968,128,128,1020,2044,128,0,3864,3852,2308,2308,2308,2556,2296,0,2040,3196,2180,2180,2180,3324,1656,0,3072,2076,2108,2144,2240,3968,3840};
// La tournée 
int povArray[] = {4092,4092,4,4,4,4,12,0,252,1020,1088,2112,1600,1020,508,0,0,0,0,3072,2048,4092,4092,2048,2048,0,1016,2044,2060,2052,3076,4088,2032,0,4088,4092,4,4,4,4088,4092,0,4092,4092,2112,2160,2136,4044,1924,0,4092,4092,512,448,32,4088,4092,0,508,804,1316,1316,2340,2340,260,0,4092,4092,2180,2180,2180,2180,2052};
// RESISTE // 
// int povArray[] = {4092,4092,2112,2160,2136,4044,1924,0,508,804,1316,1316,2340,2340,260,0,1800,3980,2436,2244,2148,3196,1080,0,2052,2052,4092,4092,2052,2052,0,1800,3980,2436,2244,2148,3196,1080,0,3072,2048,4092,4092,2048,2048,0,4092,4092,2180,2180,2180,2180,2052};

/******************************************************************************/
// FastLED /////////
#include <FastLED.h>
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6
#define CLOCKPIN   D5
CRGB leds[NUMPIXELS];
// uint32_t color = 0x121212;  // couleur pour l'affichage d'un mot
uint32_t color = 0xEE5522; // naranja intense
// uint32_t color = 0xCC3300; // naranja
//uint32_t color = 0x991100; // rouge pas trop clair
// uint32_t color = 0xFFFFFF; // test blanc

//////// ACCEL //////////////////////
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"

// Begin using the library by creating an instance of the MMA8452Q
//  class. We'll call it "accel". That's what we'll reference from
//  here on out.
MMA8452Q accel;

// int AgitAngle = 0;

float cy;
float cyLop;
float cyLopSlow;

float cyPrevious;
float cyLopPrevious;

//float cyV;
//float cyVLop;

float cyCentered;

float thresholdUp = 0.5;
float thresholdDown = 0;

bool triggered = false;


void setup()
{

  Serial.begin(115200);
#ifdef UDP_DEBUG
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(myIP, myIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("UDP_Accelerometer");

  Udp.begin(localUdpPort);
  #endif UDP_DEBUG

  
  //Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  accel.init(SCALE_8G, ODR_800);

  FastLED.addLeds<APA102, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
  // FastLED.addLeds<WS2801,DATAPIN, CLOCKPIN, RGB>(leds, NUMPIXELS);
 
  leds[0] = CRGB::Red; 
  FastLED.show();
}


void loop()
{

  updateAccelerometer();

  if ( cyCentered >= thresholdUp  ) {
    if ( triggered == false ) {
      triggered = true;
      dotDoIt();
    }
  } else if ( cyCentered <= thresholdDown) {
    triggered = false;
  }

}

void updateAccelerometer() {

  if (accel.available()) {
    // First, use accel.read() to read the new variables:
    accel.read();

    cy = accel.cx;
    //cx = accel.cx;
    cyLop = lop(cy, cyLop, 0.1); 
  
    cyLopSlow = lop(cy, cyLopSlow, 0.01); 

    cyCentered = cyLop - cyLopSlow;

    //cyLopPrevious = cyLop;
    //cyPrevious = cy;

  #ifdef UDP_DEBUG
    String data = "data ";
    data += String(cy, 3);
    data += " ";
    data += String(cyCentered, 3);
    //data += " ";
   // data += String(cyLopSlow, 3);
   // data += " ";
   // data += String(cyCentered, 3); // une moyenne de Y pour faire du lissage des données
   
    data.toCharArray(udpMessageBuffer, 32) ;

    Udp.beginPacket(remoteIP, remotePort);
    Udp.write(udpMessageBuffer);
    Udp.endPacket();
    
    #endif
  }

} // fin updateAccelerometer


float lop(float target, float current , float amount) {

  return (target - current) * amount + current;

}


