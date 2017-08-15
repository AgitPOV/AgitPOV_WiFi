
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"


class FrameAccelerator {

    struct Axis {
      float value = 0;
      float max = 0;
      float min = 0;
      float tempMax = 0;
      float tempMin = 0;
      float range = 0;
     
    };

    MMA8452Q mma8452q;

    bool triggered = false;

    float frame = 0;
    float previousFrame = 0;

    unsigned long last_min_max_time_check = 0;

    float speed;
    float rotation;

    bool canRetrigger = true;

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
      }
      if (axis->value < axis->tempMin)  {//see if we've hit a minimum for this interval
        axis->tempMin = axis->value;
      }

    }

    void updateMinMax(Axis* axis) {

      axis->max = axis->tempMax;
      axis->min = axis->tempMin;
      axis->tempMax = -8;
      axis->tempMin = 8;
      axis->range = axis->max - axis->min;
    }

  public:

    ////////////
    // SETUP //
    ////////////
    void setup() {

      mma8452q.init(SCALE_8G, ODR_800);
    }


    ////////////
    // UPDATE //
    ////////////
    void update() {

      if (mma8452q.available()) {
        // First, use accel.read() to read the new variables:
        mma8452q.read();

        x.value = mma8452q.cx;
        y.value = mma8452q.cy;
        //z.value = mma8452q.cz;

        updateAxis(&x);
        updateAxis(&y);
        //updateAxis(&z);


        if (millis() - last_min_max_time_check >= 1000) //check if an interval has passed - get new the new range
        {
          last_min_max_time_check = millis();

          updateMinMax(&x);
          updateMinMax(&y);
          //updateMinMax(&z);
          
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

      return triggered;

    }


    ///////////
    // WHEEL //
    ///////////
    bool wheel(int frameCount, int wheelSize) {
      
      if ( y.range == 0  ) {
        frame = 0;
        triggered = false;
        return triggered;
      }
      float yOscillation = (y.value - y.min) * (2) / (y.range) - 1;
      speed = -0.002;// THIS SHOULD BE SLOWED DOWN WHEN THE WHEEL IS SLOW

      if ( yOscillation <= 0 ) { // ANGLE DETECTION. Y = 0 when at top of wheel. Y = -1 when at far edge of wheel. Y = 1 when at close edge of wheel. Y = 0 at bottom of wheel
        if ( canRetrigger == true ) {
          triggered = true;
          canRetrigger = false;
          frame = frameCount - 1;
        }
      } else if ( yOscillation >= 0.5 ) { // ANGLE POSSIBLE RETRIGGER DETECTION Y = 0 when at top of wheel. Y = -1 when at far edge of wheel. Y = 1 when at close edge of wheel. Y = 0 at bottom of wheel
          canRetrigger = true;
      }
      if ( triggered) frame = frame + speed * (wheelSize);

      if ( frame < 0 ) {
        frame = 0;
        triggered = false;
      }
      if ( frame >= frameCount ) {
        frame = frameCount - 1;
        triggered = false;
      }
      
      return triggered;

    }
    int getFrame() {
      return floor(frame) ;

    }

};




