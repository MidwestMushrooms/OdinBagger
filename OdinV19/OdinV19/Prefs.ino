void PrefLoadUpdate(){
  startnote = int(startnote * 1.054805); 
  tone(buzzer, startnote, int(prefLoadSpeed *.9));
  prefIndex++;
}
void LoadPrefs() {
  currentlyRunning = 99;
  int loc = 0;
  // one byte integers
  int tempweightSetValue = 0;
  EEPROM.get(loc, quickMode); loc += sizeof(quickMode);
  EEPROM.get(loc, tempweightSetValue);  loc += sizeof(weightSetValue);
  EEPROM.get(loc, moistureSetValue);  loc += sizeof(moistureSetValue);
  EEPROM.get(loc, supplementSetValue); loc += sizeof(supplementSetValue);
  EEPROM.get(loc, ScaleCalibrationFactors[0]); loc += sizeof(ScaleCalibrationFactors[0]);
  EEPROM.get(loc, ScaleCalibrationFactors[1]); loc += sizeof(ScaleCalibrationFactors[1]);
  EEPROM.get(loc, residualMoistureSub); loc += sizeof(residualMoistureSub);
  EEPROM.get(loc, residualMoistureSup); loc += sizeof(residualMoistureSup);
  EEPROM.get(loc, substrateType); loc += sizeof(substrateType);
  EEPROM.get(loc, vibrationMode); loc += sizeof(vibrationMode);
  EEPROM.get(loc, subDensityFactor[0]); loc += sizeof(subDensityFactor[0]);
  EEPROM.get(loc, pelletHighSpeed); loc += sizeof(pelletHighSpeed);
  EEPROM.get(loc, pelletLowSpeed); loc += sizeof(pelletLowSpeed);
  EEPROM.get(loc, supHighSpeed); loc += sizeof(supHighSpeed);
  EEPROM.get(loc, supLowSpeed); loc += sizeof(supLowSpeed);
  EEPROM.get(loc, waterFullShortStop[0]); loc += sizeof(waterFullShortStop[0]);
  EEPROM.get(loc, waterFullShortStop[1]); loc += sizeof(waterFullShortStop[1]);
  EEPROM.get(loc, supDensityFactor[0]); loc += sizeof(supDensityFactor[0]);
  EEPROM.get(loc, subDensityFactor[1]); loc += sizeof(subDensityFactor[1]);
  EEPROM.get(loc, supDensityFactor[1]); loc += sizeof(supDensityFactor[1]);
//v15
  EEPROM.get(loc, useBagSensors); loc += sizeof(useBagSensors);
  // v18
  EEPROM.get(loc, noWaterMode); loc += sizeof(noWaterMode);


  supplementSetValue--;// correct for allowing a zero value (sawdust spawn mode) we store it 1 higher than we really want it
    weightSetValue = tempweightSetValue; // seems like stored data was retrieved so load it as last settings
    SetSimpleMessage( "LOAD LAST USED        ",300); 
    String tempQuickMode = "Off";
    String tempVibration = "Off";
    String tempSubstrateType = "Pellets";
    if (quickMode == 1) {
      tempQuickMode = "ON";
    }
    if (vibrationMode == 1) {
      tempVibration = "ON";
    }
    if (substrateType == 1) {
      tempSubstrateType = "Sawdust";
    } else if(substrateType == 2){
      tempSubstrateType = "DualPellet";
    }
    startnote = int(noteA); 
    tone(buzzer, startnote, 600);
    SetSimpleMessage("QuickMode " + tempQuickMode,prefLoadSpeed);PrefLoadUpdate();
    SetSimpleMessage("weightSet " + String(weightSetValue / 10.0) + "#",prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("moisture " + String(moistureSetValue) + "%",prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("supplement " + String(supplementSetValue) + "%",prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("scalcalL" + String(ScaleCalibrationFactors[0]),prefLoadSpeed) ; PrefLoadUpdate();
    SetSimpleMessage("scalcalR" + String(ScaleCalibrationFactors[1]),prefLoadSpeed) ; PrefLoadUpdate();
    SetSimpleMessage("Sub Moist " + String(residualMoistureSub) + "%",prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("Sup Moist " + String(residualMoistureSup) + "%",prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SubType " + tempSubstrateType,prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("Vibration " + tempVibration,prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SBDenF:" + String(subDensityFactor[0]),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SubHiSpd:" + String(pelletHighSpeed),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SubLoSpd:" + String(pelletLowSpeed),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SupHiSpd:" + String(supHighSpeed),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SupLoSpd:" + String(supLowSpeed),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("wtFulShSt0:" + String(waterFullShortStop[0]),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("wtFulShSt1:" + String(waterFullShortStop[1]),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SPDenL:" + String(supDensityFactor[0]),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SBDenFRight:" + String(subDensityFactor[1]),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("SPDenR:" + String(supDensityFactor[1]),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("BagS:" + String(useBagSensors),prefLoadSpeed); PrefLoadUpdate();
    SetSimpleMessage("WaterM:" + String(noWaterMode),prefLoadSpeed); PrefLoadUpdate();

    tone(buzzer, noteE * 2, 100);
    if(useBagSensors != 0 && useBagSensors !=1){
      useBagSensors=1;// default if it's wonky data
      BadSound(0);
      SetSimpleMessage( "BAG SENSORS SET TO DEFAULT" + String(useBagSensors) ,300); delay(500);
    }
    if(noWaterMode != 0 && noWaterMode !=1){
      noWaterMode=0;// default is off if wonky data
      BadSound(0);
      SetSimpleMessage( "No Water Mode SET TO DEFAULT" + String(noWaterMode) ,300); delay(500);
    }
    if (moistureSetValue < 50 || moistureSetValue > 80) {
      BadSound(0);
      SetSimpleMessage( "LOW M:WAS" + String(moistureSetValue) ,300); delay(500);
      moistureSetValue = 62;
      SetSimpleMessage( "LOW M:NOW" + String(moistureSetValue) ,300); delay(500);
      SavePrefs();
    }
    if (supplementSetValue < 0 || supplementSetValue>50) { // zero would be never set before
      BadSound(0);
      SetSimpleMessage( "LOW SP:WAS" + String(supplementSetValue) ,300);
      supplementSetValue = 20; // retrieve from memory eventually
      SetSimpleMessage( "LOW SP:NOW" + String(supplementSetValue) ,300);
      SavePrefs();
    }
    if (weightSetValue < 10 || weightSetValue > 100) {
      BadSound(0);
      SetSimpleMessage( "LOW WT:WAS" + String(weightSetValue) ,300);
      weightSetValue = 50; // default
      SetSimpleMessage( "LOW WT:NOW" + String(weightSetValue) ,300);
      SavePrefs();
    }
    if (abs(ScaleCalibrationFactors[0]) > 1 && abs(ScaleCalibrationFactors[0]) < 3000000) {
      //seems right 
    } else {
      BadSound(0);
      SetSimpleMessage( "LOW CAL:WAS" + String(ScaleCalibrationFactors[0]) ,300);
      ScaleCalibrationFactors[0] = DEFAULTSCALECALIBRATION; // set it to default
      SavePrefs();
    }
    scale0.set_scale(ScaleCalibrationFactors[0]); //Adjust scale to this saved or default calibration factor
    if (abs(ScaleCalibrationFactors[1]) > 1 && abs(ScaleCalibrationFactors[1]) < 3000000) {
      //seems right 
    } else {
      BadSound(0);
      SetSimpleMessage( "LOW CAL:WAS" + String(ScaleCalibrationFactors[1]) ,300);
      ScaleCalibrationFactors[1] = DEFAULTSCALECALIBRATION; // set it to default
      SavePrefs();
    }
    scale1.set_scale(ScaleCalibrationFactors[1]); //Adjust scale to this saved or default calibration factor

    if (abs(residualMoistureSub) > residualMoistureSubHigh || residualMoistureSub  < residualMoistureSubLow) { // seems out of range or too small
      BadSound(0);
      SetSimpleMessage( "ER RSBM:WAS" + String(residualMoistureSub) ,300);
      residualMoistureSub = 7; // use default
      SavePrefs();
    }
    if (abs(residualMoistureSup) > residualMoistureSupHigh || residualMoistureSup  < residualMoistureSupLow) { // seems out of range or too small
      BadSound(0);
      SetSimpleMessage( "ER RSPM:WAS" + String(residualMoistureSup) ,300);
      residualMoistureSup = 7; // use default
      SavePrefs();
    }
    if (subDensityFactor[0] < 0.2 || subDensityFactor[0] > 1.00  || isnan(subDensityFactor[0])) {
      BadSound(0);
      SetSimpleMessage( "BADSbDenL " + String(subDensityFactor[0] ) ,300);
      SetSimpleMessage( "Density-was" + String(subDensityFactor[0] ) ,300);
      subDensityFactor[0] = 0.85;
      SetSimpleMessage( "DFL NOW " + String(subDensityFactor[0] ) ,300);
      SavePrefs();
    }
    if (subDensityFactor[1] < 0.2 || subDensityFactor[1] > 1.00  || isnan(subDensityFactor[1])) {
      BadSound(0);
      SetSimpleMessage( "BADSbDenR " + String(subDensityFactor[1] ) ,300);
      SetSimpleMessage( "Density-was" + String(subDensityFactor[1] ) ,300);
      subDensityFactor[1] = 0.85;
      SetSimpleMessage( "DFR NOW " + String(subDensityFactor[1] ) ,300);
      SavePrefs();
    }
    if (supDensityFactor[0] < 0.2 || supDensityFactor[0] > 1.00  || isnan(supDensityFactor[0])) {
      BadSound(0);
      SetSimpleMessage( "BADSpDenL " + String(supDensityFactor[0]) ,300);
      SetSimpleMessage( "Density-was" + String(supDensityFactor[0]) ,300);
      supDensityFactor[0] = 0.85;
      SetSimpleMessage( "DFL NOW " + String(supDensityFactor[0]) ,300);
      SavePrefs();
    }
    if (supDensityFactor[1] < 0.2 || supDensityFactor[1] > 1.00  || isnan(supDensityFactor[1])) {
      BadSound(0);
      SetSimpleMessage( "BADSpDenR " + String(supDensityFactor[1]) ,300);
      SetSimpleMessage( "Density-was" + String(supDensityFactor[1]) ,300);
      supDensityFactor[1] = 0.85;
      SetSimpleMessage( "DFR NOW " + String(supDensityFactor[1]) ,300);
      SavePrefs();
    }
    if (pelletHighSpeed < 50 || isnan(pelletHighSpeed) || pelletHighSpeed > 199) {
      BadSound(0);
      SetSimpleMessage( "BadSubHiSpeed " + String(pelletHighSpeed) ,300);
      pelletHighSpeed = 150;
      SetSimpleMessage( "SubHiSpeedis" + String(pelletHighSpeed) ,300);
      SavePrefs();
    }
    if (pelletLowSpeed < 30 || isnan(pelletLowSpeed) || pelletLowSpeed > 199) {
      BadSound(0);
      SetSimpleMessage( "BadSupLoSpeed " + String(pelletLowSpeed) ,300);
      pelletLowSpeed = 60;
      SetSimpleMessage( "SubLoSpeedis" + String(pelletLowSpeed) ,300);
      SavePrefs();
    }
    if (supHighSpeed < 30 || isnan(supHighSpeed) || supHighSpeed > 199) {
      BadSound(0);
      SetSimpleMessage( "BadSupHiSpeed " + String(supHighSpeed) ,300);
      supHighSpeed = 150;
      SetSimpleMessage( "SupHiSpeedis" + String(supHighSpeed) ,300);
      SavePrefs();
    }
    if (supLowSpeed < 20 || isnan(supLowSpeed) || supLowSpeed > 100 ) {
      BadSound(0);
      SetSimpleMessage( "BadSupLoSpeed " + String(supLowSpeed) ,300);
      supLowSpeed = 30;
      SetSimpleMessage( "SupLoSpeedis" + String(supLowSpeed) ,300);
      SavePrefs();
    }
   // water null or save ranges
   if (waterFullShortStop[0] > 0.98 || isnan(waterFullShortStop[0])) {
      BadSound(0);
      SetSimpleMessage( "BADH20++" + String(waterFullShortStop[0]) ,300); delay(1000);
      waterFullShortStop[0] = 0.98;
      SavePrefs();
    } else if( waterFullShortStop[0] < 0.7){
      BadSound(0);
      SetSimpleMessage( "BADH20--" + String(waterFullShortStop[0]) ,300); delay(1000);
      waterFullShortStop[0] = 0.7;
      SavePrefs();
    }
  if (waterFullShortStop[1] > 0.98 || isnan(waterFullShortStop[1])) {
      BadSound(1);
      SetSimpleMessage( "BADH20++" + String(waterFullShortStop[1]) ,300); delay(1000);
      waterFullShortStop[1] = 0.98;
      SavePrefs();
    } else if( waterFullShortStop[1] < 0.7){
      BadSound(1);
      SetSimpleMessage( "BADH20--" + String(waterFullShortStop[1]) ,300); delay(1000);
      waterFullShortStop[1] = 0.7;
      SavePrefs();
    }  
    
    motorThresholdPhase2[0] = 0.96 * subDensityFactor[0];
    motorThresholdPhase2[1] = 0.96 * subDensityFactor[1];
    motorThresholdPhase1Sup[0] = 0.91 * supDensityFactor[0]; // % of the target weight where it slows down
    motorThresholdPhase1Sup[1] = 0.91 * supDensityFactor[1]; // % of the target weight where it slows down

  currentlyRunning = 0;
}

void SavePrefs() {
  int loc = 0;
  EEPROM.put(loc, int(quickMode));  loc += sizeof(quickMode);
  EEPROM.put(loc, weightSetValue);  loc += sizeof(weightSetValue);
  EEPROM.put(loc, moistureSetValue);  loc += sizeof(moistureSetValue);
  EEPROM.put(loc, (supplementSetValue + 1));  loc += sizeof(supplementSetValue); // stores it 1 higher than actual
  EEPROM.put(loc, ScaleCalibrationFactors[0]);  loc += sizeof(ScaleCalibrationFactors[0]);
  EEPROM.put(loc, ScaleCalibrationFactors[1]);  loc += sizeof(ScaleCalibrationFactors[1]);
  EEPROM.put(loc, residualMoistureSub);  loc += sizeof(residualMoistureSub);
  EEPROM.put(loc, residualMoistureSup);  loc += sizeof(residualMoistureSup);
  EEPROM.put(loc, int(substrateType));  loc += sizeof(substrateType);
  EEPROM.put(loc, int(vibrationMode));  loc += sizeof(vibrationMode);
  EEPROM.put(loc, subDensityFactor[0]);  loc += sizeof(subDensityFactor[0]);
  EEPROM.put(loc, pelletHighSpeed);  loc += sizeof(pelletHighSpeed);
  EEPROM.put(loc, pelletLowSpeed);  loc += sizeof(pelletLowSpeed);
  EEPROM.put(loc, supHighSpeed);  loc += sizeof(supHighSpeed);
  EEPROM.put(loc, supLowSpeed);  loc += sizeof(supLowSpeed);
  EEPROM.put(loc, waterFullShortStop[0]);  loc += sizeof(waterFullShortStop[0]);
  EEPROM.put(loc, waterFullShortStop[1]);  loc += sizeof(waterFullShortStop[1]);
  EEPROM.put(loc, supDensityFactor[0]);  loc += sizeof(supDensityFactor[0]);
  EEPROM.put(loc, subDensityFactor[1]);  loc += sizeof(supDensityFactor[1]);
  EEPROM.put(loc, supDensityFactor[1]);  loc += sizeof(supDensityFactor[1]);
//v15
  EEPROM.put(loc, useBagSensors);  loc += sizeof(useBagSensors);
//v18
  EEPROM.put(loc, noWaterMode);  loc += sizeof(noWaterMode);
}
