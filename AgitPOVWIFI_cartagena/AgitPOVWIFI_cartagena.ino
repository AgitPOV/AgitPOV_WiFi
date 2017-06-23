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

// MAC ADDRESS /////////////////////////
uint8_t MAC_array[6];
char MAC_char[18];

// DOTSTAR /////////////////////////////
#include <Adafruit_DotStar.h>
#include <SPI.h>    // pour les dotstars
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6 
#define CLOCKPIN   D5
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR); 
uint32_t color = 0xCC3300; // couleur pour l'affichage d'un mot // naranja

// Pour un mot en entrée
int povArray[100];
int arrayOffset = 0; 
int iByte = 0;
int indexArr;

// int povArray[] = { 0 , 0 , 2040 , 2176 , 2176 , 2176 , 2040 , 0 , 2032 , 2056 , 2056 , 2120 , 1136 , 0 , 3064 , 0 , 2048 , 2048 , 4088 , 2048 , 2048 , 0 , 0 , 4088 , 2176 , 2176 , 2176 , 3968 , 0 , 2032 , 2056 , 2056 , 2056 , 2032 , 0 , 4064 , 16 , 8 , 16 , 4064 , 0 , 0 };
bool palabra;  // pour la condition d'écriture par la page web ou non
bool nouveauMot;
bool povDoIt;
//////// Pour la conversion du mot en entrée à son code pour les DELs

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

  WiFi.softAP(AP_NameChar);
  
  //WiFi.softAP("AgitPOV");  
  
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
  // lireFichier();
  
  strip.begin(); // DOTSTAR Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
   
} ///// fin du setup

void loop(void){
  
   // Serial.println("hello ESP");
   
   if(palabra == false){ // Sin palabra? escuchar el servidor, por el momento el se inciendio al reset ///
   
   if(nbrC<=0){
    dotInit(); // séquence de départ, arrête lorsque qu'un client se connecte
    }
   
   client_status(); // Si nous avons un client, arrêter la séquence doInit()
   
   dnsServer.processNextRequest(); /// a-t-on une requête de connexion ? 
   server.handleClient();
  } /// fin de la condition 'palabra' == false

  if(palabra == true && nouveauMot == true){ // Affiche le mot à répétition après avoir obtenu le mot, terminé par le passage d'un aimant
    dotNouveauMot();
    }
  
  if(palabra == true && povDoIt == true){  //Con palabra? Incendiaron los leds! 
    dotDoIt();
    }

} // fin du loop
