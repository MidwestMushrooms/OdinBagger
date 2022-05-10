void ChangeModifiers() {
  if (subStatusMenuTestMatrixIndex > 0) {
    int moistureModPercent = ((subStatusMenuTestMatrixIndex - 1) % 4) + 1;
    int supModPercent = ((subStatusMenuTestMatrixIndex - moistureModPercent) / 4) + 1;
    if (subStatusMenuTestMatrixLastBagNumber == 25) { // 5x5
      int ModFac1 = (currentTotalBagsCounter - 1) % 5 - 2; // figures out supplement modification factor -2,-1,0,1,2
      int ModFac2 = ((currentTotalBagsCounter - ModFac1 + 2 ) / 5 ) - 3; // figures out water modification factor - 2, -1,0,1,2
      gridModifierSup = ModFac1 * supModPercent;
      gridModifierMoisture = ModFac2 * moistureModPercent;
    } else { // 3x3
      int ModFac1 = (currentTotalBagsCounter - 1) % 3 - 1; // figures out supplement modification factor -2,-1,0,1,2
      int ModFac2 = ((currentTotalBagsCounter - ModFac1 + 1 ) / 3 ) - 2; // figures out water modification factor - 2, -1,0,1,2
      gridModifierSup = ModFac1 * supModPercent;
      gridModifierMoisture = ModFac2 * moistureModPercent;
    }
  }
}

void DispenseMath() {
  ChangeModifiers(); // function to see if we need to modify the test grid method otherwise it is inert at zero values
  // dry weight of sub + dry weight of supp + residual moisture of sub residual moisture of sup + moisture = total target weight
  int effectiveSupplementSetValue =supplementSetValue;
  if(substrateType==2){ // special case
    effectiveSupplementSetValue=0; // special combination pellet ignore any supplement
  }
  int drySubPercentage = 100 - effectiveSupplementSetValue - gridModifierSup; // 42% sup value would be 58%
  int dryPercentageOfTotal = 100 - moistureSetValue - gridModifierMoisture; // 65% moisture would be 35%

  float dryWeightOfSub = (dryPercentageOfTotal / 100.00) * (drySubPercentage / 100.00) * weightSetValue; // .35 * .58 * 10  2.03
  float dryWeightOfSup = (dryPercentageOfTotal / 100.00) * ((effectiveSupplementSetValue - gridModifierSup) / 100.00) * weightSetValue; // .35 * .42 * 10  1.47
  float drySubRatio =   (100.00 - residualMoistureSub ) / 100.00; // residual of 37% it would be 63%
  float drySupRatio =   (100.00 - residualMoistureSup ) / 100.00; // residual of 8% it would be 92%

  weightOfSubstrateToDispense = dryWeightOfSub / drySubRatio;   //  2.03 / .63 = 3.2# since high water content
  weightOfSupplementToDispense = dryWeightOfSup / drySupRatio;  // 1.47 / .92 = 1.6#
  weightOfWaterToDispense = weightSetValue - weightOfSubstrateToDispense - weightOfSupplementToDispense;
  // inclusive of the residual moisture   10# - 3.2# - 1.6# = 5.2#
  realSub = weightOfSubstrateToDispense / 10.00; // make it decimal
  realSup = weightOfSupplementToDispense / 10.00; // make it decimal
  realWater = weightOfWaterToDispense / 10.00; // make it decimal
  if ( realSup > 2) {
    supHighSpeed = 255;
    supLowSpeed = 100;
  } else if (realSup > 1) {
    supHighSpeed = 180;
    supLowSpeed = 80;
  }
}





void MenuTimeoutCheck() {
  if(timer0 % 5000 == 1){ 
    menuTimeoutCounter++; // every 100th time
  }
  if(menuTimeoutCounter>160000){menuTimeoutCounter=0;} // int overrun
  if (menuTimeoutCounter > MenuTimeout) {
    // reset the menu back to normal last status
    GoodSound(0);
    menuTimeoutCounter = 0;
    menuColumn = 0;
    menuMode = 0;
  }
}

