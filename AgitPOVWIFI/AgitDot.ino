//////////////////// CAT ///////////////////////
 void dotBlank() { // éteint toutes les lumières
   FastLED.clear();
   FastLED.show();
}

void dotPovInterval(){
  
  povInterval = _max((millis() - povTimeStamp),2)-1; // doit utiliser _max à cause d'un conflit de définition de fonction
  povTimeStamp = millis();
  povIntervalColumns = povInterval * povColumnWidth; 
  if (povIntervalColumns > 10000){

    povIntervalColumns = 10000;
  }
  
  //Serial.print("IntervalColumns : ");
  //Serial.println(povIntervalColumns); 
  
 }

void dotDoIt() {
 // Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  int arraySize = sizeof(povArray) / sizeof(int);
  for (int k = 0; k < arraySize; k++) { // la longueur du tableau qui contient le mot
   
    for (int i = 0; i <= 23; i++) { // pour chaque DEL // pour la premiére série :
  
      if ( i <=11 ) {
             if ( bitRead(povArray[k], i % 12) ) {
           
        // leds[i] = CRGB::White;
        leds[i] = color;
 
      } else {
        leds[i] = CRGB::Black; 
      }
      
      }

      else {

           if ( bitRead(povArray[arraySize-k], i % 12) ) { // bizarro world pour les DELs 23 à 12
        
        leds[35-i] = color; // countdown 23 -> 12
           
      } else {
        leds[35-i] = CRGB::Black; 
      }
        
      }
      
 
    }
    
    FastLED.show();
    delay(0);// Refresh strip //delayMicroseconds(1300); // Microseconds ne semble pas fonctionner avec le ESP8266
    delayMicroseconds(povIntervalColumns); // testing!!
    updateAccelerometer();
    
  }

  dotBlank();
} // fin dotDoIt
/////////////////// FIN CAT ///////////////////

///// testing /////

void TioDtod() {
 Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  int arraySize = sizeof(povArray) / sizeof(int);
  for (int k = 0; k < arraySize; k++) { // la longueur du tableau qui contient le mot
   
    for (int i = 0; i <= 23; i++) { // pour chaque DEL // pour la premiére série :
  
      if ( i <=11 ) {
             if ( bitRead(povArray[k], i % 12) ) {
           
        // leds[i] = CRGB::White;
        leds[i] = color;
 
      } else {
        leds[i] = CRGB::Black; 
      }
      
      }

      else {

           if ( bitRead(povArray[arraySize-k], i % 12) ) { // bizarro world pour les DELs 23 à 12
        
        leds[35-i] = color; // countdown 23 -> 12
           
      } else {
        leds[35-i] = CRGB::Black; 
      }
        
      }
      
 
    }
    
    FastLED.show();
    delay(1);// Refresh strip //delayMicroseconds(1300); // Microseconds ne semble pas fonctionner avec le ESP8266
    updateAccelerometer();
  }

  dotBlank();
} // fin dotDoIt

////// fin testing /////



 void dotInit(){ // séquence de départ
   
  for(byte i = 0;i<=23;i++){ /// UP!!
      
      for(byte j = 0;j<=120;j=j+50){
      
      leds[i] = color;
      //strip.setBrightness(j); 
      //strip.setPixelColor(i, color); 
      //strip.show(); 
      yield(); // donne un peu de temps au wifi
      // delay(1);
      // yield();
      }
      FastLED.show();
      delay(50);
      yield();
    }    
     dotBlank();  
 } // fin dotInit
