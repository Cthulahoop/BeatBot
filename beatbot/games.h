#define GREETING 0
#define BEGINNING 10
#define ASK_TO_PLAY 20
#define INSTRUCTIONS 30
#define START_GAME 50
#define GAME 100
#define YOU_WIN 120
#define TOO_HIGH 140
#define TOO_LOW 160
#define NO_GAME 200
#define GUESS_DEBOUNCE 50

// SPECIAL CHARACTERS
#define ARROW_UP 24
#define ARROW_DOWN 25
#define ARROW_RIGHT 26
#define ARROW_LEFT 27

unsigned long waitStart = 0;
unsigned long waitDuration = 0;
unsigned long localWaitStart = 0;
unsigned long localWaitDuration = 0;
unsigned long lastGuessTime = 0;
int gameStep = 0;
int guessMe = 0;
int guessCount = 0;
int theGuess = 0;
boolean instructions = true;

void resetGames() {
  stickClear();
  pauseSynth();
  waitStart = 0;
  waitDuration = 0;
  localWaitStart = 0;
  localWaitDuration = 0;
  gameStep = 0;
  guessMe = 0;
  guessCount = 0;
  theGuess = 0;
  instructions = true;
}

void wait(unsigned long t) {
  waitDuration = (t);
  waitStart = millis();
}

void localWait(unsigned long t) {
  localWaitDuration = (t);
  localWaitStart = millis();
}

boolean localReady() {
  return millis() - localWaitStart >= localWaitDuration;
}