void CheckBagStatusSensor(int whichBagger){
  
  if(whichBagger==0){
    digitalWrite(bagTrigPinsL, LOW);
  } else {
    digitalWrite(bagTrigPinsR, LOW);
  }
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  if(whichBagger==0){
    digitalWrite(bagTrigPinsL, HIGH);
  } else {
    digitalWrite(bagTrigPinsR, HIGH);
  }
  delayMicroseconds(10);
  if(whichBagger==0){
    digitalWrite(bagTrigPinsL, LOW);
  } else {
    digitalWrite(bagTrigPinsR, LOW);
  }
  if(whichBagger==0){
    bagDurations[whichBagger] = int(pulseIn(bagEchoPinsL , HIGH) * 0.00567);
  } else {
    bagDurations[whichBagger] = int(pulseIn(bagEchoPinsR , HIGH) * 0.00567);  
  }
}
void CheckBagStatus(int whichBagger, bool highConfidence) {
  // v13 change - repeatedly check sensor in case it's flaky due to voltage change that affects sensor
  // highConfidence is added so that a single report of missing bag doesn't matter, we want several reports of missing bags in a row to abort operations
  // highConfidence is only for when we're looking for a bag falling not bag missing
  // v15 just detect the bypass mode and don't bother with them.
   if(useBagSensors==0){
     // don't use them
      bagStatus[whichBagger]=1;// just assume it is there.
   } else {
    // normal use bag sensors 
      for(int i=0;i<5;i++){ // loop through sensors until we find a positive bag present or negative bag present with low confidence
        CheckBagStatusSensor(whichBagger);  
        if(bagDurations[whichBagger] < bagDistanceThreshold && bagDurations[whichBagger] > 0){ // v13 added a zero exception to avoid sensor failing
          // not zero and less than the threshold we're good no need to re-test so break out of this loop
          bagStatus[whichBagger] = 1;
          break;
        } else if(highConfidence == false) { // v13 not a high confidence check so a single test result of missing bag is fine
          bagStatus[whichBagger] = 0; 
          break;
        } else {
          delay(20); // short delay before checking again up to limit of loop
          if(i==4){ // last lap of the loop so must be the case that it's missing
            bagStatus[whichBagger] = 0;
          }
        }
      }
   }
 }

long getDecimal(float val) {
  int intPart = int(val);
  long decPart = 100000 * (val - intPart);
  if (decPart > 0)return (decPart);       //return the decimal part of float number if it is available
  else if (decPart < 0)return ((-1) * decPart); //if negative, multiply by -1
  else if (decPart = 0)return (00);       //return 0 if decimal part of float number is not available
}



void VibrationRun(int vibrationDuration) {
//  if(serialActive == 4){Serial.println("function VibrationRun " + String(vibrationDuration));}
  if (vibrationMode == 1) {
    if (vibrationDuration == 1 || vibrationDuration == 0) {
//     if(serialActive == 4){Serial.println("function VibrationRun on or off " + String(vibrationDuration));}
      if (vibrationDuration == 1) {
        digitalWrite(vibrationPin, LOW); // turn vibrator on
        vibratorStatus=1;
        TimerReSet(4);
      } else {
        vibratorStatus=0;
        digitalWrite(vibrationPin, HIGH); // turn vibrator off
      }
    } else {
      
      if(vibratorStatus==0){ // only run it if it's not running already
//       if(serialActive == 4){Serial.println("function wasn't running run it " + String(vibratorStatus));}
        digitalWrite(vibrationPin, LOW); // turn vibrator on
        vibratorStatus=1;
        TimerReSet(4);
      } else {
//       if(serialActive == 4){Serial.println("vibration already running " + String(vibratorStatus));}
        
      }
    }
   SetSimpleMessage("SHAKE STOP         ",300);
  }
}
void CancelVibratorRun(){
  if(timer2 > vibrationDurationWhenNoProgress && vibratorStatus==1){ // on and elapsed time has passed
//    if(serialActive == 4){Serial.println("function CancelVibratorRun timer2=" + String(timer2) + ",vibratorStatus=" + String(vibratorStatus));}
    digitalWrite(vibrationPin, HIGH); // turn vibrator off
    delay(100);
    vibratorStatus=0; // set it to off then turn it off
  } else if(vibratorStatus==1) {
//    if(serialActive==3){Serial.println(" CV " + String(timers[4]) + "<" + String(vibrationDurationWhenNoProgress));}
  } else {
    //nothing vibrator is off
  }
}

