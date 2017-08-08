int getNumberOfClients() {
  //struct station_info *stat_info;
  int nbrC = wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
  //stat_info = wifi_softap_get_station_info();
  //Serial.print("Nombre de clients = ");
  //Serial.println(nbrC);
  return nbrC;
  
}


void handleRoot() {
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

void handleSubmit() // ajouter l'enregistrement de la couleur
{
  String mot;

  String inputColor = server.arg(1);
  inputIntColor = inputColor.toInt();
  if (!server.hasArg("AgitPOV")) return returnFail("BAD ARGS");
  mot = server.arg("AgitPOV");
  Serial.print("mot envoyé : ");
  Serial.println(mot);

  Serial.print("Couleur envoyée : ");
  Serial.println(inputIntColor);

  colorId = inputIntColor;

/*
  //// switch //// à intégrer en fonction parce qu'elle est ré-écrite deux fois
  switch (inputIntColor) {
    case 0:
      Serial.println("rojo");
      color = 0xFF0000; // rojo
      break;
    case 1:
      Serial.println("naranja");
      color = 0xCC3300; // naranja
      break;
    case 2:
      Serial.println("amarillo"); // jaune
      color = 0xFFFF00;
      break;
    case 3:
      Serial.println("verde");
      color = 0x00FF00;
      break;
    case 4:
      Serial.println("azul"); // bleu
      color = 0x0000FF;
      break;
    case 5:
      Serial.println("morado"); // mauve
      color = 0xFF00FF;
      break;
    case 6:
      Serial.println("luz"); // lumière
      color = 0xFFFFFF;
      break;
    case 7:
      Serial.println("arcoiris");
      // insert desbinario routine aqui
      break;
  } // fin du break
  */
  ecrireFichier(mot); //
  turnItOff(); // ferme le serveur pour conserver de l'énergie
  inicio = false; // si on a un nouveau mot alors affiche tout de suite
} // fin de handleSubmit

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
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void turnItOff() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  Serial.println("le serveur est fermé");
}

