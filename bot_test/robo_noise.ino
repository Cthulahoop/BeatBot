/* 
 *  Plays robot-like sounds 
 */

void playNoise() {

  // create a randomized sound
  int time = random(50, 500); // length of noise
  int tone1 = random(0,800); // start frequency
  int tone2 = random(0,800); // end frequency
  int chop = random(20) - 5;  // if > 0 noise is "buzzy"
  int repeat = min(random(10)-5, 1); // if > 0 noise can repeat up to 5 times
  int bounce = random(3); // plays noise backwards after it plays forwards

  if (chop > 0) {
    time = time / chop;
  }

  for(int r = repeat; r>0; r--) {
    for(int i=0; i<=time; i++) {
      int note = map(i,0,time,tone1,tone2);
      tone(tonePin,  note);
      if(chop >= 0) {
        delay(chop);
        noTone(tonePin);
        delay(1);
      }
      else{
        delay(1);
      }
    }
    noTone(tonePin);
  }
  if(bounce == 0) {
    for(int i=0; i<=time; i++) {
      int note = map(i,0,time,tone2,tone1);
      tone(tonePin, note);
      if(chop >= 0) {
        delay(chop);
        noTone(tonePin);
        delay(1);
      }
      else{
        delay(1);
      }
    }
    noTone(tonePin);
  }
}