void CheckForNoProgress(int whichBagger) {
  float tScaleWeight=scaleWeights[whichBagger];// get current weight
  float tScaleLastWeight=scaleLastWeights[whichBagger];// get prior weight from last run
  float threshold=float( tScaleLastWeight * 1.015);
  float threshold2=float(tScaleWeight - tScaleLastWeight); // weight gain
  if (tScaleWeight > threshold && threshold2 > 0.01 ) { // at least 0.01 pounds per cycle or 1.5% gain
//    if(serialActive == 3){Serial.println("CheckForNoProgress tScaleWeight=" + String(tScaleWeight) + ", tScaleLastWeight=" + String(tScaleLastWeight) + ", bagger=" + String(whichBagger) + " >" + String(threshold));}
    // good, growing
    scaleSameAsLastTimeCount[whichBagger] = 0;
    scaleLastWeights[whichBagger] = scaleWeights[whichBagger]; // update latest progress since it grew
  } else if(tScaleWeight < tScaleLastWeight ){     // weird went backwards, wobble wobble update last weight
    scaleLastWeights[whichBagger] = scaleWeights[whichBagger];
  } else {
  
//    if(serialActive == 3){Serial.print(" ch:" + String(whichBagger) + "-" + String(scaleSameAsLastTimeCount[whichBagger]));}
    scaleSameAsLastTimeCount[whichBagger]++;
    // increase counter to see how long before we need to shake
    if (scaleSameAsLastTimeCount[whichBagger] > scaleSameAsLastTimeCountThresholdToVibrate) {
      VibrationRun(vibrationDurationWhenNoProgress);
      scaleSameAsLastTimeCount[whichBagger] = 0;
      scaleLastWeights[whichBagger] = scaleWeights[whichBagger]; // update the latest progress since we ran vibrate
    }
  }
}

void WaterTimingLearningAlgorithm(int whichWater) {
//  if(serialActive == 2){Serial.println("function WaterTimingLearningAlgorithm " + String(whichWater) + " WaterTimeRan=" + String(waterTimeRan[whichWater]) + " against ideal of " + String(idealShortWaterTime));}
  //time how long the second phase of water runs for
  //if it’s > idealShortWaterTime * 1.15 then increment waterFullShortStop ratio by .01 points
  //if it’s < idealShortWaterTime * 0.85 then decrement waterFullShortStop ratio by .01 points
  if (waterTimeRan[whichWater] > idealShortWaterTime * 1.05 * 1000) { // turn
    waterFullShortStop[whichWater] = waterFullShortStop[whichWater] + .01;
//    if(serialActive == 2){Serial.println("WaterTimingLearningAlgorithm NEW UP waterFullShortStop=" + String(waterFullShortStop[whichWater]) + ",waterTimeRan[whichWater]=" + String(waterTimeRan[whichWater]) + ", " + String(idealShortWaterTime * 1.15) );}
    if (waterFullShortStop[whichWater] > 0.99) {
      updateMessages[whichWater]= "H20 ADJUST CEILING" + String(waterFullShortStop[whichWater]);
      waterFullShortStop[whichWater] = 0.99;
      SavePrefs();
    } else {
      updateMessages[whichWater]= "ADJUST H20 UP" + String(waterFullShortStop[whichWater]);
    }
  } else if (waterTimeRan[whichWater] < idealShortWaterTime * 0.95 * 1000) {
    waterFullShortStop[whichWater] = waterFullShortStop[whichWater] - .01;
//    if(serialActive == 2){Serial.println("function WaterTimingLearningAlgorithm NEW DOWN waterFullShortStop=" + String(waterFullShortStop[whichWater]) + ",waterTimeRan[whichWater]=" + String(waterTimeRan[whichWater]) + ", " + String(idealShortWaterTime * 1.15) );}
    if (waterFullShortStop[whichWater] < 0.80) {
      BadSound(whichWater);
      updateMessages[whichWater]= "H20 ADJUST FLOOR" + String(waterFullShortStop[whichWater]);
      waterFullShortStop[whichWater] = 0.80;
      SavePrefs();
    } else {
      updateMessages[whichWater]= "ADJUST H20 DOWN" + String(waterFullShortStop[whichWater]);
    }
  } else {
    // sweet spot nothing to tweak
  }
}

