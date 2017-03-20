


void dotBlank() { // éteint toutes les lumières

  //for (byte i = 0; i <= 23; i++) {
    //strip.setBrightness(0);
   FastLED.clear();
    FastLED.show();
  //}
}




void dotDoIt() {
  //Serial.println("un aimant");
  int arraySize = sizeof(povArray) / sizeof(int);
  for (int k = 0; k < arraySize; k++) { // la longueur du tableau qui contient le mot
    for (int i = 0; i < 23; i++) { // pour chaque DEL
      //strip.setPixelColor(i, bitRead(povArray[k], i % 12)*color);
      if ( bitRead(povArray[k], i % 12) ) {
        leds[i] = CRGB::White; 
      } else {
        leds[i] = CRGB::Black; 
      }
    }
    //updateAccelerometer();
    FastLED.show();
    //delay(2);// Refresh strip //delayMicroseconds(1300); // Microseconds ne semble pas fonctionner avec le ESP8266
    updateAccelerometer();
  }
  // povDoIt = false; // reset pour le prochain passage // est pe en train de doubler le nombre de passages d'aimant nécessaire
  // nouveauMot = false;
  dotBlank();
} // fin dotDoIt
