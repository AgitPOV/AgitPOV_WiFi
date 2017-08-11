
#include <Wire.h> // Must include Wire library for I2C
#include "SparkFun_MMA8452Q_ESP8266.h"


class FrameAccelerator {

    struct Axis {
      float value = 0;
      float max = 0;
      float min = 0;
      float maxLop = 0;
      float minLop = 0;
      float tempMax = 0;
      float tempMin = 0;
      float range = 0;
      float rangeLop = 0;
    };



    MMA8452Q mma8452q;

    bool triggered = false;

    float frame = 0;
    float previousFrame = 0;

    unsigned long last_min_max_time_check = 0;

    float speed;
    float rotation;

  public:

    Axis x, y, z;

  private:



#ifdef UDP_DEBUG
    unsigned long lastTimeSentUdp;
#endif



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


      axis->maxLop = lop(-8, axis->maxLop, 0.0001);
      if (axis->value > axis->maxLop ) {
        axis->maxLop = axis->value;
      }

      axis->minLop = lop(8, axis->minLop, 0.0001);
      if (axis->value < axis->minLop ) {
        axis->minLop = axis->value;
      }

      axis->rangeLop = axis->maxLop - axis->minLop;


    }

    void updateMinMax(Axis* axis) {

      axis->max = axis->tempMax;
      axis->min = axis->tempMin;
      axis->tempMax = -8;
      axis->tempMin = 8;
      axis->range = axis->max - axis->min;
    }

  public:
    void setup() {

      mma8452q.init(SCALE_8G, ODR_800);
    }



    void update() {



      if (mma8452q.available()) {
        // First, use accel.read() to read the new variables:
        mma8452q.read();

        x.value = mma8452q.cx;
        y.value = mma8452q.cy;
        z.value = mma8452q.cz;

        updateAxis(&x);
        updateAxis(&y);
        updateAxis(&z);



        if (millis() - last_min_max_time_check >= 1000) //check if an interval has passed - get new the new range
        {
          last_min_max_time_check = millis();

          updateMinMax(&x);
          updateMinMax(&y);
          updateMinMax(&z);


#ifndef UDP_DEBUG
          Serial.print("x ");
          Serial.print(x.min);
          Serial.print(" ");
          Serial.println(x.max);
          Serial.print("y ");
          Serial.print(y.min);
          Serial.print(" ");
          Serial.println(y.max);
          Serial.print("z ");
          Serial.print(z.min);
          Serial.print(" ");
          Serial.println(z.max);
#else
          outgoing.beginPacket("mm");
          outgoing.addFloat(x.min);
          outgoing.addFloat(x.max);
          outgoing.addFloat(y.min);
          outgoing.addFloat(y.max);
          outgoing.addFloat(z.min);
          outgoing.addFloat(z.max);
          outgoing.endPacket();
          Udp.beginPacket(ipBroadCast, 9999);
          Udp.write(outgoing.buffer(), outgoing.size());
          Udp.endPacket();

#endif


        }

      }

#ifdef UDP_DEBUG
      if ( millis() - lastTimeSentUdp >= 10 ) {
        lastTimeSentUdp = millis();
        outgoing.beginPacket("xyz");
        outgoing.addFloat(x.value);
        outgoing.addFloat(y.value);
        outgoing.addFloat(z.value);
        outgoing.addFloat(x.minLop);
        outgoing.addFloat(x.maxLop);
        outgoing.addFloat(y.minLop);
        outgoing.addFloat(y.maxLop);
        outgoing.addFloat(z.minLop);
        outgoing.addFloat(z.maxLop);
        outgoing.endPacket();
        Udp.beginPacket(ipBroadCast, 9999);
        Udp.write(outgoing.buffer(), outgoing.size());
        Udp.endPacket();
      }
#endif
    }

    bool shaken(float threshold) {
      return (abs(x.range) > threshold) || (abs(y.range) > threshold) || (abs(z.range) > threshold);
    }

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
#ifdef UDP_DEBUG
    unsigned long lastTimeSentAngle = 0;
#endif

    bool wheel(int frameCount, int wheelSize) {

      if ( y.range == 0 ) {
        frame = 0;
        return false;
      }
      
      float newRotation = (atan2(y.value , x.value - x.minLop - 1) + PI ) / (TWO_PI) ;
      float newSpeed = newRotation - rotation;

      if ( newSpeed > 0.5) newSpeed = 1 - newSpeed;
      if ( newSpeed < -0.5 ) newSpeed = 1 + newSpeed;

      speed = lop(newSpeed, speed, 0.001);

      rotation = newRotation;


      
      float newFrame =  ( rotation) * (wheelSize);

      if ( speed > 0 ) {
        if ( newFrame > frame || newFrame - frame < frameCount * -0.5  ) frame = newFrame;
      } else {
        if ( newFrame < frame || newFrame - frame > frameCount * 0.5  ) frame = newFrame;
      }


      triggered = true;
      if ( frame < 0 ) {
        frame = 0;
      }
      if ( newFrame >= frameCount ) {
        frame = frameCount - 1;
      }



#ifdef UDP_DEBUG
      if ( millis() - lastTimeSentAngle >= 10 ) {
        outgoing.beginPacket("rotation");
        outgoing.addFloat(rotation);
        outgoing.addFloat(speed * 10000);
        outgoing.endPacket();
        Udp.beginPacket(ipBroadCast, 9999);
        Udp.write(outgoing.buffer(), outgoing.size());
        Udp.endPacket();
      }
#endif




      return triggered;

    }



    int getFrame() {
      return floor(frame) ;

    }

};