void SubTimingLearningAlgorithm(int whichSub) {
//  if(serialActive>=1){Serial.println("SubTimingLearningAlgorithm SubPhase2Part1Ratio[whichSub]= " + String(SubPhase2Part1Ratio[whichSub]) + " vs ideal=" + String(idealFinalSubPhaseRatio) + " subDensityFactor[whichSub]=" + String(subDensityFactor));}
  //time how long the second phase of sub runs for
  //if it’s > idealFinalSubPhaseRatio then increment subDensityFactor by .01 points
  //if it’s < idealFinalSubPhaseRatio then decrement subDensityFactor by .01 points
  if (SubPhase2Part1Ratio[whichSub] < idealFinalSubPhaseRatio) { //
    subDensityFactor[whichSub] = subDensityFactor[whichSub] + .01; // get it closer to 95%
    if (subDensityFactor[whichSub] > 1.00) {
      BadSound(whichSub);
      updateMessages[whichSub]="DENSITY CEILING " + String(subDensityFactor[whichSub] ); 
      subDensityFactor[whichSub] = 1.00;
    } else {
      updateMessages[whichSub]="TWEAK DENSITY UP" + String(subDensityFactor[whichSub] ); 
      SavePrefs();
    }
    motorThresholdPhase2[whichSub] = 0.96 * subDensityFactor[whichSub] ;
//    if(serialActive == 2){Serial.println("SubTimingLearningAlgorithm NEW UP subDensityFactor[whichSub]=" + String(subDensityFactor) + ",motorThresholdPhase2[whichSub]=" + String() );}

  } else if (SubPhase2Part1Ratio[whichSub] > idealFinalSubPhaseRatio) {
    subDensityFactor[whichSub]  = subDensityFactor[whichSub]  - .01;
    if (subDensityFactor[whichSub]  < 0.60) {
      BadSound(whichSub);
      updateMessages[whichSub]="DENSITY FLOOR " + String(subDensityFactor[whichSub] );  
      subDensityFactor[whichSub]  = 0.60;
      SavePrefs();
    } else {
      updateMessages[whichSub]="TWEAK DENSITY DOWN " + String(subDensityFactor[whichSub] );
    }
    motorThresholdPhase2[whichSub] = 0.96 * subDensityFactor[whichSub] ;
//    if(serialActive == 2){Serial.println("SubTimingLearningAlgorithm NEW DOWN subDensityFactor[whichSub]=" + String(subDensityFactor[whichSub]) + ",motorThresholdPhase2[whichSub]=" + String(motorThresholdPhase2[whichSub]) );}
  } else {
    // sweet spot nothing to tweak
//    if(serialActive == 2){Serial.println("Sub -- no change subDensityFactor[whichSub]=" + String(subDensityFactor[whichSub])     + ",motorThresholdPhase2[whichSub]=" + String(motorThresholdPhase2[whichSub]) );}
  }

}

void SupTimingLearningAlgorithm(int whichSup) {
//  if(serialActive == 1){Serial.println("function SupTimingLearningAlgorithm SupPart1Ratio[whichSup]= " + String(SupPart1Ratio[whichSup]) + " vs ideal=" + idealFinalSupPhaseRatio);}
  //time how long the second phase of sup runs for
  //if it’s > idealFinalSupPhaseRatio then increment supDensityFactor by .01 points
  //if it’s < idealFinalSupPhaseRatio then decrement supDensityFactor by .01 points
  if (SupPart1Ratio[whichSup] < idealFinalSupPhaseRatio) { //
    supDensityFactor[whichSup] = supDensityFactor[whichSup] + .01; // stopped short of the ideal ratio, creep up
    if (supDensityFactor[whichSup] > 1.00) {
      BadSound(whichSup);
      SetSimpleMessage("BAD SpDenF  " + String(supDensityFactor[whichSup]),10);
      supDensityFactor[whichSup] = 1.00;
    } else {
      SetSimpleMessage("TweakSpDenF++ " + String(supDensityFactor[whichSup]),10);
      SavePrefs();
    }
    motorThresholdPhase1Sup[whichSup] = 0.91 * supDensityFactor[whichSup]; // % of the target weight where it slows down
//    if(serialActive == 2){Serial.println("SupTimingLearningAlgorithm NEW UP SupPart1Ratio=" + String(SupPart1Ratio[whichSup]) + ",motorThresholdPhase1Sup[whichSup]=" + String(motorThresholdPhase1Sup[whichSup])  );}

  } else if (SupPart1Ratio[whichSup] > idealFinalSupPhaseRatio) {
    supDensityFactor[whichSup] = supDensityFactor[whichSup] - .01;
    if((SupPart1Ratio[whichSup] - idealFinalSupPhaseRatio) > .15){
//      if(serialActive == 2){Serial.println("SupTimingLearningAlgorithm EXTREME DOWN supDensityFactor[whichSup]=" + String(SupPart1Ratio[whichSup]) + ",motorThresholdPhase1Sup[whichSup]=" + String(motorThresholdPhase1Sup[whichSup])  );}
      supDensityFactor[whichSup] = supDensityFactor[whichSup] - .10;
     // extreme measures needed
    }
    if (supDensityFactor[whichSup] < 0.50) {
      BadSound(whichSup);
      SetSimpleMessage("BAD SpDenF " + String(whichSup) + " " +String(supDensityFactor[whichSup]),10);
      supDensityFactor[whichSup] = 0.50;
      SavePrefs();
    } else {
      SetSimpleMessage("TweakSpDenF-- " + String(supDensityFactor[whichSup]),10);
    }
    motorThresholdPhase1Sup[whichSup] = 0.91 * supDensityFactor[whichSup]; // % of the target weight where it slows down
//    if(serialActive == 2){Serial.println("SupTimingLearningAlgorithm NEW DOWN supDensityFactor[whichSup]" + String(supDensityFactor[whichSup]) + ",motorThresholdPhase1Sup[whichSup]=" + String(motorThresholdPhase1Sup[whichSup])  );}
  } else {
    // sweet spot nothing to tweak
//    if(serialActive == 2){Serial.println("SupTimingLearningAlgorithm no change SupPart1Ratio=" + String(SupPart1Ratio[whichSup]) + ",motorThresholdPhase1Sup[whichSup]=" + String(motorThresholdPhase1Sup[whichSup])  );}
  }

}

