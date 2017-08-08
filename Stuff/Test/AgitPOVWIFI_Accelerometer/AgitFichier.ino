// http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html

// called by button or handleclient()

// lit le système de fichiers, s'il est vide : écrit un fichier avec le mot
// écrit une ligne dans le fichier
// 12 lignes
// SPIFFS.rename(pathFrom, pathTo)
// SPIFFS.open(path, mode)

// do the blinking
// 
// variable globale 'position' pour savoir quel fichier nous ouvrons

// au démarrage, affiche séquence initiale et ouvre le serveur 
// si bouton, envoie feedback de position à l'utilisateur et 
// interrompu par l'accelérometre, dès qu'il bouge, éteint le serveur et joue le premier fichier


// 1. lire les fichiers dans le dossier
// 1.txt, 2.txt, 3.txt ... 11.txt

void listFiles(){

  Serial.println("les fichiers : ");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());
    Serial.println(" ");
    }
}

void eraseFiles(){
  
  String leFichier;
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) { // itératif tant qu'il y a des fichiers
    Serial.print("J'efface : ");
    Serial.println(dir.fileName());
    Serial.println(" ");
    leFichier = dir.fileName();
    //SPIFFS.remove("/f.txt"); // Efface le mot présent
    SPIFFS.remove(leFichier); 
    
    //File f = dir.openFile("r");
    }
}

void lireFichier(){

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
  
}

void ecrireFichier(String unMot){

    String mot = unMot;
    File f = SPIFFS.open("/1.txt", "w");     // open file for writing
    if (!f) {
      Serial.println("file open failed");
    }
    Serial.println("====== Writing palabra to SPIFFS file =========");
    // write palabra to file
    f.print(mot);
    f.close();

    // open file for reading
    f = SPIFFS.open("/1.txt", "r");
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
