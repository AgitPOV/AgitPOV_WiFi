 
 void dotStop(){ // éteint toutes les lumières
    
   for(byte i = 0;i<=23;i++){
      strip.setBrightness(0); 
      strip.setPixelColor(i, 0); 
      strip.show(); 
      }
  }

 void dotInit(){ // séquence de départ 
    for(byte i = 0;i<=23;i++){ /// UP!!  
      for(byte j = 0;j<=120;j=j+50){
      strip.setBrightness(j); 
      strip.setPixelColor(i, color); 
      strip.show(); 
      yield(); // donne un peu de temps au wifi
      delay(1);
      yield();
      } 
    } 
     dotStop();
 } // fin dotInit

 void dotIndique(){
    for (int i=0;i<=23;i++){
      strip.setPixelColor(i,color);
      }
      strip.setBrightness(60); 
      strip.show(); 
      yield(); // donne un peu de temps au wifi
      delay(100);
      yield();
 }

 void dotNouveauMot(){
  // Serial.println(nouveauMot);
  for (int k=0;k<sizeof(povArray)/sizeof(int);k++){
    for (int i=0;i<=23;i++){
      strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
      }
    strip.setBrightness(200);
    strip.show();   
    delay(3);// Refresh strip //delayMicroseconds(1300);    // semble ne plus fonctionner
    }                 
 } // fin dotNouveauMot

void dotDoIt(){
 // Serial.println("povArray :");
  for (int k=0;k<sizeof(povArray)/sizeof(int);k++){  // la longueur du tableau qui contient le mot
 //   Serial.println(povArray[k]);
    // for (int i=0;i<=23;i++){ // pour chaque DEL
    for (int i=23;i>=0;i--){ // pour chaque DEL
    strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
    }
  strip.setBrightness(200);
  strip.show();   
  delay(2);// Refresh strip //delayMicroseconds(1300); // Microseconds ne semble pas fonctionner avec le ESP8266
  }  
 // Serial.println("povArray done");               
 povDoIt = false; // reset pour le prochain passage // est pe en train de doubler le nombre de passages d'aimant nécessaire
 nouveauMot = false;
 } // fin dotDoIt
