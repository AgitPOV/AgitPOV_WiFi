////////////// AgitPOV /////////////

#include "AgitPage.h"
#include <ESP8266WiFi.h> // Server // Thank you Ivan Grokhotkov 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"     // pour écrire en mémoire avec SPIFFS,  FS.h - file system wrapper
///// De :  http://www.esp8266.com/viewtopic.php?f=32&t=5669&start=4#sthash.OFdvQdJF.dpuf ///
extern "C" { // Infos sur les clients connectés
#include<user_interface.h>
}

const byte DNS_PORT = 53;
byte nbrC; // nombre de clients connectés
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

// DOTSTAR /////////////////////////////
#include <Adafruit_DotStar.h>
#include <SPI.h>    // pour les dotstars
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6 
#define CLOCKPIN   D5
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
// uint32_t color = 0x121212;  // couleur pour l'affichage d'un mot
uint32_t color = 0xCC3300; // naranja
//uint32_t color = 0x991100; // rouge pas trop clair
// uint32_t color = 0xFFFFFF; // test blanc

// Pour un mot en entrée
int povArray[100];
int arrayOffset = 0; 
int iByte = 0;
int indexArr;

// int povArray[] = { 0 , 0 , 2040 , 2176 , 2176 , 2176 , 2040 , 0 , 2032 , 2056 , 2056 , 2120 , 1136 , 0 , 3064 , 0 , 2048 , 2048 , 4088 , 2048 , 2048 , 0 , 0 , 4088 , 2176 , 2176 , 2176 , 3968 , 0 , 2032 , 2056 , 2056 , 2056 , 2032 , 0 , 4064 , 16 , 8 , 16 , 4064 , 0 , 0 };

// {252,510,771,771,771,510,252,0,512,516,518,1023,1023,512,512,0,774,899,705,609,561,543,526,0,385,785,561,569,621,967,387,0,30,31,16,16,1020,1022,16,0,399,783,521,521,521,1017,497,0,510,995,529,529,529,1011,486,0,3,897,961,97,49,31,15}, //7

bool palabra;  // pour la condition d'écriture par la page web ou non
bool nouveauMot;
//////// Pour la conversion du mot en entrée à son code pour les DELs

/// ACCEL
//#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"

MMA8452Q accel;
float previousCy;
const int nmbrLectures = 7;  // 10 semble fonctionner
int lectures[nmbrLectures];      // the readings from the analog input
int lectureIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float moyenne = 0;                // the average


void setup(void){

 
 
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("AgitPOV");
  dnsServer.start(DNS_PORT, "*", apIP);   // if DNSServer is started with "*" for domain name, it will reply with  // provided IP to all DNS request
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Connect to http://192.168.4.1");
  
  SPIFFS.begin(); // initialise les opérations sur le système de fichiers
  // Serial.println("Please wait 30 secs for SPIFFS to be formatted");
  // SPIFFS.format(); // Besoin une seule fois pour formatter le système de fichiers // Wait 30 secs
  // Serial.println("Spiffs formatted");
  listFiles();
  // eraseFiles(); 
  lireFichier();
  
  strip.begin(); // DOTSTAR Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
 
 accel.init(SCALE_8G);
 Serial.println("All good");
   
} ///// fin du setup

void loop(void){
  
   // Serial.println("hello ESP");
   
   if(palabra == false){ // Sin palabra? escuchar el servidor, por el momento el se inciendio al reset ///
   
   if(nbrC<=0 && piton == false){
    dotInit(); // séquence de départ, arrête lorsque qu'un client se connecte
    }
   
   client_status(); // Si nous avons un client, arrêter la séquence doInit()
   
   dnsServer.processNextRequest(); /// a-t-on une requête de connexion ? 
   server.handleClient();
  } /// fin de la condition 'palabra' == false


  if(palabra == true && nouveauMot == true){ // Affiche le mot à répétition après avoir obtenu le mot, terminé par le passage d'un aimant
    dotNouveauMot();
    }
    
   if (accel.available()) {
    accel.read();
    total = total - lectures[lectureIndex];  // subtract the last reading:
    lectures[lectureIndex] = accel.cy;
    total = total + lectures[lectureIndex];  // add the reading to the total:
    lectureIndex = lectureIndex + 1; // advance to the next position in the array:
    
    if (lectureIndex >= nmbrLectures) {   // if we're at the end of the array...
      lectureIndex = 0;  // ...wrap around to the beginning:
      }

    moyenne = total / nmbrLectures;
    // Serial.println(accel.cy);
    
     if ( (previousCy == 0 && moyenne > 0 ) ) {
        povDoIt = true;
      //  if ( (previousCy > 0 && accel.cy < 0 ) ||  (previousCy < 0 && accel.cy > 0 ) ) {
      //  if ( abs(accel.cy-previousCy) < 1 ) povDoIt = true; // 
      //  if ( abs(accel.cy-previousCy) < 1 ) povDoIt = true; // 
     }
    previousCy = moyenne;
   }
  
  if(palabra == true && povDoIt == true){  //Con palabra? Incendiaron los leds! 
    dotDoIt();
    }

} // fin du loop


