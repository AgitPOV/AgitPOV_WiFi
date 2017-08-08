
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"


class FameAccelerator {

    MMA8452Q mma8452q;

    bool triggered = false;
    bool done = false;

    const float sensitivity = 2;


    float frameIncrementor = 0;

    int normal_accel;
    int last_normal_accel = 0;
    float max_accel = 8;
    float min_accel = -8;
    float temp_max_accel = 0;
    float temp_min_accel = 0;
    unsigned long last_accel_time_check = 0;
    unsigned long last_frame_time_check = 0;
    int swing_direction = 1;



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

        if (accelG > temp_max_accel) //see if we've hit a maximum for this second
        {
          temp_max_accel = accelG;
        } else if (accelG < temp_min_accel)  {//see if we've hit a minimum for this second
          temp_min_accel = accelG;
        }

        if (millis() - last_accel_time_check > 1000) //check if a second has passed - get new acceleration range
        {
          max_accel = temp_max_accel;
          min_accel = temp_min_accel;
          temp_max_accel = 0;
          temp_min_accel = 0;
          last_accel_time_check = millis();
          Serial.print("mm ");
          Serial.print(min_accel);
          Serial.print(" ");
          Serial.println(max_accel);
        }



       

        if ( (max_accel - min_accel) != 0 ) {
          frameIncrementor = (accelG - min_accel) / (max_accel - min_accel); // 0-1
          //if ( frameIncrementor < 0.5 ) frameIncrementor = sin(frameIncrementor*PI*0.5)*0.5;
          //else frameIncrementor = BROKEN sin(frameIncrementor*PI*0.5-(PI*0.5))*0.5+1;

          // GET RID OF THE CURVE
          if ( frameIncrementor < 0.5 ) frameIncrementor = sin(frameIncrementor*PI-(PI*0.5))*0.5+0.5;
          else frameIncrementor = sin((frameIncrementor-0.5)*PI)*0.5+0.5;

          
          frameIncrementor *= frameCount;
        } else {
          frameIncrementor = 0;
        }

        
        if ( frameIncrementor < 0 ) frameIncrementor = 0;
        if ( frameIncrementor > frameCount - 1 ) frameIncrementor = frameCount - 1;

        Serial.print("> ");
        Serial.print(accelG);
        Serial.print(" ");
        Serial.println(frameIncrementor);

        triggered =   abs(max_accel - min_accel) > threshold ;
       
      }

       return triggered;

    }

   

    int getFrame() {
      return floor(frameIncrementor) ;

    }

};