void TimerReSet(int whichTimer) {
//  if(serialActive==1){Serial.println("function TimerReSet " + String(whichTimer));}
  switch (whichTimer) {
    case 0: {
        timer0 = 0; break;
      }
    case 1: {
        timer1 = 0; break;
      }
    case 4: {
        timer2 = 0; break;
    }
    default: {
        break;
      }
  }
  TimersUpdate();
}
void TimersUpdate() {
  timers[0] = timer0;
  timers[1] = timer1;
  timers[2] = timer2;
}


void TestWater(int whichWater) {
//  if(serialActive==1){Serial.println("function TestWater " + String(whichWater) + "timers[0]=" + timers[0] + ",timeout=" + timeout);}
  if (timers[0] > timeout) {
    Water(whichWater, 0); currentlyRunning = 0; timers[0] = 0; GoodSound(whichWater); // shut water off, test over
    SetSimpleMessage("WATER TEST #" + String(whichWater) + " OFF        ",10);
    currentlyRunning=0;// revert to home menu
  } else {
    switch(whichWater){
      case 0: { SetSimpleMessage("< WATER ON",300);break;}
      case 1: { SetSimpleMessage("> WATER ON",300);break;}
      case 2: { SetSimpleMessage( "3 WATER ON",300);break;}
      case 3: { SetSimpleMessage( "4 WATER ON",300);break;}
    }
  }
}

void TestAllWaters(){
//  if(serialActive==1){Serial.println("function TestAllWaters");}
  if (timers[0] > timeout) {
    timeout = (timers[0] + 2000);//reset timer
    SetSimpleMessage( "Testing Water " + String(currentlyRunningWaterProcessId) ,300);
    tone(buzzer, noteE, 50);
    switch (currentlyRunningWaterProcessId){
      case 0:{;Water(0,0);Water(1,0);Water(2,0);SetSimpleMessage( "TestH20 OFF",300);break;}// closed
      case 1:{;Water(0,1);Water(1,0);Water(2,1);SetSimpleMessage( "TestH20 <FAST",300);break;}// fast
      case 2:{;Water(0,1);Water(1,0);Water(2,0);SetSimpleMessage( "TestH20 <SLOW",300);break;}// slow
      case 3:{;Water(0,0);Water(1,0);Water(2,0);SetSimpleMessage( "TestH20 OFF",300);break;}// closed
      case 4:{;Water(0,0);Water(1,1);Water(2,1);SetSimpleMessage( "TestH20 >FAST",300);break;}// fast
      case 5:{;Water(0,0);Water(1,1);Water(2,0);SetSimpleMessage( "TestH20 >SLOW",300);break;}// slow
      case 6:{;Water(0,0);Water(1,0);Water(2,0);currentlyRunning=0;SetSimpleMessage( "TestH20 OFF",300);break;}// closed
      default:{currentlyRunning=0;currentlyRunningWaterProcessId=0;menuColumn=0;}
    }
    currentlyRunningWaterProcessId++;
  } 
}



