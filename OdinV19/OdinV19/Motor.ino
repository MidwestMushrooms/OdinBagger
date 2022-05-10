
void TestSubMotor() {
//  if(serialActive==1){Serial.println("function TestSubMotor motorTestModeIndex=" + String(motorTestModeIndex) + "," + String(timers[0]) + "," + String(timeout));}
  statusMessage[0]="Motor Test Sub";
  tone(buzzer, (2 * noteA) + (100 * motorTestModeIndex), 10);
  if (timers[0] > timeout) { // hit timeout, go to next phase of test
    motorTestModeIndex++;
    if (motorTestModeIndex >= motorTestModesSize) {
      currentlyRunning = 0; // test over
      GoodSound(0);
    } else {
      TimerReSet(0); // reset the timer for the next run leave timeout alone
      StopMotors(99);
      delay(50);
      MotorTest(0);// do the Motor Test
    }
  }
}
void TestSupMotor() {
//  if(serialActive==1){Serial.println("function TestSupMotor");}
  statusMessage[0]="Motor Test Sup";
  tone(buzzer, (2 * noteF) + (100 * motorTestModeIndex), 10);
  if (timers[0] > timeout) { // hit timeout, go to next phase of test
    motorTestModeIndex++;
    if (motorTestModeIndex >= motorTestModesSize) {
      currentlyRunning = 0; // test over
      GoodSound(1);
    } else {
      TimerReSet(0); // reset the timer for the next run leave timeout alone
      StopMotors(99);
      delay(250);
      MotorTest(1);// do the Motor Test
    }
  }
}

void StartTestMotor0(){
  whichMotor = 0; // tells which motor is to be acted on
  StartAProcessAndExitMenu(12, "TEST MOTOR 1"," "," ");
}

void StartTestMotor1(){
  whichMotor = 0; // tells which motor is to be acted on
  StartAProcessAndExitMenu(13, "TEST MOTOR 2"," "," ");
}

void MotorTest(boolean WhichMotor) {
//  if(serialActive==1){Serial.println("function MotorTest " + String(WhichMotor) = ", motorTestModeIndex=" + String(motorTestModeIndex));}
  switch (motorTestModeIndex) {
    case 0: {
        statusMessage[1]="Speed 35 >>>>";
        Motor(WhichMotor, 35, 0);
        break;
      }
    case 1: {
        statusMessage[1]="Speed 40 <<<<";
        Motor(WhichMotor, 40, 1);
        break;
      }
    case 2: {
        statusMessage[1]="Speed 45 >>>>";
        Motor(WhichMotor, 45, 0);
          break;
      }
    case 3: {
        statusMessage[1]="Speed 50 <<<<";
        Motor(WhichMotor, 50, 1);
        break;
      }
    case 4: {
        statusMessage[1]="Speed 55 >>>>";
        Motor(WhichMotor, 55, 0);

        break;
      }
    case 5: {
        statusMessage[1]="Speed 16 <<<<";
        Motor(WhichMotor, 16, 1);
       break;
      }
    case 6: {
        statusMessage[1]="Speed 32 >>>>";
        Motor(WhichMotor, 32, 0);

        break;
      }
    case 7: {
        statusMessage[1]="Speed 64 <<<<";
        Motor(WhichMotor, 64, 1);
        break;
      }
    case 8: {
        statusMessage[1]="Speed 128 >>>>";
        Motor(WhichMotor, 128, 0);

        break;
      }
    case 9: {
        statusMessage[1]="Speed 250 <<<<";
        Motor(WhichMotor, 250, 1);
        break;
      }
    case 10: {
        statusMessage[1]="STOP";
        Motor(WhichMotor, 0, 0);
        break;
      }
  }
}
void Motor(boolean WhichMotor, byte mySpeed, boolean myDirection) {
//  if(serialActive==1){Serial.println("function Motor " + String(WhichMotor) + "," +String(mySpeed) + "," + String(myDirection));}
  // function to control the motors
  if(processPaused==0){
  if (WhichMotor == 0) {
    if (myDirection == 0) {
      digitalWrite(subMotorDirectionPin, HIGH);
   } else {
      digitalWrite(subMotorDirectionPin, LOW);
    }

    analogWrite(subMotorSpeedPin, mySpeed);
  } else {
    // sup
    if (myDirection == 0) {
      digitalWrite(supMotorDirectionPin, LOW);
    } else {
      digitalWrite(supMotorDirectionPin, HIGH);
    }
    analogWrite(supMotorSpeedPin, mySpeed);
  }
  if (mySpeed == 0) {
    StopMotors(WhichMotor);
  }
  }
}

void StopMotors(int whichMotor) {
//  if(serialActive==1){Serial.println("function StopMotors " + String(whichMotor));}
  if(whichMotor==0 || whichMotor==99){
    digitalWrite(subMotorDirectionPin, LOW); // changed from low to be arbitrary
    analogWrite(subMotorSpeedPin, 0);
  }
  if(whichMotor==1 || whichMotor==99) {
    digitalWrite(supMotorDirectionPin, LOW); // changed from low to be arbitrary
    analogWrite(supMotorSpeedPin, 0);
  }
}

void WaterAndMotorsOff(){
  // Shut Waters Off
  Water(0,0);
  Water(1,0);
  // Shut Motors down
  Motor(0,0,0);
  Motor(1,0,0);
}


