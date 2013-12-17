/*
Program for controlling a robot with two motors.
The robot turns when motors changes their speed and direction.
Front bumpers on left and right sides detect obstacles.
Ultrasonic sonars can be connected to analog inputs (tested on LV-MaxSonar-EZ1):
 - put pins in array sonarPins in following order: left, right, front, others..
   Examples:  
   1. only left and right sonars connected to pins 2 and 3:      sonarPins[] = {2,3}
   2. left, right and front sonars connected to pins 2, 3 and 5: sonarPins[] = {2,3,5}
   3. only front sonar connected to pin 5:                       sonarPins[] = {-1,-1,5}
   4. only left sonar connected to pin 2:                        sonarPins[] = {2}
   5. only right sonar connected to pins 3:                      sonarPins[] = {-1,3}
   6. 5 sonars connected to pins 1,2,3,4,5:                      sonarPins[] = {1,2,3,4,5}
Motor shield is used to run motors.
*/
const int Baud = 9600; //UART port speed

//Sonar properties
int sonarPins[] = {1, 2};//Analog Pin Nums to sonar sensor Pin AN
const long MinLeftDistance = 20; //Minimum allowed left distance
const long MinRightDistance = 20; //Minimum allowed right distance
const long MinFrontDistance = 15; //Minimum allowed front distance
const int SamplesAmount = 15;//more samples - smoother measurement and bigger lag
const int SonarDisplayFrequency = 10; //display only one of these lines - not all
int sonarDisplayFrequencyCount = 0; 
const long Factor = 2.54 / 2;
long samples[sizeof(sonarPins)][SamplesAmount];
int sampleIndex[sizeof(sonarPins)];

//right side
const int pinRightMotorDirection = 4; //this can be marked on motor shield as "DIR A"
const int pinRightMotorSpeed = 3; //this can be marked on motor shield as "PWM A"
const int pinRightBumper = 2; //where the right bumper is connected 

//left side
const int pinLeftMotorDirection = 7; //this can be marked on motor shield as "DIR B"
const int pinLeftMotorSpeed = 6; //this can be marked on motor shield as "PWM B"
const int pinLeftBumper = 8; //where the right bumper is connected

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
  Serial.begin(Baud);
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
  for(int i = 0; i < sizeof(sonarPins); i++)
    pinMode(sonarPins[i], INPUT);
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
  if(checkBumperIsNotPressed(pinRightBumper)       //checks if right bumper has NOT been pressed
     && measureDistance(1, 'R') > MinRightDistance //checks if the minimum allowed right distance is not reached
     && measureDistance(2, 'F') > MinFrontDistance)//checks if the minimum allowed front distance is not reached
    return;
  if(checkCounterIsNotSet(countDownWhileMovingToRight))//if the counter is not yet counting down
    runRightMotorBackward();//run the right motor backward
  countDownWhileMovingToRight = turnRightTimeout;//set the counter to maximum value to start it counting down
}

void verifyAndSetLeftSide(){
  if(checkBumperIsNotPressed(pinLeftBumper)        //checks if left bumper has NOT been pressed
     && measureDistance(0, 'L') > MinLeftDistance  //checks if the minimum allowed left distance is not reached
     && measureDistance(2, 'F') > MinFrontDistance)//checks if the minimum allowed front distance is not reached
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

//-- sonar methods --
long measureDistance(int pinIndex, char displayChar){
  if(!takeSample(pinIndex))
    return 255;//return large value when sonar is not connected
  long distance = calculateAvarageDistance(pinIndex) / Factor;
  printDistance(distance, displayChar);
  return distance;
}

void printDistance(long distance, char displayChar){
  if(--sonarDisplayFrequencyCount <= 0){
    sonarDisplayFrequencyCount = SonarDisplayFrequency; 
    return;
  }

  Serial.print(distance);
  for(int i = 12; i < distance && i < 100; i += 2)
    Serial.print(displayChar);
  Serial.println();
}

boolean takeSample(int pinIndex){
  if(pinIndex >= sizeof(sonarPins) - 1 || sonarPins[pinIndex] < 0)//invalid pin index - pin is not defined
    return false;
  long value = analogRead(sonarPins[pinIndex]);
  if(value == 0)//pin is connected to GND - not to the sonar 
    return false;
  if(++sampleIndex[pinIndex] >= SamplesAmount)
    sampleIndex[pinIndex] = 0;
  samples[pinIndex][sampleIndex[pinIndex]] = value;
  return true;
}

long calculateAvarageDistance(int pinIndex){
  long  average = 0;
  for(int i = 0; i < SamplesAmount; i++) 
    average += samples[pinIndex][i];
  return average / SamplesAmount;
}
