/*
 *  logic for the sequencer
 */

void initSequencer() {
  bar = 0;

  // setup_voice(voice, waveform, pitch, envelope, length,  mod)
  setup_voice(0, (unsigned int)TriangleTable,400.0,(unsigned int)Env0,2.0,512); // thump
  setup_voice(1,(unsigned int)SquareTable,100.0,(unsigned int)Env1,2.0,200);
  setup_voice(2,(unsigned int)SawTable,200.0,(unsigned int)Env1 ,1.0,512);
  setup_voice(3,(unsigned int)NoiseTable,800.0,(unsigned int)Env3,.02,500); // snap

}

// stops the sequencer
void pauseSeq() {
  stickClear();
  matrixClear();
  paused = true;
}

void startSeq() {
    paused = false;
}

void sequencer() {

  if(paused) {
    if(FallBtns&SM_L) {
      paused = false;
    }
  }
  else if(synthTick()) {

    // update values used for synth timing
    bar=counter&0x1f;

    //************************************************
    // Buttons & Dials
    //************************************************

    updateDials();

    // check for pause button
    // if(btns[SM_L].fallingEdge()) {
    if(FallBtns&SM_L) {
      paused = true;
    }

    // if no buttons are pressed
    if(!BtnStates) {
      if(newLeft) {
        note = 32+(map(left,0,1023,0,63));
        //          mtrigger(currentVoice,note);
      }
      if(newRight) {
        setup_length(currentVoice, map(right,0,1023,24,2));
      }
    }

    // handle small right: x
    if(btns[SM_R].read() == LOW) {
      if(newLeft) {
        setup_pitch(1,50+(left/2.0));
      }
      if(newRight) {
        setup_mod(currentVoice, right);

      }
    }

    // handle small mid-right: +
    if(btns[SM_MR].read() == LOW) {
      if(newLeft) {
        setup_wave(1,map(left,0,1023,1,6));
      }
      if(newRight) {
        setup_env(currentVoice,map(right,0,1023,1,7));
      }
    }

    // handle small mid-left: o
    if(btns[SM_ML].read() == LOW) {
      if(newLeft) {
        setup_wave(1,map(left,0,1023,1,6));
      }
      if(newRight) {
        setup_env(currentVoice,map(right,0,1023,1,7));
      }
    }

    if(btns[BIG_L].fallingEdge()) {
      if(btns[SM_ML].read() == LOW) {
        currentVoice = 0;
      }
      else if(btns[SM_R].read() == LOW) {
        pattern[0][bar] = 0;
      }
      else {
        pattern[0][bar] = note;
      }
    }

    if(btns[BIG_ML].fallingEdge()) {
      if(btns[SM_ML].read() == LOW) {
        currentVoice = 1;
      }
      else if(btns[SM_R].read() == LOW) {
        pattern[1][bar] = 0;
      }
      else {
        pattern[1][bar] = note;
      }
    }

    if(btns[BIG_MR].fallingEdge()) {
      if(btns[SM_ML].read() == LOW) {
        currentVoice = 2;
      }
      else if(btns[SM_R].read() == LOW) {
        pattern[2][bar] = 0;
      }
      else {
        pattern[2][bar] = note;
      }
    }

    if(btns[BIG_R].fallingEdge()) {
      if(btns[SM_ML].read() == LOW) {
        currentVoice = 3;
      }
      else if(btns[SM_R].read() == LOW) {
        pattern[3][bar] = 0;
      }
      else {
        pattern[3][bar] = note;
      }
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

    byte quad = bar/8;
    matrix.clear();
    matrix.fillRect((quad%2)*4,(quad/2)*4,4,4, LED_ON);
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
      // trigger(3);
      mtrigger(3,pattern[3][bar]);

    //************************************************
    // Handle timers
    //************************************************
    tim=0;
    counter++;

  }

  clearBtnStates();
}
