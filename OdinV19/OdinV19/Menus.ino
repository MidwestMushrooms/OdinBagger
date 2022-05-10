void exitMenus(){
  menuMode=0; 
  subMenuNumber=0; 
  selectedMenuLength = menuLevel1Length; 
  MenuSelectedItem=0; 
  menuScreenChanged=0; // turn menu off and resume
  runScreenChanged=1;
}

void DrawMenu() {
  // menuColumn is which level of menu we are in 
  // subMenuNumber is which item within the menu we are in   
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(MAINFONT); 
  int vScroll=0; // normally not scrolled.
  int vPos=VLEADING;
   if(MenuSelectedItem >= int(SCREENITEMS)){
    vScroll = int((MenuSelectedItem - int(SCREENITEMS)) * VLEADING);
  }
  int xPos=5;
  String suffix = "";
  String valueSetting="";
  String units="";
  float tempweight = weightSetValue;
  int dec = int(tempweight) % 10;
  int whole = round(tempweight / 10);
  String Qmode = " ";
  if (quickMode == 1) {
    Qmode = "Q";
  }
  String Vmode = " ";
  if (vibrationMode == 1) {
    Vmode = "V";
  }
  String tstring = "W " +  String( 1.0 * weightSetValue / 10) + "# S" + String(supplementSetValue) + "% M" + String(moistureSetValue) + "% " + String(Vmode) + String(Qmode);
  char tmessage3 [24];
  tstring.toCharArray(tmessage3,24);
      selectedMenuLength=menuLevel1Length;

    switch (menuColumn){
    case (0,1): { // main Menu
      selectedMenuLength=menuLevel1Length;
    }
    case 2: {
      switch (subMenuNumber) {
          case 0: {break;}
          case 2: {selectedMenuLength=menuLevel2SetupLength;break;}
          case 3: {selectedMenuLength=menuLevel2RecipeLength;break;}
          case 4: {selectedMenuLength=menuLevel2PreferencesLength;break;}
          case 5: {selectedMenuLength=menuLevel2TestLength;break;}
        }
      }
    case 3:{ ////////////////////// tweaking a value
        switch (subMenuNumber) {
          case 0: {break;}
          case 2: { // Setup Menu
            switch (MenuSelectedItem - 1) {
              case 0:  { 
                // calibration 
                suffix = "pounds";
                  if (menuLevel3CalibrationIndex <= 1) {
                    valueSetting = "3 pounds"; // default 
                    scaleCalibrationTarget=3.000;
                  } else if (menuLevel3CalibrationIndex == 1) {
                    valueSetting = "3 pounds";
                    scaleCalibrationTarget=3.000;
                  } else if (menuLevel3CalibrationIndex == 2) {
                    valueSetting = "5 pounds";
                    scaleCalibrationTarget=5.000;
                  } else if (menuLevel3CalibrationIndex == 3) {
                    valueSetting = "6 pounds";
                    scaleCalibrationTarget=6.000;
                  } else if (menuLevel3CalibrationIndex == 4) {
                    valueSetting = "10 pounds";
                    scaleCalibrationTarget=10.000;
                  } else if (menuLevel3CalibrationIndex >= 5) {
                    valueSetting = "12 pounds";
                    scaleCalibrationTarget=12.000;
                  }          
                break;}
                default: { break;}
              }; 
            break;}
          case 3: {
            suffix = menuLevel2Recipe[(MenuSelectedItem)];
            units = menuLevel2RecipeUnits[(MenuSelectedItem)];
            switch (MenuSelectedItem - 1) {
              case 0: {valueSetting= String(moistureSetValue);break;}
              case 1: {int tempweight = weightSetValue;int dec = int(tempweight) % 10;int whole = round(tempweight / 10);valueSetting= String(whole) + "." + String(dec); break;}
              case 2: {valueSetting=String(supplementSetValue);break;}
              case 3: {valueSetting= String(residualMoistureSub); break;}
              case 4: {valueSetting= String(residualMoistureSup); break;}
              case 5: {
                suffix="SubStrate Type:";
                if (substrateType == 0) {
                  valueSetting="SAWDUST";
                } else if(substrateType == 1) {
                  valueSetting="PELLET";
                } else if(substrateType == 2){
                  valueSetting="COMBOP";
                } else {
                  // rolled backwards
                  substrateType=2;valueSetting="COMBOP";
                }
                break;}
              case 6: {
                suffix="Vibration Mode:";
                valueSetting= "OFF";
                if(vibrationMode){
                  valueSetting="ON";
                }
                break;}
            }
             
            break;}
          case 4: {
            switch (MenuSelectedItem -1) {
              case 0: {suffix = "Sub Density Factor:";valueSetting=String(subDensityFactor[0]);break;}// "Set SbDenF:"
              case 1: {suffix = "Sup Density Factor:";valueSetting=String(supDensityFactor[0]);break;}// "Set SpDenF:"
              case 2: {suffix = "Sub High Speed Limit:";valueSetting=String(pelletHighSpeed); break;}// "Set SubHi:"
              case 3: {suffix = "Sub Low Speed Limit:";valueSetting=String(pelletLowSpeed);break;}//, "Set SubLo:"
              case 4: {suffix = "Sup High Speed Limit:";valueSetting=String(supHighSpeed); break;}// "Set SupHi:"
              case 5: {suffix = "Sup Low Speed Limit:";valueSetting=String(supLowSpeed); break;}// "Set SupLo:"
              case 6: {suffix = "Use Bag Sensors:";
                 // v15         
                if(useBagSensors == 1){
                    valueSetting="YES Use";
                  } else {
                    valueSetting="NO Ignore";
                  }
                break;}
              case 7: {suffix = "No Water Mode:";
                 // v18         
                if(noWaterMode == 1){
                    valueSetting="No Water";
                  } else {
                    valueSetting="Use Water";
                  }
                break;}
            }
            break;}
          case 5: {
            break;}
          }        
    }
  }

  if(menuColumn <3 ){  
    for (int i = 0; i < selectedMenuLength && i < 50; i++) {
      char tmessageMenu [24];
      getMenuItem(i).toCharArray(tmessageMenu,24);
      u8g2.drawStr(xPos, vPos - vScroll, tmessageMenu);
      if(i==MenuSelectedItem){
        u8g2.drawFrame(2,vPos - VLEADING - vScroll,(128-xPos-xPos),(VLEADING+2));
      }
      vPos = vPos + VLEADING; // increment the menu position
    } 
  } else {
    // tweaking a value
    char tempChar [24];
    
    // show the big value
    valueSetting.toCharArray(tempChar,9);
    u8g2.setFont(BIGFONT);
    int hOffset=0;
    hOffset = int(u8g2.getStrWidth(tempChar)/2);
    u8g2.drawStr(60 - hOffset, vPos + (5*VLEADING), tempChar);

    u8g2.setFont(MAINFONT);
    tstring.toCharArray(tempChar,24);
    u8g2.drawStr(5, vPos, tempChar);

    String tLabel = "Adjust with knob:";
    tLabel.toCharArray(tempChar,24);
    u8g2.drawStr(5, vPos + (1*VLEADING), tempChar);


    suffix.toCharArray(tempChar,24);
    u8g2.drawStr(5, vPos + (2*VLEADING), tempChar);

    units.toCharArray(tempChar,24);
    u8g2.drawStr(hOffset + 60 + 3 , vPos + (4*VLEADING) + 3, tempChar);

  }
  u8g2.sendBuffer();
  menuScreenChanged = 0;
}



void ExitMenus() {
  clearOLED();
  //  processPaused=0; // resume
  menuColumn = 0; // exit function
  subMenuNumber=0; // reset the choice of sub menu
  
}
