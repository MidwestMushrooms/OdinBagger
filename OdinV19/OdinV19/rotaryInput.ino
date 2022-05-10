void checkRotaryInput() {
  // Is someone pressing the rotary switch?
  if (!digitalRead(PinSW)) {
    setLight(0, "green", 1);
    setLight(1, "green", 1);

    if ( rotaryCounter > 5) { // BUTTON PUSHED for at least 5 cycles

      rotaryChangeCounter = 0; // reset the other counter
      //      if (serialActive == 6) {
      //        Serial.println(F("counter > 5"));
      //      }
      // if process is paused resume
      if ( menuMode == 1 && MenuSelectedItem == 2 && menuColumn == 1 ) { //v17 stop running important to check this first
        // Shut Waters Off
        Water(0, 0);
        Water(1, 0);
        // Shut Motors down
        Motor(0, 0, 0);
        Motor(1, 0, 0);
        BlinkAllLightsWarning();
        currentlyRunning = 0; // reset it to not running
        //v13fix reset the step number so that stopping stops everything from previous run
        baggerStepNumber[0] = 0;
        baggerStepNumber[1] = 0;
        menuMode = 0;
        menuScreenChanged = 0;
        processPaused = 0; // Use emergency Stop so we are not pausing and reset the pause if it was paused
        exitMenus();
        BlinkAllLightsWarning();
        drawRunScreen();
      } else if (processPaused == 1 && menuMode == 0) {
        menuMode = 0; // turn off menu mode
        menuColumn = 1; subMenuNumber = 0; selectedMenuLength = menuLevel1Length; MenuSelectedItem = 0;  // revert to level zero
        processPaused = 0; // actually un-pause because the button was pushed after pausing and a delay
        // presuming everything is ok if it spilled, let's start with a new max weight of what's on the scale now.
        rotaryCounter = 0; // v13 reset rotary counter so that it won't execute this next time de-bounce
        CheckScaleWeight(0, 0);
        CheckScaleWeight(1, 0);
        maxScaleWeights[0] = scaleWeights[0];
        maxScaleWeights[1] = scaleWeights[1];
        drawRunScreen();
      } else if (currentlyRunning == 1 && menuMode == 0) {
        WaterAndMotorsOff();
        BlinkAllLightsWarning();
        EmergencyStop(); // if it was running and button pushed stop everything and pause
      } else if (currentlyRunning > 11 && currentlyRunning < 23 && menuMode == 0) {
        // abort any running process because you clicked something
        WaterAndMotorsOff();
        currentlyRunning = 0;
        processPaused = 0;
      } else {
        if (menuMode == 0) {  // NOT IN THE MENUS YET
          menuMode = 1; menuColumn = 1; subMenuNumber = 0; selectedMenuLength = menuLevel1Length; MenuSelectedItem = 0; setLight(0, "green", 1);
          menuScreenChanged = 1;
        } else if (menuColumn == 1) {
          if (MenuSelectedItem == 0) {
            // return / exit pressed
            if (currentlyRunning == 1) { // resume
              processPaused = 0;
              BlinkAllLightsWarning();
            }
            exitMenus();
          } else if (MenuSelectedItem == 1) {

            if (currentlyRunning == 0) { // if it's off start it
              StartFilling(0);
              // right or left side only stuff
            } else { // they clicked run so resume
              processPaused = 0;
            }
            tone(buzzer, noteA * 4, 500);
          } else if (MenuSelectedItem == 2) {
            // STOP RUNNING
            // Shut Waters Off
            Water(0, 0);
            Water(1, 0);
            // Shut Motors down
            Motor(0, 0, 0);
            Motor(1, 0, 0);
            BlinkAllLightsWarning();
            currentlyRunning = 0; // reset it to not running
            //v13fix reset the step number so that stopping stops everything from previous run
            baggerStepNumber[0] = 0;
            baggerStepNumber[1] = 0;
            menuMode = 0;
            menuScreenChanged = 0;
            processPaused = 0; // Use emergency Stop so we are not pausing and reset the pause if it was paused
            exitMenus();
            BlinkAllLightsWarning();
            drawRunScreen();
          } else {
            // selecting a sub-menu
            menuColumn = 2;
            menuScreenChanged = 1;
            subMenuNumber = MenuSelectedItem - 1; // - 1 is the exit return link
            MenuSelectedItem = 0; // reset to the zeroth item in the list
          }
        } else if (menuColumn == 2) {

          if (MenuSelectedItem == 0) {
            // return / exit pressed

            menuColumn = 1; subMenuNumber = 0; selectedMenuLength = menuLevel1Length; MenuSelectedItem = 0; menuScreenChanged = 1; // back up a level
          } else {
            // SELECTING ITEM IN A SUBMENU
            setLight(0, "green", 1); menuScreenChanged = 1;
            ///////////////////////////////////////////
            switch (subMenuNumber) {
              case 0: {
                  break;
                }
              case 2: {// setup;
                  switch (MenuSelectedItem) {
                    case 1: { // "Calib scales"
                        menuColumn = 3;
                        break;
                      }
                    case 2: { // "Reset scales"
                        ResetScales();
                        break;
                      }
                    case 3: { // "Empty Sub <"
                        StartEmptySub();
                        break;
                      }
                    case 4: { // "Empty Sup >"
                        StartEmptySup();
                        break;
                      }
                    case 5: { // "Grid Test 5x5"
                        break;
                      }
                    case 6: { // "Grid Test 3x3"
                        break;
                      }
                  }
                  break;
                }
              case 3: { //recipe
                  switch (MenuSelectedItem) {
                    case 1: case 2: case 3: case 4: case 5: case 6: case 7: { //Moisture%
                        menuColumn = 3; break;
                      }
                  }
                  break;
                }
              case 4: { // preferences
                  switch (MenuSelectedItem) {
                    case 1: { //Set SBDenF:
                        menuColumn = 3; break;
                      }
                    case 2: { //Set SPDenF:
                        menuColumn = 3; break;
                      }
                    case 3: { //Set SubHi:
                        menuColumn = 3; break;
                      }
                    case 4: { //Set SubLo:
                        menuColumn = 3; break;
                      }
                    case 5: { //Set SupHi:
                        menuColumn = 3; break;
                      }
                    case 6: { //Set SupLo:
                        menuColumn = 3; break;
                      }
                    case 7: { //Ignore bag sensors:
                        menuColumn = 3; break;
                      }
                    case 8: { //No Water Mode:
                        menuColumn = 3; break;
                      }
                  }
                  break;
                }
              case 5: case 6: case 7: { // Test and anything after Test(nothing)
                  switch (MenuSelectedItem) {
                    case 1: { //Test Sub Motr
                        StartTestMotor0();
                        break;
                      }
                    case 2: { //Test Sup Motr
                        StartTestMotor1();
                        break;
                      }
                    case 3: { //Test Left Watr
                        StartTestLeftWater();
                        break;
                      }
                    case 4: { //Test Right Watr
                        StartTestRightWater();
                        break;
                      }
                    case 5: { //Test Scales
                        StartTestScales();
                        break;
                      }
                    case 6: { //Test Bags
                        StartTestBags();
                        break;
                      }
                    case 7: { //Test Water Valves
                        StartTestAllWaters();
                        break;
                      }
                    case 8: { //Test Water Valves
                        StartTestLeds();
                        break;
                      }
                  }
                  break;
                }// test

            }
          }
        } else if (menuColumn == 3) {
          if (subMenuNumber == 2) {
            // special case for calibration
            StartCalibration();
          } else {
            menuColumn = 2; delay(200); menuScreenChanged = 1;
          }
        }
        menuTimeoutCounter = 0; //reset menu timer
        delay(200);
      }
    } else {
      rotaryCounter++;
      if (rotaryCounter > 1000) {
        rotaryCounter = 6; //keep it from flipping over
      }
    }
  }
}

