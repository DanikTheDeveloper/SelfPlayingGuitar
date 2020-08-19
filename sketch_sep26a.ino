/* Sweep
  by BARRAGAN <http://barraganstudio.com>
  This example code is in the public domain.

  modified 8 Nov 2013
  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

int const potPin  = A0;
int const piano = A5;
int potVal;

int count = 0;
int amp = 35;
int range = (180 - amp)/2;
int from = range + amp * 2 / 5;
bool def = true;

Servo servo1, servo2;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 60;    // variable to store the servo position

void strum(const int del) {
  if (pos < 45) {
    for (pos = 30; pos <= 60; pos++) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo1.write(pos);              // tell servo to go to position in variable 'pos'
      delay(del);                       // waits 15ms for the servo to reach the position
    }
  }
  else {
    for (pos = 60; pos >= 30; pos--) { // goes from 180 degrees to 0 degrees
      servo1.write(pos);              // tell servo to go to position in variable 'pos'
      delay(del);                       // waits 15ms for the servo to reach the position
    }
  }
}
void moveTo(int from, const int to) {
  if (from > to) {
    for (; from >= to; from--) {
      servo2.write(from);
      delay(1);
    }
  }
  else  {
    for (; from <= to; from++) {
      servo2.write(from);
      delay(1);
    }
  }
  potVal = analogRead(potPin);
  delay(potVal / 4.0);
}

void setup() {
  Serial.begin(9600);
  servo1.attach(9);  // attaches the servo on pin 9 to the servo object
  servo2.attach(8);
}
void loop() {
  if (count < 7) {
    if (count == 0 || count == 2 || count == 4 || count == 6) {
      moveTo(from, range - 3 + amp * 2 / 4);
    }
    else if (count == 3) {
      moveTo(from, range + 1.5);
    }
    else {
      moveTo(from, range + amp + 1.5);
    }
    strum(10);
    potVal = analogRead(potPin);
    delay(potVal / 4.0);
  }
  else{
    strum(6);
    delay(potVal / 100.0);
  }
  count++;
  if (count > 9) {
    count = 0;
    delay(potVal / 4.0);
  }
  Serial.println(analogRead(piano));
}
