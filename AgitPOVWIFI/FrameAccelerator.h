
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"
//#include "Responsive.h"

extern int povArrayLength; // désolé
extern int sampleCounter;

class FrameAccelerator {

    struct Axis {
      float value = 0;
      float max = 0;
      float min = 0;
      float tempMax = 0;
      float tempMin = 0;
      float range = 0;
      float tempSum = 0;
      float tempSum2 = 0;
      float average = 0;
      float stdev = 0;
    };
    
    MMA8452Q mma8452q;

    bool triggered = false;

    float frame = 0;
    float previousFrame = 0;

    unsigned long last_min_max_time_check = 0;
    unsigned long minMaxInterval = 1000;

    float speed;
    float rotation;
    bool hitHi = false;
    bool hitLo = false;
    //Responsive yResponsive = Responsive(false);

    int sampleCounter = 0;

  public:

    Axis x, y, z;

  private:


    //////// LOP FILTER //////////
    float lop(float target, float current , float amount) {
      return (target - current) * amount + current;
    }

    bool wheelAngleTrigger = false;
    unsigned long wheelInterval = 500;
    unsigned long lasTimeDetectedWheelInterval = 0;

    void updateAxis(Axis* axis) {
      if (axis->value > axis->tempMax) { //see if we've hit a maximum for this interval
        axis->tempMax = axis->value;
        // axis->max = axis->value; // était en commentaire
      }
      if (axis->value < axis->tempMin)  {//see if we've hit a minimum for this interval
        axis->tempMin = axis->value;
        // axis->min = axis->value; // était en commentaire
      }
      axis->tempSum+=axis->value;
      axis->tempSum2+=axis->value*axis->value;
    }

    void updateMinMax(Axis* axis) {
      axis->max = axis->tempMax;
      axis->min = axis->tempMin;
      axis->range = axis->max - axis->min;
      axis->average = axis->tempSum/sampleCounter;
      axis->stdev = sqrt(axis->tempSum2/sampleCounter - axis->average*axis->average);
      axis->tempMax = -8;
      axis->tempMin = 8;
      axis->tempSum=0;
      axis->tempSum2=0;
    }

  public:

    ////////////
    // SETUP //
    ////////////
    void setup() {

      mma8452q.init(SCALE_8G, ODR_800); // +-8G, 800 Hz 
     
    }

    ////////////
    // UPDATE //
    ////////////
    void update() {

      if (mma8452q.available()) {
        // First, use accel.read() to read the new variables:
        mma8452q.read();

        sampleCounter++;

        x.value = lop(mma8452q.cx,x.value,0.2);
        y.value = lop(mma8452q.cy,y.value,0.2);
        //z.value = mma8452q.cz;

        //yResponsive.update( map(y.value,-8.0,8.0,0.,1023.) );

        updateAxis(&x);
        updateAxis(&y);
        //updateAxis(&z);

#ifdef UDP_DEBUGING
        static int w=0;
        w+=1;
        if (w==4) { 
          udp.beginPacket(destinationIp, 9999);
          udp.print("sample ");
          udp.print(x.value); udp.print(" ");
          udp.print(y.value); udp.print(" ");
          udp.print(frame/(float)povArrayLength); udp.print(" ");
          udp.print((y.value - y.average) / (y.stdev * sqrt(2))); // yOscillation de 2017.09.04
          udp.println();
          udp.endPacket();
          w=0;
        }
#endif
        if (millis() - last_min_max_time_check >= minMaxInterval) //check if an interval has passed - get new the new range
        {
          last_min_max_time_check = millis();

          updateMinMax(&x);
          updateMinMax(&y);
          //updateMinMax(&z);

#ifdef UDP_DEBUGING
          udp.beginPacket(destinationIp, 9999);
          udp.print("x ");  udp.print(x.min);
          udp.print(" ");   udp.print(x.max);
          udp.print(" ");   udp.print(x.average);
          udp.print(" ");   udp.print(x.stdev);
          udp.print(" y "); udp.print(y.min);
          udp.print(" ");   udp.print(y.max);
          udp.print(" ");   udp.print(y.average);
          udp.print(" ");   udp.print(y.stdev);
          udp.print(" samples ");
          udp.print(sampleCounter);
//          udp.print( " responsive ");
//          udp.print(yResponsive.getValue() );
          udp.print(" mode "); udp.print(triggered);
          udp.print(" "); udp.print(hitHi);
          udp.print(" "); udp.print(hitLo);
          udp.println();
          udp.endPacket();
#endif          
          sampleCounter = 0;

        }
      }
    }

    //////////
    // SHAKE //
    //////////
    bool shaken(float threshold) {
      return (abs(x.range) > threshold) || (abs(y.range) > threshold) || (abs(z.range) > threshold);
    }

    //////////
    // WAVE //
    //////////
    bool wave(int frameCount, float threshold) {
      if ( (y.range) != 0 ) {
        frame = (y.value - y.min) / (y.range); // 0-1
        // GET RID OF THE CURVE
        if ( frame < 0.5 ) frame = sin(frame * PI - (PI * 0.5)) * 0.5 + 0.5;
        else frame = sin((frame - 0.5) * PI) * 0.5 + 0.5;
        frame *= frameCount;
      } else {
        frame = 0;
      }

      // limit so everything stays in range
      if ( frame < 0 ) frame = 0;
      if ( frame > frameCount - 1 ) frame = frameCount - 1;

      // is triggered only if over threshold
      triggered =   abs(y.range) > threshold ;

      if ( triggered ) minMaxInterval = 1000;
      return triggered;
    }

    ///////////
    // WHEEL //
    ///////////
    bool wheel(int frameCount, int wheelSize) { // true s'il reste à afficher, false si fini
      if (y.range == 0) {
        frame = 0;
        triggered = false;
        return triggered;
      }
      //  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
      //float yOscillation = (y.value - y.min)*2/y.range - 1;
      float yOscillation = (y.value - y.average) / (y.stdev * sqrt(2)); 
      yOscillation = constrain(yOscillation, -1.0, 1.0);
      speed = -0.00025 * abs(x.min);// SPEED INCREASES AS THE WHEEL GOES FASTER
      const float threshStart= -0.3; // -0.1; // -0.05 (original) // -0.3 // 0.25
      const float threshHi   = 0.70; // 0.65 // 0.70 (orig.) // 0.75
      const float threshLo   =-0.70;
      // ANGLE DETECTION. Y = 0 when at top of wheel. Y = -1 when at far edge of wheel. Y = 1 when at close edge of wheel. Y = 0 at bottom of whee
      if (yOscillation <= threshStart && hitLo && hitHi && !triggered) {
          triggered = true;
          hitLo=hitHi=false;
          frame = frameCount - 1;
      } else if (yOscillation>=threshHi) { // ANGLE POSSIBLE RETRIGGER DETECTION Y = 0 when at top of wheel. Y = -1 when at far edge of wheel. Y = 1 when at close edge of wheel. Y = 0 at bottom of wheel
        hitHi=true;
      } else if (yOscillation<=threshLo) {
        hitLo=true;
      }
      if (triggered) frame += speed*wheelSize;
      if (frame < 0) {
        frame = 0;
        triggered = false;
      }
      if (frame >= frameCount) {
        frame = frameCount-1;
        triggered = false;
      }

      if ( triggered ) minMaxInterval = 350; //500 est ok

      return triggered;

    }
    int getFrame() {
      return floor(frame) ;

    }

};




