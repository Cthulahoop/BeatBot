/* ********************************************************
 *
 *  Bot Test Sketch
 *  v 1.0 08.2013 by Ric Ewing
 *  for the MakerHaus Arduino for Kids class
 *
 *  This tests out all the hardware for the BeatBot 6000
 *  we built in class. 
 *  
 *  Hardware checks:
 *  - The 8x8 matrix shows a dot that travels across all
 *    64 pixels of the display one at a time
 *  - The pixelStick should have a single pixel moving to
 *    the right when no buttons are pressed. 
 *  - The dials change the color of the moving dot on the
 *    pixelStick.
 *  - All buttons light up a pixel on the pixel Stick. The
 *    big buttons should light up the left 4 pixels in
 *    order, the small buttons ther right 4. 
 *  - Big buttons play notes (use headphones)
 *  - Small buttons on the left play tones based on
 *    dial positions
 *  - Small buttons on the right play random robot sounds
 *    (this delays playback, and is expected).
 *
 *  If your buttons don't light up the right pixels, or
 *  seem to be out of order, you can change the pin 
 *  numbers in the Bounce assignment below. 
 *  once they are correct, save this file and remember
 *  the pin assignments as you will need to update the
 *  beatbot sketch to match this.
 *
 */


#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Bounce.h>

// dials
#define LEFT_DIAL A2   // 10kΩ linear potentiometer
#define RIGHT_DIAL A3  // 10kΩ linear potentiometer

// store frequency values for 8 octives
PROGMEM byte notes[] = { 
  31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 93, 
  98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 
  247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 
  622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 
  1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 
  3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978 
};

int noteCount = sizeof(notes); // how many notes are there?

int bTime = 5; // millisecond wait for debounce

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
Adafruit_NeoPixel stick = Adafruit_NeoPixel(8, 12, NEO_GRB + NEO_KHZ800);


Bounce btns[8] = {
  // ******************************************************
  // if needed, change the pin numbers to match your setup 
  // ******************************************************
  Bounce( 5, bTime ),  // big btn right
  Bounce( 4, bTime ),  // big btn mid-right
  Bounce( 3, bTime ),  // big btn mid-left
  Bounce( 2, bTime ),  // big btn left

  Bounce( 11, bTime ),  // small btn right
  Bounce( 10, bTime ),  // small btn mid-right
  Bounce( 9, bTime ),   // small btn mid-left
  Bounce( 8, bTime )    // small btn left
};

byte BtnStates = 0; // a bitmask that holds the current btn pressed state

int count = 0;
int tonePin = 6;

void setup() {
  // start the serial output
  Serial.begin(9600);
  Serial.println("Testing the BeatBot configuration");
  
  // this makes random more ... random. :)
  randomSeed(analogRead(A0));
  // here's why this works. just as long as we don't have
  // anything hooked up to A0, the read value on that pin
  // is noise in the system (between 0 and 1023). things like 
  // humidity, nearby components, other electronics all
  // can influence this reading. whatever we get back is passed
  // into randomSeed, which dictates how the random function
  // values are generated. because we're using a real world
  // random value, the output should be different every time.

  // setup btn read pins
  pinMode(2,INPUT_PULLUP);  // big right
  pinMode(3,INPUT_PULLUP);  // big mid-right
  pinMode(4,INPUT_PULLUP);  // big mid-left
  pinMode(5,INPUT_PULLUP);  // big left
  pinMode(8,INPUT_PULLUP);  // small right
  pinMode(9,INPUT_PULLUP);  // small mid-right
  pinMode(10,INPUT_PULLUP); // small mid-left
  pinMode(11,INPUT_PULLUP); // small left

  // start the matrix
  matrix.begin(0x70);
  matrix.setBrightness(8);
  
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  
  matrix.clear();
  matrix.writeDisplay();

  // start the pixel stick
  stick.begin();
  stick.setBrightness(64); // levels are 1 to 255
  stick.show();            // Initialize all pixels to 'off'
}

void loop() {

  int r = map(analogRead(LEFT_DIAL),0,1023,0,255);
  int g = map(analogRead(RIGHT_DIAL),0,1023,0,255);
  int pn = map(count,0,1023,0,64);

  matrix.clear();
  matrix.drawPixel(pn%8,floor(pn/8),LED_ON);
  matrix.writeDisplay();
  
  updateBtns();
  
  if ( BtnStates ) {
    stickSetPixels( BtnStates, stick.Color(r,g,32+pn*2) );
  }
  else {
    stickSetPixels( 1<<(pn/8), stick.Color(r,g,32) );
  }
  
  if(btns[0].read() == LOW) {
    tone(tonePin, notes[27]);
  }
  else if(btns[1].read() == LOW) {
    tone(tonePin, notes[30]);
  }
  else if(btns[2].read() == LOW) {
    tone(tonePin, notes[33]);
  }
  else if(btns[3].read() == LOW) {
    tone(tonePin, notes[36]);
  }
  else if(btns[4].read() == LOW) {
    tone(tonePin, analogRead(LEFT_DIAL)+32);
  }
  else if(btns[5].read() == LOW) {
    tone(tonePin, analogRead(RIGHT_DIAL)+32);
  }
  else if(btns[6].read() == LOW) {
    playNoise();
  }
  else if(btns[7].read() == LOW) {
    playNoise();
  }
  else {
    noTone(tonePin);
  }
  
  count++;
  count %= 1024;  
}


