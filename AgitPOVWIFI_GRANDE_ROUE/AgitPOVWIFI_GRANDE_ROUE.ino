// Version 2017-08-29
/*
   AgitPOV Wifi: 24-RGB LED dual sided POV with Wifi (ESP8266)

   (c) 2011-2017
   Contributors over the years

        Thomas Ouellet Fredericks - Debuging, Accelerometer, LED engine and animation code
        Alexandre Castonguay
        Alan Kwok
        Andre Girard andre@andre-girard.com
        Sofian Audry
        Mariangela Aponte Nuñez
        Jean-Pascal Bellemare
        Daniel Felipe Valencia dfvalen0223@gmail.com
        Alex Keeling

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// #include "AgitPage.h"
#include <ESP8266WiFi.h> // Server // Thank you Ivan Grokhotkov 
// const char* ssid     = "objet";
// const char* password = "alx4670gar";
// const char* host = "cociclo.io";
const char* ssid     = "Agit-KM3";
const char* password = "$1Agit2POV3KM34!";
const char* host = "192.168.32.15";
/* 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"     // pour écrire en mémoire avec SPIFFS,  FS.h - file system wrapper
// De :  http://www.esp8266.com/viewtopic.php?f=32&t=5669&start=4#sthash.OFdvQdJF.dpuf ///
extern "C" { // Infos sur les clients connectés
#include<user_interface.h>
}
*/

////////// ACCEL ////////////

#include <Math.h>
#include "FrameAccelerator.h"
FrameAccelerator frameAccelerator;
///////// FIN ACCEL /////////

/*
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);
*/

////////// Leds //////////
#include "Leds.h"
Leds leds;

int colorId = 7; // see matching colors in Leds.h, 7 is RAINBOW

#define POV_ARRAY_MAX_SIZE 180

int povArray[POV_ARRAY_MAX_SIZE];
int povArrayLength = 0;

String line;
int inputIntColor = 0;

bool waitingForNewWord = false;

int previousFrameDisplayed = -1;
bool blanked;

//bool wheelModeWasTriggered = false;
//unsigned long wheelModeWasTriggeredTime = 0;

// TIMER
#include <elapsedMillis.h>
elapsedMillis timeElapsed;

// delay in milliseconds between blinks of the LED
unsigned int interval = 30000; // 30 secondes

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long intervalle = 0;

void setup(void) {

  Serial.begin(115200);
/////// connecte au RPI
delay(100);

  // We start by connecting to a WiFi network
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  
  lireMot();
  
  leds.setup();

  leds.blank();

  Serial.println("Setuping frameAccelerator");
  frameAccelerator.setup();
  
  leds.fill(colorId);

  Serial.println("Fading");
  leds.blockingFadeOut(colorId, 1000);

  Serial.println("Good to go");

} ///// fin du setup

void blank() {

  if ( !blanked ) {
    blanked = true;
    leds.blank();
  }
}



void loop() {

if (timeElapsed > interval) { 
Serial.print("check pour un mot");
lireMot();
  timeElapsed = 0;  
}


  // Update accelerator values
  frameAccelerator.update();


  // MODE SELECTOR
  if (  frameAccelerator.y.range < 6   ) {

    // WHEEL MODE
    if ( frameAccelerator.wheel(povArrayLength, POV_ARRAY_MAX_SIZE) ) {

      int frame = frameAccelerator.getFrame();
      if ( frame != previousFrameDisplayed) {
        previousFrameDisplayed = frame;
        // display         side a,          side b,                               with this colorId
        leds.displayFrame( povArray[povArrayLength - frame - 1], povArray[frame] , colorId);
      }
      blanked = false;
    } else {
      blank();
    }

  } else  {

    // WAVE MODE

    //   bool wave(int frameCount, float threshold) return true of it is triggered
    if ( frameAccelerator.wave(povArrayLength, 2) ) {
      int frame = frameAccelerator.getFrame();
      if ( frame != previousFrameDisplayed) {
        previousFrameDisplayed = frame;
        // display         side a,          side b,                               with this colorId
        leds.displayInversedFrame( povArray[frame], povArray[povArrayLength - frame - 1]  , colorId);
      }
      blanked = false;
    } else {
      blank();
    }
  }

} // fin du loop




