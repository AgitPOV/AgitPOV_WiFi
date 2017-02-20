 
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
      
      for(int k = 120;k>=0;k=k-50){ /// Down
        strip.setBrightness(k); 
        strip.setPixelColor(i, color); 
        strip.show(); 
        yield(); // donne un peu de temps au wifi
        delay(1);
        yield();
      }
       dotStop();

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
      delay(500);
      yield();
      }

void dotNouveauMot(){
  // Serial.println(nouveauMot);
  for (int k=0;k<sizeof(povArray)/sizeof(int);k++){
    for (int i=0;i<23;i++){
      strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
      }
    strip.show();   
    delay(5);// Refresh strip //delayMicroseconds(1300);    // semble ne plus fonctionner
    }                 
} // fin dotNouveauMot

void dotDoIt(){
  // Serial.println("un aimant");
  for (int k=0;k<sizeof(povArray)/sizeof(int);k++){  /// était k<42
    for (int i=0;i<23;i++){
    strip.setPixelColor(i, bitRead(povArray[k],i%12)*color);
    }
  strip.show();   
  delay(5);// Refresh strip //delayMicroseconds(1300); // Microseconds ne semble pas fonctionner avec le ESP8266
  }                 
 povDoIt = false; // reset pour le prochain passage
 nouveauMot = false;
 } // fin dotDoIt
