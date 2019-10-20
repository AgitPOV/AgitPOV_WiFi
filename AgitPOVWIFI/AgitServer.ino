int getNumberOfClients() {
  //struct station_info *stat_info;
  int nbrC = wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
  //stat_info = wifi_softap_get_station_info();
  //Serial.print("Nombre de clients = ");
  //Serial.println(nbrC);
  return nbrC;
  
}


void handleRoot() {
  
  if (lecture == true) {
    lecture = false;
    Serial.println("on a demandé lireFichier()");
    String mot = lireFichier();
    doc = String(INDEX_HTML) + mot + String(INDEX_HTML2);
    }
  else if (server.hasArg("AgitPOV")) {
    Serial.println("on avait server.hasArg = AgitPOV");
    handleSubmit();
  }
  else {
    server.send(200, "text/html", doc);
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

  inputIntColor = server.arg("color").toInt() + (server.arg("dark").toInt()*7); // génère les couleurs de 0 à 13
  if (!server.hasArg("AgitPOV")) return returnFail("BAD ARGS");
  String mot = server.arg("AgitPOV");
  Serial.print("mot envoyé : ");
  Serial.println(mot);

  Serial.print("Couleur envoyée : ");
  Serial.println(inputIntColor);

  String cmd = server.arg("cmd");
  if (cmd=="GO") {
    colorId = inputIntColor;
    ecrireFichier(mot);
    waitingForNewWord = false;
  } else if (cmd=="LS") {
    String answer = "";
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      File f = dir.openFile("r");
      answer += dir.fileName() + " " + f.size() + "\n";
    }
    server.send(200, "text/plain", answer);
  } else if (cmd=="AFFICHE") {
    if (SPIFFS.exists(mot)) {
      File f = SPIFFS.open(mot,"r");
      Serial.println("va afficher le fichier "+mot);
      int t=0;
      while (f.position()<f.size()) {
        //Serial.println(f.readString());
        Serial.print(f.readStringUntil('\n'));
        Serial.print(" ");
        Serial.println(t);
        t++;
        if ((t%100)==0) {yield();}
      }
      Serial.println("fin de "+mot);
      /*
      String answer = "";
      char *buf = new char[f.size()];
      f.readBytes(buf,f.size());
      //while (f.position() < f.size()) answer.concat(f.readString());
      answer = new String();
      server.send(200, "text/plain", answer);
      */
      //server.streamFile(f,"text/plain");
    } else {
      server.send(404, "text/plain", mot + "not found");      
    }
  } else if (cmd=="EFFACE") {
    String answer = "";
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String leFichier = dir.fileName();
      if (leFichier.startsWith("/record")) {
        SPIFFS.remove(leFichier);
        answer += "efface " + leFichier + "<br>\n";
      }
    }
    server.send(200, "text/plain", answer);    
  }
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
