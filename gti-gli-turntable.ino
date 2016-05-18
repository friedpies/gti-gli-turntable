#include <SoftwareSerial.h>
#include <Sabertooth.h>

// Controller code for GTI-GLI turntable obstacle.

#define armSwitch 3 // 1 is off, 0 is on
#define activateButton 2
#define slider A0 //goes from 0-1024

SoftwareSerial SWSerial(NOT_A_PIN, 11);
Sabertooth ST(128, SWSerial);

int maxSpeed = 0; // set with slider
float currentSpeed = 0;
float accel = .5;
int readSpeed = 0;

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

  if (turntableState == UNARMED_STATE) {
    Serial.println("UNARMED");
    if (digitalRead(armSwitch) == LOW) {
      turntableState = ARMED_STATE;
    }

  }

  if (turntableState == ARMED_STATE) {
    Serial.println("ARMED");
    if (!digitalRead(activateButton)) {
      maxSpeed = map(analogRead(slider), 0,1024, 0, 127);
      turntableState = ACCEL_STATE;
      delay(50); // debounce delay
    }

    if (digitalRead(armSwitch) == HIGH) {
      turntableState = UNARMED_STATE;
    }

  }

  if (turntableState == ACCEL_STATE) {
    Serial.println("ACCEL");
    ST.motor(int(currentSpeed));
    currentSpeed = currentSpeed + accel; //accelerate motor until it reaches maxSpeed
    Serial.println(currentSpeed);

    if (currentSpeed >= maxSpeed) {
      turntableState = RUNNING_STATE;
      ST.motor(currentSpeed);
    }

    if (digitalRead(armSwitch)) { // if switch is unarmed, slow the turntable down before stopping
      turntableState = DECEL_STATE;
    }

  }

  if (turntableState == RUNNING_STATE) {
    Serial.println("RUNNING");
    ST.motor(currentSpeed);
    //add some sort of counter

    if (digitalRead(armSwitch)) { // || counter is met) { // if switch is unarmed, slow the turntable down before stopping
      turntableState = DECEL_STATE;
    }

  }

  if (turntableState == DECEL_STATE) {
    Serial.println("DECEL");
    ST.motor(int(currentSpeed));
    currentSpeed = currentSpeed - accel;
    Serial.println(currentSpeed);

    if (currentSpeed <= 0) {
      turntableState = UNARMED_STATE;
      ST.motor(0);
    }
  }

  //Serial.println(digitalRead(armSwitch));
}



