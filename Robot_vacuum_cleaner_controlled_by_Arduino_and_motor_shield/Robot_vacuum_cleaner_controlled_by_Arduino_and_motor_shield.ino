/*
Program for controlling a robot with two motors.
The robot turns when motors changes their speed and direction.
Front bumpers on left and right sides detect obstacles.
Motor shield is used to run motors.
*/

//right side
const int pinRightMotorDirection = 4; //this can be marked on motor shield as "DIR A"
const int pinRightMotorSpeed = 3; //this can be marked on motor shield as "PWM A"
const int pinRightBumper = 2;

//left side
const int pinLeftMotorDirection = 7; //this can be marked on motor shield as "DIR B"
const int pinLeftMotorSpeed = 6; //this can be marked on motor shield as "PWM B"
const int pinLeftBumper = 8;

//uncomment next 2 lines if Motor Shield has breaks
//const int pinRightMotorBreak = PUT_BREAK_PIN_HERE; //this can be marked on motor shield as "BREAKE A"
//const int pinLeftMotorBreak = PUT_BREAK_PIN_HERE; //this can be marked on motor shield as "BREAKE B"

//fields
const int turnRightTimeout = 100;
const int turnLeftTimeout = 150;
//set in counter how long a motor is running back: N/10 (in milliseconds)
int countDownWhileMovingToRight;
int countDownWhileMovingToLeft;

//Initialization
void setup() {

  initPins();

//uncomment next 4 lines if Motor Shield has breaks
//  pinMode(pinLeftMotorBreak, OUTPUT);
//  pinMode(pinRightMotorBreak, OUTPUT);
//  digitalWrite(pinLeftMotorBreak, LOW); //turn off breaks 
//  digitalWrite(pinRightMotorBreak, LOW); //turn off breaks 

  runRightMotorForward();
  runLeftMotorForward();
  startMotors();
}

//Main loop
void loop() {
  
  verifyAndSetRightSide();
  verifyAndSetLeftSide();

  processRightSide();
  processLeftSide();

  delay(10);//repeat every 10 milliseconds
}

//---------------------------------------------------
void initPins(){
  pinMode(pinRightMotorDirection, OUTPUT);
  pinMode(pinRightMotorSpeed, OUTPUT);
  pinMode(pinRightBumper, INPUT);
  pinMode(pinLeftMotorDirection, OUTPUT);
  pinMode(pinLeftMotorSpeed, OUTPUT);
  pinMode(pinLeftBumper, INPUT);
}

void startMotors(){
  setMotorSpeed(pinRightMotorSpeed, 255);
  setMotorSpeed(pinLeftMotorSpeed, 255);
}

void waitWhileAnyBumperIsPressed(){
  while(checkBumperIsNotPressed(pinRightBumper)
        && checkBumperIsNotPressed(pinLeftBumper)){
    delay(20);//check every 20 milliseconds
  }
}
 
void processRightSide(){
  if(countDownWhileMovingToRight <= 0)//checks if counter was NOT ran when bumper had been pressed
    return;
  //otherwise - counter is counting down (as a delay) while the right motor is moving backward 
  countDownWhileMovingToRight--;//decrease the counter if it WAS ran when bumper had been pressed
  if(countDownWhileMovingToRight <= 0)//if the running counter got down to zero
    runRightMotorForward();//run the right motor forward
}
  
void processLeftSide(){
  if(countDownWhileMovingToLeft <= 0)
    return;
  countDownWhileMovingToLeft--;
  if(countDownWhileMovingToLeft <= 0)
    runLeftMotorForward();
}
  
void verifyAndSetRightSide(){
  if(checkBumperIsNotPressed(pinRightBumper))//checks if right bumper has NOT been pressed
    return;
  if(checkCounterIsNotSet(countDownWhileMovingToRight))//if the counter is not yet counting down
    runRightMotorBackward();//run the right motor backward
  countDownWhileMovingToRight = turnRightTimeout;//set the counter to maximum value to start it counting down
}

void verifyAndSetLeftSide(){
  if(checkBumperIsNotPressed(pinLeftBumper))//checks if left bumper has NOT been pressed
    return;
  if(checkCounterIsNotSet(countDownWhileMovingToLeft))//if the counter is not yet counting down
    runLeftMotorBackward();//run the right motor backward
  countDownWhileMovingToLeft = turnLeftTimeout;//set the counter to maximum value to start it counting down
}

bool checkCounterIsNotSet(int counter){
  return counter <= 0;
}

bool checkBumperIsNotPressed(int pinBumper){
  return digitalRead(pinBumper);
}

void runRightMotorForward(){
  runMotorForward(pinRightMotorDirection);
}

void runLeftMotorForward(){
  runMotorForward(pinLeftMotorDirection);
}

void runRightMotorBackward(){
  runMotorBackward(pinRightMotorDirection);
}

void runLeftMotorBackward(){
  runMotorBackward(pinLeftMotorDirection);
}

void runMotorForward(int pinMotorDirection){
  digitalWrite(pinMotorDirection, HIGH); //set direction forward 
}

void runMotorBackward(int pinMotorDirection){
  digitalWrite(pinMotorDirection, LOW); //set direction backward 
}

void setMotorSpeed(int pinMotorSpeed, int motorSpeed){
    analogWrite(pinMotorSpeed, motorSpeed);
}

