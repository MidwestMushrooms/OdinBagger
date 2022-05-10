
String getMenuItem(int whichItem){
  switch (menuColumn){
    case 0: case 1:{ 
      if(whichItem < menuLevel1Length){        //  it's before the end so  return it
        strcpy_P(progMemBuffer, (PGM_P)pgm_read_word(&menuLevel1[whichItem])); // Necessary casts and dereferencing, just copy.             
        return String(progMemBuffer);
      } else { 
        return "";
      };
      break;}
    case 2:{
       switch (subMenuNumber) {
          case 0: {break;}
          case 2: {
              strcpy_P(progMemBuffer, (PGM_P)pgm_read_word(&menuLevel2Setup[whichItem])); // Necessary casts and dereferencing, just copy.             
              return String(progMemBuffer);
              break;}
          case 3: { 
              strcpy_P(progMemBuffer, (PGM_P)pgm_read_word(&menuLevel2Recipe[whichItem])); // Necessary casts and dereferencing, just copy.             
              return String(progMemBuffer);
              break;}
          case 4: { 
              strcpy_P(progMemBuffer, (PGM_P)pgm_read_word(&menuLevel2Preferences[whichItem])); // Necessary casts and dereferencing, just copy.             
              return String(progMemBuffer);
              break;}
          case 5: case 6: case 7:{
              strcpy_P(progMemBuffer, (PGM_P)pgm_read_word(&menuLevel2Test[whichItem])); // Necessary casts and dereferencing, just copy.             
              return String(progMemBuffer);
              break;}
        }
        break;
        }
  }
}


