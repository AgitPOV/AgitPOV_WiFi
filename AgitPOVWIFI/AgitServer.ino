/////// à intégrer ////////
/*  De: https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/example-sketch-ap-web-server
   // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}
*/
void client_status() { /// This works, ty sohialsadiq!
  struct station_info *stat_info;
  nbrC= wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
  stat_info = wifi_softap_get_station_info();
  Serial.print("Nombre de clients = ");
  Serial.println(nbrC);
  
  // bPosition = 1;  // test pour donner une indication visuelle d'une connection
  yield();
  delay(50);
  yield();
  } 


void handleRoot()
{
  Serial.print("nombre arguments : ");
  Serial.println(server.args()); // un tableau contenant le nombre d'arguments  
  Serial.print("arg name 0 : ");
  Serial.println(server.argName(0));
  Serial.print("arg name 1 : ");
  Serial.println(server.argName(1));
  Serial.print("arg 0 : ");
  Serial.println(server.arg(0));
  Serial.print("arg 1 : ");
  Serial.println(server.arg(1));

  /*
server arg : 
nombre arguments : 2
arg name 0 : name
arg name 1 : value
arg 0 : AgitPOV
arg 1 : XYZ
-=-=-
server arg : motff
nombre arguments : 1
arg name 0 : AgitPOV
arg name 1 : 
arg 0 : motff
arg 1 : 
   */
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
    Serial.print("mot envoyé : ");
    Serial.println(mot);
    ecrireFichier(mot); // 
    turnItOff(); // ferme le serveur pour conserver de l'énergie   
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

void turnItOff(){
  WiFi.disconnect(); 
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  Serial.println("le serveur est fermé");
}

