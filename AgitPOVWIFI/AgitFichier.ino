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

void lireFichier(){

  Serial.println("====== Reading palabra from SPIFFS file =========");

  Serial.println("le fichier : ");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.println(dir.fileName());
    File f = dir.openFile("r");
     Serial.print("taille du fichier : ");
    Serial.println(f.size());

   String s=f.readStringUntil('\n');
      Serial.print("Texte : ");
      Serial.println(s);
      String motMem = s.substring(0, s.length()-1); // change la valeur de mot et passe-le à la fonction nouveauArray, ensuite démarre le tout.

      Serial.print("Texte : ");
      Serial.println(motMem); 
      
      String inputColor = s.substring(s.length()-1); // tente d'isoler le dernier charactère du texte en mémoire.
      inputIntColor = inputColor.toInt();

    
      Serial.print("inputIntColor : ");
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
      color = 0xCCCCCC;
      break;
    case 7:  
      Serial.println("arcoiris");
      // insert desbinario routine aqui
      color = 7;
      break;
} // fin du break
      */
      nouveauArray(motMem); // construit le tableau
      
   
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

void ecrireFichier(String unMot){

    String mot = unMot;
    File f = SPIFFS.open("/1.txt", "w");     // open file for writing
    if (!f) {
      Serial.println("file open failed");
    }
    Serial.println("====== Writing palabra to SPIFFS file =========");
    // write palabra to file
    f.print(mot);
    f.print(inputIntColor);
    f.close();

   lireFichier();
      
}
