#include <Bounce.h>

/*
  Bounce Library Docs

  Bounce(byte pin,unsigned long debounce_interval)
    Instantiates a Debounce object for the specified pin with a debounce time.
    Because Bounce does not use interrupts, you have to "update" the object
    before reading its value.

  int update()
    Updates Bounce. Returns true if the pin state changed (HIGH to LOW or LOW
    to HIGH). False if not.

  void interval(unsigned long interval)
    Changes the debounce time in milliseconds.

  int read()
    Reads the updated pin state.

  void write(int state)
    Sets the stored pin state

  void rebounce(unsigned long interval)
    Forces the pin to signal a state change in X milliseconds even if the state
    does not actually change. Example: A button that repeats every X
    milliseconds as long as it is held down.

  unsigned long duration(void)
    Returns the number of milliseconds the pin has been in the current state.

  bool risingEdge()
    The risingEdge method is true for one scan after the de-bounced input goes
    from off-to-on.

  bool fallingEdge();
    The fallingEdge method it true for one scan after the de-bounced input goes
    from on-to-off.
*/

#define BIG_L 0
#define BIG_ML 1
#define BIG_MR 2
#define BIG_R 3

#define SM_L 4
#define SM_ML 5
#define SM_MR 6
#define SM_R 7

unsigned long bTime = 40;

Bounce btns[8] = {
  // if needed, change the pin numbers to match your setup
  Bounce( 5, bTime ),  // 0: big btn left
  Bounce( 4, bTime ),  // 1: big btn mid-left
  Bounce( 3, bTime ),  // 2: big btn mid-right
  Bounce( 2, bTime ),  // 3: big btn right

  Bounce( 11, bTime ),  // 4: small btn left
  Bounce( 10, bTime ),  // 5: small btn mid-left
  Bounce( 9, bTime ),   // 6: small btn mid-right
  Bounce( 8, bTime )    // 7: small btn right
};

byte BtnStates = 0; // a bitmask that holds the current btn pressed state
byte NewBtns = 0; // a bitmask that holds the current newly pressed btns

void initBtns() {
  // setup btn read pins
  pinMode(2,INPUT_PULLUP);  // big right
  pinMode(3,INPUT_PULLUP);  // big mid-right
  pinMode(4,INPUT_PULLUP);  // big mid-left
  pinMode(5,INPUT_PULLUP);  // big left
  pinMode(8,INPUT_PULLUP);  // small right
  pinMode(9,INPUT_PULLUP);  // small mid-right
  pinMode(10,INPUT_PULLUP); // small mid-left
  pinMode(11,INPUT_PULLUP); // small left
}

boolean updateBtns() {
  Serial.println("*");
  // clear bitmasks
  NewBtns = 0;
  BtnStates =0;

  // loop through btns and check for changes
  for(int i=0; i<8; i++) {
    if(btns[i].update()) {
      bitSet(NewBtns,i);
    }
    if(btns[i].read() == LOW) {
      bitSet(BtnStates,i);
    }
  }
  // return true if any button state has changed
  return NewBtns>0;
}

