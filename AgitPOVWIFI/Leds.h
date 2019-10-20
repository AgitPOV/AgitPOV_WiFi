#include <FastLED.h>

#define DATAPIN    D6
#define CLOCKPIN   D5

class Leds {
    CRGB leds[24];

#define COLOR_COUNT 14

    //                           rojo,    naranja, amarillo,   verde,    azul,     morado,   luz, dark rojo, dark naranja, dark amarillo, dark verde, dark azul, muy dark morado, darkluz
    CRGB colors[COLOR_COUNT] = { 0xFF0000, 0xCC3300, 0xFFFF00, 0x00FF00, 0x0000FF, 0xFF00FF, 0xEEEEEE, 0x110000, 0x221100, 0x111100, 0x112200, 0x000011, 0x110011, 0x080808 };


  public:

    void setup() {

      FastLED.addLeds<APA102, DATAPIN, CLOCKPIN, BGR>(leds, 24);


    }
    void blank() { // éteint toutes les lumières
      FastLED.clear();
      FastLED.show();
      
    }



CRGB colorIdToColor(int colorId, int rainbowOffset) {
      if ( colorId == 42 ) return colors[(rainbowOffset / 2) % COLOR_COUNT];

      else if ( colorId > 42) return colors[( ( rainbowOffset / 2 ) + 7) % COLOR_COUNT];
      
      else if ( colorId == 0 ) {
        // Serial.println("on a du rouge!");
        // return colors[0]; // n'arrivas pas à envoyer du rouge, peut-être du au '0' de l'argument?
        return 0xFF0000;
      }
     
      else {
       // Serial.print( "couleur unie : ");
       // Serial.println(colors[colorId]);
        return colors[colorId];
      }
      
    }

///////////////////////////////////////////////////////
    /*
        void blockingAnimation(int colorId, unsigned long duration) {
          unsigned long timeStarted = millis();
          float animationCounter = 0;
          while ( millis() - timeStarted < duration ) {
            byte grey = floor(sin(animationCounter*0.4-PI*0.5)*127+128);
            animationCounter++;
            for (int i = 0; i < 24; i++) {
              leds[i] = grey | ( grey << 8 ) | ( grey << 16 ) ;
            }
            FastLED.show();
            unsigned long yieldStarted = millis(); while ( millis() - yieldStarted <= 100 ) yield(); // 100 ms yield delay
          }
        }
    */

    void nonBlockingOsXAnimation() {

      //byte grey = floor(sin(millis() * 0.005 - PI * 0.5) * 127 + 128); // full intensity
      byte grey = floor(sin(millis() * 0.005 - PI * 0.5) * 32 + 32); // half intensity

      for (int i = 0; i < 24; i++) {
        leds[i] = grey | ( grey << 8 ) | ( grey << 16 ) ;
      }
      FastLED.show();

    }

    void blockingFadeOut(int colorId, unsigned long duration) {
      Serial.print("couleur de fadeOut : ");
      Serial.println(colorId);
      for (int i = 0; i < 24; i++) {
        leds[i] = colorIdToColor(colorId, i) ;
      }
      FastLED.show();

      unsigned long startTime = millis();
      unsigned long stepInterval = duration / 256;

      while ( millis() - startTime < duration ) {
        for (int i = 0; i < 24; i++) {
          leds[i]--;
        }

        FastLED.show();
        unsigned long yieldStarted = millis(); while ( millis() - yieldStarted <= stepInterval ) yield(); // stepInterval ms yield delay
      }
    }

    // Does an initilizing sequence timed with millis(). Returns immediatly.
    void nonBlockingRainbowAnimation() {


      for (int i = 0; i < 24 ; i++) { /// COLOR!!
        leds[i] = colorIdToColor(42, i + (millis() / 100));
      }

      FastLED.show();

    }

    /*
       // Does an initilizing sequence timed with millis(). Returns immediatly.
      void nonBlockingRainbowAnimation(int colorId) {

      int last = (millis() / 50) % 24;

      for (int  i = 0; i <= last ; i++) { /// COLOR!!
        leds[i] = colorIdToColor(colorId, i);
      }

      for (int  i = 23; i > last  ; i--) { /// BLACK!!
        leds[i] = CRGB::Black;
      }


      FastLED.show();

      }
    */

    void fill(int colorId) {

      for (int i = 0; i < 24; i++) {
        leds[i] = colorIdToColor(colorId, i);
      }
      FastLED.show();
    }

    void displayFrame(int sideAFrame, int sideBFrame, int colorId) {

      for (int i = 0; i < 12; i++) { // pour chaque DEL d'un côté
        if ( sideAFrame & 0x01 ) {
          leds[i] = colorIdToColor(colorId, i);
        } else {
          leds[i] = CRGB::Black;
        }
        sideAFrame = sideAFrame >> 1;
      }

      for (int i = 23; i > 11; i--) { // pour chaque DEL de l'autre côté dans le sens inverse vertical
        if ( sideBFrame & 0x01 ) {
          leds[i] = colorIdToColor(colorId, 23 - i);
        } else {
          leds[i] = CRGB::Black;
        }
        sideBFrame = sideBFrame >> 1;
      }

      FastLED.show();
    }

     void displayInversedFrame(int sideAFrame, int sideBFrame, int colorId) {

      for (int i = 11; i >= 0; i--) { // pour chaque DEL d'un côté
        if ( sideAFrame & 0x01 ) {
          leds[i] = colorIdToColor(colorId, i);
        } else {
          leds[i] = CRGB::Black;
        }
        sideAFrame = sideAFrame >> 1;
      }

      for (int i = 12; i < 24; i++) { // pour chaque DEL de l'autre côté dans le sens inverse vertical
        if ( sideBFrame & 0x01 ) {
          leds[i] = colorIdToColor(colorId, 23 - i);
        } else {
          leds[i] = CRGB::Black;
        }
        sideBFrame = sideBFrame >> 1;
      }

      FastLED.show();
    }


};
