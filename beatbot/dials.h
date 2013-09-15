/* ===================================================================
 *
 *  A Class for reading 2 potentiometers
 *  by Ric Ewing
 *
 *  v1.1  15.09.2013
 *    - implemented low pass filter
 *    - added version/history
 *  v1.0  08.08.2013
 *    - initial stable version
 *
 *  TODO:
 *    The input from some potentiometers can be
 *    really noisy and make for a lot of false
 *    positives on the new flags. The 10k pots
 *    I've been using jump around quite a bit,
 *    so I'm examining using smoothing or
 *    remapping values to reudce the jumpyness
 *    of the pot inputs.
 *
 *    [15.09.2013]
 *    Implemented low pass filtering where:
 *    new_value = (A*old_value) + (B*new_value)
 *    where A+B=1, and the smaller B is (ie. 0.2)
 *    the bigger the filtering effect.
 *    Due to filtering, maximum value is never
 *    returned, will have to adjust in code.
 *
 *  [NOTES]
 *    To fix in hardware, you can add a small
 *    capacitor (around .1µ) between the read
 *    pin and ground to smooth out the voltage
 *    returned. This may alter the minimu or
 *    maximum values you can read from the pot,
 *    so test and handle appropriately in code.
 *
 *    [15.09.2013]
 *    Tried remapping input values from 1023 to
 *    255. Still get too much jitter at value
 *    boundries (e.g. jitter at original
 *    reading of ~40 shows as remapped jitter
 *    between 10 and 11).
 *
 */

const int LEFT_DIAL = A2;
const int RIGHT_DIAL = A3;
const float LPF_A = 0.4; // low pass filter value
const float LPF_B = 0.6; // weighted towards the new reading

int left;   // read the dial settings
int right;  //

int previousLeft = 0;
int previousRight = 0;

boolean newLeft = false;   // flags to check if dials have changed
boolean newRight = false;  //

void initDials() {
  for (int i; i < 20; i++) {
    // pre filter dial values;
    left = (previousLeft * LPF_A) + (analogRead(LEFT_DIAL) * LPF_B);   // read the dial settings
    right = (previousRight * LPF_A) + (analogRead(RIGHT_DIAL) * LPF_B); //
  }
}


void updateDials() {
  newLeft = false;
  newRight = false;

  // left = analogRead(LEFT_DIAL);   // read the dial settings
  // right = analogRead(RIGHT_DIAL); //

  left = ceil((previousLeft * LPF_A) + (analogRead(LEFT_DIAL) * LPF_B));   // read the dial settings
  right = ceil((previousRight * LPF_A) + (analogRead(RIGHT_DIAL) * LPF_B)); // and filter

  // check dials against the previous values and
  if(left != previousLeft) {
    previousLeft = left;
    newLeft = true;
    // Serial.print("left dial: ");
    // Serial.println(left);
  }
  if(right != previousRight) {
    previousRight = right;
    newRight = true;
    // Serial.print("right dial: ");
    // Serial.println(right);
  }

}
