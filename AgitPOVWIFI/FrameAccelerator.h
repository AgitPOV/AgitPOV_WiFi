
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"


class FrameAccelerator {

    MMA8452Q mma8452q;

    bool triggered = false;

    float frame = 0;

    unsigned long last_min_max_time_check = 0;

    float cx = 0;
    float x_max = 0;
    float x_min = 0;
    float temp_x_max = 0;
    float temp_x_min = 0;
    float x_range = 0;

    float cy = 0;
    float y_max = 0;
    float y_min = 0;
    float temp_y_max = 0;
    float temp_y_min = 0;
    float y_range = 0;

    float cz = 0;
    float z_max = 0;
    float z_min = 0;
    float temp_z_max = 0;
    float temp_z_min = 0;
    float z_range = 0;







    //////// LOP FILTER //////////
    float lop(float target, float current , float amount) {
      return (target - current) * amount + current;
    }
  public:
    void setup() {

      mma8452q.init(SCALE_8G, ODR_800);
    }

    void update() {

      if (mma8452q.available()) {
        // First, use accel.read() to read the new variables:
        mma8452q.read();

        cx = mma8452q.cx;
        cy = mma8452q.cy;
        cz = mma8452q.cz;

        if (cx > temp_x_max) { //see if we've hit a maximum for this interval
          temp_x_max = cx;
        } else if (cx < temp_x_min)  {//see if we've hit a minimum for this interval
          temp_x_min = cx;
        }


        if (cy > temp_y_max) { //see if we've hit a maximum for this interval
          temp_y_max = cy;
        } else if (cy < temp_y_min)  {//see if we've hit a minimum for this interval
          temp_y_min = cy;
        }


        if (cz > temp_z_max) { //see if we've hit a maximum for this interval
          temp_z_max = cz;
        } else if (cz < temp_z_min)  {//see if we've hit a minimum for this interval
          temp_z_min = cz;
        }

        if (millis() - last_min_max_time_check >= 1000) //check if an interval has passed - get new the new range
        {
          last_min_max_time_check = millis();

          x_max = temp_x_max;
          x_min = temp_x_min;
          temp_x_max = 0;
          temp_x_min = 0;
          x_range = x_max - x_min;

          y_max = temp_y_max;
          y_min = temp_y_min;
          temp_y_max = 0;
          temp_y_min = 0;
          y_range = y_max - y_min;


          z_max = temp_z_max;
          z_min = temp_z_min;
          temp_z_max = 0;
          temp_z_min = 0;
          z_range = z_max - z_min;

          Serial.print("x ");
          Serial.print(x_min);
          Serial.print(" ");
          Serial.println(x_max);
          Serial.print("y ");
          Serial.print(y_min);
          Serial.print(" ");
          Serial.println(y_max);
          Serial.print("z ");
          Serial.print(z_min);
          Serial.print(" ");
          Serial.println(z_max);

        }

      }
    }

    bool shaken(float threshold) {
      return (abs(x_range) > threshold) || (abs(y_range) > threshold) || (abs(z_range) > threshold);
    }

    bool wave(int frameCount, float threshold) {


      if ( (y_range) != 0 ) {
        frame = (cy - y_min) / (y_range); // 0-1

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
      triggered =   abs(y_range) > threshold ;

      return triggered;

    }



    int getFrame() {
      return floor(frame) ;

    }

};





