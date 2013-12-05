// http://jeelabs.net/
// http://opensource.org/licenses/mit-license.php
// digiRead() & digiWrite() - ports D pin
// anaRead() & digiWrite2() - Ports A pin 
// mode sets Digital pin mode2 sets ports Analog pin
// 11/21/13 Todd Miller GreenhouseRX8

#include <JeeLib.h>
#include <PortsLCD.h>
#include <avr/sleep.h>

Port ledOne (1);
Port ledTwo (2);
Port ledThree (3);
PortI2C myI2C (4);
LiquidCrystalI2C lcd (myI2C);
int temp;
int data;
byte y =0;
byte r = 0;
byte threshold= 34;
int backlightSwitch;
int tmpHigh;
int tmpLow;
byte tmp=0;
int pkg1=0;
int pkg2=0;
int pkg3=0;
int yCount = 0;

unsigned long interval = 150000;

unsigned long previousMillis;

#define NOTE_C4  262

int melody2[] = {NOTE_C4, NOTE_C4, NOTE_C4};

int noteDurations[] = {4, 4, 4 };

void setup () {
  lcd.begin(20, 4);
  homeScreen();
  ledOne.mode(OUTPUT);
  ledTwo.mode(INPUT); // digital pin for backlight switch
  ledTwo.mode2(OUTPUT); // analog pin for led
  ledThree.mode(OUTPUT);
  Serial.begin(57600);
  rf12_initialize('R', RF12_433MHZ, 100);
  ledOne.digiWrite(1);
  ledTwo.digiWrite(1);
  ledTwo.digiWrite2(0);
  ledThree.digiWrite(0);
  
  //test only - remove when working
   for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000/noteDurations[thisNote];
      tone(4, melody2[thisNote],noteDuration);//pin 4 or port 1 digital pin
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(4);
    }
}

void loop () {
  noTone(4);
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();
  unsigned long currentMillis = millis();
  backlightSwitch=ledTwo.digiRead();
  if (backlightSwitch == LOW)
    lcd.noBacklight();
  else
    lcd.backlight();

  if (currentMillis-previousMillis >interval){
    yellowLed();
    previousMillis=currentMillis;
  }

  if (rf12_recvDone() && rf12_crc == 0) {
     previousMillis=currentMillis;

    if (tmp==0)
    {
      tmp=1;
      tmpLow=rf12_data[0];
      tmpHigh=rf12_data[0];
    }
    int* data = (int*) rf12_data;
    pkg1++;
    if (pkg1>999){
      pkg1=0;
      pkg2++;
      if(pkg2>999){
        pkg2=0;
        pkg3++;
      }
    }

    if(rf12_data[0] <tmpLow)
      tmpLow=rf12_data[0];

    if(rf12_data[0] >tmpHigh)
      tmpHigh=rf12_data[0];

    if(rf12_data[0]<=threshold)
    {
      redLed();
    }
    // process data here

    else {
      greenLed();
      lcd.setCursor(14, 2);
      lcd.print(rf12_data[0]);
      lcd.setCursor(4, 1);
      lcd.print(pkg1);
      lcd.setCursor(12, 1);
      lcd.print(pkg2);
      lcd.setCursor(18, 1);
      lcd.print(yCount);
      lcd.setCursor(6, 3);
      lcd.print(tmpHigh);
      lcd.setCursor(15, 3);
      lcd.print(tmpLow);
      // lcd.print((int) rf12_hdr);
    }

    if (RF12_WANTS_ACK)
      rf12_sendStart(RF12_ACK_REPLY,0,0);
  }
}

void greenLed()
{
  ledOne.digiWrite(1);
  ledTwo.digiWrite2(0);
  ledThree.digiWrite(0); 

  if (y==1 || r==1){
    homeScreen(); 
    y=0; 
    r=0;
  }
}

void yellowLed()
{
  ledOne.digiWrite(0);
  ledTwo.digiWrite2(1);
  ledThree.digiWrite(0);
  if(y==0 || r==1)
    yCount++;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Check sensor!"));
  lcd.setCursor(0, 1);
  lcd.print(F("Battery may need"));
  lcd.setCursor(0, 2);
  lcd.print(F("recharging."));
  y=1;
  r=0;

}

void redLed()
{
  ledOne.digiWrite(0);
  ledTwo.digiWrite2(0);
  ledThree.digiWrite(1);
  if(r==0 || y==1){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Check greenhouse!"));
    lcd.setCursor(0, 1);
    lcd.print(F("Current Temp:"));
    lcd.setCursor(14, 1);
    lcd.print(rf12_data[0]);
    lcd.setCursor(17, 1);
    lcd.print(F("F"));
    r=1;
    y=0;

    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000/noteDurations[thisNote];
      tone(4, melody2[thisNote],noteDuration);//pin 4 or port 1 digital pin
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(4);
    }
  }
}

void homeScreen()
{
  lcd.clear();
  lcd.print(F("Snowdrift Farms"));
  lcd.setCursor(0, 1);
  lcd.print(F("P1:"));
  lcd.setCursor(8, 1);
  lcd.print(F("P2:"));
  lcd.setCursor(15, 1);
  lcd.print(F("Y:"));
  lcd.setCursor(0, 2);
  lcd.print(F("Current Temp: "));
  lcd.setCursor(0, 3);
  lcd.print(F("High:"));
  lcd.setCursor(10, 3);
  lcd.print(F("Low:"));
}



