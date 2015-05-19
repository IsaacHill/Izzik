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

//Constants for running mode that define breaking.
#define BRAKETHRESHOLD    200

//defines whether the user is currently slowing down
//0 defines not currently slowing down, 1 defines that they are i.e they 
//are slowing down at faster than the BRAKETHRESHOLD. If braking is equal to 
//2 then that signifies that the user was just braking but is no
//longer. Initialize to 0, not breaking.
int braking = 0;


int set = 0;
float i = 0;

//Initialize strip (chain of leds), first input is number of leds in chain.
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

//Array of colours arranged in order of severity/intensity
uint32_t colorArray [10] = {strip.Color(255,255,255),
                   strip.Color(153,204,255),
                   strip.Color(0,0,255),
                   strip.Color(153,255,51),
                   strip.Color(0,204,204),
                   strip.Color(204,204,204),
                   strip.Color(251,51,153),
                   strip.Color(255,255,0),
                   strip.Color(255,128,0),
                   strip.Color(255,0,0)};



void setup() {
  strip.begin();
  strip.setBrightness(20);
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
    //Get new info from accelerometer/magnetometer
    getValues();
    Serial.println("Rotation");
  }
  while (mode == 2) {
    Serial.println("Running");
    runningMode();
    //Placeholder light cycling for testing, (lights stop cycling
    //if a mode is chosen or the code hangs for whatever reason).
    //flashTest(200);
    //colorWipe(strip.Color(0, 0, 255),strip.Color(0, 255,255), 100);
  }
}

//Read in new data from accelerometer/magnetometer
void getValues() {
    prevX = currentX;
    prevY = currentY;
    prevZ = currentZ;
    lsm.read();
    currentX = (int)lsm.accelData.x;
    currentY = (int)lsm.accelData.y;
    currentZ = (int)lsm.accelData.z;
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
  if (rotation > 90 ) {
    mode = 1;
  }
  //Detects if user is accelerating forward and isnt rotating
  //and if so sets belt to running mode.
  else if (acceleration > 150) {
    mode = 2;
  }
}

//Mode that loops when mode is set to running
void runningMode() {
  int k = 7;
  int j = 8;
  while(j < 16) {
  runningSetPixel(k,j);
  k--;
  j++;
  delay(200);
  }
}


void runningSetPixel(int k, int j) {
  getValues();
  checkBraking();
  if(braking == 1) {
    showBraking();
  }
  //If braking  = 2 then the checkBraking just 
  //changed from braking to not breaking so reset back lights.
  if(braking == 2){
     clearBraking();
     //Re initialize braking.
     braking = 0;
  }
  if(braking == 1 && k == 7 || k == 6) {
    if( k == 7) {
        strip.setPixelColor(0, colorArray[0]);
        strip.setPixelColor(15, colorArray[0]);
      
    }
    else {
        strip.setPixelColor(k - 1, colorArray[0]);
        strip.setPixelColor(j - 1, colorArray[0]); 
      
    }
  }
  else {
    strip.setPixelColor(k, colorArray[2]);
    strip.setPixelColor(j, colorArray[2]);
    //Reset previous leds to white.
    if(k != 7) {
      strip.setPixelColor(k + 1, colorArray[0]);
      strip.setPixelColor(j - 1, colorArray[0]);
    }
    else{
      strip.setPixelColor(0, colorArray[0]);
      strip.setPixelColor(15, colorArray[0]); 
    }
  }
  strip.show();
}

//Checks if user is currently braking and sets
//braking to reflect current status.
void checkBraking() {
  if((prevX - currentX) >= BRAKETHRESHOLD) {
    Serial.print("Im braking");
    braking = 1;}
  else{
   if(braking == 1) {
     braking = 2; }
   else {
     braking = 0;}
  }
}

void clearBraking() {
    strip.setPixelColor(0, colorArray[0]);
    strip.setPixelColor(1, colorArray[0]);
    strip.setPixelColor(2, colorArray[0]);
    strip.setPixelColor(3, colorArray[0]);
  
}

//Set rear lights to red to denote breaking.
void showBraking() {
    strip.setPixelColor(0, colorArray[9]);
    strip.setPixelColor(1, colorArray[9]);
    strip.setPixelColor(2, colorArray[9]);
    strip.setPixelColor(3, colorArray[9]);
    strip.show(); 
}

//Clear rear lights when no braking is detected.


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

//Testing for flashing alternating lights
void flashTest(int wait) {
  Serial.println(i);
  Serial.println(colorArray[4]);
  Serial.println(strip.Color(0,204,204));
  if(i == 0){
    strip.setPixelColor(0, colorArray[9]);
    strip.setPixelColor(2, colorArray[9]);
    strip.setPixelColor(1, colorArray[4]);
    strip.setPixelColor(3, colorArray[4]);
    i = 1;
  }
  else{
    strip.setPixelColor(1, colorArray[9]);
    strip.setPixelColor(3, colorArray[9]);
    strip.setPixelColor(0, colorArray[4]);
    strip.setPixelColor(2, colorArray[4]);
    i = 0;
  }
  strip.show();
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
