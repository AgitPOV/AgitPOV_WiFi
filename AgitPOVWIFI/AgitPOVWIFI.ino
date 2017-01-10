
///// from https://gist.github.com/bbx10/5a2885a700f30af75fc5

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"     // pour écrire en mémoire avec SPIFFS

// DOTSTAR ///////////////////////////////////////////////
#include <Adafruit_DotStar.h>
#include <SPI.h>    // pour les dotstars
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6 
#define CLOCKPIN   D5
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

// Pour un mot en entrée
int povArray[100];
// AGITPOV
// int povArray[] = { 0 , 0 , 2040 , 2176 , 2176 , 2176 , 2040 , 0 , 2032 , 2056 , 2056 , 2120 , 1136 , 0 , 3064 , 0 , 2048 , 2048 , 4088 , 2048 , 2048 , 0 , 0 , 4088 , 2176 , 2176 , 2176 , 3968 , 0 , 2032 , 2056 , 2056 , 2056 , 2032 , 0 , 4064 , 16 , 8 , 16 , 4064 , 0 , 0 };
boolean palabra;  // pour la condition d'écriture par la page web ou non
uint32_t color = 0x121212;  // couleur pour l'affichage d'un mot
/////////////////////////////////////////////// DOTSTAR //

///////////////// HALL SENSOR //////
#define HALL_PIN D3               //
volatile boolean povDoIt = false; //
////////////////////////////////////

////////////////
boolean nouveauMot;

//////// Pour la conversion du mot en entrée à son code pour les DELs
char lettre = 'Z';
//int povArray[100] = {};
int arrayOffset = 0; // variable globale??

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

const char INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>AgitPOV</title>"
"<style>"
//"\"body { background-color: #FF00FF; font-family: Arial, Helvetica, Sans-Serif;  }\""

"h1 {  font-family: Arial, Helvetica, sans-serif;Color: #030303;}"
"p {  font-family: Arial, Helvetica, sans-serif;Color: #030303;font-size: 30px;}"

"</style>"
"</head>"
"<body bgcolor=\"#EE7700\" >"
"<h1>AgitPOV</h1>"
"<FORM action=\"/\" method=\"post\">"
"<P>"
"<br>"
"<INPUT type=\"text\" name=\"AgitPOV\" value=\"mot\" style=\"width:100px; height:40px;\"><BR>"
"<INPUT type=\"submit\" value=\"Send\"> <INPUT type=\"reset\">"
"</P>"
"</FORM>"
"</body>"
"</html>";

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

 ///// Serial.println("Please wait 30 secs for SPIFFS to be formatted");
SPIFFS.format(); // Besoin une seule fois ou pour effacer le mot
 //// Serial.println("Spiffs formatted");
  
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

///////////

