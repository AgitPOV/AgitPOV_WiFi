void lireMot(){
  Serial.println("===== Lecture du mot depuis le serveur ======");
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  // const int httpPort = 80;
  const int httpPort = 8000;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  // String url = "/AgitPOV/palabra0.txt";
  String url = "/palabra7.txt";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    line = client.readStringUntil('\r');
  }
    
    String motMem = line.substring(0, line.length()-3); // change la valeur de mot et passe-le à la fonction nouveauArray, ensuite démarre le tout.
    motMem.toUpperCase();
    Serial.print("Texte : ");
    Serial.println(motMem); 

    Serial.print("longueur");
    Serial.println(motMem.length());
      
    String inputColor = line.substring(line.length()-3); // tente d'isoler le dernier charactère du texte en mémoire.
    inputIntColor = inputColor.toInt();

    Serial.print("inputIntColor : ");
    Serial.println(inputIntColor); 

    colorId = inputIntColor;

    nouveauArray(motMem); // construit le tableau
  
  Serial.println();
  Serial.println("closing connection");
  
  }
