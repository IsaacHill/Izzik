#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_NeoPixel.h>
#include <Math.h>

//Define the output pin which will be used for the Neo Pixels
#define PIN 6

//Initialize strip (chain of leds), first input is number of leds in chain.
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

//Array of colours arranged in order of severity/intensity
uint32_t colorArray [40] = {strip.Color(32, 32, 32),
                            strip.Color(0, 0, 102),
                            strip.Color(0, 0, 153),
                            strip.Color(76, 0, 153),
                            strip.Color(0, 102, 104),
                            strip.Color(0, 0, 255),
                            strip.Color(127, 0, 255),
                            strip.Color(153, 51, 255),
                            strip.Color(153, 0, 153),
                            strip.Color(204, 0, 204),
                            strip.Color(153, 0, 76),
                            strip.Color(255, 0, 255),
                            strip.Color(255, 0, 127),
                            strip.Color(255, 51, 255),
                            strip.Color(255, 51, 153),
                            strip.Color(51, 102, 0),
                            strip.Color(0, 102, 0),
                            strip.Color(76, 153, 0),
                            strip.Color(0, 153, 0),
                            strip.Color(0, 204, 0),
                            strip.Color(128, 255, 0),
                            strip.Color(0, 255, 0),
                            strip.Color(153, 255, 51),
                            strip.Color(51, 255, 51),
                            strip.Color(255, 255, 0),
                            strip.Color(255, 128, 0),
                            strip.Color(255, 255, 51),
                            strip.Color(255, 153, 51),
                            strip.Color(255, 255, 102),
                            strip.Color(255, 178, 102),
                            strip.Color(255, 204, 204),
                            strip.Color(255, 0, 0),
                            strip.Color(255, 51, 51),
                            strip.Color(255, 102, 102),
                            strip.Color(255, 153, 153),
                            strip.Color(255, 204, 204),
                            strip.Color(255, 229, 204),
                            strip.Color(255, 255, 204),
                            strip.Color(229, 255, 204),
                            strip.Color(96, 96, 96),
                           };
                           
//Due to the nature of assignments and pointers in c
//this variable contains the size of the colorArray, for
//ease of use.
int colourSize = 40;

//Define a name for the accelerometer for ease of reference.
Adafruit_LSM303 lsm;

//Initialize Pi, for degree calculations
const float pi = 3.14159;
//Variable to check if arduino is running the first loop.
int firstRun = 0;

//Variable for previous acceleration in X direction
float prevX = 0;
//Variable for current accerlation in X direction
float currentX = 0;
//Variable for previous acceleration in Y direction
float prevY = 0;
//Variable for current acceleration in Y direction
float currentY = 0;
//Variable for current direction in degrees from North
float currentDirection = 0;
//Variable for previous direction in degrees from North.
float previousDirection = 0;
//Variable for dance mode that defines light positions initilized to
//3 i.e the fourth LED.
int danceLocation = 3;
//Variable to define direction of LED motion in dancing mode.
//Initialize to 1 (positive).
int danceDirection = 1;


//Initialize mode as unset, 1 defines dancing 2 defines running.
int mode = 0;

//An integer to track when the acceleration Colour should be changed
int changeAccelColour = 4;
//An integer to store which location in the colour array will be accesed
//to set colour on the belt.
int colourLED = 0;

//Constants for accelerometer normalization.
float xMin = -2040;
float xMax = 2040;
float yMin = -2040;
float yMax = 2040;

//Left counter variable for running
int leftIn = 2;
//Right counter variable for running
int rightIn = 3;

/*
 *  Function: setup
 *  -----------------------
 *  Runs when the arduino is first turned on, initializes the
 *  chain of LEDs and turns them on. Also detects if the accelerometer
 *  is connected and if it is not found stops the arduino and 
 *  prints an error message.
 *
 *  Return: Void.
 */
//Runs whenn arduino is first turned on, initializes the 
//chain of LEDs and turns them on.
void setup() {
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  Serial.begin(9600);
  // Initialize the digital pin as an output.
  if (!lsm.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1);
  }
}

/*
 *  Function: loop
 *  ---------------------
 *  After the arduino has run setup, this function loops infinetely
 *  and depending on several global variables places the arduino in either
 *  detectMode, runningMode or dancingMode
 *
 *  Return: Void
 */
