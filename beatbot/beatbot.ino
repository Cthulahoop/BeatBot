/* ********************************************************
 *     ___           __  ___       __    ____ __  __  __
 *    / _ )___ ___ _/ /_/ _ )___  / /_  / __//  \/  \/  \
 *   / _ </ -_) _ `/ __/ _ </ _ \/ __/ / _ \/ / / / / / /
 *  /____/\__/\_,_/\__/____/\___/\__/  \___/\__/\__/\__/
 *
 *  v 1.3 08.2013 by Ric Ewing
 *  for the MakerHaus Arduino for Kids class
 *
 *    Based on and inspired by:
 *    - Simple AVR wavetable synthesizer V1.0 by DZL
 *      https://github.com/Illutron/ArduinoSynth
 *      Thank you to DZL for permission to use and
 *      distribute.
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

byte mode = 1;

void setup() {

  // Serial.begin(9600);

  // setup
  initBtns();
  initLeds();
  initSynth();
  initSequencer();

  // this makes random more ... random. :)
  // see: http://arduino.cc/en/Reference/RandomSeed
  randomSeed(analogRead(A0));
}


//************************************************
// Main Loop
//************************************************

void loop() {

  updateBtns();

  if(FallBtns&SM_L || FallBtns&SM_R) {
    // check for > and x buttons being
    // pressed at the same time
    if(BtnStates == SM_L+SM_R) {
      // reset modules
      stickClear();
      matrixClear();
      startSeq();
      resetGames();
      clearBtnStates();
      updateBtns();
      //switch mode
      mode++;
      mode %= 2;
      if(mode == 0) {
        unpauseSynth();
      }
      else {
        pauseSynth();
      }
    }
  }

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


}


