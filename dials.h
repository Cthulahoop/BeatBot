#define LEFT_DIAL A2
#define RIGHT_DIAL A3

int left;   // read the dial settings
int right;  //

int previousLeft = 0;
int previousRight = 0;

boolean newLeft = false;   // flags to check if dials have changed
boolean newRight = false;  //

void updateDials() {
  newLeft = false;
  newRight = false;

  left = analogRead(LEFT_DIAL);   // read the dial settings
  right = analogRead(RIGHT_DIAL); //

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
