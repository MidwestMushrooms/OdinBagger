void DoYourJob() {
//  if(serialActive==4 && timers[0] % 100 == 0){
//      Serial.println("processPaused=" + String(processPaused) + ",currentlyRunning=" + String(currentlyRunning) ); };
  // run through each 1-n baggers independently and share resources with a priority deference to the lowest numbered bagger
  // each step that requires delays should use the independent timers and separate steps instead of delay which could impact other busy baggers
  if(processPaused==1){
    drawRunScreen();
  } else {
    if (currentlyRunning == 1) {
      if(timers[0] % 9 == 1){ // not every cycle.
        drawRunScreen();
      }
      RunIt(0); // run all the steps of normal bagging operations
      RunIt(1); // run all the steps of normal bagging operations
      // --- DONE NORMAL RUNNING CODE , CHECK OTHER FUNCTIONS -------------------------------------------------------------------------------//
    } else if (currentlyRunning == 6) { // calibration
      ScaleCalibration();
    } else if (currentlyRunning == 12 && processPaused == 0) { // testing Left Motor
      TestSubMotor();
    } else if (currentlyRunning == 13  && processPaused == 0) { // testing right Motor
      TestSupMotor();
    } else if (currentlyRunning == 14 && processPaused == 0) { // testing left water if water is off and timer is less than expire, turn it on
      TestWater(0);
    } else if (currentlyRunning == 15 && processPaused == 0) { // testing right water if water is off and timer is less than expire, turn it on
      TestWater(1);
    } else if (currentlyRunning == 16) { // testing scales
      TestScales();
    } else if (currentlyRunning == 17){
      testLEDs();
    } else if (currentlyRunning == 18) { // emptying substrate
      EmptySup();
    } else if (currentlyRunning == 19) { // emptying substrate
      EmptySub();
    } else if (currentlyRunning == 22  && processPaused == 0) { // testing all water gates
      TestAllWaters();
    } else if (currentlyRunning == 20 && processPaused == 0) { // testing bag sensors
      TestBagSensors();
    } else {
      // not running
      DispenseMath(); // calculate the math on what we have to displense
      drawRunScreen();
    }
    if(vibrationMode==1){ // check the vibrator only if we're in vibration mode
      CancelVibratorRun();// timeout for cancelling vibration
    }
    if (currentlyRunning > 1 ){
      // one of the things above
      DrawDisplayAsAlertMessage(200);
    }
  }
 checkRotaryInput();  // checks for abort 
}
