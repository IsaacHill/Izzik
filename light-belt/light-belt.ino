#include <Wire.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#define PIN 6
Adafruit_LSM303 lsm;

int start = 0;

int prevX = 0;
int currentX = 0;

int prevY = 0;
int currentY = 0;

int prevZ = 0;
int currentZ = 0;
//Initialize mode as unset, 1 defines dancing 2 definces running.
int mode = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
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
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
 
// the loop routine runs over and over again forever:
void loop() {
  //Need to find the mode as given by users motion
  //while(mode == 0) {
    
  //}
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
  }
  if(lsm.accelData.x - prevX >  100) {
       colorWipe(strip.Color(0, 255, 0), 0);
  }
    if(lsm.accelData.x -prevX < -100) {
       colorWipe(strip.Color(0, 0, 255), 0);
  }
  prevX = lsm.accelData.x;
  prevY = lsm.accelData.y;
  delay(200);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
     delay(wait);
  }
}