void EmptySub() {
  CheckBagStatus(0,0);
//  if(serialActive==1){Serial.println("function EmptySub");}
  if (processPaused == 0) {
    if (timers[0] > timeout) { // 3 second warning is up
       // open gate just in case it isn't open but only after timer to keep tidy
      if (bagStatus[0] == 1) { // makes shure a bucket or something is in place
        Motor(0, 250 , 0);// high speed empty substrate
        digitalWrite(vibrationPin, LOW); // turn vibrator on

        SetSimpleMessage( "< EMPTYING SUB",300);
      } else {
        digitalWrite(vibrationPin, HIGH); // turn vibrator on
        StopMotors(99);
        BadSound(0); // bag missing
        SetSimpleMessage( "< MISSING CATCH  ",300);
      }
    } else {
      //time tick town tone warning
      float ttime = timeout / timers[0];
      tone(buzzer, noteE - int(ttime * 50), 15);
    }
  } else {
    // process is paused
    StopMotors(99);
    if (bagStatus[0] == 1) {
      SetSimpleMessage( "<PS:ADD CATCH  ",300);
    } else {
      SetSimpleMessage( "<PS:RB 2 CONT  ",300);
    }
  }
}

void EmptySup() {
  CheckBagStatus(1,0);
//  if(serialActive==1){Serial.println("function EmptySup");}
  if (processPaused == 0) {
    if (timers[0] > timeout) { // 3 second warning is up
       // open gate just in case it isn't open but only after timer to keep tidy
      if (bagStatus[1] == 1) { // makes shure a bucket or something is in place
        Motor(1, 250 , 1);// high speed empty substrate
        SetSimpleMessage( "> EMPTYING SUPPLEMENT",300);
      } else {
        StopMotors(99);
        BadSound(1); // bag missing
        SetSimpleMessage( "> MISSING CATCH",300);
      }
    } else {
      //time tick town tone warning
      float ttime = timeout / timers[0];
      tone(buzzer, noteA - int(ttime * 50), 15);
    }
  } else {
    // process is paused
    StopMotors(99);
    if (bagStatus[1] == 1) {
      SetSimpleMessage(">PAUSED: ADD CATCH",300);
    } else {
      SetSimpleMessage(">PAUSED: CLK 2 CONT",300);
    }
  }
}

void SetSimpleMessage(String msg, int d){
  statusMessage[0] = msg;
  statusMessage[1] = " ";
  statusMessage[2] = " ";
  DrawDisplayAsAlertMessage(d);
}
void TestWater() {
//  if(serialActive==1){Serial.println("function TestWater");}
  if (timers[0] > timeout) {
    timeout = (timers[0] + 2000);//reset timer
    SetSimpleMessage("Testing Water " + String(currentlyRunningWaterProcessId), 300);
    tone(buzzer, noteE, 50);
    switch (currentlyRunningWaterProcessId) {
      case 0: {
          ;  // closed
          
          Water(0, 0);
          Water(1, 0);
          Water(2, 0);
          SetSimpleMessage( "Test H20 OFF",300);
          break;
        }
      case 1: {
          ;  // fast
          
          Water(0, 1);
          Water(1, 0);
          Water(2, 1);
          SetSimpleMessage( "Test H20 < FAST",300);
          break;
        }
      case 2: {
          ;  // slow
          
          Water(0, 1);
          Water(1, 0);
          Water(2, 0);
          SetSimpleMessage( "Test H20 < SLOW",300);
          DrawDisplayAsAlertMessage(300);
          break;
        }
      case 3: {
          ;  // closed
          
          Water(0, 0);
          Water(1, 0);
          Water(2, 0);
          SetSimpleMessage( "Test H20 OFF",300);
          DrawDisplayAsAlertMessage(300);
          break;
        }
      case 4: {
          ;  // fast
          
          Water(0, 0);
          Water(1, 1);
          Water(2, 1);
          SetSimpleMessage( "Test H20 > FAST",300);
          DrawDisplayAsAlertMessage(300);
          break;
        }
      case 5: {
          ;  // slow
          
          Water(0, 0);
          Water(1, 1);
          Water(2, 0);
          SetSimpleMessage( "Test H20 > SLOW",300);
          DrawDisplayAsAlertMessage(300);
          break;
        }
      case 6: {
          ;  // closed
          
          Water(0, 0);
          Water(1, 0);
          Water(2, 0);
          currentlyRunning = 0;
          SetSimpleMessage( "Test H20 OFF",300);
          
          break;
        }
      default: {
          currentlyRunning = 0;
          currentlyRunningWaterProcessId = 0;
        }
    }
    currentlyRunningWaterProcessId++;
  }
}

