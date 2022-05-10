void StartTestScales(){
  StartAProcessAndExitMenu(16, "TEST SCALES ","ZEROING BOTH SCALES"," ");
  scale0.tare();
  delay(50);
  scale0.tare();
  delay(50);
  scale1.tare();
  delay(50);
  scale1.tare();
  delay(50);
}
void CheckScaleWeight(int whichBagger, bool testMode) {
  if(serialActive==1){Serial.println("function CheckScaleWeight " + String(whichBagger));}
  switch (whichBagger) {
    case 0: {
        scaleWeights[0] = (10.0 * float(scale0.get_units(3))) / 10.00;
        break;
      }
    case 1: {
        scaleWeights[1] = (10.0 * float(scale1.get_units(3))) / 10.00;
        break;
      }
    case 44: {
        scaleWeights[0] = (10.0 * float(scale0.get_units(3))) / 10.00; 
        scaleWeights[1] = (10.0 * float(scale1.get_units(3))) / 10.00; 
        break;
      }
  }
  percentComplete[whichBagger] = 1000 * (scaleWeights[0]/weightSetValue);
  if (scaleWeights[0] < .001) {
    scaleWeights[0] = 0;
  }
  if (scaleWeights[1] < .001) {
    scaleWeights[1] = 0;
  }
  if (scaleWeights[2] < .001) {
    scaleWeights[2] = 0;
  }
  if (scaleWeights[3] < .001) {
    scaleWeights[3] = 0;
  }
  if(scaleWeights[0] > maxScaleWeights[0]){
    maxScaleWeights[0]= scaleWeights[0]; // scale grew, so keep track of how high it got
  }
  if(scaleWeights[1] > maxScaleWeights[1]){
    maxScaleWeights[1]= scaleWeights[1]; // scale grew, so keep track of how high it got
  }
}

void TareScales(int whichScale) {
  if(serialActive==1){Serial.println("function TareScales " + String(whichScale));}
  switch (whichScale) {
    case 0: {
        scale0.tare();
        break;
      }
    case 1: {
        scale1.tare();
        break;
      }
    default: {
        break;
      }
  }
}
void ScaleCalibration() {
  menuColumn=0;
  TimersUpdate(); // update all the timer arrays
  statusMessage[0] = "Scale Calibration";  

  float correction_factor = 1.0;
  AllLightsOff();setLight(0, "red", 1);

  statusMessage[1] = "<- ADD " + String(int(scaleCalibrationTarget)) + "# TO LEFT";  DrawDisplayAsAlertMessage(1);
  for(int i=0;i< 20;i++){
    CheckScaleWeight(0,0);
    delay(500);
    if(scaleWeights[0]> 0.1){
      AllLightsOff();setLight(0, "green", 1);
      delay(1000); // stall until scale is stable
      break;
    }
  }
  // print some diagnostic stuff
  statusMessage[2] = "<- Cal. Value:" + String(ScaleCalibrationFactors[0]);DrawDisplayAsAlertMessage(1);
  delay(1000);
  CheckScaleWeight(0,0);
  statusMessage[2] = "<- Scale Weight:" + String(scaleWeights[0] / 1.0);DrawDisplayAsAlertMessage(1);
  delay(3000);
  correction_factor = scaleCalibrationTarget / scaleWeights[0] ;
  statusMessage[2] = "<- Variance:" +String(correction_factor);
  delay(3000);
  ScaleCalibrationFactors[0] = ScaleCalibrationFactors[0] / correction_factor;
  statusMessage[2] = "<- New Cal.:" +String(ScaleCalibrationFactors[0]);DrawDisplayAsAlertMessage(1);
  scale0.set_scale(ScaleCalibrationFactors[0]); //Adjust to this new calibration factor
  delay(3000);
  CheckScaleWeight(0,0);
  statusMessage[2] = "<- New Weight:" + String(scaleWeights[0] / 1.0);DrawDisplayAsAlertMessage(1);
  AllLightsOff();
  delay(3000);
  GoodSound(0);
   AllLightsOff();setLight(1, "red", 1);
  statusMessage[1] = "-> ADD " + String(int(scaleCalibrationTarget)) + "# TO Right";
  statusMessage[2]= "Now";DrawDisplayAsAlertMessage(1);
  for(int i=0;i< 20;i++){
    CheckScaleWeight(1,0);
    delay(500);
    if(scaleWeights[1]> 0.1){
      AllLightsOff();setLight(1, "green", 1);
      delay(1000); // stall until scale is stable
      break;
    }
  }
  // print some diagnostic stuff
  statusMessage[2] = "-> Cal. Value:" + String(ScaleCalibrationFactors[1]);DrawDisplayAsAlertMessage(1);
  delay(1000);
  CheckScaleWeight(1,0);
  statusMessage[2] = "-> Scale Weight:" + String(scaleWeights[1] / 1.0);DrawDisplayAsAlertMessage(1);
  delay(3000);
  correction_factor = scaleCalibrationTarget / scaleWeights[1] ;
  statusMessage[2] = "-> Variance:" +String(correction_factor);DrawDisplayAsAlertMessage(1);
  delay(3000);
  ScaleCalibrationFactors[1] = ScaleCalibrationFactors[1] / correction_factor;
  statusMessage[2] = "-> New Cal.:" +String(ScaleCalibrationFactors[1]);DrawDisplayAsAlertMessage(1);
  scale1.set_scale(ScaleCalibrationFactors[1]); //Adjust to this new calibration factor
  delay(3000);
  CheckScaleWeight(1,0);
  statusMessage[2] = "-> New Weight:" + String(scaleWeights[0] / 1.0);DrawDisplayAsAlertMessage(1);
  delay(3000);
  AllLightsOff();setLight(0, "green", 1);setLight(1, "green", 1);

  statusMessage[0] = "Calibration Done";
  statusMessage[1] = " ";
  statusMessage[2] = " ";
  DrawDisplayAsAlertMessage(1);
  delay(3000);
  GoodSound(0);
  SavePrefs();
  currentlyRunning = 0;
  ExitMenus();
  
}

void ResetScales(){

  ScaleCalibrationFactors[0] = DEFAULTSCALECALIBRATION;
  ScaleCalibrationFactors[1] = DEFAULTSCALECALIBRATION;
  scale0.set_scale(ScaleCalibrationFactors[0]); //Adjust to this new calibration factor
  scale1.set_scale(ScaleCalibrationFactors[1]); //Adjust to this new calibration factor
  statusMessage[0] = "Scales Were Reset";
  statusMessage[1] = "Test Functional Next";
  statusMessage[2] = " ";
  DrawDisplayAsAlertMessage(1);
  delay(3000);
  GoodSound(0);
  SavePrefs();
  currentlyRunning = 16; // go to Test Scales
  processPaused=0;
  ExitMenus();
}

void TestScales() {
  if(serialActive==1){Serial.println("function TestScales");}
  CheckScaleWeight(0,1);
  CheckScaleWeight(1,1);
  String tmessageScales =  "<:" + String(scaleWeights[0]) + "   >:" + String(scaleWeights[1]);
  String tmessageScales2 = "<:" + String(ScaleCalibrationFactors[0]) + "   >:" + String(ScaleCalibrationFactors[1]);
  String tmessageScales3 = "<:" + String(float(scale0.get_units(3))) + "   >:" + String(float(scale1.get_units(3)));
  statusMessage[0] = tmessageScales;
  statusMessage[1] = tmessageScales2;
  statusMessage[2] = tmessageScales3;
}
