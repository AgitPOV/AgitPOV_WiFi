


void dotBlank() { // éteint toutes les lumières

  //for (byte i = 0; i <= 23; i++) {
    //strip.setBrightness(0);
   FastLED.clear();
    FastLED.show();
  //}
}


void dotDoIt() {
 // Serial.println("un aimant");
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
