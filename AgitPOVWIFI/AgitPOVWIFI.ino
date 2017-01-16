
/*
 * AgitPOV
 * 12-RGB LED POV for Arduino
 *
 * (c) 2011-2017
 * Contributors over the years 
 
        Alan Kwok
        Alexandre Castonguay
        Sofian Audry
        Mariangela Aponte Nuñez
        Jean-Pascal Bellemare
        Daniel Felipe Valencia dfvalen0223@gmail.com
        Andre Girard andre@andre-girard.com 
        Alex Keeling
        Thomas Ouellet Fredericks
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


///// from https://gist.github.com/bbx10/5a2885a700f30af75fc5

#include "AgitPage.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"     // pour écrire en mémoire avec SPIFFS
#include <string.h>
#include <string>

// DOTSTAR ///////////////////////////////////////////////
#include <Adafruit_DotStar.h>
#include <SPI.h>    // pour les dotstars
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6 
#define CLOCKPIN   D5
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
/////////////////////////////////////////////// DOTSTAR //

// Pour un mot en entrée
int povArray[100];
int indexArr;
// AGITPOV
// int povArray[] = { 0 , 0 , 2040 , 2176 , 2176 , 2176 , 2040 , 0 , 2032 , 2056 , 2056 , 2120 , 1136 , 0 , 3064 , 0 , 2048 , 2048 , 4088 , 2048 , 2048 , 0 , 0 , 4088 , 2176 , 2176 , 2176 , 3968 , 0 , 2032 , 2056 , 2056 , 2056 , 2032 , 0 , 4064 , 16 , 8 , 16 , 4064 , 0 , 0 };
boolean palabra;  // pour la condition d'écriture par la page web ou non
boolean nouveauMot;
//////// Pour la conversion du mot en entrée à son code pour les DELs
char lettre = 'Z';
int arrayOffset = 0; 
uint32_t color = 0x121212;  // couleur pour l'affichage d'un mot
//uint32_t color = 0xCC3300; // naranja
//uint32_t color = 0x991100; // rouge pas trop clair
// uint32_t color = 0xFFFFFF; // test blanc
  

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

///////////////// HALL SENSOR //////
#define HALL_PIN D3               //
volatile boolean povDoIt = false; //
////////////////////////////////////


void handleRoot()
{
  if (server.hasArg("AgitPOV")) {
    handleSubmit();
  }
  else {
    server.send(200, "text/html", INDEX_HTML);
  }
}

void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void handleSubmit()
{
  String mot;

  if (!server.hasArg("AgitPOV")) return returnFail("BAD ARGS");
    mot = server.arg("AgitPOV");
    Serial.println("mot envoyé : ");
    Serial.println(mot);
    server.send(200, "text/html", INDEX_HTML);
    
    ////////////////////////////////////////
    // open file for writing
    File f = SPIFFS.open("/f.txt", "w");
    if (!f) {
      Serial.println("file open failed");
    }
    Serial.println("====== Writing palabra to SPIFFS file =========");
    // write palabra to file
    f.print(mot);
    f.close();

    // open file for reading
    f = SPIFFS.open("/f.txt", "r");
      if (!f) {
        Serial.println("file open failed");
      }  
      Serial.println("====== Reading from SPIFFS file =======");
      // read palabra
      String s=f.readStringUntil('\n');
      Serial.println("mot en mémoire : ");
      Serial.println(s);
      mot = s; // change la valeur de mot et passe-le à la fonction nouveauArray, ensuite démarre le tout.
      nouveauArray(mot);
      arrayOffset = 0;
      palabra = true;
      nouveauMot = true; // essai de le faire afficher le mot à répétition.
  }
 
void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


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

  SPIFFS.begin();

Serial.println("Please wait 30 secs for SPIFFS to be formatted");
SPIFFS.format(); // Besoin une seule fois ou pour effacer le mot
Serial.println("Spiffs formatted");
  
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
      Serial.println(s.length());
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
        ////////  DOTSTAR /////////////////////////////////////
        strip.begin(); // Initialize pins for output
        strip.show();  // Turn all LEDs off ASAP
        ////////////////////////////////////////// DOTSTAR ////

    ////////////////////// initialize HALL with interrupt //
    pinMode(HALL_PIN, INPUT_PULLUP); // D3 and D4 have an internal pullup 10k resistor
    attachInterrupt(HALL_PIN, hallInterrupt, FALLING);
        
} ///// fin du setup


void loop(void)
  {
  /// Sin palabra? Abrir servidor, por el momento el se inciendio al reset ///
   if(palabra == false){

  /*for(int i = 0;i<=12;i++){

    for(int j = 0;j<=120;j++){
        strip.setBrightness(j); 
        strip.setPixelColor(i, color); 
        strip.show(); 
        delay(1); 
        }
        /*for(int k = 0;k<=12;k++){ // pour éteindre les lumières
        strip.setPixelColor(k, 000000);  
        
        strip.show();
        delay(1);
    }
    */
    
 ///////////// a-t-on une requête de connexion ? /////////////
 ///////////// doit être accéléré parce que le serveur n'est pas disponible assez longtemps
  dnsServer.processNextRequest();
  server.handleClient();
  } /// fin de la condition 'palabra' == false

/////////// Con palabra? Incendiaron los leds! ////////

if(palabra == true && povDoIt == true){  
Serial.println("un aimant");

for (int k=0;k<sizeof(povArray)/sizeof(int);k++){  /// était k<42
//Serial.print(" K :");
//Serial.println(k);

strip.setPixelColor(0, bitRead(povArray[k], 0)*color); 
strip.setPixelColor(1, bitRead(povArray[k], 1)*color); 
strip.setPixelColor(2, bitRead(povArray[k], 2)*color); 
strip.setPixelColor(3, bitRead(povArray[k], 3)*color); 
strip.setPixelColor(4, bitRead(povArray[k], 4)*color); 
strip.setPixelColor(5, bitRead(povArray[k], 5)*color); 
strip.setPixelColor(6, bitRead(povArray[k], 6)*color); 
strip.setPixelColor(7, bitRead(povArray[k], 7)*color); 
strip.setPixelColor(8, bitRead(povArray[k], 8)*color); 
strip.setPixelColor(9, bitRead(povArray[k], 9)*color); 
strip.setPixelColor(10, bitRead(povArray[k], 10)*color); 
strip.setPixelColor(11, bitRead(povArray[k], 11)*color); 
  // Serial.println(k);
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


strip.setPixelColor(0, bitRead(povArray[k], 0)*color); 
strip.setPixelColor(1, bitRead(povArray[k], 1)*color); 
strip.setPixelColor(2, bitRead(povArray[k], 2)*color); 
strip.setPixelColor(3, bitRead(povArray[k], 3)*color); 
strip.setPixelColor(4, bitRead(povArray[k], 4)*color); 
strip.setPixelColor(5, bitRead(povArray[k], 5)*color); 
strip.setPixelColor(6, bitRead(povArray[k], 6)*color); 
strip.setPixelColor(7, bitRead(povArray[k], 7)*color); 
strip.setPixelColor(8, bitRead(povArray[k], 8)*color);
strip.setPixelColor(9, bitRead(povArray[k], 9)*color); 
strip.setPixelColor(10, bitRead(povArray[k], 10)*color);
strip.setPixelColor(11, bitRead(povArray[k], 11)*color); 

// Serial.println(k);
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