void TestBagSensors() {
  //v15
  if(useBagSensors==0){
      statusMessage[0]= "TESTING BAG SENSORS";
      DrawDisplayAsAlertMessage(1000);
      BadSound(0);
      statusMessage[0]= "YOU DISABLED SENSORS";
      DrawDisplayAsAlertMessage(3000);
      
  } else {
    // normal test them
    CheckBagStatus(0,0);
    delay(50);
    CheckBagStatus(1,0);
    delay(50);
    statusMessage[0]= "TESTING BAG SENSORS";
    if(bagStatus[0]==1){
      statusMessage[1]= "<LEFT BAG YES " + String(bagDurations[0]) + " in." ;
    } else {
      statusMessage[1]= "<LEFT BAG NO  " + String(bagDurations[0]) + " in." ;    
    }
    if(bagStatus[1]==1){
      statusMessage[2]= "> RIGHT BAG YES " + String(bagDurations[1]) + " in." ;
    } else {
      statusMessage[2]= "< RIGHT BAG NO " + String(bagDurations[1]) + " in." ;    
    }
    DrawDisplayAsAlertMessage(1000);
  }
}



void StartFilling(boolean myWhichSide) {
//  if(serialActive>1){Serial.println("function StartFilling " + String(myWhichSide));}
  BlinkAllLightsWarning();
  CheckBagStatus(0,0);
  CheckBagStatus(1,0);
  VibrationRun(1); /// if it's off, on
  SetSimpleMessage( "Get Ready",100);
  DrawDisplayAsAlertMessage(10);
  whichSideFillingNow = myWhichSide;
  scaleSameAsLastTimeCount[0]=0;
  scaleSameAsLastTimeCount[1]=0;
  VibrationRun(0); /// if it's on ,off
  GoodSound(myWhichSide);
  // stuff for actually starting it
  baggerStepNumber[0]=15;// double check a bag is present
  baggerStepNumber[1]=15;// double check a bag is present
  currentlyRunning=1;
  exitMenus();
  processPaused=0;
}


void StartAProcessAndExitMenu( byte itm, String msg, String msg2, String msg3){
  tone(buzzer, noteG * (1 + whichMotor), 100);
  menuMode=0;
  menuColumn=0; 
  SettingModeMenuIndex=itm;
  currentlyRunning=itm;
  processPaused = 0;
  statusMessage[0]=msg;
  statusMessage[1]=msg2;
  statusMessage[2]=msg3;
  motorTestModeIndex = 0; // reset the motor test mode
  runScreenChanged=1;
  DrawDisplayAsAlertMessage(1000);
  timers[0]=0;TimerReSet(0);
  timeout = 3000; // rest the water test timer to 2 seconds
}

void StartEmptySub(){
  CheckBagStatus(0,0);
  StartAProcessAndExitMenu(19,">EMPTY:ANY BTN STOP  "," "," ");
}

void StartEmptySup(){
  CheckBagStatus(1,0);
  StartAProcessAndExitMenu(18,"<EMPTY:ANY BTN STOP  "," "," ");
}


void StartTestBags(){
  StartAProcessAndExitMenu(20,"TEST BAG SENSORS "," "," ");
}

void StartTestAllWaters(){
  StartAProcessAndExitMenu(22,"TEST WATER VALVES "," "," ");
}

void StartTestLeds(){
  testLEDindex=0; // v19fix
  StartAProcessAndExitMenu(17,"TEST LED LIGHTS "," "," ");
  
}

void StartTestLeftWater(){
  Water(0, 1); // turn the water on
  StartAProcessAndExitMenu(14,"TEST LEFT WATER "," "," ");

}
void StartTestRightWater(){
  Water(1, 1); // turn the water on
  StartAProcessAndExitMenu(15, "TEST RIGHT WATER "," "," ");
}


void StartCalibration(){
  StartAProcessAndExitMenu(6,"CALIBRATION "," "," ");
}

void EmergencyStop(){
  // Shut Waters Off
  Water(0,0);
  Water(1,0);
  // Shut Motors down
  Motor(0,0,0);
  Motor(1,0,0);
  BlinkAllLightsWarning();
  // put system into process paused status.
  rotaryCounter=0;//prevents double clicks
  processPaused=1;// pause everything
}

