////////////// AgitPOV /////////////

#include "AgitPage.h"
#include <ESP8266WiFi.h> ///// Server /// Thank you Ivan Grokhotkov 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"     // pour écrire en mémoire avec SPIFFS,  FS.h - file system wrapper

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

// DOTSTAR ///////////////////////////////////////////////
#include <Adafruit_DotStar.h>
#include <SPI.h>    // pour les dotstars
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6 
#define CLOCKPIN   D5
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
uint32_t color = 0x121212;  // couleur pour l'affichage d'un mot
//uint32_t color = 0xCC3300; // naranja
//uint32_t color = 0x991100; // rouge pas trop clair
// uint32_t color = 0xFFFFFF; // test blanc

// Pour un mot en entrée
int povArray[100];
int indexArr;

// int povArray[] = { 0 , 0 , 2040 , 2176 , 2176 , 2176 , 2040 , 0 , 2032 , 2056 , 2056 , 2120 , 1136 , 0 , 3064 , 0 , 2048 , 2048 , 4088 , 2048 , 2048 , 0 , 0 , 4088 , 2176 , 2176 , 2176 , 3968 , 0 , 2032 , 2056 , 2056 , 2056 , 2032 , 0 , 4064 , 16 , 8 , 16 , 4064 , 0 , 0 };
boolean palabra;  // pour la condition d'écriture par la page web ou non
boolean nouveauMot;
//////// Pour la conversion du mot en entrée à son code pour les DELs
char lettre = 'Z';
int arrayOffset = 0; 

///////////////// HALL SENSOR //////
#define HALL_PIN D3               //
volatile boolean povDoIt = false; //
////////////////////////////////////

void setup(void)
{

  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("AgitPOV");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Connect to http://192.168.4.1");

  //////// initialise les opérations sur le système de fichiers
  SPIFFS.begin();
  /*
  Serial.println("Please wait 30 secs for SPIFFS to be formatted");
  SPIFFS.format(); // Besoin une seule fois pour formatter le système de fchiers
  Serial.println("Spiffs formatted");
  */
  Serial.println("j'efface le fichier enregistré");
  SPIFFS.remove("/f.txt"); // Efface le mot présent

  
  ///////////////////////////////////////////////////
  // Commence par vérifier si un mot est présent   //
    File f = SPIFFS.open("/f.txt", "r");
      if (!f) {
        Serial.println("file open failed");
      }  
      Serial.println("====== Lecture du fichier SPIFFS =======");
      // read palabra
      String s=f.readStringUntil('\n');
      Serial.print("mot en mémoire : ");
      Serial.println(s);
      // Serial.println(s.length());
      if(s.length()>1){
        palabra = true;
        Serial.print("palabra : ");
        Serial.println(palabra);
      }
      else{
        palabra = false;
        Serial.print("palabra : ");
        Serial.println(palabra);
      }
      // Nous avons un mot, fermons le serveur pour conserver de l'énergie // à faire //
      
   
    strip.begin(); // DOTSTAR Initialize pins for output
    strip.show();  // Turn all LEDs off ASAP
  
    ////////////////////// initialize HALL with interrupt //
    pinMode(HALL_PIN, INPUT_PULLUP); // D3 and D4 have an internal pullup 10k resistor
    attachInterrupt(HALL_PIN, hallInterrupt, FALLING);
        
  } ///// fin du setup


void loop(void)
  {
  /// Sin palabra? Abrir servidor, por el momento el se inciendio al reset ///
   if(palabra == false){


//// for(int i = 0;i<=23;i++){

  ////   for(int j = 0;j<=120;j++){
    ///     strip.setBrightness(j); 
  ///      strip.setPixelColor(i, color); 
  ///      strip.show(); 
  ////      delay(1); 
   ////     }
        
        
        /*for(int k = 0;k<=12;k++){ // pour éteindre les lumières
        strip.setPixelColor(k, 000000);  
        strip.show();
        delay(1);
        */
   ////  }
     

     delay(100);
  ///////////// a-t-on une requête de connexion ? /////////////
  ///////////// doit être accéléré parce que le serveur n'est pas disponible assez longtemps
  dnsServer.processNextRequest();
  server.handleClient();
  } /// fin de la condition 'palabra' == false

/////////// Con palabra? Incendiaron los leds! ////////

if(palabra == true && povDoIt == true){  
Serial.println("un aimant");

for (int k=0;k<sizeof(povArray)/sizeof(int);k++){  /// était k<42
  // Serial.print(" K :");
  // Serial.println(k);

  for (int i=0;i<23;i++){
  strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
  }

  strip.show();   
  delay(5);// Refresh strip
  //delayMicroseconds(1300);    // Microseconds ne semble plus fonctionner

}                 
 povDoIt = false; // reset 
 nouveauMot = false;
}

//////// Affiche le mot à répétition après avoir obtenu le mot, terminé par le passage d'un aimant
if(palabra == true && nouveauMot == true){

//if(palabra == true){ // pour entrer dans cette routine avec un mot programmé dans le code
  
for (int k=0;k<66;k++){
   //Serial.print(" K :");

  for (int i=0;i<23;i++){
    strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
    }

  strip.show();   
  delay(5);// Refresh strip
  //delayMicroseconds(1300);    // semble ne plus fonctionner

  }                 

 /// Serial.println(nouveauMot);
}


} // fin du loop


void hallInterrupt() {

  povDoIt = true;
  /*// Restart
  _povArrayIndex = _povArraySize-1;
  povInterval = max((millis() - povTimeStamp),2)-1;
  povTimeStamp = millis() ;
  povIntervalColumns = povInterval  * _povColumnWidth ; 
  */
}