void adjustCurrentMenu(int mydir){
  rotaryThrottle++;
  if(rotaryThrottle < rotarySpeed){
    return;
  } else {
    rotaryThrottle=0;// reset for next time
  }
  if(menuColumn < 3){
    if(mydir == -1){
  
      if(MenuSelectedItem==0){ // loop round
        MenuSelectedItem = selectedMenuLength + 1;
      } else {
        MenuSelectedItem = MenuSelectedItem + mydir;
      }
    } else {
      if(MenuSelectedItem >= (selectedMenuLength ) || MenuSelectedItem < 0){ // loop round
         MenuSelectedItem=0;
      } else {
        MenuSelectedItem =  MenuSelectedItem + mydir;
      }
    }
  } else {
    // tweak a value in menucolumn 3
//    mydir = mydir * -1; // v13 change invert it for the adjustment of values
    switch (subMenuNumber) {
      case 2: { // setup - calibration is only one choice
        switch(MenuSelectedItem){
           case 1: { // Which Calibration size to use: scaleCalibrationTarget is the final value as real number
              
              menuLevel3CalibrationIndex = menuLevel3CalibrationIndex +  mydir;
              if(menuLevel3CalibrationIndex< 1){
                menuLevel3CalibrationIndex=0;
              } else if(menuLevel3CalibrationIndex > 5){
                menuLevel3CalibrationIndex=5;
              }
              if (menuLevel3CalibrationIndex == 0) {
                scaleCalibrationTarget = 3.000; // default 
              if (menuLevel3CalibrationIndex == 1) {
                scaleCalibrationTarget = 3.000;
              } else if(menuLevel3CalibrationIndex == 2) {
                scaleCalibrationTarget = 5.000;
              } else if(menuLevel3CalibrationIndex == 3) {
                scaleCalibrationTarget = 6.000;
              } else if(menuLevel3CalibrationIndex == 4) {
                scaleCalibrationTarget = 10.000;
              } else if(menuLevel3CalibrationIndex >= 5) {
                scaleCalibrationTarget = 12.000;
              } else {
                menuLevel3CalibrationIndex =1;
                scaleCalibrationTarget = 3.000; 
              }
              break;
            }
          }
        }
        break;
      }
      case 3: { // Recipe settings
        switch(MenuSelectedItem-1){
          case 0:{ // moisture
            moistureSetValue=moistureSetValue + mydir;tone(buzzer, noteG, 50);
            if (moistureSetValue > moistureHigh) { BadSound(0);moistureSetValue = moistureHigh;}
            if (moistureSetValue < moistureLow) {BadSound(0);moistureSetValue = moistureLow;}break;}
          case 1:{ // weight lb
            weightSetValue=weightSetValue + mydir;tone(buzzer, noteE, 50);
            if (weightSetValue > weightHigh) {BadSound(0);weightSetValue = weightHigh;}
            if (weightSetValue < weightLow) {BadSound(0);weightSetValue = weightLow;}break;}
          case 2:{ // supmix%
              supplementSetValue=supplementSetValue+ mydir;tone(buzzer, noteF, 50);
            if (supplementSetValue > supplementMixMax) {BadSound(0);supplementSetValue = supplementMixMax;}
            if (supplementSetValue < supplementMixMin) {BadSound(0);supplementSetValue = supplementMixMin;}break;}
          case 3:{ //, "SubMoist"    
            residualMoistureSub=residualMoistureSub + mydir;tone(buzzer, noteE, 50);
            if (residualMoistureSub > residualMoistureSubHigh) {BadSound(0);residualMoistureSub = residualMoistureSubHigh;}
            if (residualMoistureSub < residualMoistureSubLow) {BadSound(0);residualMoistureSub = residualMoistureSubLow;}break;}
          case 4: { // , "SupMoist"   
            residualMoistureSup=residualMoistureSup + mydir; tone(buzzer, noteE, 50);
            if (residualMoistureSup > residualMoistureSupHigh) {BadSound(0);residualMoistureSup = residualMoistureSupHigh;}
            if (residualMoistureSup < residualMoistureSupLow) {BadSound(0);residualMoistureSup = residualMoistureSupLow;}break;}
          case 5: { // , "Substrate type rotate through 3 types regardless of direciton
              if (substrateType == 0) {
                substrateType = 1;
              } else if(substrateType == 1) {
                substrateType = 2;
              } else {
                substrateType = 0;
              }
            break;
          }
         case 6: { // , "Vibration mode"
            if (vibrationMode == 0) {
              vibrationMode = 1;
            } else {
              vibrationMode = 0;
            }
            break;
          }
        }
        break ;}
      case 4: {
        switch(MenuSelectedItem - 1){
          case 0:{ // subDensityFactor
            subDensityFactor[0] = subDensityFactor[0] + (mydir * .01);
            if (subDensityFactor[0] > 1.0) {BadSound(0);subDensityFactor[0] = 1.00;}
            if (subDensityFactor[0] < .2) {BadSound(0);subDensityFactor[0] = .2;}
            break;}
          case 1:{ // suPDensityFactor
            supDensityFactor[0] = supDensityFactor[0] + (mydir *  0.01);
            if (supDensityFactor[0] > 1.0) {BadSound(0);supDensityFactor[0] = 1.00;}
            if (supDensityFactor[0] < .2) {BadSound(0);supDensityFactor[0] = .2;}
            break;}
          case 2:{
            pelletHighSpeed = pelletHighSpeed + mydir;
            if (pelletHighSpeed > 200) {BadSound(0);pelletHighSpeed = 200;}
            if (pelletHighSpeed < 30) {BadSound(0);pelletHighSpeed = 30;}
            break;}
          case 3:{
            pelletLowSpeed = pelletLowSpeed + mydir;
            if (pelletLowSpeed > 150) {BadSound(0);pelletLowSpeed = 150;}
            if (pelletLowSpeed < 30) {BadSound(0);pelletLowSpeed = 30;}
            break;}
          case 4:{
            supHighSpeed = supHighSpeed + mydir;
            if (supHighSpeed > 200) {BadSound(0);supHighSpeed = 200;}
            if (supHighSpeed < 30) {BadSound(0);supHighSpeed = 30;}
            break;}
          case 5:{
            supLowSpeed = supLowSpeed + mydir;
            if (supLowSpeed > 150) {BadSound(0);supLowSpeed = 150;}
            if (supLowSpeed < 30) {BadSound(0);supLowSpeed = 30;}
            break;}
          case 6:{ 
            // use bag Sensors
              if (useBagSensors == 0) {
                useBagSensors = 1;
              } else {
                useBagSensors = 0;
              }
              break;
            }
           case 7:{ 
            // no Water mode
              if (noWaterMode == 0) {
                noWaterMode = 1;
              } else {
                noWaterMode = 0;
              }
              break;
            }
          }
        break ;} 
    }
   menuScreenChanged=1;     
    menuTimeoutCounter = 0; // reset the timeoutcounter
    SavePrefs();
  }
}

