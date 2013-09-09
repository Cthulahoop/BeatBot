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


boolean updateBtns() {
  // we use a boolean to return if any changes have happend
  boolean flag = false;

  // loop through btns and check for changes
  for(int i=0; i<8; i++) {
    flag |= btns[i].update(); // updates the btn and sets flag if changed
  }
  // update BtnStates
  if(flag) {
    BtnStates = getBtnStates();
  }
  // return true if any button state has changed
  return flag;
}

byte getBtnStates() {
  byte b = 0;
  for(int i=0; i<8; i++) {
    // because INPUT_PULLUP reverses the read state LOW = pressed
    if(btns[i].read() == LOW) {
      bitSet(b,i); // sets bit i to 1
    }
  }
  return b;
}


