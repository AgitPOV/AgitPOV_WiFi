 
 void dotStop(){ // éteint toutes les lumières
    
   for(byte i = 0;i<=23;i++){
      strip.setBrightness(0); 
      strip.setPixelColor(i, 0); 
      strip.show(); 
      }
  }

 void dotInit(){ // séquence de départ
    
    dotStop();
    
  for(byte i = 0;i<=23;i++){ /// UP!!
      
      for(byte j = 0;j<=120;j=j+50){
      strip.setBrightness(j); 
      strip.setPixelColor(i, color); 
      strip.show(); 
      yield(); // donne un peu de temps au wifi
      delay(1);
      yield();
      }
      
     //  dotStop(); // Une seule LED allumée à la fois

    } 
     dotStop();
      
 } // fin dotInit

 void dotIndique(byte numbah){
  
  dotStop();
  
  // Serial.print("numbah : ");
  // Serial.print(numbah);
  
      strip.setBrightness(120); 
      strip.setPixelColor(numbah, color); 
      strip.show(); 
      yield(); // donne un peu de temps au wifi
      delay(1);
      yield();
      }

void dotNouveauMot(){
  // Serial.println(nouveauMot); 
  for (int k=0;k<sizeof(povArray)/sizeof(int);k++){
   // for (int i=0;i<23;i++){
    for (int i=23;i>0;i--){
      strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
      }
    strip.setBrightness(120);
    strip.show();   
    delay(3);// Refresh strip //delayMicroseconds(1300);    // semble ne plus fonctionner
    }                 
} // fin dotNouveauMot

void dotDoIt(){
 // Serial.println("un aimant");
  for (int k=0;k<sizeof(povArray)/sizeof(int);k++){  // la longueur du tableau qui contient le mot
    for (int i=0;i<23;i++){ // pour chaque DEL // change order
    strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
    }
  strip.show();   
  delay(3);// Refresh strip //delayMicroseconds(1300); // Microseconds ne semble pas fonctionner avec le ESP8266
  }                 
 povDoIt = false; // reset pour le prochain passage // est pe en train de doubler le nombre de passages d'aimant nécessaire
 nouveauMot = false;
 } // fin dotDoIt
