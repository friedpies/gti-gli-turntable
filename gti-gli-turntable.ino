#include <SoftwareSerial.h>
#include <Sabertooth.h>

// Controller code for GTI-GLI turntable obstacle.

#define armSwitch 2 // 1 is off, 0 is on
#define activateButton 3
#define slider A0 //goes from 0-1024

SoftwareSerial SWSerial(NOT_A_PIN, 11);
Sabertooth ST(128, SWSerial);

int maxSpeed = 0; // set with slider
float currentSpeed = 0;
int holdDelay(5000);
float accel = .1;
int readSpeed = 0;

boolean GO = LOW;
boolean STOP = HIGH;

enum states {
  UNARMED_STATE,
  ARMED_STATE,
  ACCEL_STATE,
  RUNNING_STATE,
  DECEL_STATE
};

enum states turntableState = UNARMED_STATE;

void setup() {
  Serial.begin(9600);
  SWSerial.begin(9600);
  ST.autobaud();

  pinMode(armSwitch, INPUT_PULLUP);
  pinMode(activateButton, INPUT_PULLUP);
}

void loop() {


  //  if (digitalRead(blueButton) == LOW) {
  //    Serial.println(readSpeed);
  //    ST.motor(1, readSpeed);
  //  }

  if (turntableState == UNARMED_STATE) {
    if (digitalRead(armSwitch) == HIGH) {
      turntableState == ARMED_STATE;
    }
  }

  if (turntableState == ARMED_STATE) {

    if (digitalRead(activateButton) == LOW) {
      maxSpeed = map(analogRead(slider), 0, 1024, 0, 127);
      turntableState == ACCEL_STATE;
      delay(50); // debounce delay
    }

    if (digitalRead(armSwitch) == LOW) {
      turntableState == UNARMED_STATE;
    }

  }

  if (turntableState == ACCEL_STATE) {
    ST.motor(int(currentSpeed));
    currentSpeed = currentSpeed + accel; //accelerate motor until it reaches maxSpeed

    if (currentSpeed == maxSpeed) {
      turntableState == RUNNING_STATE;
    }

    if (digitalRead(armSwitch) == LOW) { // if switch is unarmed, slow the turntable down before stopping
      turntableState == DECEL_STATE;
    }

  }

  if (turntableState == RUNNING_STATE) {

    ST.motor(currentSpeed);
    //add some sort of counter

    if (digitalRead(armSwitch) == LOW){ // || counter is met) { // if switch is unarmed, slow the turntable down before stopping
        turntableState == DECEL_STATE;
  }

}

if (turntableState == DECEL_STATE) {

  ST.motor(int(currentSpeed));
  currentSpeed = currentSpeed - accel;

  if (currentSpeed == 0) {
    turntableState == UNARMED_STATE;
  }
}

}