void RotaryInput () {
  if (RotaryBusy == 0) {
    RotaryBusy = 1;
    //  static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();

    int MSB = digitalRead(PinA); //MSB = most significant bit
    int LSB = digitalRead(PinB); //LSB = least significant bit
    if (interruptTime - lastInterruptTime > 5) {
      if (rotaryChangeCounter > rotaryChangeThreshold || menuMode == 1) {
        if (currentlyRunning == 1 && processPaused == 0) { // stop since we're running right now
          EmergencyStop();
          WaterAndMotorsOff();
          BlinkAllLightsWarning();
          menuScreenChanged = 1;
        } else if (currentlyRunning == 1 && processPaused == 1) {
          // we were running but still twisting the knob
          menuScreenChanged = 1;
          if (menuMode == 0) { // we paused but didn't get into the menu yet so let's do that
            menuScreenChanged = 1; // global redraw requested
            menuMode = 1; menuColumn = 1; subMenuNumber = 0; selectedMenuLength = menuLevel1Length; MenuSelectedItem = 0;
            setLight(0, "green", 1);
          } else { // we're in the menu and paused just redraw menu screen
            // done above
          }
        } else if (menuMode == 0) { // not in menu mode but not running
          menuScreenChanged = 1; // global redraw requested
          menuMode = 1; menuColumn = 1; subMenuNumber = 0; selectedMenuLength = menuLevel1Length; MenuSelectedItem = 0;
          setLight(0, "green", 1);
        } else if (currentlyRunning > 11 ) { // something running in test mode, twisting knob should abort back to home menu
          menuMode = 1; menuColumn = 1; currentlyRunning = 0; subMenuNumber = 0; selectedMenuLength = menuLevel1Length; MenuSelectedItem = 0;
        } else {
          menuScreenChanged = 1; // global redraw requested
        }
        int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
        int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) adjustCurrentMenu(-1);
        if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) adjustCurrentMenu(1);
        lastEncoded = encoded; //store this value for next time
        rotaryChangeCounter = 0;
      } else {
        rotaryChangeCounter++; // pick it up on the next pass
      }
    }
    // Keep track of when we were here last (no more than every 5ms)
    lastInterruptTime = interruptTime;
    RotaryBusy = 0; //release lock
    menuTimeoutCounter = 0; //reset menu timer
  }
}
