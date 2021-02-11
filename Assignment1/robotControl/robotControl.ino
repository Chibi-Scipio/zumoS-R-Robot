#include <Wire.h>
#include <Zumo32U4.h>
#include "TurnSensor.h"


L3G gyro;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LineSensors sensors;
Zumo32U4Buzzer buzzer;
Zumo32U4ProximitySensors proxSens;
int turnSpeed = 100;                        //lower speed of movement for better control
int moveSpeed = 150;                        //Value between 0 - 400 (max speed)
int autoMoveSpeed = 75;                     //Much slower to check for collision
bool sysStart = false;
unsigned int proxSensThreshold = 1;         //Prox sensors have to read over 1 to detect object
unsigned int lineSensorValues[3];           //Holds line sensor intial values
unsigned int currentSensorValues[3];        //Holds current line sensor reading
char received;                              //Current character pulled from Serial1.read()
/*==============================
        SETUP FUNCTION
  =============================*/
void setup() {
  Serial1.begin(9600);                      //Serial communication - 9600 bits - Serial1 is needed for xbee



  char received = (char) Serial1.read();    //Pulls from GUI current commands
  while (received != 'q')                   //Loops until...
  {
    received = (char) Serial1.read();       //Calibrate function is called
  }
  delay(1000);
  sensorCalib();
  for (int i = 0; i < 3; i++)               //Countdown for start up sequence complete
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);    // 3, 2, 1...
  }
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);      //Go
  delay(1000);


}


/*==================
    LOOP FUNCTION
  ==================*/
void loop() {
  turnSensorUpdate();                     //Loops update on gyro sensor to monitor any movement

  int angleTurned = (((int32_t)turnAngle >> 16) * 360) >> 16;
  delay(1000);


  checkInput();                           //Checks for first instruction from GUI to start robot as there are multiple options
  if (sysStart)
  {
    wallDetection();                      //After first input is complete robot enters autonomous mode in which it travels by itself
  }
}

/*===========================
    HELPER FUNCTIONS
  ===========================*/

/*---------------------
  Sensor Calibration - removes code from start up function and cals all sensor calibration in one place (Proxy, Line & Gyro)
  ---------------------*/
//Sensor Calibration function    --------- Helps reduce size of setup function breaking into smaller easier chunks
void sensorCalib()
{
  sensors.initThreeSensors();                //Initialise front sensors
  proxSens.initFrontSensor();                //Initialise proximity sensors
  sensors.read(lineSensorValues);            //Takes current reading at start up
  for (int i = 0; i < 3; i++)
  {
    lineSensorValues[i] += 10;                //Margin of error - barrier lines will jump into 300-400 range
  }                                          //Sensor can go up randomly from 40 to 70 causing false positive
  turnSensorSetup();                        //Initialises sensor for gyro turning
  delay(500);
  turnSensorReset();                        //Resets gyro value to 0
  delay(500);

}
/*-----------------------
  Turn Left - turns robot by given amount of degrees to the left
  -------------------------*/
void turnLeft(int degrees) {
  turnSensorReset();
  motors.setSpeeds(-turnSpeed, turnSpeed);
  int angleTurned = 0;
  do {
    delay(1);
    turnSensorUpdate();                                         //Gets current angle from the updated gyro sensor
    angleTurned = (((int32_t)turnAngle >> 16) * 360) >> 16;     //Converts angle into degrees turned since last sensor reset
  } while (angleTurned < degrees);                              //Turns left until angle is same as degree of turn
  motors.setSpeeds(0, 0);                                       //Stop Turning
}

/*---------------------
  Turn right - turns robot by given amount of degrees to the right
  ---------------------*/
void turnRight(int degrees) {
  turnSensorReset();                                            //Resets gyro value to 0
  motors.setSpeeds(turnSpeed, -turnSpeed);
  int angleTurned = 0;
  do {
    delay(1);
    turnSensorUpdate();                                         //Gets current angle from the updated gyro sensor
    angleTurned = (((int32_t)turnAngle >> 16) * 360) >> 16;     //Converts angle into degrees turned since last sensor reset
  } while (angleTurned > -degrees);                             //Turns right until angle is same as degree of turn
  motors.setSpeeds(0, 0);                                       //Stop Turning
}



/*---------------------
  Check input - Checks first input to decide what to do before auto move: Manual movement, Room Searching or Straight to auto move
  ----------------------*/
//Check inital input value
void checkInput()
{

  while (Serial1.available() > 0)                               //Check that the GUI can still be communicated with
  {
    sysStart = true;                                            //Run rest of ability once this first input is completed
    received = (char) Serial1.read();                           //Takes current input from GUI

    if (received == 'x')                                        //'x' will halt movement - manual control given
    {
      motors.setSpeeds(0, 0);
      zumoManualControl();
    }
    else if (received == 'c')                                   //'c' start corridor movement and autonomy
    {
      //Empty to move straight to autonomous move
    }
    else if (received == 'i')
    {
      roomSearch();
    }
  }
}



