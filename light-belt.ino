#include <Wire.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#define PIN 6
Adafruit_LSM303 lsm;

// Pin D7 has an LED connected on FLORA.
// give it a name:
int led = 7;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  while (!Serial);
  Serial.begin(9600); 
  Serial.println("Accelerometer Test"); Serial.println(""); 
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);   
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
  Serial.print("Accel X: "); Serial.print(" ");
  lsm.read();
  Serial.print("Accel X: "); Serial.print((int)lsm.accelData.x); Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.accelData.y);       Serial.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.accelData.z);     Serial.print(" ");
  Serial.print("Mag X: "); Serial.print((int)lsm.magData.x);     Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.magData.y);         Serial.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.magData.z);       Serial.print(" ");
  if(lsm.accelData.x > 20) {
       colorWipe(strip.Color(0, 255, 0), 0);
  }
    if(lsm.accelData.x < -20) {
       colorWipe(strip.Color(0, 0, 255), 0);
  }
  delay(200);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
     strip.show();
     delay(wait);
  }
}
