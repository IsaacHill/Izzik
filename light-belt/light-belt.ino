#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_NeoPixel.h>
#include <Math.h>

#define PIN 6
Adafruit_LSM303 lsm;

//Initialize Pi, for degree calculations
const float Pi = 3.14159;
int start = 0;

float heading = 0;
int prevX = 0;
int currentX = 0;

int prevY = 0;
int currentY = 0;

int prevZ = 0;
int currentZ = 0;

float currentDirection = 0;
float previousDirection = 0;
//Initialize mode as unset, 1 defines dancing 2 defines running.
int mode = 0;

#define BRAKETHRESHOLD = 0;



float set = 0;
float i = 0;

//Initialize strip (chain of leds), first input is number of leds in chain.
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(5);
  strip.show();
  Serial.begin(9600);
  Serial.println("currentelerometer Test"); Serial.println("");
  // initialize the digital pin as an output.
  if (!lsm.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  //Need to find the mode as given by users motion
  while (mode == 0) {
    //Sets up reading frame for lsm
    lsm.read();
    currentX = (int)lsm.accelData.x;
    currentY = (int)lsm.accelData.y;
    currentZ = (int)lsm.accelData.z;
    Serial.println("Yo");
    //Find the current orientation of the device
    currentDirection = (atan2(lsm.magData.y, lsm.magData.x) * 180) / Pi;
    Serial.println("Current Direction");
    Serial.println(currentDirection);
    //We need to normalize this value to be between 0 < 360
    if (currentDirection < 0) {
      currentDirection = currentDirection + 360;
    }
    //If we know the previous direction and acceleration
    if (set == 1) {
      detectMode();
    }
    //If we dont then we need to set them and wait.
    else {
      previousDirection = currentDirection;
      prevX = currentX;
      prevY = currentY;
      prevZ = currentZ;
      set = 1;
    }
    previousDirection = currentDirection;
    //Placeholder delay need to test how fast the specific motions take,
    //i.e spinning etc probably should be lower.
    rainbowCycle(1.5);
  }
  while (mode  == 1) {
    //Sets up reading frame for lsm
    lsm.read();
    currentX = (int)lsm.accelData.x;
    currentY = (int)lsm.accelData.y;
    currentZ = (int)lsm.accelData.z;
    Serial.println("Rotation");
    colorWipe(strip.Color(255, 0, 255), strip.Color(255, 255, 255), 100);
  }
  while (mode == 2) {
    //Sets up reading frame for lsm
    lsm.read();
    currentX = (int)lsm.accelData.x;
    currentY = (int)lsm.accelData.y;
    currentZ = (int)lsm.accelData.z;
    Serial.println("Running");
    detectRunning();
    //Placeholder light cycling for testing, (lights stop cycling
    //if a mode is chosen or the code hangs for whatever reason).
    flashTest(100);
    //colorWipe(strip.Color(0, 0, 255),strip.Color(0, 255,255), 100);
  }
}

//Detects if the users motion is setting a mode and if so
//sets mode to the relevant integer.
void detectMode() {
  float rotation = abs(currentDirection - previousDirection);
  Serial.println(currentDirection);
  Serial.println(previousDirection);
  //This could be Y cant check till monday!
  float acceleration = abs(prevX - currentX);
  Serial.println("rotation");
  Serial.println(rotation);
  if (rotation > 20 ) {
    mode = 1;
  }
  //Detects if user is accelerating forward and isnt rotating
  //and if so sets belt to running mode.
  else if (acceleration > 150) {
    mode = 2;
  }
}


void colorWipe(uint32_t c, uint32_t d, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if (i == 0) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
    }
    else {
      strip.setPixelColor(i, d);
      strip.show();
      delay(wait);
    }
  }
}

//Mode that loops when mode is set to running
void detectRunning() {
  Serial.println(currentX);



}
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 1; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void flashTest(uint8_t wait) {
  Serial.println("asdasd");
  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.setPixelColor(2, strip.Color(0, 0, 255));
  delay(wait);
  strip.setPixelColor(1, strip.Color(0, 255, 255));
  strip.setPixelColor(3, strip.Color(0, 255, 255));
  delay(wait);
  strip.setPixelColor(1, strip.Color(0, 0, 255));
  strip.setPixelColor(3, strip.Color(0, 0, 255));
  delay(wait);
  strip.setPixelColor(0, strip.Color(0, 255, 255));
  strip.setPixelColor(2, strip.Color(0, 255, 255));
  delay(wait);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
