
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"


class FameAccelerator {

    MMA8452Q mma8452q;

    bool triggered = false;
  
    float frame = 0;

    float max_accel = 0;
    float min_accel = 0;
    float temp_max_accel = 0;
    float temp_min_accel = 0;
    unsigned long last_accel_time_check = 0;
    unsigned long last_frame_time_check = 0;


    //////// LOP FILTER //////////
    float lop(float target, float current , float amount) {
      return (target - current) * amount + current;
    }
  public:
    void setup() {
      mma8452q.init(SCALE_8G, ODR_800);
    }

    bool wave(int frameCount, float threshold) {

      if (mma8452q.available()) {
        // First, use accel.read() to read the new variables:
        mma8452q.read();

        float accelG = mma8452q.cy;

        if (accelG > temp_max_accel) //see if we've hit a maximum for this interval
        {
          temp_max_accel = accelG;
        } else if (accelG < temp_min_accel)  {//see if we've hit a minimum for this interval
          temp_min_accel = accelG;
        }

        if (millis() - last_accel_time_check >= 1000) //check if an interval has passed - get new the new range
        {
          max_accel = temp_max_accel;//(temp_max_accel-max_accel)*0.1+max_accel;
          min_accel = temp_min_accel;//(temp_min_accel-min_accel)*0.1+min_accel;
          temp_max_accel = 0;
          temp_min_accel = 0;
          last_accel_time_check = millis();
          
          Serial.print("mm ");
          Serial.print(min_accel);
          Serial.print(" ");
          Serial.println(max_accel);
          
        }

        if ( (max_accel - min_accel) != 0 ) {
          frame = (accelG - min_accel) / (max_accel - min_accel); // 0-1

          // GET RID OF THE CURVE
          if ( frame < 0.5 ) frame = sin(frame*PI-(PI*0.5))*0.5+0.5;
          else frame = sin((frame-0.5)*PI)*0.5+0.5;

          frame *= frameCount;
        } else {
          frame = 0;
        }

        // limit so everything stays in range
        if ( frame < 0 ) frame = 0;
        if ( frame > frameCount - 1 ) frame = frameCount - 1;

        // is triggered only if over threshold
        triggered =   abs(max_accel - min_accel) > threshold ;
       
      }

       return triggered;

    }

   

    int getFrame() {
      return floor(frame) ;

    }

};