void game() {
  // check if we're still waiting
  if( millis() - waitStart >= waitDuration ) {

    wait(32);
    /*
     *  the game sequence is controled by this long switch case
     *  statement. It's a bit unweildy, but serviceable.
     */
    switch(gameStep) {

      /* ---------------------------------------------------------
       *  GREETING:
       *    reset values and say hello
       */
      case GREETING: {
        // start game mode
        resetGames();
        // clear pixel stick
        stickClear();
        // clear matrix and show smile
        matrix.clear();
        matrix.blinkRate(0);
        matrix.setBrightness(8);
        matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
        matrix.writeDisplay();
        // advance game sequence one step
        gameStep++;
        // pause for 2 sec
        wait(2000);
        // each case must end in break;
        break;
      }
      case GREETING+1: {
        // wink ;)
        matrix.clear();
        matrix.drawBitmap(0, 0, wink_bmp, 8, 8, LED_ON);
        matrix.writeDisplay();
        // advance game sequence one step
        gameStep++;
        // pause for .2 sec
        wait(200);
        break;
      }
      case GREETING+2: {
        // smile
        matrix.clear();
        matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
        matrix.writeDisplay();
        // advance game sequence one step
        gameStep++;
        // pause for 1 sec
        wait(1000);
        break;
      }
      case GREETING+3: {
        // setup animated welcome text
        matrixSetMessage("Hi!  I'm BeatBot ");
        gameStep++;
        break;
      }

      case GREETING+4: {
        // the matrixScrollText method returns true if the text
        // has scrolled off screen to the left
        if( matrixScrollText() ) {
          gameStep = BEGINNING;
        }
        break;
      }

      /* ---------------------------------------------------------
       *  BEGINING
       *    idle state. smile, blink, wink or go to ask to play.
       */
      case BEGINNING: {
          matrix.clear();
          matrix.setBrightness(8);
          matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
          matrix.writeDisplay();
          // localWait lets us check for input while paused, where
          // the wait command prevents input. We use 'localReady'
          // below to check if we've waited long enough.
          localWait(random(10,50)*100);
          gameStep++;
        break;
      }

      case BEGINNING+1: {
        // check if we've waited long enough, or if a button
        // was pressed
        if(localReady() || RiseBtns) {
          // 15% chance of asking to play, or if button pressed
          if( (random(100) > 85) || RiseBtns ) {
            gameStep = ASK_TO_PLAY;
          }
          // only 5% chance of wink ;)
          else if(random(100) > 95) {
            matrix.clear();
            matrix.drawBitmap(0, 0, wink_bmp, 8, 8, LED_ON);
            matrix.writeDisplay();
            wait(400);
            gameStep = BEGINNING;
          }
          // blink
          else {
            matrix.clear();
            matrix.drawBitmap(0, 0, blink_bmp, 8, 8, LED_ON);
            matrix.writeDisplay();
            wait(100);
            gameStep = BEGINNING;
          }
        }
        break;
      }

      /* ---------------------------------------------------------
       *  ASK_TO_PLAY
       *    ask user to play then wait for (and handle) input.
       */
      case ASK_TO_PLAY: {
        matrixSetMessage("Want to play a game?");
        gameStep++;
        break;
      }
      case ASK_TO_PLAY+1: {
        if( matrixScrollText() ) {
          gameStep++;
        }
        break;
      }
      case ASK_TO_PLAY+2: {
        // show wait state
        matrix.clear();
        matrix.blinkRate(1); // rate 1 = fast blink
        matrix.setCursor(1,0); // centers the '?'
        matrix.print("?");
        matrix.writeDisplay();
        // show yes/no buttons as red/green
        stickClear();
        stick.setPixelColor(0, stick.Color(0,64,0));
        stick.setPixelColor(1, stick.Color(0,64,0));
        stick.setPixelColor(6, stick.Color(64,0,0));
        stick.setPixelColor(7, stick.Color(64,0,0));
        stick.show();
        localWait(4000);
        gameStep++;
        break;
      }
      case ASK_TO_PLAY+3: {
        // check btns
        if(RiseBtns & BIG_L) {
          matrix.blinkRate(0);
          matrixClear();
          stickClear();
          // only give instructions the 1st time
          if(instructions) {
            instructions = false;
            gameStep = INSTRUCTIONS;
          }
          else {
            matrixSetMessage("Yay!  ");
            gameStep++;
          }
        }
        else if (RiseBtns & BIG_R) {
          stickClear();
          gameStep = NO_GAME;
        }
        else if( localReady() ) {
          stickClear();
          gameStep = NO_GAME+4;
        }
        break;
      }
      case ASK_TO_PLAY+4: {
        if(matrixScrollText()) {
          gameStep = START_GAME;
        }
        break;
      }

      /* ---------------------------------------------------------
       *  INSTRUCTIONS
       *    tell user how to play
       */
      case INSTRUCTIONS: {
        matrixSetMessage("Let's play guess my number!");
        gameStep++;
        break;
      }
      case INSTRUCTIONS+1: {
        if(matrixScrollText()) {
          matrixSetMessage("Turn this dial");
          gameStep++;
        }
        break;
      }
      case INSTRUCTIONS+2: {
        if(matrixScrollText()) {
          // show wait state
          matrix.clear();
          matrix.setCursor(1,0);
          matrix.blinkRate(1);
          matrix.print((char)ARROW_RIGHT);
          matrix.writeDisplay();
          wait(1500);
          gameStep++;
        }
        break;
      }
      case INSTRUCTIONS+3: {
        String msg = "Press " + String((char)ARROW_DOWN) + " to choose";
        matrixSetMessage(msg);
        matrix.blinkRate(0);
        // show ok btn
        stick.setPixelColor(0, stick.Color(0,64,0));
        stick.setPixelColor(1, stick.Color(0,64,0));
        stick.show();
        gameStep++;
        break;
      }
      case INSTRUCTIONS+4: {
        if(matrixScrollText()) {
          gameStep = START_GAME;
        }
        break;
      }

      /* ---------------------------------------------------------
       *  START_GAME
       *    clear out game states and update display
       */
      case START_GAME: {
        // set number to guess
        guessMe = random(1,64);
        guessCount = 0;
        // get number from right dial
        updateDials();
        theGuess = map(right,0,1023,1,64);
        // we have to convert int to a char array
        matrixSetMessage((String)theGuess);
        // show ok btn
        stick.setPixelColor(0, stick.Color(0,64,0));
        stick.setPixelColor(1, stick.Color(0,64,0));
        stick.show();
        gameStep = GAME;
        break;
      }

      /* ---------------------------------------------------------
       *  GAME
       *    update dials, update display if needed, handle input
       */
      case GAME: {
        updateDials();
        if(RiseBtns & BIG_L) {
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
          if( temp != theGuess ) {
            theGuess = temp;
            char tmp[3];
            matrixSetMessage((String)theGuess);
            matrixSetScrollPos(1);
          }
        }
        matrixScrollText();
        break;
      }
      /* ---------------------------------------------------------
       *  TOO_LOW
       *    show message and loop back
       */
      case TOO_LOW: {
        matrixSetMessage("Too low...");
        gameStep++;
        break;
      }
      case TOO_LOW+1: {
        if(matrixScrollText()) {
          gameStep = GAME;
        }
        break;
      }
      /* ---------------------------------------------------------
       *  TOO_HIGH
       *    show message and loop back
       */
      case TOO_HIGH: {
        matrixSetMessage("Too high...");
        gameStep++;
        break;
      }
      case TOO_HIGH+1: {
        if(matrixScrollText()) {
          gameStep = GAME;
        }
        break;
      }
      /* ---------------------------------------------------------
       *  YOU_WIN
       *    shows win message and how many guesses it took
       */
      case YOU_WIN: {
        matrixClear();
        stickClear();
        matrixSetMessage("YOU WIN!!  YOU WIN!!  ");
        gameStep++;
        break;
      }
      case YOU_WIN+1: {
        stickClear();
        if(matrixScrollText()) {
          gameStep++;
        }
        break;
      }
      case YOU_WIN+2: {
        matrixSetMessage("That was ");
        gameStep++;
        break;
      }
      case YOU_WIN+3: {
        if(matrixScrollText()) {
          // show score
          matrix.clear();
          matrix.setCursor(1,0);
          matrix.blinkRate(1);
          matrix.print(guessCount);
          matrix.writeDisplay();
          wait(2000);
          gameStep++;
        }
        break;
      }
      case YOU_WIN+4: {
        matrixSetMessage("guesses! ");
        gameStep++;
        break;
      }
      case YOU_WIN+5: {
        if(matrixScrollText()) {
          matrix.clear();
          matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
          matrix.writeDisplay();
          wait(1500);
          gameStep++;
        }
        break;
      }
      case YOU_WIN+6: {
        matrixSetMessage("Play again?");
        gameStep++;
        break;
      }
      case YOU_WIN+7: {
        if(matrixScrollText()) {
          gameStep = ASK_TO_PLAY+2;
        }
        break;
      }
      /* ---------------------------------------------------------
       *  NO_GAME
       *    shows sad face
       */
      case NO_GAME: {
        matrix.clear();
        matrix.blinkRate(0);
        matrix.setBrightness(1);
        matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
        matrix.writeDisplay();
        wait(1000);
        gameStep++;
        break;
      }
      case NO_GAME+1: {
        matrixSetMessage("awww... I'm sad...");
        gameStep++;
        break;
      }
      case NO_GAME+2: {
        if(matrixScrollText()) {
          gameStep++;
        }
        break;
      }
      case NO_GAME+3: {
        matrix.clear();
        matrix.blinkRate(0);
        matrix.setBrightness(2);
        matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
        matrix.writeDisplay();
        wait(5000);
        for(int ppp = 0; ppp < 20; ppp++) {
          tone(6,32);
          // delay(1);
          noTone(6);
          delay(15);
        }
        gameStep++;
        break;
      }
      case NO_GAME+4: {
        matrix.clear();
        matrix.setBrightness(4);
        matrix.blinkRate(0);
        matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
        matrix.writeDisplay();
        wait(4000);
        gameStep = BEGINNING;
        break;
      }

      /* ---------------------------------------------------------
       *  default
       *    if we get lost, restart
       */
      default: {
        if(matrixScrollText()) {
          gameStep = GREETING; // start over
        }
        break;
      }
    }
    clearBtnStates();
  }
}

