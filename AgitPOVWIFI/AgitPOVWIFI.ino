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

////////// ACCEL ////////////
#include <Chrono.h>
#include <Math.h>
///////// FIN ACCEL /////////

////////// FastLED //////////
#include <FastLED.h>
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6
#define CLOCKPIN   D5
CRGB leds[NUMPIXELS];
///////// FIN FastLED ///////

const byte DNS_PORT = 53;
byte nbrC; // nombre de clients connectés
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

// MAC ADDRESS /////////////////////////
uint8_t MAC_array[6];
char MAC_char[18];

uint32_t color = 0x221100; // couleur pour l'affichage d'un mot // naranja

// Pour un mot en entrée
int povArray[100];
int arrayOffset = 0; 
int iByte = 0;
int indexArr;

// int povArray[] = { 0 , 0 , 2040 , 2176 , 2176 , 2176 , 2040 , 0 , 2032 , 2056 , 2056 , 2120 , 1136 , 0 , 3064 , 0 , 2048 , 2048 , 4088 , 2048 , 2048 , 0 , 0 , 4088 , 2176 , 2176 , 2176 , 3968 , 0 , 2032 , 2056 , 2056 , 2056 , 2032 , 0 , 4064 , 16 , 8 , 16 , 4064 , 0 , 0 };
bool nouveauMot;
// bool povDoIt;
bool inicio = true;
//////// Pour la conversion du mot en entrée à son code pour les DELs

//////// accel //////////////////////
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
/////////// FIN accel //////////////

//////// Test interval entre les colonnes ////////

unsigned long povIntervalColumns = 3300;
int povColumnWidth = 4;
volatile unsigned long povInterval = 1100;
volatile unsigned long povTimeStamp;

/////////////////////////////////////////////////

int inputIntColor = 0;

int ouvertureServeur = 60000; // ouvrir le serveur pendant une minute

void setup(void){

  Serial.begin(115200);
// MAC ADDRESS ////////
    WiFi.macAddress(MAC_array);
    for (int i = 0; i < sizeof(MAC_array); ++i){
      sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
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

for (int i=0; i<AP_NameString.length(); i++){
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
lireFichier(); 

Serial.println("Début init LEDs");
Serial.println("////////// Cuidado, los LEDs necesitan la bateria par iniciarse!! ///////////////");
Serial.println("/////// Attention, les LEds ont besoin de la pile pour s'initialiser !! /////////");
////////// CAT accel FastLEDs ///////////
  accel.init(SCALE_8G, ODR_800);

  FastLED.addLeds<APA102, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
  // FastLED.addLeds<WS2801,DATAPIN, CLOCKPIN, RGB>(leds, NUMPIXELS);
/*
 for (int i = 0; i <24; i++){
  leds[i] = CRGB::Blue; 
 }
 */
  leds[0] = CRGB::Blue; 
  FastLED.show();

  Serial.println("fin init LEDs");
/////////// FIN CAT accel FastLEDs //////

} ///// fin du setup

void loop(){
  // Serial.println("world?");
   if(inicio == true){ // abrir el servidor al inicio //


  //    Serial.println("hello");
      
   if(nbrC<=0){
    dotInit(); // séquence de départ, arrête lorsque qu'un client se connecte
    }
   
   client_status(); // Si nous avons un client, arrêter la séquence doInit()
   
   dnsServer.processNextRequest(); /// a-t-on une requête de connexion ? 
   server.handleClient();

    if(millis() > ouvertureServeur){ // temps d'ouverture du serveur pour recevoir les mots '5000' pour tester 
      inicio = false; 
      turnItOff(); // fermeture du serveur
   }
  } /// fin de la condition initiale

 else if (inicio != true) {

  // détection pour savoir si on est en mode 'manuel' et non 'vélo'
  // peut-être en utilisant cx et cy au début?
  // nécessite de comparer les résultats entre la roue de vélo et la main

//////////// CAT LOOP /////////////
  updateAccelerometer();
  
  // Serial.print("cyCentered : ");
  // Serial.println(cyCentered);
  if ( cyCentered >= thresholdUp  ) {
      dotPovInterval();
    // Serial.println("errr?");
    if ( triggered == false ) {
      triggered = true;
      dotDoIt();
      // compter l'intervalle entre les tours
    }
  } else if ( cyCentered <= thresholdDown) {
    triggered = false;
    // TioDtod(); // on écrit à l'envers, un test
  }
  
////////// FIN CAT LOOP ///////////
  
}

} // fin du loop