void Water(int whichWater, boolean swtch) {
//  if(serialActive==1){Serial.println("function Water " + String(whichWater) + ",swtch=" + String(swtch));}
  if (whichWater == 0) {
    if (swtch == 1) {
      // turn it on
      digitalWrite(waterPinsL, LOW);
      delayMicroseconds(10);
      waterOpen[whichWater] = 1;
    } else {
      digitalWrite(waterPinsL, HIGH);
      delayMicroseconds(10);
      waterOpen[whichWater] = 0;
    }
  } else if (whichWater == 1) {
    if (swtch == 1) {
      // turn it on
      digitalWrite(waterPinsR, LOW);
      waterOpen[whichWater] = 1;
    } else {

      digitalWrite(waterPinsR, HIGH);
      waterOpen[whichWater] = 0;
    }
  } else if (whichWater == 2) {
    if (swtch == 1) {
      // turn it on
      digitalWrite(fullWaterPin, LOW);
      fullWaterOpen = 1;
    } else {
      // turn off full water
      digitalWrite(fullWaterPin, HIGH);
      fullWaterOpen = 0;
    }
  }
}




void PukeDebugData(int whichBagger){
  float tempAddedWater = float(scaleWeights[whichBagger] - scaleDryGoodsWeights[whichBagger]);
  float tempAdjustedWaterTarget = float (scaleDryGoodsWeights[whichBagger] + (scaleOverfills[whichBagger] * realWater * waterShortStop[whichBagger] ) );
  float tempWetSubTotal = float(scaleDryGoodsWeights[whichBagger] - scaleFinalSupWeight[whichBagger]);
  float tempDrySubTotal = float(( tempWetSubTotal * float(100 - residualMoistureSub)  ) / 100.00);
  float tempWetSupTotal = float(scaleFinalSupWeight[whichBagger]);
  float tempDrySupTotal = float(( tempWetSupTotal * float(100 - residualMoistureSup)  ) / 100.00);;
  float tempSubAccuracy = float((100 * tempWetSubTotal) / realSub);
  float tempSupAccuracy = float((100 * tempWetSupTotal) / realSup);
  float tempActualDryMixRatio = float(100.00 * (tempDrySupTotal / (tempDrySupTotal + tempDrySubTotal)));
  float tempActualSubResidualWater = float(tempWetSubTotal * (residualMoistureSub / 100.00));
  float tempActualSupResidualWater = float(tempWetSupTotal * (residualMoistureSup / 100.00));
  float tempActualMoistureLevel = float(tempActualSubResidualWater + tempActualSupResidualWater + tempAddedWater);
  float tempWaterAccuracy = float(1.000 * tempAddedWater / tempAdjustedWaterTarget);
  float tempWeightAccuracy = float(scaleWeights[whichBagger] / (.10 * weightSetValue));
  Serial.println(" ");
  Serial.println(String(whichBagger) +
             "__Summary______________________________");
  Serial.println(String(whichBagger) + " SUB WetTotal/DrySubTotal/Target/Ac: " + String(tempWetSubTotal) + "/" + String(tempDrySubTotal) + "/" + String(realSub) + "/" + String(tempSubAccuracy) + "%");
  Serial.println(String(whichBagger) + " SUP WetTotal/DrySupTtoal/Target/Ac: " + String(scaleFinalSupWeight[whichBagger]) + "/" + String(tempDrySupTotal) +  "/" + String(realSup) + "/" + String(tempSupAccuracy) + "%");
  Serial.println(String(whichBagger) + " Mix%:" + String(tempActualDryMixRatio) +  "% achieved vs " + String(supplementSetValue) + "% target");
  Serial.println(String(whichBagger) + " H20:(Added/Orig/Adj/Ac)" + String(tempAddedWater) + "/" + String(realWater) + "/" + String(tempAdjustedWaterTarget) + "/" + String(tempWaterAccuracy));
  Serial.println(String(whichBagger) + " weight=(Ac/Tg/Ac)" + String(scaleWeights[whichBagger]) + "/" + String(weightSetValue / 10.00) + "/" + String(tempWeightAccuracy));
  Serial.println(String(whichBagger) + " moisture:  SubResidual of " + String(tempActualSubResidualWater) + " + " + String(tempActualSupResidualWater) + " SupResidual +  " + String(tempAddedWater) + " added water = " + String(tempActualMoistureLevel) + " for final moisture of " + String(100 * ( tempActualMoistureLevel / scaleWeights[whichBagger])) + "%");
  Serial.println(" ");

}

//v14
boolean TestFootPedal(int whichFootPedal){
  if(whichFootPedal==0){
    // left
    int val=analogRead(FOOTPEDALLEFT);
    if(val < 500 ){// ground state 
      return 0;
    } else { // high state
      return 1;
    }
  } else {
    //right
    int val=analogRead(FOOTPEDALRIGHT);
    if(val < 500 ){// ground state 
      return 0;
    } else { // high state
      return 1;
    }
  }
}

