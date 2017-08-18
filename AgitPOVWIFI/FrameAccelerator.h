
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"
//#include "Responsive.h"

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
    unsigned long minMaxInterval = 1000;

    float speed;
    float rotation;

    bool canRetrigger = true;
    //Responsive yResponsive = Responsive(false);



#ifdef UDP_DEBUG
    int sampleCounter = 0;
#endif

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
        // axis->max = axis->value;
      }
      if (axis->value < axis->tempMin)  {//see if we've hit a minimum for this interval
        axis->tempMin = axis->value;
        // axis->min = axis->value;
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

#ifdef UDP_DEBUG
        sampleCounter++;
#endif

        x.value = mma8452q.cx;
        y.value = mma8452q.cy;
        //z.value = mma8452q.cz;

        //yResponsive.update( map(y.value,-8.0,8.0,0.,1023.) );

        updateAxis(&x);
        updateAxis(&y);
        //updateAxis(&z);


        if (millis() - last_min_max_time_check >= minMaxInterval) //check if an interval has passed - get new the new range
        {
          last_min_max_time_check = millis();

          updateMinMax(&x);
          updateMinMax(&y);
          //updateMinMax(&z);

#ifdef UDP_DEBUGING
          udp.beginPacket(destinationIp, 9999);
          udp.print("x ");
          udp.print(x.min);
          udp.print(" ");
          udp.print(x.max);
          udp.print(" y ");
          udp.print(y.min);
          udp.print(" ");
          udp.print(y.max);
          udp.print(" samples ");
          udp.print(sampleCounter);
//          udp.print( " responsive ");
//          udp.print(yResponsive.getValue() );
          udp.println();
          udp.endPacket();
          sampleCounter = 0;
#endif

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
    bool wheel(int frameCount, int wheelSize) {

      if ( y.range == 0  ) {
        frame = 0;
        triggered = false;
        return triggered;
      }
      //  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
      float yOscillation = (y.value - y.min) * (2) / (y.range) - 1;
      yOscillation = constrain(yOscillation, -1.0, 1.0);
      speed = -0.00025 * abs(x.min);// SPEED INCREASES AS THE WHEEL GOES FASTER


      
            if ( yOscillation <= -0.3 ) { // ANGLE DETECTION. Y = 0 when at top of wheel. Y = -1 when at far edge of wheel. Y = 1 when at close edge of wheel. Y = 0 at bottom of wheel
              if ( canRetrigger == true ) {
                triggered = true;
                canRetrigger = false;
                frame = frameCount - 1;
              }
            } else if ( yOscillation >= 0.70 ) { // ANGLE POSSIBLE RETRIGGER DETECTION Y = 0 when at top of wheel. Y = -1 when at far edge of wheel. Y = 1 when at close edge of wheel. Y = 0 at bottom of wheel
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

      if ( triggered ) minMaxInterval = 500;

      return triggered;

    }
    int getFrame() {
      return floor(frame) ;

    }

};




