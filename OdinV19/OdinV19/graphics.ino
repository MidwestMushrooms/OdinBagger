void testLEDs(){
  AllLightsOff();
  switch(testLEDindex){
    case 0:{setLight(0, "green", 1);  delay(260);break;}
    case 1:{setLight(0, "red", 1);  delay(260);break;}
    case 2:{setLight(1, "green", 1);  delay(260);break;}
    case 3:{setLight(1, "red", 1);  delay(260);break;}
    case 4:{
      tone(buzzer, noteG, 150);
      setLight(0, "green", 1);
      setLight(1, "green", 1);
      setLight(0, "red", 1);
      setLight(1, "red", 1);
      delay(10);
      break;}
    default: {break;}
  }
  testLEDindex++;
  
  if(testLEDindex==5){
    currentlyRunning=0;currentlyRunningWaterProcessId=0;menuColumn=0;testLEDindex=0; // v19 fix to reset the counter once it's done.
  }
  AllLightsOff();
}

void BlinkAllLightsWarning(){
  for ( int i = 0; i < 4; i++ ){
    tone(buzzer, noteA * 4, 100);
    AllLightsOn();
    AllLightsOff();
    delay(100);
  }
  delay(500);
}
void AllLightsOff(){
  setLight(0, "green",0);
  setLight(0, "red",0);
  setLight(1, "green",0);
  setLight(1, "red",0);
}
void AllLightsOn(){
  setLight(0, "green",1);
  setLight(0, "red",1);
  setLight(1, "green",1);
  setLight(1, "red",1);
}
void setLight(int whichBagger, String whichLight, bool lightCommand){
//  if(serialActive==6 && false){
//      Serial.println("f setLight " + String(whichBagger) + ",whichLight=" + String(whichLight) 
//      + ",lightCommand=" + String(lightCommand));   
//  };

  if(whichLight=="green"){
    if(lightCommand==0){
      if(greenLightStatus[whichBagger]==1){
        if(whichBagger==0){
          digitalWrite(LEDGreenL, LOW);
        } else {
          digitalWrite(LEDGreenR, LOW);
        }
//        digitalWrite(LEDGreenPins[whichBagger], LOW);
        greenLightStatus[whichBagger]=0;
      }
    } else {
      if(greenLightStatus[whichBagger]==0){
        if(whichBagger==0){
          digitalWrite(LEDGreenL, HIGH);
        } else {
          digitalWrite(LEDGreenR, HIGH);
        }
//        digitalWrite(LEDGreenPins[whichBagger], HIGH);
        greenLightStatus[whichBagger]=1;
      }
    }
  } else {
    // red
    if(lightCommand==0){
      if(redLightStatus[whichBagger]==1){
        if(whichBagger==0){
          digitalWrite(LEDRedL, LOW);
        } else {
          digitalWrite(LEDRedR, LOW);
        }
        redLightStatus[whichBagger]=0;
      }
    } else {
      if(redLightStatus[whichBagger]==0){
        if(whichBagger==0){
          digitalWrite(LEDRedL, HIGH);
        } else {
          digitalWrite(LEDRedR, HIGH);
        }
        redLightStatus[whichBagger]=1;
      }
    }
  }
}



