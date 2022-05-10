
void RunIt(int whichBagger) {
//  if (serialActive == 5 ) {
//    Serial.println("function RunIt bagger#" + String(whichBagger) + ",step=" + String(baggerStepNumber[whichBagger]) + ",bagstatus=" + bagStatus[whichBagger]);
//  };

  int subMotor = 0; // default is 2 motors for 2 baggers
  int supMotor = 1; //
  int supFlowDirection = 0;
  int subFlowDirection = 0;
  int fastValveNumber = 2; //
  switch (whichBagger) {
    case 0: {
        supFlowDirection = 0;
        subFlowDirection = 0;
        break;
      }
    case 1: {
        supFlowDirection = 1;
        subFlowDirection = 1;
        break;
      }
  }
  if ( substrateType == 2 ) {
    //    if(serialActive==5 && timers[0] % 10 == 0){
    //      Serial.println("function RunIt bagger#" + String(whichBagger) + " special substrate type");}


    // special case of motor
    subMotor = whichBagger;

    if (subMotor == 1) {
      supMotor = 0;
      subFlowDirection = 1;
    }
    if (subMotor == 0) {
      supMotor = 1;
      subFlowDirection = 0;
    }
  }


  if (motorsAreASharedResource != 1) {
    // each has it's own motor TBD
    // flow directions will need altering
    // fast valves may be for each
  }
  TimersUpdate(); // update all the timer arrays
  switch (baggerStepNumber[whichBagger]) {
    case 0: {  // check for bag present
        updateMessages[whichBagger]="";
        baggerStepProgressPercentage[whichBagger] = 0;
        CheckBagStatus(whichBagger,1);// check for next code lap
        if (bagStatus[whichBagger] == 1) { // bag is in place
          setLight(whichBagger, "green", 1);
          setLight(whichBagger, "red", 0);          
          maxScaleWeights[whichBagger]= 0; // reset the scales to watch for growth
          baggerStepProgressPercentage[whichBagger] = 0;
          TimerReSet(whichBagger); baggerStepNumber[whichBagger]++;
        } else {
          setLight(whichBagger, "red", 1);
          setLight(whichBagger, "green", 1);
          if (serialActive == 5 ) {
            Serial.println("waiting for bag on bagger#" + String(whichBagger) );
          };
        }
        break;
      }
    //--------
    case 1: { // Tare scale
        baggerStepProgressPercentage[whichBagger] = 0;
        TareScales(whichBagger);
        bagDropConfidenceCounter=0;// reset the bag drop detector
        scaleTapped[whichBagger] = 0; // resets for this run
        if (substrateType == 1 && residualMoistureSub > 22) { //sawdust and high moisture, we'll need to shake each time
          VibrationRun(1);
        }
        maxScaleWeights[whichBagger]= 0; // reset the scales to watch for growth
        scaleLastWeights[whichBagger] = 0;
        baggerStepProgressPercentage[whichBagger] = 100;
        TimerReSet(whichBagger); baggerStepNumber[whichBagger]++;
        break;
      }
    //--------
    case 2: { // Substrate part 1 run the motor
        updateMessages[whichBagger]=" ";
        if ( substrateType == 2  && bagStatus[whichBagger] == 1) { // combopellet
          scaleLastWeights[whichBagger] = 0; // reset the counter
          scaleCumulativeSubWeights[whichBagger] = 0; // reset the counter
          baggerStepNumber[whichBagger] = 7; // skip sub and sup
          scaleFinalSupWeight[whichBagger] = 0; // offical amount we filled for sup
          scaleDryGoodsWeights[whichBagger] = 0;
          scaleSameAsLastTimeCount[whichBagger] = 0; // RESET THIS BAGGER COUNTER
          scaleOverfills[whichBagger] = 1; // skipping phases so assume the skp was perfect

        } else if (quickMode == 1 ) {
          baggerStepNumber[whichBagger]++; // skip this step if it's quick mode
        } else if ( bagStatus[whichBagger] == 1 && // bag is there
                    (motorStatus[subMotor] == whichBagger  // it's already me
                     || motorsAreASharedResource == 0  // or it's not a shared resource
                     || (  motorsAreASharedResource == 1  // or it is but
                           && motorStatus[subMotor] == -1  // the motor isn't being used
                           && (oneMotorAtATime == 0 || motorStatus[supMotor] == -1) // and it's not 1 motor at a time or at least the other motor isn't running
                        )
                    )
                  ) {
          // we're : already using this motor, or it's not a shared resource and dedicated or it's not used and available to us, snag it
          motorStatus[subMotor] = whichBagger; // we're using it now set it or confirm it
          // start motor and watch for weight. eventually attenuate speed as it gets close
          CheckScaleWeight(whichBagger, 0);
          if (scaleWeights[whichBagger] < -0.1) {
            if (serialActive == 2 ) {
              Serial.println(String(whichBagger) + " re-tarre a scale -negative value for some reaosn");
            };

            TareScales(whichBagger);// negative? weird ok let's tarre again
          } else {
            if (scaleWeights[whichBagger] < (motorThresholdPhase1 * realSub)  - SubPhase1TweakFromLastRun[whichBagger] ) { // bring it to target % before turning motor off
              setLight(whichBagger, "green", 0);
              setLight(whichBagger, "red", 0);
              // under first threshold
              Motor(subMotor, (int(quickMode) * 20) + pelletHighSpeed, subFlowDirection);// high speed don't slow down just stop after finish line
              baggerStepProgressPercentage[whichBagger] = int((100 * scaleWeights[whichBagger]) /  (motorThresholdPhase1 * realSub) );

            } else { // ok it's over or equal to the target weight.
              if (serialActive == 2 ) {
                Serial.println(String(whichBagger) + " SUBP1 over threshold of " + String((motorThresholdPhase1 * realSub))  + " with a tweak of " + String(SubPhase1TweakFromLastRun[whichBagger]));
              };

              ////(motorThresholdPhase1 * realSub)/
              baggerStepProgressPercentage[whichBagger] = int((100 * scaleWeights[whichBagger]) /  (motorThresholdPhase1 * realSub) );
              StopMotors(subMotor);
              if (motorsAreASharedResource == 1) {
                motorStatus[subMotor] = -1; // we're using it now set it or confirm it
              } else {
                // code for multiple baggers
              }
              TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // reset the timer so that we can wait the right time
            }
            CheckForNoProgress(whichBagger);
          }
        }; break;
      }
    //--------
    case 3: { //  Substrate part 1 weight
      updateMessages[whichBagger]=" ";
      if (timers[whichBagger] > 360) { // time delay from StopMotor function before we can weight it
          CheckScaleWeight(whichBagger, 0); // measure it
          if (serialActive == 2 ) {
            Serial.println(
              String(whichBagger) + " SUBP1 stable ... weight="
              + String(scaleWeights[whichBagger])
              + " against target threshold of " + String((motorThresholdPhase1 * realSub))
              + " using a weighttweak of " + String(SubPhase1TweakFromLastRun[whichBagger])
              + " difference of " + String( scaleWeights[whichBagger] - (motorThresholdPhase1 * realSub))
            );
          };

          SubPhase1TweakFromLastRun[whichBagger] = SubPhase1TweakFromLastRun[whichBagger] + (scaleWeights[whichBagger] - (motorThresholdPhase1 * realSub));

          scaleCumulativeSubWeights[whichBagger] = scaleWeights[whichBagger]; // this is officially how much sub we have so far
          baggerStepProgressPercentage[whichBagger] = 100;
          TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // reset the timer again
          scaleLastWeights[whichBagger] = scaleWeights[whichBagger];
          break;
        }

        break;
      }
    //--------
    case 4: { // Supplement part 1
       updateMessages[whichBagger]=" ";
       if (bagStatus[whichBagger] == 1) { // we have a bag and we're using shared resources and the resource is available.
          if (supplementSetValue == 0) { // spawn mode skip sup
            scaleOverfills[whichBagger] = 1.0;
            CheckScaleWeight(whichBagger, 0);
            scaleFinalSupWeight[whichBagger] = scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]; // offical amount we filled for sup
            scaleDryGoodsWeights[whichBagger] = scaleWeights[whichBagger];
            TimerReSet(whichBagger); baggerStepNumber[whichBagger]++;
          } else if ( bagStatus[whichBagger] == 1 &&
                      (motorStatus[supMotor] == whichBagger
                       || motorsAreASharedResource == 0
                       || (motorsAreASharedResource == 1 && motorStatus[supMotor] == -1
                           && (oneMotorAtATime == 0 || motorStatus[subMotor] == -1) // and it's not 1 motor at a time or at least the other motor isn't running
                          )
                      )
                    ) {
            // we're : already using this motor, or it's not a shared resource and dedicated or it's not used and available to us, snag it
            motorStatus[supMotor] = whichBagger; // we're using it now set it or confirm it
            CheckScaleWeight(whichBagger, 0);
            if (abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]) < (motorThresholdPhase1Sup[whichBagger] * realSup )   ) { // flowing normal
              int mSpeed = 100;
              if (realSup < 0.35) { // lower value of supplement so for accuracy let us just use low speed, no throttle
                mSpeed = supLowSpeed;
              } else {
                mSpeed = supHighSpeed;
              }
              baggerStepProgressPercentage[whichBagger] = int((100 * abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]))  /  (motorThresholdPhase1Sup[whichBagger] * realSup ) );

              Motor(supMotor, (int(quickMode) * 20) + mSpeed, supFlowDirection);// appropriate speed
              TimerReSet(whichBagger);
            } else {
              StopMotors(supMotor); // stop it once over the threshold
              supFillPhase2Stopped[whichBagger] = 1;
              baggerStepProgressPercentage[whichBagger] = int((100 * abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]))  /  (motorThresholdPhase1Sup[whichBagger] * realSup ) );
              if (serialActive == 2 ) {
                Serial.println(String(whichBagger) +  " weight when motor orderd to stop tot= " + String(scaleWeights[whichBagger]) + ", supweight=" + String( abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger])) + " against target sup weight of " +  String( realSup ) );
              };
              // wait here until gravityDelayTime milliseconds after stop, weigh and then proceed to see how close we got
              if (timers[whichBagger] > gravityDelayTime) {
                if (serialActive == 2 ) {
                  Serial.println(String(whichBagger) + " weight after delay tot= " + String(scaleWeights[whichBagger]) + ", supweight=" + String( abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger])) + " against target sup weight of " +  String( realSup ) );
                };
                SupPart1Ratio[whichBagger] = ( abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]))  /  ( realSup ) ;
                if (serialActive == 2 ) {
                  Serial.println(String(whichBagger) + " SUP part 1 got this close sup weight=" + String( abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger])) +  " to target of " + String( realSup ) );
                };
                SupTimingLearningAlgorithm(whichBagger);
                TimerReSet(whichBagger);
                baggerStepNumber[whichBagger]++; // next step
              }
            }
            CheckForNoProgress(whichBagger);
          }

        } else {
          BagMissingSound(whichBagger);
        }

        break;
      }
    //--------
    case 5: {  //  Supplement slower
        updateMessages[whichBagger]=" ";
        // see how much supplemen
        // when we first get to the slow phase of supplement, where are we at hitting that threshold value
        if (bagStatus[whichBagger] == 1) { // we still have a bag
          CheckScaleWeight(whichBagger, 0); // measure it
          if ((scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]) < ( motorThresholdPhase2Sup * realSup )  ) { // bring it slowly to close to final weight
            float diff = abs(1500 * (   ( motorThresholdPhase2[whichBagger] * realSup )  - (scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger] ))); // as we get closer, pulse delay should get longer .2 lbs = 200ms
            if (supFillPhase2Stopped[whichBagger] == 0 && timers[whichBagger] > 70) {
              StopMotors(supMotor); TimerReSet(whichBagger);
              supFillPhase2Stopped[whichBagger] = 1;
            } else {
              int delayAmount = int(abs(burstModeDelay - int(abs(diff))));
              if (timers[whichBagger] > delayAmount) {
                supFillPhase2Stopped[whichBagger] = 0;
                baggerStepProgressPercentage[whichBagger] = int((100 * abs(scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]))  /  ( motorThresholdPhase2Sup * realSup )  );
                if (serialActive == 2 ) {
                  Serial.println(String(whichBagger) + " PULSE SUP with diff=" +  String(diff) + " and delay of " + String(delayAmount) + " " + String(baggerStepProgressPercentage[whichBagger]) + "% " + String((scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger])) + " of " + String(( motorThresholdPhase2Sup * realSup )) );
                };


                Motor(supMotor, supLowSpeed, supFlowDirection); // the pulse
                TimerReSet(whichBagger);
              }
            }
            CheckForNoProgress(whichBagger);
          } else { // ok it's over the target weight.
            StopMotors(supMotor);
            motorStatus[supMotor] = -1; // relinquish the sup motor for use
            TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // next step
          }

        } else {
          BagMissingSound(whichBagger);
        }

        break;
      }
    //--------
    case 6: {  // Supplement weight
        updateMessages[whichBagger]=" ";
        if (timers[whichBagger] > gravityDelayTime) { // gravity pause
          CheckScaleWeight(whichBagger, 0); // measure it
          scaleOverfills[whichBagger] = ((scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]) / realSup) / 1.00; // this is how much did the sup overfill
          if (scaleOverfills[whichBagger] > 1.2) {
            updateMessages[whichBagger]= "OVERSUP CAP  " + String(100 * scaleOverfills[whichBagger] / 100.00);
            scaleOverfills[whichBagger] = 1.2;
          } // cap it to 20% over accidents happen with sup tumbling but we dont' want a huge bag
          if (serialActive == 2 ) {
            Serial.println(String(whichBagger) + " SUP DONE : Overfill=" + String(scaleOverfills[whichBagger]) );
          };

          scaleFinalSupWeight[whichBagger] = scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]; // offical amount we filled for sup
          scaleDryGoodsWeights[whichBagger] = scaleWeights[whichBagger];
          scaleSameAsLastTimeCount[whichBagger] = 0; // RESET THIS BAGGER COUNTER
          if (substrateType == 1 && residualMoistureSub > 22) { //sawdust and high moisture, we'll need to shake each time
            VibrationRun(vibrationDurationWhenNoProgress);
          }
          scaleLastWeights[whichBagger] = scaleWeights[whichBagger];
          TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // next step
          drawRunScreen();
        }
        break;
      }
    //--------
    case 7: { // "SUBP2++"
        if (bagStatus[whichBagger] == 1) { // we have a bag
          if (serialActive == 5 && timers[0] % 10 == 0) {
            Serial.println("part 7 RunIt bagger#" + String(whichBagger) + " we have a bag substatus " + motorStatus[subMotor] + ",motorsAreASharedResource=" + motorsAreASharedResource + ",oneMotorAtATime=" + oneMotorAtATime );
          }

          if (
            (motorStatus[subMotor] == whichBagger
             || motorsAreASharedResource == 0
             || (motorsAreASharedResource == 1 && motorStatus[subMotor] == -1
                 && (oneMotorAtATime == 0 || motorStatus[supMotor] == -1) // and it's not 1 motor at a time or at least the other motor isn't running
                )
            )
          ) {

            // we're : already using this motor, or it's not a shared resource and dedicated or it's not used and available to us, snag it
            motorStatus[subMotor] = whichBagger; // we're using it now set it or confirm it
            CheckScaleWeight(whichBagger, 0);
            baggerStepProgressPercentage[whichBagger] = int((100 * abs(scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger]))  /  (scaleOverfills[whichBagger] * motorThresholdPhase2[whichBagger] * realSub)  );
            float throttle = int((pelletHighSpeed - pelletLowSpeed) * (     pow(scaleWeights[whichBagger], motorAsymtoteAgressiveness) / pow((((scaleOverfills[whichBagger] * realSub) + scaleFinalSupWeight[whichBagger]) * motorThresholdPhase2[whichBagger] ), motorAsymtoteAgressiveness) )) ;
            if (throttle >  (pelletHighSpeed - pelletLowSpeed) ) {
              throttle =  (pelletHighSpeed - pelletLowSpeed);
            }// too much throttle}
            if (serialActive == 5) {
              Serial.println("part 7 RunIt bagger#" + String(whichBagger) + " throttle=" + String(throttle) + ", " + baggerStepProgressPercentage[whichBagger] + "%, weight=" + scaleWeights[whichBagger] );
            }
            if ( substrateType == 0 ) { // not pellet
              throttle = 0; // don't slow sawdust down
            }
            if ((scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger]) < (scaleOverfills[whichBagger] * motorThresholdPhase2[whichBagger] * realSub)   ) { // final sub but fast
              
              //v16 if combo pellets and 9pound bag or higher, then don't bother throttling until 60% threshold
              if( (substrateType == 2 && realSub > 3.6) && ( // combo pellets and 9 pounds or larger
                (scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger])  // just the sub
                < 
                0.65 * (scaleOverfills[whichBagger] * motorThresholdPhase2[whichBagger] * realSub) ) ){ // 65% of the target weight
                
                Motor(subMotor, (int(quickMode) * 20) + pelletHighSpeed , subFlowDirection);// high speed  but slow down as you get near
              } else {
                //normal or smaller weights
                Motor(subMotor, (int(quickMode) * 20) + pelletHighSpeed - round(abs(throttle)) , subFlowDirection);// high speed  but slow down as you get near
              }
              if (serialActive == 5 && timers[0] % 10 == 0) {
                Serial.println("part 7 RunIt bagger#" + String(whichBagger) + " motor " + subMotor + " at speed " + ((int(quickMode) * 20) + pelletHighSpeed) + " in direction " + subFlowDirection );
              }

              CheckForNoProgress(whichBagger);
              timers[whichBagger] = 0; // reset this bagger timer so we have a zero value when it last ran to compare to in the next bit
            } else {
              // over phase 1++ threshold
              StopMotors(subMotor); // stop it once over the threshold
              subFillPhase2Stopped[whichBagger] = 0 ; // indicate motor not stopped but it is
              timeoutPulsedMotor[whichBagger] = 0; // sets up the initial flag for processing first loop in next phase
              // wait here until gravityDelayTime milliseconds after stop, weigh and then proceed to see how close we got
              if (timers[whichBagger] > gravityDelayTime) {
                SubPhase2TimerStart[whichBagger] = timers[whichBagger];// timer for the last time these conditions exist
                SubPhase2Part1Ratio[whichBagger] = ( abs(scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger]))  /  (scaleOverfills[whichBagger] * realSub);
                if (serialActive == 2 ) {
                  Serial.println(String(whichBagger) + " SUB phase 2 part 1 got this close ratio:" + String(SubPhase2Part1Ratio[whichBagger]) + " and weight: " + String((scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger])) + " to " + String((scaleOverfills[whichBagger] * motorThresholdPhase2[whichBagger] * realSub)) );
                };
                SubTimingLearningAlgorithm(whichBagger);
                baggerStepNumber[whichBagger]++; // next step
                drawRunScreen();
              }
            }
          }
        } else {
          BagMissingSound(whichBagger);
        }

        break;
      }
    //--------
    case 8: { // "SUBP2+"
        updateMessages[whichBagger]=" ";
        if (bagStatus[whichBagger] == 1) { // we have a bag
          CheckScaleWeight(whichBagger, 0);
          if ((scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger]) <  (scaleOverfills[whichBagger] * motorThresholdPhase3 * realSub)   ) { // bring it slowly to close to final weight
            float diff = 2000 *  ((( motorThresholdPhase3 * realSub * scaleOverfills[whichBagger]) - ( scaleWeights[whichBagger] - scaleCumulativeSubWeights[whichBagger]))); // as we get closer, pulse delay should get longer .2 lbs = 200ms
            //motor not stopped and timer elapsed, stop
            if (subFillPhase2Stopped[whichBagger] == 0 && (timeoutPulsedMotor[whichBagger] == 0 || timers[whichBagger] > timeoutPulsedMotor[whichBagger])) { // first setting or after timeout
              CheckForNoProgress(whichBagger);
              StopMotors(subMotor);
              TimerReSet(whichBagger);
              subFillPhase2Stopped[whichBagger] = 1;
              timeoutPulsedMotor[whichBagger] = timers[whichBagger] + (abs(burstModeDelay - int(abs(diff)))); // sets timeout for next ON time
              if (serialActive == 2 ) {
                Serial.println(String(whichBagger) + " SUB STOP PULSE next timeout" + String(timeoutPulsedMotor[whichBagger]) + " " + String(baggerStepProgressPercentage[whichBagger]) + "% " + String((scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger])) + " of " + String(scaleOverfills[whichBagger] * motorThresholdPhase3 * realSub) );
              };

            } else if (subFillPhase2Stopped[whichBagger] == 1 && timers[whichBagger] > timeoutPulsedMotor[whichBagger]) { // start, we waited long enough to start it again
              // motor stopped and timer elapsed
              baggerStepProgressPercentage[whichBagger] = int((100 * abs(scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger]))  /  (scaleOverfills[whichBagger] * motorThresholdPhase3 * realSub) );

              TimerReSet(whichBagger);
              timers[whichBagger] = 0; // reset this bagger timer so we have a zero value when it last ran to compare to in the next bit
              subFillPhase2Stopped[whichBagger] = 0; // not stopped
              Motor(subMotor, pelletLowSpeed, subFlowDirection);// low speed until it hits target
              timeoutPulsedMotor[whichBagger] = timers[whichBagger] + SubPulseTime + (.300 * diff); // sets timeout for how long it's on time longer if further
              if (serialActive == 2 ) {
                Serial.println(String(whichBagger) + " PULSE SUB with diff=" + String(diff) + " and pulse duration of " + String(timeoutPulsedMotor[whichBagger]) + " " + String(baggerStepProgressPercentage[whichBagger]) + "% " + String((scaleWeights[whichBagger] - scaleFinalSupWeight[whichBagger])) + " of " + String(scaleOverfills[whichBagger] * motorThresholdPhase3 * realSub) );
              };
            }

          } else { // ok it's over the target weight.
            StopMotors(subMotor);
            motorStatus[subMotor] = -1; // relinquish motor as available
            TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // next step
            drawRunScreen();
          }
        } else {
          BagMissingSound(whichBagger);
        }
        scaleLastWeights[whichBagger] = scaleWeights[whichBagger];
        break;
      }
    //--------
    case 9: { // "SUBP2#"
       if (timers[whichBagger] > gravityDelayTime) { // gravity wait
          CheckScaleWeight(whichBagger, 0);
          BagFellCheck(whichBagger);  
          scaleDryGoodsWeights[whichBagger] = scaleWeights[whichBagger]; // this is officially how much sub we have.
          scaleOverfills[whichBagger] = scaleWeights[whichBagger] / (realSub + realSup) / 1.00; // figure out the final % we have as a float 1.1 = 110% too much sub
          updateMessages[whichBagger]= "Over Filled " + String(100.00 * scaleOverfills[whichBagger]) + "%";
          if (serialActive == 2 ) {
            Serial.println(String(whichBagger) + " OVERFILL " + String(100.00 * scaleOverfills[whichBagger]) + "% , going to water next, scale=" + String(scaleWeights[whichBagger]) + " and drygoods weight=" + String(scaleDryGoodsWeights[whichBagger]));
          }
          scaleLastWeights[whichBagger] = scaleWeights[whichBagger];
          TimerReSet(whichBagger); 
          baggerStepNumber[whichBagger]++; // next step
          if(noWaterMode==1){
            //v18 special case just skip water
            baggerStepNumber[whichBagger]=13;
            updateMessages[whichBagger]=  String(scaleWeights[whichBagger]) + "# no H20 ";
          }
          drawRunScreen();
        }
        break;
      }
    //--------
    case 10: { // "H20++"

        CheckBagStatus(whichBagger,1);
        BagFellCheck(whichBagger);  
        if (bagStatus[whichBagger] == 1) { // bag still in place
          setLight(whichBagger, "green", 0);
          setLight(whichBagger, "red", 0);
         
          CheckScaleWeight(whichBagger, 0); // measure it
          if (scaleWeights[whichBagger] < (scaleDryGoodsWeights[whichBagger] + (scaleOverfills[whichBagger] * realWater * waterFullShortStop[whichBagger] ) ) ) { // second term is water to add
            Water(whichBagger, 1); // open normal valve
            if ( fullWaterOpen == 0) {
              baggerStepProgressPercentage[whichBagger] = int((100 * scaleWeights[whichBagger])  /  (scaleDryGoodsWeights[whichBagger] + (scaleOverfills[whichBagger] * realWater * waterFullShortStop[whichBagger] ) )  );
              Water(fastValveNumber, 1); // open full blast
            }
          } else {
            // move to H20 slow
            if ( fullWaterOpen == 1 ) {
              Water(fastValveNumber, 0); // shut full blast off
              WaterTimerStart[whichBagger] = timers[whichBagger];// start the timer for how long it takes until we're running slow water
              baggerStepNumber[whichBagger]++; // next step
              drawRunScreen();
            }
          }
        } else {
          setLight(whichBagger, "red", 1);
          BagMissingSound(whichBagger);
        }
        scaleLastWeights[whichBagger] = scaleWeights[whichBagger];
        break;
      }
    //--------
    case 11: { // "H20+"
        updateMessages[whichBagger]=" ";
        CheckBagStatus(whichBagger,1);
        BagFellCheck(whichBagger);  
        if (bagStatus[whichBagger] == 1) { // bag still in place  /// eventually detect falling bag scaleLastWeights[whichBagger] > current weight + some buffer
          CheckScaleWeight(whichBagger, 0);
          if (scaleWeights[whichBagger] < (scaleDryGoodsWeights[whichBagger] + (scaleOverfills[whichBagger] * realWater * waterShortStop[whichBagger] ) )  ) { // target water is just the actual water we need but minus whatever water still drops after shutting down the valve.
            if (waterOpen[whichBagger] == 0) { // open the valve
              Water(whichBagger, 1);
            }
            baggerStepProgressPercentage[whichBagger] = int( (100 * scaleWeights[whichBagger])  /  (scaleDryGoodsWeights[whichBagger] + (scaleOverfills[whichBagger] * realWater * waterShortStop[whichBagger] ) )  );

          } else { // ok water's over the target weight.
            Water(whichBagger, 0); // shut water off
            baggerStepProgressPercentage[whichBagger] = 100;
            waterTimerWeightAtStopCommand[whichBagger] = scaleWeights[whichBagger]; // preserve the weight when we tried to stop it
            waterTimerEnd[whichBagger] = timers[whichBagger]; // record the time when we stopped it.
            waterTimeRan[whichBagger] = waterTimerEnd[whichBagger] - WaterTimerStart[whichBagger]; // total time it ran
            WaterTimingLearningAlgorithm(whichBagger);
            TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // next step
          }
        } else {
          BagMissingSound(whichBagger);
        }
        break;
      }
    //--------
    case 12: { // "H20#"
        if (timers[whichBagger] > gravityDelayTime) { // gravity stall
          CheckScaleWeight(whichBagger, 0); // measure it
//        if (serialActive == 6 ) {PukeDebugData(whichBagger);};
          TimerReSet(whichBagger); 
          // find moisture in residual ingredients and add moisture dropped by water steps;
          float netWaterDropped = scaleWeights[whichBagger] - scaleDryGoodsWeights[whichBagger]; // just water that fell
          float totalSubDropped = scaleDryGoodsWeights[whichBagger] - scaleFinalSupWeight[whichBagger];// dry goods less supplement is substrate net
          float netMoistureInDryGoods = (residualMoistureSub * .01 * totalSubDropped) + (residualMoistureSup * .01 * scaleFinalSupWeight[whichBagger]);// percentage of moisture in constituents * what fell
          float moistureNet = (100.00 * (netWaterDropped + netMoistureInDryGoods ))/ scaleWeights[whichBagger];// water in dry and added divided by final weight as percent
          updateMessages[whichBagger]=  String(scaleWeights[whichBagger]) + "# H20 " + String(moistureNet) + "%";
          footPedalLastStatus[whichBagger] = TestFootPedal(whichBagger);// v14 set the status of the foot pedal to the current state
          baggerStepNumber[whichBagger]++; // next step
          drawRunScreen();
        }
        break;
      }
    //--------
    case 13: { // "BAG-"      // detect removal of the bag and stop if grid mode
        setLight(whichBagger, "green", 1);
        setLight(whichBagger, "red", 0);
        CheckScaleWeight(whichBagger, 0); // measure it to see when we remove it
        if (subStatusMenuTestMatrixIndex > 0) {
          if (currentTotalBagsCounter == subStatusMenuTestMatrixLastBagNumber) { // last bag
            GoodSound(whichBagger); // play the done sound
            delay(2000);
            SetSimpleMessage( "DONE GRID TEST !", 300);
            currentlyRunning = 0; // stop at this test, can do normal run for rest of the bags
            ExitMenus(); // pause the process and indicate complete
          }
        }
        CheckBagStatus(whichBagger,0);
        CheckScaleWeight(whichBagger, 0); // measure it
        if (scaleWeights[whichBagger] < 0.5) { // detected removal of the bag
          currentTotalBagsCounter++;
          TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // next step
        }
        break;
      }
    //--------
    case 14: { // "BAGX" bag is missing
        setLight(whichBagger, "green", 0);
        setLight(whichBagger, "red", 1);
        CheckBagStatus(whichBagger,0);
        TimerReSet(whichBagger); baggerStepNumber[whichBagger]++; // next step
        break;
      }
    case 15: { // "BAG+"
        CheckBagStatus(whichBagger,0);
//v14 //v15 too since it will assume it's there.
        boolean bagFootPedalChanged=0;
        if(footPedalLastStatus[whichBagger] != TestFootPedal(whichBagger) ){bagFootPedalChanged=1;}
        CheckScaleWeight(whichBagger, 0);
 // v14 change if foot pedal status is different than it just was set to then it's a change and count on bag being present
 // v15 need to only advance if we've disabled bag sensors by scale tap or foot pedal
        if (bagFootPedalChanged ==1 
          || (bagStatus[whichBagger] == 1 && vibratorStatus == 0 ) ) { // bag is back on and vibrator isn't running 
          if (bagFootPedalChanged == 1
              || (bagRemovedCounterConfidence[whichBagger] > bagConfidenceThreshold && useBagSensors==1) //v15 only advance with bag sensors if we're indeed using them
              || (scaleWeights[whichBagger] < 0.25 && scaleTapped[whichBagger] >= 0.25)
             ) { // best to be really sure it's back or the scale was tapped or foot pedal
            scaleTapped[whichBagger] = 0; // reset, since we're moving on
//            if (currentTotalBagsCounter % 3 == 0 ) {
              baggerStepNumber[whichBagger] = 1; 
              setLight(whichBagger, "green", 1);
              setLight(whichBagger, "red", 0);

//            } else {
//              baggerStepNumber[whichBagger] = 2;
//            }
//            GoodSound(whichBagger);
            TimerReSet(whichBagger);  // next step
            bagRemovedCounterConfidence[whichBagger] = 0; //reset the counter
          } else {
            bagRemovedCounterConfidence[whichBagger]++; // we're thinking it's there now
            if (scaleWeights[whichBagger] > 0.25 ) {
              // bag holder was tapped
              scaleTapped[whichBagger] = scaleWeights[whichBagger]; // store the tapped amount
              GoodSound(whichBagger); // feedback to the user
            }
          }

        } else if (bagStatus[whichBagger] == 0 && bagRemovedCounterConfidence[whichBagger] > 0) {
          // vote no confidence, bag missing reset to zero
          bagRemovedCounterConfidence[whichBagger] = 0;
        }
        // ignore bag sensor for now regardless of bag sensor
        if (scaleWeights[whichBagger] > 0.50  && scaleWeights[whichBagger] < maxScaleWeights[whichBagger]/1.5 ){ // scale pressure detection half pound to 2/3 of max weight seen
          // bag holder was tapped
          scaleTapped[whichBagger] = scaleWeights[whichBagger]; // store the tapped amount
          setLight(whichBagger, "green", 1);
          setLight(whichBagger, "red", 0);
          GoodSound(whichBagger); // feedback to the user
        }
        break;
      }

    default: {
        break;
      }
  }
}


void BagFellCheck(int wb){
  // did a bag fall off due to shitty placement, usually happens during water phase but can happen durring pellet fill.
  // assumption is that at least 1 pound of stuff is on the scale and then we look for a drop of weight by a threshold below the max amount we've seen on the scale.
  // puts everything into emergency stop and click to continue.
  // a confidence counter is used to ensure it's not impulse of momentum (water falling or pellets falling makes weight jump momentarily.
  
  if( maxScaleWeights[wb] > 1.0 && (scaleWeights[wb] < (maxScaleWeights[wb] - 0.5))){   
    // at least a pound and weight dropped enough to notice
    if(bagDropConfidenceCounter > DROPPEDWEIGHTCONFIDENCETHRESHOLD ){
      EmergencyStop();
      SetSimpleMessage("ALERT",500);
      BadSound(wb);
      bagDropConfidenceCounter=0;// reset for next time
    } else {
      bagDropConfidenceCounter++; // increment a confidence counter
    }
  } else {
    if(bagDropConfidenceCounter > 0){
      bagDropConfidenceCounter --;// decrement the counter down towards zero.. eliminates glitches      
    }
  }
}