void loop() {
  //Need to find the mode as given by users motion
  while (mode == 0) {
    //Sets up reading frame for lsm
    lsm.read();
    //read in acceleration values from arduino.
    currentX = (int)lsm.accelData.x;
    currentY = (int)lsm.accelData.y;
    //Find the current orientation of the device
    currentDirection = (atan2(lsm.magData.y, lsm.magData.x) * 180) / pi;
    //We need to normalize this value to be between 0 < 360
    if (currentDirection < 0) {
      currentDirection = currentDirection + 360;
    }
    //If we know the previous direction and acceleration
    if (firstRun == 1) {
      detectMode();
    }
    //If we dont then we need to set them and wait.
    else {
      previousDirection = currentDirection;
      prevX = currentX;
      prevY = currentY;
      firstRun = 1;
    }
    previousDirection = currentDirection;
    //While Waiting for inputs to detect mode, show rainbow
    //rotation to inform user.
    lightCycle(1.5);
  }
  //If mode == 1 then we go into the dancing mode of the belt.
  while (mode  == 1) {
    dancingMode();
  }
  //If mode == 2 then we go into the running mode of the belt.
  while (mode == 2) {
    runningMode();
  }
}

/*
 *  Function: getValues
 *  ------------------------
 *  Reads in new values fromaccelerometer/magnetometer.
 * 
 *  Return: Void
 */
void getValues() {
  //Set the previous values to the old current values.
  prevX = currentX;
  prevY = currentY;
  //Set the previous values to the old direction.
  previousDirection = currentDirection;
  //Sets up a new reading frame from the lsm.
  lsm.read();
  //Read in new values for x and y acceleration.
  currentX = (int)lsm.accelData.x;
  currentY = (int)lsm.accelData.y;
  //Find current heading (from North) in degrees.
  currentDirection = (atan2(lsm.magData.y, lsm.magData.x) * 180) / pi;
  //This if statement activates every four loops of the function
  if (changeAccelColour % 4 == 0) {
    //Normalize acceleration based on max values for the accelerometer
    float xNorm = abs(currentX) / xMax;
    float yNorm = abs(currentY) / yMax;
    float crossXY = sqrt(xNorm * yNorm);
    colourLED = colourSize * crossXY;
  }
  //Increment this variable to become closer to changing the
  //colour of the LED's
  changeAccelColour += 1;
}

/*
 *  Function: detectMode
 *  -------------------------
 *  Detects if the users current motion is setting a motion to set a 
 *  mode and if so sets the global variable mode to represent this. 
 *  Setting mode to either 1 to represent dancing or 2 representing 
 *  running.
 *
 *  Return: Void
 */
void detectMode() {
  float rotation = abs(currentDirection - previousDirection);
  float acceleration = abs(prevX - currentX);
  //As rotating creates acceleration in x and y, check rotation 
  //first to ensure that that wasnt the intended motion.  
  if (rotation > 40 ) {
    mode = 1;
  }
  //Detects if user is accelerating forward and isn't rotating
  //and if so sets belt to running mode.
  else if (acceleration > 800) {
    mode = 2;
  }
}

/*
 *  Function: dancingMode 
 *  ------------------------
 *  The main mode that runs when the arduino is currently set 
 *  to dancing mode. 
 *
 *  Return: Void
 */
void dancingMode() {
  getValues();
  setDance();
  //change denotes the change in degrees from the previous
  //read of the heading.
  int changeDegrees = currentDirection - previousDirection;
  if (abs(changeDegrees) > 15) {
    if (changeDegrees > 0) {
      danceDirection = 0;
    }
    else {
      danceDirection = 1  ;
    }
  }
  delay(100);
}

/*
 * Function: setDance
 * ------------------
 *  Detects the users last dance direction and sets the LEDs 
 *  one more along the chain in that direction to a colour depending
 *  on the users current acceleration. It then resets the previous LEDs
 *  colour.
 *
 *  Return: Void.
 */