void SetMessage(int whichBagger, int whichLine, String whichMessage){
  switch (whichLine){
    case 1:{ 
      if(messagesLine1[whichBagger] != whichMessage){
        messagesLine1[whichBagger] = whichMessage;runScreenChanged=1;
      }
      break;}
    case 2:{
      if(messagesLine2[whichBagger] != whichMessage){
        messagesLine2[whichBagger] = whichMessage;runScreenChanged=1;
      }
      break;}
    case 3:{       
      if(messagesLine3[whichBagger] != whichMessage){
        messagesLine3[whichBagger] = whichMessage;runScreenChanged=1;
      }
      break;}
  }
}

void drawRunScreen(void) {
  // rebuild the picture after some delay
 
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setDrawColor(1);
  if(processPaused==1){
    // process pause
    Water(0,0); // ensure water is stopped for sure
    Water(1,0);
    u8g2.setFont(BIGFONT);
    u8g2.drawStr(0, 20, "STOPPED" );
    u8g2.setFont(MAINFONT);
    u8g2.drawStr(1, 40, "Click 2x to resume");
//v13
    u8g2.drawStr(1, 40 + VLEADING, "Twist for menu");
    char tmessage0 [24];
    String("Bags so Far: " + String(currentTotalBagsCounter)).toCharArray(tmessage0,24);
    u8g2.drawStr(1, 40 + VLEADING + VLEADING, tmessage0);
  } else if(currentlyRunning==0){
    u8g2.setFont(BIGFONT);
    u8g2.drawStr(0, 20, "READY?");
    u8g2.setFont(MAINFONT);
    u8g2.drawStr(1, 40, "To Start select");
    u8g2.drawStr(1, 50, "RUN NOW in menu");
  } else if(runScreenChanged==1){
  
    // set percentages
    // DRAW LINE TO DIVIDE SCREEN
    u8g2.setFont(MAINFONT);
    u8g2.drawLine(0,int(SCREENHEIGHT/2),SCREENWIDTH-1,int(SCREENHEIGHT/2));
    // DRAW TRIANGLES
    u8g2.drawTriangle(triangleWidth + 1,int((32-triangleHeight)/2), triangleWidth + 1,int((32-triangleHeight)/2) + triangleHeight, 1,int(((32-triangleHeight)/2) + (triangleHeight/2)));
    u8g2.drawTriangle(SCREENWIDTH - triangleWidth - 1,Bagger2VStart + int((32-triangleHeight)/2), SCREENWIDTH - triangleWidth - 2,Bagger2VStart+ int((32-triangleHeight)/2) + triangleHeight, SCREENWIDTH - 1,Bagger2VStart + int(((32-triangleHeight)/2) + (triangleHeight/2)));
    // DRAW BAG ICONS
    int bagfull1 = int (percentComplete[0] / 4.6);
    int bagfull2 = int (percentComplete[1] / 4.6);
    u8g2.drawBox(triangleWidth + 2,(int((32-triangleHeight)/2) + triangleHeight - 3 -  bagfull1), bagWidth,bagfull1 + 1);
    u8g2.drawBox(SCREENWIDTH - triangleWidth - 3 - bagWidth,(Bagger2VStart+ int((32-triangleHeight)/2) + triangleHeight - bagfull2 - 2), bagWidth,bagfull2 + 1);  

  // DRAW LINES OF TEXT
    if(baggerStepNumber[0]==13){
      // finish screen
      u8g2.setFont(BIGFONT);
      char tmessage9 [24];
      float netWaterDropped = scaleWeights[0] - scaleDryGoodsWeights[0]; // just water that fell
      float totalSubDropped = scaleDryGoodsWeights[0] - scaleFinalSupWeight[0];// dry goods less supplement is substrate net
      float netMoistureInDryGoods = (residualMoistureSub * .01 * totalSubDropped) + (residualMoistureSup * .01 * scaleFinalSupWeight[0]);// percentage of moisture in constituents * what fell
      float moistureNet = (100.0 * (netWaterDropped + netMoistureInDryGoods ))/ scaleWeights[0];// water in dry and added divided by final weight as percent
      messagesLine3[0] = String(scaleWeights[0]) + "#" + String(moistureNet) + "%";
      messagesLine3[0].toCharArray(tmessage9,24);
      u8g2.drawStr(runXInset, 2*VLEADING, tmessage9);
    } else {
       u8g2.setFont(MAINFONT);
      // First Line of Left side
      char tmessage0 [24];
      
      messagesLine1[0] =String(baggerStepNumber[0])+ ": " + String(bagMessages[bagStatus[0]]);
      messagesLine1[0].toCharArray(tmessage0,24);
      u8g2.drawStr(runXInset, VLEADING, tmessage0);
      // Second Line of Left side
      char tmessage2 [24];
      strcpy_P(tmessage2, (PGM_P)pgm_read_word(&subStatusMenu[baggerStepNumber[0]])); // Necessary casts and dereferencing, just copy.             
      messagesLine2[0].toCharArray(tmessage2,24);
      u8g2.drawStr(runXInset, 2*VLEADING, tmessage2);
      char tmessage4 [24];
      if(updateMessages[0]==" "){
        messagesLine3[0] = "W:" + String(scaleWeights[0]) + " " + String(baggerStepProgressPercentage[0]) + "% O:" + String(scaleOverfills[0]) + " of " +  String(realSub);
      } else {
        messagesLine3[0] = updateMessages[0];
      }
      messagesLine3[0].toCharArray(tmessage4,24);
      u8g2.drawStr(runXInset, 3*VLEADING, tmessage4);
    }
//v13
    if(baggerStepNumber[1]==13){
      // finish screen
      u8g2.setFont(BIGFONT);
      char tmessage8 [24];
      float netWaterDropped = scaleWeights[1] - scaleDryGoodsWeights[1]; // just water that fell
      float totalSubDropped = scaleDryGoodsWeights[1] - scaleFinalSupWeight[1];// dry goods less supplement is substrate net
      float netMoistureInDryGoods = (residualMoistureSub * .01 * totalSubDropped) + (residualMoistureSup * .01 * scaleFinalSupWeight[1]);// percentage of moisture in constituents * what fell
      float moistureNet = (100.0 * (netWaterDropped + netMoistureInDryGoods ))/ scaleWeights[1];// water in dry and added divided by final weight as percent
      messagesLine3[1] = String(scaleWeights[1]) + "#" + String(moistureNet) + "%";

      messagesLine3[1].toCharArray(tmessage8,24);
      u8g2.drawStr(runXInset, Bagger2VStart + VLEADING+ VLEADING, tmessage8);
    } else {
       u8g2.setFont(MAINFONT);
      // regular display
      // First line of Right side
      char tmessage1 [24];
      messagesLine1[1] = String(baggerStepNumber[1]) + ": " + String(bagMessages[bagStatus[1]]);
      messagesLine1[1].toCharArray(tmessage1,24);
      u8g2.drawStr(runXInset, Bagger2VStart + VLEADING, tmessage1);
      // Second Line of Right Side
      char tmessage3 [24];
      strcpy_P(tmessage3, (PGM_P)pgm_read_word(&subStatusMenu[baggerStepNumber[1]])); // Necessary casts and dereferencing, just copy.             
      messagesLine2[1].toCharArray(tmessage3,24);
      u8g2.drawStr(runXInset, Bagger2VStart + 2*VLEADING, tmessage3);
      // third line of right side
      if(updateMessages[1]==" "){
        messagesLine3[1] = "W:" + String(scaleWeights[0]) + " " + String(baggerStepProgressPercentage[0]) + "% O:" + String(scaleOverfills[0]) + " of " +  String(realSub);
      } else {
        messagesLine3[1] = updateMessages[1];
      }
      
      char tmessage5 [24];
      messagesLine3[1].toCharArray(tmessage5,24);
      u8g2.drawStr(runXInset, Bagger2VStart + 3*VLEADING, tmessage5);
    }

  }
  u8g2.sendBuffer();
}

void clearOLED(){
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    delay(10);
}




void DrawDisplayAsAlertMessage(int d) {
  // default is running status
  // graphic commands to redraw the complete screen should be placed here  
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setDrawColor(1);
  u8g2.setFont(MAINFONT);
  char tmessage [24];
  statusMessage[0].toCharArray(tmessage,24);
  u8g2.drawStr(0, 30, tmessage);
  statusMessage[1].toCharArray(tmessage,24);
  u8g2.drawStr(0, 30+VLEADING, tmessage);
  statusMessage[2].toCharArray(tmessage,24);
  u8g2.drawStr(0, 30+(2*VLEADING), tmessage);
  u8g2.sendBuffer();
  delay(d);
}


void drawStartup(void) {
  u8g2.begin();
  // graphic commands to redraw the complete screen should be placed here  
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setDrawColor(1);
  u8g2.setFont(BIGFONT);
  u8g2.drawStr(0, 30, "ODIN");
  u8g2.setFont(MAINFONT);
  u8g2.drawStr(0, 50, "Twist knob to start");
  u8g2.drawStr(0, 60, "v18 by Brad Kuhns");
  u8g2.sendBuffer();
}
