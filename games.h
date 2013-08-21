#define GAME 100
#define YOU_WIN 120
#define TOO_HIGH 140
#define TOO_LOW 160
#define NO_GAME 200

unsigned long waitTime = 10;
unsigned long waitUntil = 0;
unsigned long interval = 0;
int gameStep = 0;
int scrollXPos = 0;
int scrollYPos = 0;
int timeout = 0;
int targetTime = 0;
int guessMe = 0;
int guessCount = 0;
int theGuess = 0;

void resetGames() {
//  TCCR1B=0x03;
//  TCCR0B=0x03; 
  waitUntil = 0;
  interval = 0;
  gameStep = 0;
  scrollXPos = 0;
  scrollYPos = 0;
  timeout = 0;
  targetTime = 0;
  guessMe = 0;
  guessCount = 0;
  theGuess = 0;
}

void wait(unsigned long t) {
  delay(t*32);
}

void gameTimeout(unsigned long count) {
  timeout = count * 6400;
}

void game() {
  wait(25);
  // get btns
  updateBtns();

  switch(gameStep) {
    case 0: {
      // start game mode
      resetGames();
      matrix.clear();
      matrix.blinkRate(0);
      matrix.setBrightness(8);
      matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      gameStep++;
      // pause for 1/2 sec
      wait(500);
      break;
    }
    case 1: {
      // setup animated intro text
      matrixSetScrollText("Hi, want to play a game?");
      gameStep++;
      break;
    }
    case 2: {
      if(matrixScrollText()) {
        gameStep++;
      }
      break;
    }
    case 3: {
      // show wait state
      matrix.clear();
      matrix.setCursor(1,0);
      matrix.print("?");
      matrix.writeDisplay();
      matrix.blinkRate(3);
      // show yes/no buttons as red/green
      stickClear();
      stick.setPixelColor(0, stick.Color(0,255,0));
      stick.setPixelColor(1, stick.Color(0,255,0));
      stick.setPixelColor(6, stick.Color(255,0,0));
      stick.setPixelColor(7, stick.Color(255,0,0));
      stick.show();
      // set timeout as countdown
      gameTimeout(5000);
      gameStep++;
      break;
    }
    case 4: {
      // check btns
      if(btns[BIG_L].risingEdge()) {
        matrix.blinkRate(0);
        matrixClear();
        stickClear();
        gameStep++;
      }
      else if (btns[BIG_R].risingEdge()) {
        stickClear();
        gameStep = NO_GAME;
      }
      else if(timeout-- < 0) {
        stickClear();
        gameStep = NO_GAME+3;
      }
      break;
    }
    case 5: {
      matrixSetScrollText("Yay! Let's play Guess My Number!");
      // set number to guess
      guessMe = random(1,64);
      gameStep++;
      break;
    }
    case 6: {
      if(matrixScrollText()) {
        gameStep++;
      }
      break;
    }
    case 7: {
      matrixSetScrollText("Pick a number with the right dial.");
      gameStep++;
      break;
    }
    case 8: {
      if(matrixScrollText()) {
        gameStep++;
      }
      break;
    }
    case 9: {
      matrixSetScrollText("Press the green button to choose.");
      gameStep++;
      break;
    }
    case 10: {
      if(matrixScrollText()) {
        guessCount = 0;
        updateDials();
        theGuess = map(left,0,1023,1,64);
        matrixSetScrollText((String)theGuess);
        gameStep = GAME;
      }
      break;
    }
    case GAME: {
      // show ok btn
      stick.setPixelColor(0, stick.Color(0,255,0));
      stick.setPixelColor(1, stick.Color(0,255,0));
      stick.show();
      updateDials();
      if(btns[BIG_L].risingEdge()) {
        guessCount++;
        if(theGuess == guessMe) {
          gameStep = YOU_WIN;
        }
        else if(theGuess > guessMe) {
          gameStep = TOO_HIGH;
        }
        else if(theGuess < guessMe) {
          gameStep = TOO_LOW;
        }
      }
      else if(newRight) {
        int temp = map(right,0,1023,1,64);
        if(temp != theGuess) {
          theGuess = temp;
          matrixSetScrollText((String)theGuess);
        }
      }
      matrixScrollText();
      break;
    }
    case TOO_LOW: {
      matrixSetScrollText("Too low...");
      gameStep++;
      break;
    }
    case TOO_LOW+1: {
      if(matrixScrollText()) {
        gameStep = GAME;
      }
      break;
    }
    case TOO_HIGH: {
      matrixSetScrollText("Too high...");
      gameStep++;
      break;
    }
    case TOO_HIGH+1: {
      if(matrixScrollText()) {
        gameStep = GAME;
      }
      break;
    }
    case YOU_WIN: {
      matrixSetScrollText("YOU WIN!!  YOU WIN!!");
      gameStep++;
      break;
    }
    case YOU_WIN+1: {
      if(matrixScrollText()) {
        gameStep++;
      }
      break;
    }
    case YOU_WIN+2: {
      String scoreStr = "It took you ";
      scoreStr += guessCount;
      scoreStr += " guesses! ";
      scoreStr += scoreStr; // play message 2x
      matrixSetScrollText(scoreStr);
      gameStep++;
      break;
    }
    case YOU_WIN+3: {
      if(matrixScrollText()) {
        gameStep = 0; // start over
      }
      break;
    }
    case NO_GAME: {
      matrix.clear();
      matrix.blinkRate(0);
      matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      wait(1000);
      matrixSetScrollText("awww... I'm sad...");
      gameStep++;
      break;
    }
    case NO_GAME+1: {
      if(matrixScrollText()) {
        gameStep++;
      }
      break;
    }
    case NO_GAME+2: {
      matrix.clear();
      matrix.blinkRate(3);
      matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      wait(2000);
      gameStep++;
      break;
    }
    case NO_GAME+3: {
      matrix.clear();
      matrix.blinkRate(0);
      matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      wait(1000);
      gameStep++;
      break;
    }
    default: {
      if(matrixScrollText()) {
        gameStep = 0; // start over
      }
      break;
    }
  }
}