void setDance() {
  if (danceDirection == 1) {
    //Set new pixels to colour
    strip.setPixelColor((danceLocation + 1) % 16, colorArray[39 - colourLED]);
    strip.setPixelColor((danceLocation + 5) % 16, colorArray[39 - colourLED]);
    strip.setPixelColor((danceLocation + 9) % 16, colorArray[39 - colourLED]);
    strip.setPixelColor((danceLocation + 13) % 16, colorArray[39 - colourLED]);
    //Reset Previous pixels to non rotating colour.
    strip.setPixelColor(danceLocation, colorArray[6]);
    strip.setPixelColor((danceLocation + 4) % 16, colorArray[39 - (colourLED + 1)]);
    strip.setPixelColor((danceLocation + 8) % 16, colorArray[39 - (colourLED + 1)]);
    strip.setPixelColor((danceLocation + 12) % 16, colorArray[39 - (colourLED + 1)]);
    danceLocation = (danceLocation + 1) % 16;
  }
  else {
    //Set new pixels to colour
    strip.setPixelColor((danceLocation + 15) % 16, colorArray[40 - colourLED]);
    strip.setPixelColor((danceLocation + 11) % 16, colorArray[40 - colourLED]);
    strip.setPixelColor((danceLocation + 7) % 16, colorArray[40 - colourLED]);
    strip.setPixelColor((danceLocation + 3) % 16, colorArray[40 - colourLED]);
    //Reset Previous pixels to non rotating colour.
    strip.setPixelColor(danceLocation, colorArray[6]);
    strip.setPixelColor((danceLocation + 4) % 16, colorArray[40 - (colourLED + 1)]);
    strip.setPixelColor((danceLocation + 8) % 16, colorArray[40 - (colourLED + 1)]);
    strip.setPixelColor((danceLocation + 12) % 16, colorArray[40 - (colourLED + 1)]);
    danceLocation = (danceLocation + 15) % 16;
  }
  strip.show();
}

/*
 *  Function: runningMode
 * -----------------------
 *  This function simply tracks the rotation of
 *  two positions accros the belt and on each loop moves
 *  these positions in unison across the belt. 
 *  
 *
 *  Return: Void
 *
 */
void runningMode() {
    runningSetPixel();
    //As we use an integer to track the location of the left
    //roating pixel ensure that when it crosses from the 
    //0'th pixel to the 15th that it is set to 15.
    if (--leftIn%16 == -1) {
    	leftIn = 15;
    }
    //If left in equals 10 than the pixels
    //have traversed the entire belt and need to be 
    //reset to strating positions.
    if(leftIn == 10) {
       leftIn = 2;
       rightIn = 3;
    }
    else{ 
      rightIn++;
    }
    delay(200);
  
}

/*
 *  Function: runningSetPixel
 *  --------------------------
 *  Sets the pixels on the belt to represent the
 *  location of the two traveling pixels in running mode.
 *  The colour of these pixels is dependant on the users soeed.
 *
 *  Return: Void
 *
 */
void runningSetPixel() {
  getValues();
  strip.setPixelColor(leftIn, colorArray[colourLED]);
  strip.setPixelColor(rightIn, colorArray[colourLED]);
  //Reset previous led to white.
  //Need to ensure for the  edge case that leftIn has just been
  //reset to 2, this implies that the previous two pixels where
  //10 and 11 so we need to reset those.
  if (leftIn == 2) {
    strip.setPixelColor(10, colorArray[0]);
    strip.setPixelColor(11, colorArray[0]); 
   } else {
     if (leftIn != 11) {
       strip.setPixelColor((leftIn+1)%16, colorArray[0]);
       strip.setPixelColor(rightIn-1, colorArray[0]);
     } else {
       strip.setPixelColor((leftIn+1)%16, colorArray[0]);
      strip.setPixelColor(rightIn-1, colorArray[0]);  
     }
    }
  strip.show();
}

/*
 *  Function: lightCycle
 *  Cycles lights across the entire belt, utilizing findColour to retrieve a
 *  Rainbow like effect.
 *
 *  Return: Void.
 *
 */
void lightCycle(uint8_t wait) {
  uint16_t k, x;
  for (x = 0; x < 256; x++) { 
    for (k = 0; k < strip.numPixels(); k++) {
      strip.setPixelColor(k, findColour(((k * 256 / strip.numPixels()) + x) & 255));
    }
    strip.show();
    delay(wait);
  }
}

/*
 *  Function: findColour
 *  ----------------------
 *  Recieves an input colour in range of 0 to 255 and 
 *  returns a colour.
 *
 *  pos: A number to base the returned colour on.
 *
 *  Return: Returns a colour value.
 */
uint32_t findColour(byte pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return strip.Color(255 - pos * 3, 0, pos * 3);
  } else if (pos < 170) {
    pos -= 85;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  } else {
    pos -= 170;
    return strip.Color(pos * 3, 255 - pos * 3, 0);
  }
}
