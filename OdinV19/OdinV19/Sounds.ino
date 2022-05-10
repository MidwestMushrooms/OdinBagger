void GoodSound(boolean myWhichSide) {
  // bo bo beep
  tone(buzzer, lowNote * (1 + myWhichSide) , 25);
  delay(30);
  tone(buzzer, lowNote * (1 + myWhichSide), 15);
  delay(15);
  tone(buzzer, lowNote * (1 + myWhichSide) * 2, 200);
}
void BadSound(boolean myWhichSide) {
  // duh duuuuh
  tone(buzzer, noteG * (1 + myWhichSide), 35);
  delay(50);
  tone(buzzer, ((1 + myWhichSide) * noteG) / 2, 70);
}

void BagMissingSound(boolean myWhichSide) {
  if(serialActive==1){Serial.println("function BagMissingSound " + String(myWhichSide));}
  // v13 change make it just stop one side not both
  Water(myWhichSide, 0); // stop water
  StopMotors(myWhichSide); // stop motors

  tone(buzzer, ((1 + myWhichSide) * noteG) / 2, 2);
//  delay(50);
  boolean opside = 0; // assuming side is 1
  if (myWhichSide == 0) { //but if it wasn't
    opside = 1; // set to the opposite side
  }
}

void Valkyyries(){
  tone(buzzer,(noteD0 * keyChange),hs);delay(hs* nL);
  //////////////////
  tone(buzzer,(noteG * keyChange),fs);delay(int(fs* nL));
  tone(buzzer,(noteD0 * keyChange),qs);delay(int(qs* nL));
  tone(buzzer,(noteG * keyChange),hs);delay(int(hs* nL));
  tone(buzzer,(noteB * keyChange),fs *1.25);delay(int(fs* nL * 1.25));
  tone(buzzer,(noteG * keyChange),fs * 1.5);delay(int(fs* nL * 1.5));
  //////////////////  
  tone(buzzer,(noteB * keyChange),hs);delay(int(hs* nL));
  tone(buzzer,(noteG * keyChange),qs);delay(int(qs* nL));
  tone(buzzer,(noteB * keyChange),hs);delay(int(hs* nL));
  tone(buzzer,(noteD * keyChange),fs * 1.25);delay(int(fs* nL * 1.25));
  tone(buzzer,(noteB * keyChange),fs* 1.5);delay(int(fs* nL* 1.5));
  //////////////////
  tone(buzzer,(noteD * keyChange),hs);delay(int(hs* nL));
  tone(buzzer,(noteB * keyChange),qs);delay(int(qs* nL));
  tone(buzzer,(noteD * keyChange),hs);delay(int(hs* nL));
  tone(buzzer,(noteF2 * keyChange),fs * 1.25);delay(int(fs* nL * 1.25));
  tone(buzzer,(noteF * keyChange),fs* 1.5);delay(int(fs* nL* 1.5));
  //////////////////
  tone(buzzer,(noteB * keyChange),hs);delay(int(hs* nL));
  tone(buzzer,(noteF * keyChange),qs);delay(int(qs* nL));
  tone(buzzer,(noteB * keyChange),qs);delay(int(qs* nL));
  tone(buzzer,(noteD * keyChange),fs*1.5);
}
