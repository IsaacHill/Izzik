#include <Wire.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#define PIN 6
Adafruit_LSM303 lsm;

const float Pi = 3.14159;
int start = 0;

int prevX = 0;
int currentX = 0;

int prevY = 0;
int currentY = 0;

int prevZ = 0;
int currentZ = 0;

int prevZMag = 0;
int currentZMag = 0;

//Initialize mode as unset, 1 defines dancing 2 definces running.
int mode = 0;
int i = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  strip.begin();
  strip.setBrightness(20);
  strip.show();
  while (!Serial);
  Serial.begin(9600); 
  Serial.println("Accelerometer Test"); Serial.println(""); 
  // initialize the digital pin as an output.   
  if(!lsm.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}
 
// the loop routine runs over and over again forever:
void loop() {
  //Need to find the mode as given by users motion
  while(mode == 0) {
    lsm.read();
    if(prevX == 0 && prevY == 0 &&prevZ) {
       prevX = (int)lsm.accelData.x;
       prevY = (int)lsm.accelData.y;
       prevZ = (int)lsm.accelData.z;
       prevZMag = (int)lsm.magData.z;
     } 
     else{
       currentX = (int)lsm.accelData.x;
       currentY = (int)lsm.accelData.y; 
       currentZ = (int)lsm.accelData.z;
       currentZMag = (int)lsm.magData.z;
     }
     //Serial.print("Base X: "); Serial.print((int)lsm.accelData.x); Serial.print(" ");
     //Serial.print("Y: "); Serial.print((int)lsm.accelData.y);       Serial.print(" ");
     //Serial.print("Z: "); Serial.println((int)lsm.accelData.z);     Serial.print(" ");
     //Serial.print("Mag Z: "); Serial.print((int)lsm.magData.z);     Serial.print(" ");
     //Serial.print("CHANGE X: "); Serial.print(prevX - currentX); Serial.print(" ");
     //Serial.print("Y: "); Serial.print((int)prevY - currentY);       Serial.print(" ");
     //Serial.print("Z: "); Serial.println((int)prevZ - currentZ);     Serial.print(" ");
     //Serial.print("MaG Z: "); Serial.print((int)prevZMag - currentZMag);     Serial.print(" ");
     float direction = (atan2(lsm.magData.y,lsm.magData.x) * 180) / Pi;
     Serial.print(direction); Serial.print(" ");
     prevX = currentX;
     prevY = currentY;
     prevZ = currentZ;
     if(i == 0) {
      colorWipe(strip.Color(0, 0, 255),strip.Color(255,0, 0), 100);
      i = 1; 
    }
    else {
      colorWipe(strip.Color(102, 0, 255),strip.Color(0, 0,255), 100);
      i=0;
    }
     delay(200);
  }
  //In dancing mode
  //while (mode == 1) {
  //  
  //}
  //In runnning mode
  //while (mode == 2) {
  //  
  //}
  Serial.print("Accel X: "); Serial.print(" ");
  lsm.read();
  Serial.print("Accel X: "); Serial.print((int)lsm.accelData.x); Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.accelData.y);       Serial.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.accelData.z);     Serial.print(" ");
  Serial.print("Mag X: "); Serial.print((int)lsm.magData.x);     Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.magData.y);         Serial.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.magData.z);       Serial.print(" ");
  if(start = 0){
     prevX = lsm.accelData.x;
     prevY = lsm.accelData.y;
     start = 1;
  }
  //if(lsm.accelData.x - prevX >  100) {
  //     colorWipe(strip.Color(0, 255, 0),strip.Color(0, 0,255), 0);
  //}
  //  if(lsm.accelData.x -prevX < -100) {
  //     colorWipe(strip.Color(0, 0, 255),strip.Color(255,0, 0), 0);
  //}
  Serial.print("Yo");

  Serial.print("Oh");
  prevX = lsm.accelData.x;
  prevY = lsm.accelData.y;
  delay(300);
}

void colorWipe(uint32_t c,uint32_t d, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i == 0) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
    }
    else{
      strip.setPixelColor(i, d);
      strip.show();
      delay(wait);
    }
  }
}
