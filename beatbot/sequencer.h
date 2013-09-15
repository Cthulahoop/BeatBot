#define MIN_NOTE 32

byte quad;

/* *************************************************************************
 *
 *   A 32 step sequencer for the synth
 *
 *   v 1.6  14.09.2013
 *    - added comments and history
 *    - added bmp for wave forms
 *    - began moving more logic into methods
 *
 */
void initSequencer() {
  bar = 0;

  // setup_voice(voice, waveform, pitch, envelope, length,  mod)
  setup_voice(0, (unsigned int)TriangleTable,200.0,(unsigned int)Env2,1.0,512); // thump
  setup_voice(1,(unsigned int)SquareTable,100.0,(unsigned int)Env1,2.0,200);
  setup_voice(2,(unsigned int)SawTable,200.0,(unsigned int)Env1 ,1.0,512);
  setup_voice(3,(unsigned int)NoiseTable,800.0,(unsigned int)Env3,.02,512); // snap

  matrix.setBrightness(15);

}

// stops the sequencer
void pauseSeq() {
  stickClear();
  matrixClear();
  matrix.fillRect(0,0,3,8,LED_ON);
  matrix.fillRect(5,0,3,8,LED_ON);
  matrix.writeDisplay();
  pauseSynth();
  paused = true;
  clearBtnStates();
}

void startSeq() {
    unpauseSynth();
    paused = false;
    clearBtnStates();
}


void updateNote() {
  if(newLeft) {
    int temp = map(left,DIAL_MIN_VAL,DIAL_MAX_VAL,0,63);
    if(temp + MIN_NOTE != note) {
      note= temp + MIN_NOTE;
      matrixClear();
      matrix.drawPixel(temp/8,temp%8,LED_ON);
      // matrix.writeDisplay();
    }
  }
  if(newRight) {
    setup_decay(currentVoice, map(right,DIAL_MIN_VAL,DIAL_MAX_VAL,100,4));
  }
}

void updateWave() {
  int temp;
  if(newLeft) {
    temp = map(left,DIAL_MIN_VAL,DIAL_MAX_VAL,1,6);
    matrixClear();
    switch(temp) {
      case 1:
        matrix.drawBitmap(0, 0, sin_bmp, 8, 8, LED_ON);
        break;
      case 2:
        matrix.drawBitmap(0, 0, tri_bmp, 8, 8, LED_ON);
        break;
      case 3:
        matrix.drawBitmap(0, 0, square_bmp, 8, 8, LED_ON);
        break;
      case 4:
        matrix.drawBitmap(0, 0, saw_bmp, 8, 8, LED_ON);
        break;
      case 5:
        matrix.drawBitmap(0, 0, ramp_bmp, 8, 8, LED_ON);
        break;
      case 6:
        matrix.drawBitmap(0, 0, noise_bmp, 8, 8, LED_ON);
        break;
      default:
        matrix.drawBitmap(0, 0, sin_bmp, 8, 8, LED_ON);
        break;
    }
    setup_wave(currentVoice,temp);
  }
  if(newRight) {
    temp = map(right,DIAL_MIN_VAL,DIAL_MAX_VAL,1,4);
    matrixClear();
    switch(temp) {
      case 1:
        matrix.drawBitmap(0, 0, env1_bmp, 8, 8, LED_ON);
        break;
      case 2:
        matrix.drawBitmap(0, 0, env2_bmp, 8, 8, LED_ON);
        break;
      case 3:
        matrix.drawBitmap(0, 0, env3_bmp, 8, 8, LED_ON);
        break;
      case 4:
        matrix.drawBitmap(0, 0, env4_bmp, 8, 8, LED_ON);
        break;
    }
    setup_env(currentVoice,temp);
  }
}

void resetPattern() {
  for(int v = 0; v < 4; v++) {
    for(int b = 0; b < 32; b++) {
      pattern[v][b]=0;
    }
  }
}

void sequencer() {

  if(FallBtns&SM_L) {
    if(paused == true) {
      startSeq();
    }
    else {
      pauseSeq();
    }
  }
  else if(!paused && synthTick()) {

    // update values used for synth timing
    bar=counter&0x1f; // loops at 32

    updateDials();

    // ---------------------------------------
    // Handle No Buttons State
    //

    // if no buttons are pressed
    if(!BtnStates) {
      updateNote();
    }

    // ---------------------------------------
    // Handle Small Buttons
    //

    // handle small right: x
    if(BtnStates&SM_R) {
      if(BtnStates == SM_R) {
        pattern[currentVoice][bar] = 0;
      }
      if(BtnStates == (SM_R + SM_MR)) {
        resetPattern();
      }
    }

    // handle small mid-right: +
    if(BtnStates&SM_MR) {
      if(FallBtns&BIG_L) {
        currentVoice = 0;
      }
      if(FallBtns&BIG_ML) {
        currentVoice = 1;
      }
      if(FallBtns&BIG_MR) {
        currentVoice = 2;
      }
      if(FallBtns&BIG_R) {
        currentVoice = 3;
      }
      updateWave();
    }

    // handle small mid-left: o
    if(BtnStates&SM_ML) {
      if(newLeft) {
        setup_tempo(map(left,DIAL_MIN_VAL,DIAL_MAX_VAL,10,60));
      }
      if(newRight) {
        setup_mod(currentVoice, right);
      }
    }

    /* ---------------------------------------------------------
     *  Big Buttons
     *    check for falling edge on big buttons so they only
     *    fire once.
     */

    if(FallBtns&BIG_L) {
      pattern[0][bar] = note;
    }

    if(FallBtns&BIG_ML) {
      pattern[1][bar] = note;
    }

    if(FallBtns&BIG_MR) {
      pattern[2][bar] = note;
    }

    if(FallBtns&BIG_R) {
      pattern[3][bar] = note;
    }


    //************************************************
    // Update lights
    //************************************************
    int px = bar%8;
    stick.setPixelColor(px,Wheel(px*32));
    if(px == 0)
      stick.setPixelColor(7,stick.Color(0,0,0));
    else
      stick.setPixelColor(px-1,stick.Color(0,0,0));
    stick.show();

    // show which measure we're in as a square in the
    // matrix.
    if(bar/8 != quad) {
      quad = bar/8;
      matrix.clear();
      byte tx = (quad%2)*4;
      byte ty = (quad/2)*4;
      matrix.drawPixel(tx+1,ty+1, LED_ON);
      matrix.drawPixel(tx+2,ty+1, LED_ON);
      matrix.drawPixel(tx+2,ty+2, LED_ON);
      matrix.drawPixel(tx+1,ty+2, LED_ON);
    }
    matrix.writeDisplay();

    //************************************************
    // Play Sounds
    //************************************************

    if(pattern[0][bar])
      mtrigger(0,pattern[0][bar]);

    if(pattern[1][bar])
      mtrigger(1,pattern[1][bar]);

    if(pattern[2][bar])
      mtrigger(2,pattern[2][bar]);

    if(pattern[3][bar])
      mtrigger(3,pattern[3][bar]);

    //************************************************
    // Handle timers
    //************************************************
    tim=0;
    counter++;

    clearBtnStates();
  }

}
