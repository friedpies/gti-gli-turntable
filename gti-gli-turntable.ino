#include <SoftwareSerial.h>
#include <Sabertooth.h>

// Controller code for GTI-GLI turntable obstacle.

#define armSwitch 3 // 1 is off, 0 is on
#define activateButton 2
#define slider A0 //goes from 0-1024

SoftwareSerial SWSerial(NOT_A_PIN, 11);
Sabertooth ST(128, SWSerial);

int maxSpeed = 90; 
int readSpeed = 0;
int turnDirection = 1; // CW is 1, CCW is -1
float currentSpeed = 0;
float accel = .5;
long currentTime = 0;
long timeElapsed = 0;
long obstacleTime = 10000; // obstacle runs for 10 seconds

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
    ST.motor(0);
    if (digitalRead(armSwitch) == LOW) {
      turntableState = ARMED_STATE;
    }

  }

  if (turntableState == ARMED_STATE) {
    Serial.println("ARMED");
    if (digitalRead(activateButton) == LOW) {
//      maxSpeed = map(analogRead(slider), 0, 1024, 0, 127);
      turnDirection = getRandomDirection();
      turntableState = ACCEL_STATE;
      delay(50); // debounce delay
    }

    if (digitalRead(armSwitch) == HIGH) {
      turntableState = UNARMED_STATE;
    }

  }

  if (turntableState == ACCEL_STATE) {
    Serial.println("ACCEL");
    ST.motor(int(currentSpeed * turnDirection));
    currentSpeed = currentSpeed + accel; //accelerate motor until it reaches maxSpeed
    Serial.println(currentSpeed * turnDirection);

    if (currentSpeed >= maxSpeed) {
      turntableState = RUNNING_STATE;
      currentTime = millis();
      timeElapsed = 0;
      ST.motor(currentSpeed * turnDirection);
    }

    if (digitalRead(armSwitch)) { // if switch is unarmed, slow the turntable down before stopping
      turntableState = DECEL_STATE;
    }

  }

  if (turntableState == RUNNING_STATE) {
    
    Serial.println("RUNNING");
    ST.motor(currentSpeed * turnDirection);
    timeElapsed = millis() - currentTime;

    if (digitalRead(armSwitch) || (timeElapsed >= obstacleTime)) { 
      turntableState = DECEL_STATE;
      timeElapsed = 0;
      currentTime = 0;
    }

  }

  if (turntableState == DECEL_STATE) {
    Serial.println("DECEL");
    ST.motor(int(currentSpeed * turnDirection));
    currentSpeed = currentSpeed - accel;
    Serial.println(currentSpeed * turnDirection);

    if (currentSpeed <= 0) {
      turntableState = UNARMED_STATE;
      ST.motor(0);
    }
  }

}

int getRandomDirection(){
  int randomNumber = random(0,2);
  if (randomNumber == 0){
    return -1;
  }
  if (randomNumber == 1){
    return 1;
  }
}




