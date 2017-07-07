//////// updateAccelerometer ///////////
void updateAccelerometer() {

  if (accel.available()) {
    // First, use accel.read() to read the new variables:
    accel.read();

    cy = accel.cy;
    cyLop = lop(cy, cyLop, 0.1); // calcul de cy avec un 'low pass filter'
  
    cyLopSlow = lop(cy, cyLopSlow, 0.01); 

    cyCentered = cyLop - cyLopSlow;
    
    //cyLopPrevious = cyLop;
    //cyPrevious = cy;  
  }
} // fin updateAccelerometer

//////// LOP FILTER //////////
float lop(float target, float current , float amount) {

  return (target - current) * amount + current;
}

