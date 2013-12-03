// http://jeelabs.net/
//http://opensource.org/licenses/mit-license.php
//11/21/13 Todd Miller GreenhouseTX6

#include <JeeLib.h>
#include <avr/sleep.h>
Port tmp (2);
int value;
int radioIsOn=1;
MilliTimer readoutTimer, aliveTimer;

void setup() {
  tmp.mode2(INPUT);
  rf12_initialize(1, RF12_433MHZ, 100);
  rf12_easyInit(15); // every 10 seconds send out pkg
  Serial.begin(57600);
  Serial.println(F("Initializing..."));
  delay(45);//wait for power to settle
}

void loop() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();
  if (radioIsOn && rf12_easyPoll() == 0) {
    rf12_sleep(0); // turn the radio off
    radioIsOn = 0;
  }

  //rf12_easyPoll();
  value = tmp.anaRead();
  calcTemp();

  byte sending = rf12_easySend(&value, sizeof value);
  //rf12_easySend(&value, sizeof value);
  if (sending){
    Serial.print(F("Header = "));
    Serial.print((int) rf12_hdr);
    Serial.print(F(" Temp: "));
    Serial.print(rf12_data[0]);
    Serial.println(F(" F"));
    Serial.println(F(" --------------------"));
  }

  if (aliveTimer.poll(60000)){
    sending = rf12_easySend(0, 0); // always returns 1
    Serial.println(F("Sending 'Alive' Message"));
  }
  if (sending) {
    // make sure the radio is on again
    if (!radioIsOn)
      rf12_sleep(-1); // turn the radio back on
    radioIsOn = 1;
  }
}
void calcTemp(){
  for (int i=0; i <= 3; i++){
    value = value + tmp.anaRead();
    delay(2);
  } 
  value = (value/5);
  float voltage = value * 3.3;
  voltage /= 1024.0; 
  float temperatureC = (voltage - 0.5) * 100 ; 
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  value=temperatureF;
}

