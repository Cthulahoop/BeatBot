/* ********************************************************
 *     ___           __  ___       __    ____ __  __  __
 *    / _ )___ ___ _/ /_/ _ )___  / /_  / __//  \/  \/  \
 *   / _ </ -_) _ `/ __/ _ </ _ \/ __/ / _ \/ / / / / / /
 *  /____/\__/\_,_/\__/____/\___/\__/  \___/\__/\__/\__/
 *
 *  v 1.4  15.09.2013 by Ric Ewing
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
 *    Known Issues:
 *    - The size of this sketch is nearing the maximum
 *      for an Arduino UNO. Some functions can fail or
 *      behave badly as the compiled skectch size
 *      increases.
 *
 *      For example, I've been able to consistently
 *      cause the String methods to fail once the
 *      skectch nears 28k in compiled size.
 *
 *      If you modify this code, you may want to seek
 *      ways to either avoid using the String object
 *      (http://arduino.cc/en/Reference/StringObject)
 *      or find ways to optomize the sketch size to
 *      free up memory to allow them to run).
 *
 *    - Enabling the Serial output will cause the
 *      String methods to fail for the above reasons.
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
#include "synth.h"
#include "leds.h"
#include "buttons.h"
#include "dials.h"
#include "games.h"
#include "sequencer.h"

byte mode = 1;

void setup() {

  // NOTE: Enabling Serial may cause String methods
  // to stop working due to lack of memory.
  // Serial.begin(9600);

  // setup
  initBtns();
  initDials();
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


