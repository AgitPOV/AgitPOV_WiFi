
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
    Serial.print("mot envoyé : ");
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
      Serial.print("mot en mémoire : ");
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


