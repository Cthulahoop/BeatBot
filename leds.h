String scrollTxt = "";
int scrollPos = 0;
int endScrollPos = 0;

void initLeds() {
  // start pixelStick
  stick.begin();
  stick.setBrightness(64);
  stick.show(); // Initialize all pixels to 'off'


  // start the matrix
  matrix.begin(0x70);
  matrix.setBrightness(8);

  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);

  matrix.clear();
  matrix.writeDisplay();
}

// sets all pixels to a color
void stickColorFill(uint32_t c) {
  for(uint16_t i=0; i<stick.numPixels(); i++) {
    stick.setPixelColor(i, c);
  }
  stick.show();
}

// shorthand for clearing out the pixelStick
void stickClear() {
  stickColorFill(0);
}

void matrixClear() {
  matrix.clear();
  matrix.writeDisplay();
}

void matrixSetScrollText(String str) {
  scrollTxt = str;
  scrollPos = 8;
  endScrollPos = scrollTxt.length() * -6;
}

boolean matrixScrollText() {
  matrix.clear();
  if(scrollPos > endScrollPos) {
    matrix.setCursor(scrollPos--,0);
    matrix.print(scrollTxt);
    matrix.writeDisplay();
    return false; // are we done scrolling?
  }
  else if(scrollPos == endScrollPos) {
//    matrixClear();
    return true; // yes, we're done
  }
  else {
    scrollPos = 8;
    return true; // 2nd chance on done
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return stick.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return stick.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return stick.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

