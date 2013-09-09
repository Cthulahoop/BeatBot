
// sets all pixels to a color
void stickColorFill(uint32_t c) {
  for(uint16_t i=0; i<stick.numPixels(); i++) {
    stick.setPixelColor(i, c);
  }
  stick.show();
}

// uses a bitmask to turn pixels on or off
void stickSetPixels(byte b, uint32_t c) {
  for(uint16_t i=0; i<stick.numPixels(); i++) {
    // if we multiply the color by bit value (0 or 1) it
    // will turn the current pixel (i) on or off.
    stick.setPixelColor(i, c*bitRead(b,i));
  }
  stick.show();
}

// scroll should be between 8 and str.length() * -6;
void mtxDrawText(String str, int scroll) {
  matrix.clear();
  matrix.setCursor(scroll,0);
  matrix.print(str);
  matrix.writeDisplay();
}

