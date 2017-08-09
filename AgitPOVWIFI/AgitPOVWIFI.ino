/*
   AgitPOV Wifi: 24-RGB LED dual sided POV with Wifi (ESP8266)

   (c) 2011-2017
   Contributors over the years

        Thomas Ouellet Fredericks - Debuging, Accelerometer and LED engine and animation code
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

#include "AgitPage.h"
#include <ESP8266WiFi.h> // Server // Thank you Ivan Grokhotkov 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"     // pour écrire en mémoire avec SPIFFS,  FS.h - file system wrapper
///// De :  http://www.esp8266.com/viewtopic.php?f=32&t=5669&start=4#sthash.OFdvQdJF.dpuf ///
extern "C" { // Infos sur les clients connectés
#include<user_interface.h>
}

////////// ACCEL ////////////
#include <Chrono.h>
#include <Math.h>
#include "FrameAccelerator.h"
FameAccelerator frameAccelerator;
///////// FIN ACCEL /////////



const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

// MAC ADDRESS /////////////////////////
uint8_t MAC_array[6];
char MAC_char[18];




////////// Leds //////////
#include "Leds.h"
Leds leds;

int colorId = 7; // see matching colors in Leds.h, 7 is RAINBOW





#define POV_ARRAY_MAX_SIZE 105


int povArray[POV_ARRAY_MAX_SIZE];
int povArrayLength = 0;
bool nouveauMot;
bool inicio = true;


//////// Pour la conversion du mot en entrée à son code pour les DELs



// int AgitAngle = 0;

/*
  float thresholdUp = 0.5;
  float thresholdDown = 0;

  bool triggered = false;
*/
/////////// FIN accel //////////////

//////// Test interval entre les colonnes ////////

/*
  unsigned long povIntervalColumns = 3300;
  int povColumnWidth = 4;
  volatile unsigned long povInterval = 1100;
  volatile unsigned long povTimeStamp;
*/
/////////////////////////////////////////////////

int inputIntColor = 0;

bool waitingForNewWord = false;


void setup(void) {

  Serial.begin(115200);

  leds.setup();

  leds.blank();

  

  // MAC ADDRESS ////////
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i) {
    sprintf(MAC_char, "%s%02x:", MAC_char, MAC_array[i]);
  }
  Serial.println(MAC_char);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  ///// MAC ID ACCESS POINT ////

  Serial.print("MAC address, last pair : ");
  Serial.println(String(MAC_array[5]));
  String AP_NameString = "AgitPOV" + String(MAC_array[5]) ;
  Serial.print("AP_NameString : ");
  Serial.println(AP_NameString);

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i = 0; i < AP_NameString.length(); i++) {
    AP_NameChar[i] = AP_NameString.charAt(i);

  }

  WiFi.softAP(AP_NameChar);  //WiFi.softAP("AgitPOV")
  dnsServer.start(DNS_PORT, "*", apIP);   // if DNSServer is started with "*" for domain name, it will reply with  // provided IP to all DNS request
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Connect to http://192.168.4.1");

  SPIFFS.begin(); // initialise les opérations sur le système de fichiers
  // Serial.println("Please wait 30 secs for SPIFFS to be formatted");
  // SPIFFS.format(); // Besoin une seule fois pour formatter le système de fichiers // Wait 30 secs
  // Serial.println("Spiffs formatted");

  // eraseFiles();
  // ecrireFichier("AgitPOV1"); // pour programmer un mot


  // WAIT FOR CONNEXION
  unsigned long keepServerOpenInterval = 45000; // ouvrir le serveur pendant 45 secondes
  unsigned long timeServerStarted = millis();

  int numberOfClients = 0;

  // WAIT FOR CLIENTS
  while ( numberOfClients <= 0 && (millis() - timeServerStarted < keepServerOpenInterval) ) {
    numberOfClients = getNumberOfClients();
    leds.nonBlockingOsXAnimation();
    yield();
  }


  


  // GOT A CONNEXION : WAIT TILL ITS CONCLUSION
  // QUIT IF THE CONNEXION IS LOST
  if ( numberOfClients > 0  ) waitingForNewWord = true ;
  
  while ( numberOfClients > 0 && waitingForNewWord ) {

    leds.nonBlockingRainbowAnimation();

    dnsServer.processNextRequest(); /// a-t-on une requête de connexion ?
    server.handleClient();

    yield();

    numberOfClients = getNumberOfClients();

  }

  turnItOff(); // fermeture du serveur

  lireFichier();

  leds.fill(colorId);

  Serial.println("Setuping frameAccelerator");
  frameAccelerator.setup();

  Serial.println("Fading");
  leds.blockingFadeOut(colorId,2500);

  Serial.println("Good to go");

} ///// fin du setup

void loop() {

  //   bool wave(int frameCount, float threshold)
  if ( frameAccelerator.wave(povArrayLength, 2) ) {
    int frame = frameAccelerator.getFrame();

    // display         side a,          side b,                               with this colorId
    leds.displayFrame( povArray[povArrayLength - frame - 1], povArray[frame] , colorId);

  } else {
    leds.blank();
  }

} // fin du loop
