/******************************************************************************
MMA8452Q_Basic.ino
SFE_MMA8452Q Library Basic Example Sketch
Jim Lindblom @ SparkFun Electronics
Original Creation Date: June 3, 2014
https://github.com/sparkfun/MMA8452_Accelerometer

This sketch uses the SparkFun_MMA8452Q library to initialize the
accelerometer, and stream values from it.

Hardware hookup:
  Arduino --------------- MMA8452Q Breakout
    3.3V  ---------------     3.3V
    GND   ---------------     GND
  SDA (A4) --\/330 Ohm\/--    SDA
  SCL (A5) --\/330 Ohm\/--    SCL

The MMA8452Q is a 3.3V max sensor, so you'll need to do some 
level-shifting between the Arduino and the breakout. Series
resistors on the SDA and SCL lines should do the trick.

Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Uno
	
	**Updated for Arduino 1.6.4 5/2015**

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.



******************************************************************************/
// DOTSTAR /////////////////////////////
#include <Adafruit_DotStar.h>
#include <SPI.h>    // pour les dotstars
#define NUMPIXELS 24 // Number of LEDs in strip
#define DATAPIN    D6 
#define CLOCKPIN   D5
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
// uint32_t color = 0x121212;  // couleur pour l'affichage d'un mot
uint32_t color = 0xCC3300; // naranja
//uint32_t color = 0x991100; // rouge pas trop clair
// uint32_t color = 0xFFFFFF; // test blanc

#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library
#include <Math.h>

// Begin using the library by creating an instance of the MMA8452Q
//  class. We'll call it "accel". That's what we'll reference from
//  here on out.
MMA8452Q accel;

// The setup function simply starts serial and initializes the
//  accelerometer.
int AgitAngle = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("MMA8452Q Test Code!");
  strip.begin(); // DOTSTAR Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  
  
  // Choose your adventure! There are a few options when it comes
  // to initializing the MMA8452Q:
  //  1. Default init. This will set the accelerometer up
  //     with a full-scale range of +/-2g, and an output data rate
  //     of 800 Hz (fastest).
  //accel.init();
  //  2. Initialize with FULL-SCALE setting. You can set the scale
  //     using either SCALE_2G, SCALE_4G, or SCALE_8G as the value.
  //     That'll set the scale to +/-2g, 4g, or 8g respectively.
  //accel.init(SCALE_4G); // Uncomment this out if you'd like
  //  3. Initialize with FULL-SCALE and DATA RATE setting. If you
  //     want control over how fast your accelerometer produces
  //     data use one of the following options in the second param:
  //     ODR_800, ODR_400, ODR_200, ODR_100, ODR_50, ODR_12,
  //     ODR_6, or ODR_1. 
  //     Sets to 800, 400, 200, 100, 50, 12.5, 6.25, or 1.56 Hz.
  accel.init(SCALE_8G, ODR_800);
}

// The loop function will simply check for new data from the
//  accelerometer and print it out if it's available.
void loop()
{
  // Use the accel.available() function to wait for new data
  //  from the accelerometer.
  if (accel.available())
  {
    // First, use accel.read() to read the new variables:
    accel.read();

    AgitAngle = round(atan2 (accel.cx,accel.cy) * 180/3.14159265 ); // radians to degrees and rounding
    //Serial.println(AgitAngle);
      if(AgitAngle > -120 && AgitAngle < -50){
      //  Serial.println("Bon AgitAngle");
        dotInit();
        }
    
    // accel.read() will update two sets of variables. 
    // * int's x, y, and z will store the signed 12-bit values 
    //   read out of the accelerometer.
    // * floats cx, cy, and cz will store the calculated 
    //   acceleration from those 12-bit values. These variables 
    //   are in units of g's.
    // Check the two function declarations below for an example
    // of how to use these variables.
  // printCalculatedAccels();
    //printAccels(); // Uncomment to print digital readings
    
    // The library also supports the portrait/landscape detection
    //  of the MMA8452Q. Check out this function declaration for
    //  an example of how to use that.
    // printOrientation();
    
    // Serial.println(); // Print new line every time.
  }
}

// The function demonstrates how to use the accel.x, accel.y and
//  accel.z variables.
// Before using these variables you must call the accel.read()
//  function!
void printAccels()
{
  Serial.print(accel.x, 3);
  Serial.print("\t");
  Serial.print(accel.y, 3);
  Serial.print("\t");
  Serial.print(accel.z, 3);
  Serial.print("\t");
}

// This function demonstrates how to use the accel.cx, accel.cy,
//  and accel.cz variables.
// Before using these variables you must call the accel.read()
//  function!
void printCalculatedAccels()
{ 
  /*
  Serial.print(accel.cx, 3);
  Serial.print("\t");
  Serial.print(accel.cy, 3);
  Serial.print("\t");
  Serial.print(accel.cz, 3);
  Serial.print("\t");
  */
  AgitAngle = round(atan2 (accel.cx,accel.cy) * 180/3.14159265 ); // radians to degrees and rounding
   Serial.println(AgitAngle);
  if(AgitAngle > -90 && AgitAngle < -80){
    Serial.println("Bon AgitAngle");
   dotInit();
  }
}

// This function demonstrates how to use the accel.readPL()
// function, which reads the portrait/landscape status of the
// sensor.
void printOrientation()
{
  // accel.readPL() will return a byte containing information
  // about the orientation of the sensor. It will be either
  // PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L, or
  // LOCKOUT.
  byte pl = accel.readPL();
  switch (pl)
  {
  case PORTRAIT_U:
    Serial.print("Portrait Up");
    dotStop();
    break;
  case PORTRAIT_D:
    Serial.print("Portrait Down");
    dotStop();
    break;
  case LANDSCAPE_R:
    Serial.print("Landscape Right");
    dotStop();
    break;
  case LANDSCAPE_L:
    Serial.print("Landscape Left");
    dotInit();
    break;
  case LOCKOUT:
    Serial.print("Flat");
    dotStop();
    break;
  }
}

 void dotStop(){ // éteint toutes les lumières
    
   for(byte i = 0;i<=23;i++){
      strip.setBrightness(0); 
      strip.setPixelColor(i, 0); 
      strip.show(); 
      }
  }

   void dotInit(){ // séquence de départ
    
    dotStop();
    
  for(byte i = 0;i<=2;i++){ /// UP!!
      
      for(byte j = 0;j<=120;j=j+50){
      strip.setBrightness(j); 
      strip.setPixelColor(i, color); 
      strip.show(); 
      yield(); // donne un peu de temps au wifi
      delay(1);
      yield();
      }
      
    /*  for(int k = 120;k>=0;k=k-50){ /// Down
        strip.setBrightness(k); 
        strip.setPixelColor(i, color); 
        strip.show(); 
        yield(); // donne un peu de temps au wifi
        delay(1);
        yield();
      }*/
     //  dotStop(); // Une seule LED allumée à la fois

    } 
     dotStop();
      
 } // fin dotInit