void nouveauArray(String leMot){

 Serial.print("mot  : ");
  Serial.println(leMot);
 
  for(int i =0;i < leMot.length();i++){
    
    // Serial.print("leMot longueur : ");
    // Serial.println(leMot.length());
    lettre = leMot.charAt(i); // isole les lettres une par une
    // Serial.println(lettre);
    // delay(500);

    povArray[arrayOffset]= 0; // ajouter un espace au début, revoir sur la roue
    arrayOffset++; 
    
    switch(lettre){

      case 'A':
      {
      int AArray[] = {254 , 272 , 272 , 272 , 272 , 254};
      for(int j = 0;j<sizeof(AArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=AArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(AArray)/sizeof(int); // ajouter le nombre de colonnes au décalage pour l'écriture dans le tableau
      break;
      }

      case 'B': /// à voir la définition de B
      {
      int BArray[] = {510 , 290 , 290 , 290 , 290 , 220};
      for(int j = 0;j<sizeof(BArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=BArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(BArray)/sizeof(int);
      break;
      }

      case 'C':
      {
      int CArray[] = {510 , 290 , 290 , 290 , 290 , 220};
      for(int j = 0;j<sizeof(CArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=CArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(CArray)/sizeof(int);
      break;
      }

      case 'D':
      {
      int DArray[] = {252 , 258 , 258 , 258 , 258 , 132};
      for(int j = 0;j<sizeof(DArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=DArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(DArray)/sizeof(int);
      break;
      }

      case 'E':
      {
      int EArray[] = {252 , 258 , 258 , 258 , 258 , 132};
      for(int j = 0;j<sizeof(EArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=EArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(EArray)/sizeof(int);
      break;
      }

      case 'F':
      {
      int FArray[] = {510 , 258 , 258 , 258 , 132 , 120};
      for(int j = 0;j<sizeof(FArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=FArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(FArray)/sizeof(int);
      break;
      }

      case 'G':
      {
      int GArray[] = {510 , 288 , 288 , 288 , 256 , 120 , 132 , 258 , 274 , 274 , 156};
      for(int j = 0;j<sizeof(GArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=GArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(GArray)/sizeof(int);
      break;
      }

      case 'H':
      {
      int HArray[] = {510 , 32 , 32 , 32 , 32 , 510};
      for(int j = 0;j<sizeof(HArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=HArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(HArray)/sizeof(int);
      break;
      }

      case 'I':
      {
      int IArray[] = {258 , 510 , 258};
      for(int j = 0;j<sizeof(IArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=IArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(IArray)/sizeof(int);
      break;
      }
      
      case 'J':
      {
      int JArray[] = {2 , 258 , 258 , 508};
      for(int j = 0;j<sizeof(JArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=JArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(JArray)/sizeof(int);
      break;
      }

      case 'K':
      {
      int KArray[] = {510 , 32 , 32 , 80 , 398};
      for(int j = 0;j<sizeof(KArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=KArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(KArray)/sizeof(int);
      break;
      }

      case 'L':
      {
      int LArray[] = {510 , 2 , 2 , 2 , 2};
      for(int j = 0;j<sizeof(LArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=LArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(LArray)/sizeof(int);
      break;
      }

      case 'M':
      {
      int MArray[] = {510 , 384 , 96 , 24 , 96 , 384 , 510};
      for(int j = 0;j<sizeof(MArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=MArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(MArray)/sizeof(int);
      break;
      }

      case 'N':
      {
      int NArray[] = {510 , 384 , 96 , 24 , 6 , 510};
      for(int j = 0;j<sizeof(NArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=NArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(NArray)/sizeof(int);
      break;
      }

      case 'O':
      {
      int OArray[] = {120 , 132 , 258 , 258 , 258 , 132 , 120};
      for(int j = 0;j<sizeof(OArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=OArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(OArray)/sizeof(int);
      break;
      }

      case 'P':
      {
      int PArray[] = {510 , 272 , 272 , 272 , 224};
      for(int j = 0;j<sizeof(PArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=PArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(PArray)/sizeof(int);
      break;
      }

      case 'Q':
      {
      int QArray[] = {120 , 132 , 258 , 258 , 266 , 132 , 122};
      for(int j = 0;j<sizeof(QArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=QArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(QArray)/sizeof(int);
      break;
      }

      case 'R':
      {
      int RArray[] = {510 , 272 , 272 , 280 , 228 , 2};
      for(int j = 0;j<sizeof(RArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=RArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(RArray)/sizeof(int);
      break;
      }

      case 'S':
      {
      int SArray[] = {196 , 290 , 290 , 274 , 274 , 140};
      for(int j = 0;j<sizeof(SArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=SArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(SArray)/sizeof(int);
      break;
      }

      case 'T':
      {
      int TArray[] = {256 , 256 , 510 , 256 , 256};
      for(int j = 0;j<sizeof(TArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=TArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(TArray)/sizeof(int);
      break;
      }
      
      case 'U':
      {
      int UArray[] = {508 , 2 , 2 , 2 , 2 , 508};
      for(int j = 0;j<sizeof(UArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=UArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(UArray)/sizeof(int);
      break;
      }

      case 'V':
      {
      int VArray[] = {448 , 56 , 6 , 6 , 56 , 448};
      for(int j = 0;j<sizeof(VArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=VArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(VArray)/sizeof(int);
      break;
      }

      case 'W':
      {
      int WArray[] = {384 , 120 , 6 , 120 , 384 , 120 , 6 , 120 , 384};
      for(int j = 0;j<sizeof(WArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=WArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(WArray)/sizeof(int);
      break;
      }

      case 'X':
      {
      int XArray[] = {390 , 72 , 48 , 48 , 72 , 390};
      for(int j = 0;j<sizeof(XArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=XArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(XArray)/sizeof(int);
      break;
      }

      case 'Y':
      {
      int YArray[] = {512 , 256 , 128 , 126 , 128 , 256 , 512};
      for(int j = 0;j<sizeof(YArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=YArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(YArray)/sizeof(int);
      break;
      }

      case 'Z':
      {
      int ZArray[] = {518 , 522 , 530 , 546 , 578 , 642 , 770};
      for(int j = 0;j<sizeof(ZArray)/sizeof(int);j++){
        povArray[j+arrayOffset]=ZArray[j];  
      }
      arrayOffset = arrayOffset+sizeof(ZArray)/sizeof(int);
      break;
      }

      
      } /////////////////////////////////// fin du switch lettre


  } // fin du for 

// Serial.println("arrayOffset : ");
// Serial.println(arrayOffset);

/*    for(int k=0;k<sizeof(povArray)/sizeof(int);k++){  // sizeof donne le nombre de valeurs * la taille du type de données contenue
        Serial.println(k);
        Serial.print("povArray : ");
        Serial.println(povArray[k]); 
        }
*/
  
}  //// fin de la fonction 'nouveauArray()'



///////////

void loop(void)
{
  ////////////// Sin palabra? Abrir servidor, por el momento el se inciendia al reset ///////////
   if(palabra == false){

  //////////  

////vuint32_t color = 0xCC3300; // naranja

uint32_t color = 0x991100; // rouge pas trop clair
    //////////  
 //////// uint32_t color = 0xFFFFFF; // test blanc
  

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
  // Serial.println(k);
  strip.show();   
  delay(5);// Refresh strip
  //delayMicroseconds(1300);    // semble ne plus fonctionner

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
