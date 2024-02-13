//#include <LiquidCrystal.h>


#include <Wire.h>
#include <inttypes.h>
#include <LCDi2cW.h>
LCDi2cW lcd = LCDi2cW(4, 20, 0x4C, 0);

uint8_t rows = 4;
uint8_t cols = 20;

uint8_t timeset = 0;
int clearneeded;

//LiquidCrystal lcd(12,11,10,9,8,7);
bool ledState = LOW;
volatile bool ppsTriggered = false;
char serialBuffer[100];

///////////////////////
void ppsHandler(void);

void ppsHandler(void)
{
  ppsTriggered = true;
}
//////////////////////////////////
void pulseTheTime(int);


void pulseTheTime(long counter) {
  digitalWrite(4, HIGH); //pulse the reset output
  delay(10);
  digitalWrite(4, LOW);
  for (int x = 0; x < counter; x++) {  //count up the time for right now.
    //digitalWrite(5,HIGH);
    //delay(1);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(5, LOW);

  }
  return;
}


/////////////////////////////////////////////////////////////////////////
void setup() {

  //lcd.begin(20, 2);
  lcd.init();
  lcd.clear();
  lcd.print(" ---[RuhNet GPS]---");
  lcd.setCursor(0, 1);
  //lcd.write("PROGRAM STARTING...");
  lcd.print("PROGRAM STARTING...");
  Serial.begin(4800);
  pinMode(13, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(5, LOW);




  //pulseTheTime(1000);







  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), ppsHandler, RISING);
  for (int x = 0; x < 20; x++) {
    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);
    delay(10);
  }
  lcd.clear();

}


//////////////////////////////////////////////
void loop() {

  lcd.setCursor(0, 0);
  if (ppsTriggered) {
    ppsTriggered = false;
    ledState = !ledState;
    digitalWrite(13, ledState);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(5, LOW);

    //digitalWrite(7, ledState);
  }

  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(400);

    while (Serial.available() > 0) {
      for (int n = 0; n < 100; n++) {
        serialBuffer[n] = Serial.read();
      }
    }
    String gpsString = serialBuffer;
    char gpsValid = gpsString.charAt(18);
    lcd.print(gpsString.substring(7, 9));
    lcd.print(":");
    lcd.print(gpsString.substring(9, 11));
    lcd.print(":");
    lcd.print(gpsString.substring(11, 13));
    lcd.print(" UTC");

    lcd.print(" ruel.io");







    String timestring = gpsString.substring(7, 9) + gpsString.substring(9, 11) + gpsString.substring(11, 13);
    //        int timeint = (gpsString.substring(7,9) + gpsString.substring(9, 11) + gpsString.substring(11, 13)).toInt();
    long timeint = timestring.toInt();
    long advance = 0;

    if (!timeset) {
      advance = 2;
      pulseTheTime(advance);
      advance = timeint;
      pulseTheTime(advance);
      timeset = 1;
    }

    if (gpsString.substring(11, 13) == "00") {
      advance = 40;
      pulseTheTime(advance);
    }
    if (gpsString.substring(9, 11) == "00") {
      advance = 4040;
      pulseTheTime(advance);
    }


    //String test = gpsString.substring(11,13);

    lcd.setCursor(0, 3);
    //lcd.print(timeint);

    //clearneeded = 0;

    clearneeded--;

    if (advance > 0) {
      lcd.setCursor(0, 3);
      lcd.print("Advance Pulse:");
      clearneeded = 2;
      lcd.print(advance);
    } else if (!clearneeded) {
      lcd.print("GPS SYNC TIME");
      lcd.print("   ");
      lcd.write(127);
      lcd.print("  |");
      //lcd.print(gpsString.substring(11,13));
    }
    lcd.setCursor(0, 2);





    if (gpsValid == 'V') {
      lcd.print("  NO POSITION LOCK  ");
      lcd.setCursor(0, 1);
      if (ledState) {
        lcd.print("--**<RuhNet GPS>**--");
      } else {
        lcd.print("Searching for GPS...");
      }
      // digitalWrite(8,LOW); //lock indicator LED
    } else {
      lcd.print(" GPS LOCK");
      lcd.setCursor(0, 1);
      lcd.print(gpsString.substring(20, 22));
      lcd.print(".");
      lcd.print(gpsString.substring(22, 24));
      lcd.print(gpsString.substring(25, 27));
      lcd.print("N ");
      lcd.print(gpsString.substring(33, 35));
      lcd.print(".");
      lcd.print(gpsString.substring(35, 37));
      lcd.print(gpsString.substring(38, 40));
      lcd.print("W ");
      lcd.print(gpsString.substring(40, 42));
      //digitalWrite(8,HIGH); //lock indicator LED
    }
  }



}

