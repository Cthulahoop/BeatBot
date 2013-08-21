/* ********************************************************
 *     ___           __  ___       __    ____ __  __  __ 
 *    / _ )___ ___ _/ /_/ _ )___  / /_  / __//  \/  \/  \
 *   / _ </ -_) _ `/ __/ _ </ _ \/ __/ / _ \/ / / / / / /
 *  /____/\__/\_,_/\__/____/\___/\__/  \___/\__/\__/\__/
 *
 *  v 1.2 08.2013 by Ric Ewing
 *  for the MakerHaus Arduino for Kids class
 *
 *    Inspired and based on work by:
 *    - Simple AVR wavetable synthesizer V1.0 by DZL
 *      https://github.com/Illutron/ArduinoSynth
 *
 *    - Noise Machine Instructables Mads Høbye
 •      http://www.instructables.com/member/madshobye
 *
 */

#include <Wire.h>
#include <Bounce.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel stick = Adafruit_NeoPixel(8, 12, NEO_GRB + NEO_KHZ800);
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

#include "vars.h"
#include "leds.h"
#include "buttons.h"
#include "dials.h"
#include "synth.h"
#include "games.h"
#include "sequencer.h"

byte mode = 0;
int modeSwitch = B10010000;

void setup() {
  Serial.begin(9600);

  initSynth();
  initBtns();
  initLeds();
  initSequencer();
  
  // this makes random more ... random. :)
  randomSeed(analogRead(A0));
}


//************************************************
// Main Loop
//************************************************

void loop() {
    
  switch(mode) {
    case 0: {
      sequencer();
      break;
    }
    case 1: {
      game();
      break;
    }
  }

  if(btns[SM_L].read()==LOW) {
    Serial.println(btns[SM_L].duration());
  }
   
   
  if(btns[SM_L].fallingEdge() || btns[SM_R].fallingEdge()) { 
    if(BtnStates == modeSwitch) {
      stickClear();
      matrixClear();
      startSeq();
      resetGames();
      initSynth();
      //switch mode
      mode++;
      mode %= 2;
      Serial.println(mode);
    }
  }
}