/*------------------
  Manual Movement - Takes Serial input from GUI and translates into movement until halt is pressed: Movement options key included
  ------------------*/
//Manual Movement
void zumoManualControl()
{
  char dirInput = ' ';

  /*
      Char input for different commands
      Calibrate Sensors - q
      Forward - w
      Reverse - s
      Left - a
      Right - d
      Left Auto-Turn - l
      Right Auto-turn - r
      Halt Movement - x
      Corridor Moving - c
      Room - i
  */
  while (dirInput != 'x')                                         //Will run until stop button is pressed
  {
    dirInput = (char) Serial1.read();                              //Check GUI for new directional input
    switch (dirInput) {
      case 'w':
        motors.setSpeeds(moveSpeed, moveSpeed);                      //After each move, the speed is reset to 0
        delay(200);                                                  //Allows for concurrent move inputs
        motors.setSpeeds(0, 0);
        break;
      case 's':
        motors.setSpeeds(-moveSpeed, -moveSpeed);
        delay(200);
        motors.setSpeeds(0, 0);
        break;
      case 'a':
        motors.setSpeeds(-turnSpeed, turnSpeed);
        delay(200);
        motors.setSpeeds(0, 0);
        break;
      case 'd':
        motors.setSpeeds(turnSpeed, -turnSpeed);
        delay(200);
        motors.setSpeeds(0, 0);
        break;
      case 'l':                                                     //Used for cases of auto rotating round corner
        turnLeft(90);                                               //Robot stops itself after rotating 90 degress to the left
        delay(200);
        break;
      case 'r':
        turnRight(90);                                              //Robot stops itself after rotating 90 degress to the left
        delay(200);
        break;
      case 'i':
        roomSearch();
        delay(200);
        break;
      default:                                                       //Default case stops movement
        motors.setSpeeds(0, 0);
        delay(200);
        break;
    }
  }
}
/*---------------
 Wall detector - Based on border detect example. Will monitor each sensor and turn the robot away from walls - as long as it isn't a corner
 ----------------*/
void wallDetection()
{
  sensors.read(currentSensorValues);                                    //Read current sensor values
  if (!cornerHit())                                                     //If not at a corner then..
  {
    if (currentSensorValues[0] > (lineSensorValues[0]))                 //Left wall check - current reading > initialise value
    {
      motors.setSpeeds(turnSpeed, -turnSpeed);                          //Turn to right
      delay(150);
      motors.setSpeeds(autoMoveSpeed, autoMoveSpeed);                   //Continue
    }
    if (currentSensorValues[2] > lineSensorValues[2])                   //Right wall check
    {
      motors.setSpeeds(-turnSpeed, turnSpeed);                          //Turn to left
      delay(150);
      motors.setSpeeds(autoMoveSpeed, autoMoveSpeed);                   //Continue
    }
    else
    {
      motors.setSpeeds(autoMoveSpeed, autoMoveSpeed);                   //If neither continue forward
    }
  }
}
/*------------
 Corner hit - boolean function as it controls wall detection behaviour - Warns user that it has hit a corner and allows for turn
 ------------*/
bool cornerHit()
{
  if (currentSensorValues[0] > lineSensorValues[0] || currentSensorValues[2] > lineSensorValues[2])
  {
    if (currentSensorValues[1] > (lineSensorValues[1]) - 5){   //Margin of error available - sensor increase by 4 randomly while moving
                                                               //if other sensors have changed as well as middle it means it has hit a wall in front and needs turning
      motors.setSpeeds(0, 0);
      buzzer.playNote(NOTE_A(4), 300, 15);                    //Audible warning of hitting wall
      zumoManualControl();
      return true;
    }
    return false;
  }
  return false;
}


/*----------------
 Room search - Uses proximity sensors to detect objects within a room and respond with a tone to display if am object is seen
 Based on the face towards opponent example
 ----------------*/
void roomSearch()
{
  char roomDir = ' ';
  unsigned int leftProxSens = 0;
  unsigned int rightProxSens = 0;
  bool personDetected = false;

  proxSens.read();
  leftProxSens = proxSens.countsFrontWithLeftLeds();                                          //Gets current left sensor data
  rightProxSens = proxSens.countsFrontWithRightLeds();                                        //Gets current right sensor data

  personDetected = leftProxSens >= proxSensThreshold || rightProxSens >= proxSensThreshold;   //Check to compare with proximity sensor threshold
  if (personDetected)                                                                         //If found play 4 beeps to signal person found or...
  {
    for (int i = 0; i < 4; i++)
    {
      buzzer.playNote(NOTE_G(4), 500, 15);
      delay(1000);
    }

  }
  else {
    buzzer.playNote(NOTE_B(2), 500, 15);                                                      //Play one beep to signal no person found
    return;
  }
}
