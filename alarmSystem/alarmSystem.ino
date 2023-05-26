#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define buzzer 13
#define trigPin 10
#define echoPin 11

long duration;
int distance, initialDistance, currentDistance, i;
int screenOffMsg =0;
String password="1234";
String tempPassword;
boolean activated = false; // State of the alarm
boolean isActivated;
boolean activateAlarm = false;
boolean alarmActivated = false;
boolean enteredPassword; // State of the entered password to stop the alarm
boolean passChangeMode = false;
boolean passChanged = false;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keypressed;
//define the cymbols on the buttons of the keypads
char keyMap[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {5, 4, 3, 2};  //Row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //Column pinouts of the keypad

Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 

void setup() { 
  Serial.begin(9600);  // Set the baud rate to 9600 (same as Serial Monitor)

  lcd.begin(); 
  pinMode(buzzer, OUTPUT); // Set buzzer as an output
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop() {

  if (activateAlarm) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alarm will be");
    lcd.setCursor(0,1);
    lcd.print("activated in");
   
    int countdown = 9; // 9 seconds count down before activating the alarm
    while (countdown != 0) {
      lcd.setCursor(13,1);
      lcd.print(countdown);
      countdown--;
      tone(buzzer, 700, 100);
      delay(1000);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alarm Activated!");
    initialDistance = getDistance();
    activateAlarm = false;
    alarmActivated = true;
  }

  if (alarmActivated == true){
      currentDistance = getDistance() + 10;
      if ( currentDistance < initialDistance) {
        Despacito();
        lcd.clear();
        enterPassword();
       
      }
    }

  if (!alarmActivated) {
    if (screenOffMsg == 0 ){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A - Activate");
      lcd.setCursor(0,1);
      lcd.print("B - Change Pass");
      screenOffMsg = 1;
    }
    keypressed = myKeypad.getKey();
     if (keypressed =='A'){        //If A is pressed, activate the alarm
      tone(buzzer, 1000, 200);
      activateAlarm = true;            
    }
    else if (keypressed =='B') {
      lcd.clear();
      int i=1;
      tone(buzzer, 2000, 100);
      tempPassword = "";
      lcd.setCursor(0,0);
      lcd.print("Current Password");
      lcd.setCursor(0,1);
      lcd.print(">");
      passChangeMode = true;
      passChanged = true;   
      while(passChanged) {      
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY){
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
            keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
            keypressed == '8' || keypressed == '9' ) {
         tempPassword += keypressed;
         lcd.setCursor(i,1);
         lcd.print("*");
         i++;
         tone(buzzer, 2000, 100);
        }
      }
      if (i > 5 || keypressed == '#') {
        tempPassword = "";
        i=1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Current Password");
        lcd.setCursor(0,1);
        lcd.print(">"); 
      }
      if ( keypressed == '*') {
        i=1;
        tone(buzzer, 2000, 100);
        if (password == tempPassword) {
          tempPassword="";
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Set New Password");
          lcd.setCursor(0,1);
          lcd.print(">");
          while(passChangeMode) {
            keypressed = myKeypad.getKey();
            if (keypressed != NO_KEY){
              if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
                  keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
                  keypressed == '8' || keypressed == '9' ) {
                tempPassword += keypressed;
                lcd.setCursor(i,1);
                lcd.print("*");
                i++;
                tone(buzzer, 2000, 100);
              }
            }
            if (i > 5 || keypressed == '#') {
              tempPassword = "";
              i=1;
              tone(buzzer, 2000, 100);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Set New Password");
              lcd.setCursor(0,1);
              lcd.print(">");
            }
            if ( keypressed == '*') {
              i=1;
              tone(buzzer, 2000, 100);
              password = tempPassword;
              passChangeMode = false;
              passChanged = false;
              screenOffMsg = 0;
            }            
          }
        }
      }
    }
   }
 }
}

void enterPassword() {
  int k=5;
  tempPassword = "";
  activated = true;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" *** ALARM *** ");
  lcd.setCursor(0,1);
  lcd.print("Pass>");
      while(activated) {
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY){
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
            keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
            keypressed == '8' || keypressed == '9' ) {
          tempPassword += keypressed;
          lcd.setCursor(k,1);
          lcd.print("*");
          k++;
        }
      }
      if (k > 9 || keypressed == '#') {
        tempPassword = "";
        k=5;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" *** ALARM *** ");
        lcd.setCursor(0,1);
        lcd.print("Pass>");
      }
      if ( keypressed == '*') {
        if ( tempPassword == password ) {
          activated = false;
          alarmActivated = false;
          noTone(buzzer);
          screenOffMsg = 0; 
        }
        else if (tempPassword != password) {
          lcd.setCursor(0,1);
          lcd.print("Wrong! Try Again");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" *** ALARM *** ");
          lcd.setCursor(0,1);
          lcd.print("Pass>");
        }
      }    
    }
}
// Custom function for the Ultrasonic sensor
long getDistance(){
  //int i=10;
  
  //while( i<=10 ) {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration*0.034/2;
  //sumDistance += distance;
  //}

  //int averageDistance= sumDistance/10;
  return distance;

}

void Despacito() {

  tone(buzzer
, 587, 709.720327982);
  delay(788.578142202);
  delay(10.3082110092);
  tone(buzzer
, 554, 709.720327982);
  delay(788.578142202);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 369, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 391, 412.843850917);
  delay(458.715389908);
  delay(5.15410550459);

  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);

  tone(buzzer
, 440, 412.843850917);
  delay(458.715389908);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 440, 41.7482545872);
  delay(46.3869495413);
  delay(36.0787385321);
  tone(buzzer
, 440, 37.109559633);
  delay(41.2328440367);

  delay(30.9246330275);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);
  delay(30.9246330275);
  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);
  delay(20.6164220183);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 554, 691.165548165);
  delay(767.961720183);
  delay(314.40043578);
  tone(buzzer
, 587, 552.004699541);
  delay(613.338555046);
  delay(5.15410550459);
  tone(buzzer
, 554, 552.004699541);
  delay(613.338555046);
  delay(5.15410550459);

  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 369, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 46.3869495413);
  delay(51.5410550459);
  delay(30.9246330275);
  tone(buzzer
, 493, 46.3869495413);
  delay(51.5410550459);
  delay(20.6164220183);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 391, 273.683002294);
  delay(304.092224771);
  delay(159.777270642);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 391, 41.7482545872);
  delay(46.3869495413);
  delay(30.9246330275);
  tone(buzzer
, 391, 37.109559633);
  delay(41.2328440367);

  delay(36.0787385321);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 440, 412.843850917);
  delay(458.715389908);
  delay(5.15410550459);

  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 440, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);
  tone(buzzer
, 440, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(30.9246330275);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(10.3082110092);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(314.40043578);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 412.843850917);
  delay(458.715389908);
  delay(5.15410550459);

  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 739, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 412.843850917);
  delay(458.715389908);
  delay(469.023600917);
  tone(buzzer
, 783, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 125.244763761);
  delay(139.160848624);
  delay(36.0787385321);
  delay(134.006743119);
  tone(buzzer
, 783, 4.63869495413);
  delay(5.15410550459);
  tone(buzzer
, 880, 129.883458716);
  delay(144.314954128);
  delay(5.15410550459);
  tone(buzzer
, 880, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 739, 111.328678899);
  delay(123.69853211);
  delay(958.663623853);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 880, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 739, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 554, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 273.683002294);
  delay(304.092224771);
  delay(185.547798165);
  tone(buzzer
, 659, 199.463883028);
  delay(221.626536697);
  delay(25.7705275229);

  delay(206.164220183);
  tone(buzzer
, 739, 4.63869495413);
  delay(5.15410550459);
  tone(buzzer
, 659, 171.631713303);
  delay(190.70190367);
  tone(buzzer
, 739, 190.186493119);
  delay(211.318325688);
  delay(41.2328440367);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 412.843850917);
  delay(458.715389908);
  delay(314.40043578);

  tone(buzzer
, 783, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 783, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 739, 552.004699541);
  delay(613.338555046);
  delay(469.023600917);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 41.7482545872);
  delay(46.3869495413);
  delay(30.9246330275);
  tone(buzzer
, 739, 37.109559633);
  delay(41.2328440367);
  delay(36.0787385321);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(190.70190367);
  tone(buzzer
, 880, 78.8578142202);
  delay(87.619793578);
  delay(36.0787385321);
  tone(buzzer
, 880, 78.8578142202);
  delay(87.619793578);
  delay(30.9246330275);
  tone(buzzer
, 783, 190.186493119);
  delay(211.318325688);

  delay(25.7705275229);
  tone(buzzer
, 739, 190.186493119);
  delay(211.318325688);
  delay(36.0787385321);
  tone(buzzer
, 659, 807.132922018);
  delay(896.814357798);
  delay(201.010114679);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 32.4708646789);
  delay(36.0787385321);
  delay(41.2328440367);

  tone(buzzer
, 587, 32.4708646789);
  delay(36.0787385321);
  delay(41.2328440367);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);
  delay(159.777270642);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);
  delay(30.9246330275);
  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);
  delay(20.6164220183);
  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);
  delay(159.777270642);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 554, 27.8321697248);
  delay(30.9246330275);
  delay(41.2328440367);

  tone(buzzer
, 554, 27.8321697248);
  delay(30.9246330275);
  delay(51.5410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);
  delay(159.777270642);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 41.7482545872);
  delay(46.3869495413);
  delay(30.9246330275);
  tone(buzzer
, 587, 41.7482545872);
  delay(46.3869495413);
  delay(30.9246330275);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(30.9246330275);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(10.3082110092);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 51.0256444954);
  delay(56.6951605505);
  delay(25.7705275229);
  tone(buzzer
, 493, 46.3869495413);
  delay(51.5410550459);
  delay(20.6164220183);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);

  delay(5.15410550459);
  tone(buzzer
, 493, 64.9417293578);
  delay(72.1574770642);
  delay(314.40043578);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 659, 46.3869495413);
  delay(51.5410550459);
  delay(30.9246330275);
  tone(buzzer
, 659, 46.3869495413);
  delay(51.5410550459);
  delay(20.6164220183);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 659, 60.3030344037);
  delay(67.0033715596);
  delay(20.6164220183);
  tone(buzzer
, 659, 60.3030344037);
  delay(67.0033715596);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 51.0256444954);
  delay(56.6951605505);

  delay(25.7705275229);
  tone(buzzer
, 659, 51.0256444954);
  delay(56.6951605505);
  delay(15.4623165138);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 440, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 587, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);

  tone(buzzer
, 587, 64.9417293578);
  delay(72.1574770642);
  delay(82.4656880734);
  tone(buzzer
, 587, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);
  tone(buzzer
, 587, 64.9417293578);
  delay(72.1574770642);
  delay(82.4656880734);
  tone(buzzer
, 587, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);

  tone(buzzer
, 587, 64.9417293578);
  delay(72.1574770642);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 880, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 739, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 166.993018349);
  delay(185.547798165);
  delay(77.3115825688);
  tone(buzzer
, 659, 125.244763761);
  delay(139.160848624);
  delay(72.1574770642);
  tone(buzzer
, 659, 129.883458716);
  delay(144.314954128);
  delay(92.7738990826);
  tone(buzzer
, 659, 143.799543578);
  delay(159.777270642);
  delay(97.9280045872);
  tone(buzzer
, 587, 148.438238532);
  delay(164.931376147);
  delay(82.4656880734);

  tone(buzzer
, 659, 259.766917431);
  delay(288.629908257);
  delay(15.4623165138);
  tone(buzzer
, 587, 148.438238532);
  delay(164.931376147);
  delay(15.4623165138);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(20.6164220183);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(20.6164220183);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);

  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 106.689983945);
  delay(118.544426606);
  delay(36.0787385321);
  tone(buzzer
, 587, 106.689983945);
  delay(118.544426606);
  delay(36.0787385321);

  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 369, 83.4965091743);
  delay(92.7738990826);
  delay(61.849266055);
  tone(buzzer
, 369, 83.4965091743);
  delay(92.7738990826);
  delay(61.849266055);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);
  delay(20.6164220183);
  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);

  delay(30.9246330275);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);
  delay(20.6164220183);
  tone(buzzer
, 587, 46.3869495413);
  delay(51.5410550459);
  delay(30.9246330275);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(30.9246330275);
  tone(buzzer
, 554, 125.244763761);
  delay(139.160848624);
  delay(56.6951605505);
  tone(buzzer
, 554, 111.328678899);
  delay(123.69853211);
  delay(51.5410550459);
  tone(buzzer
, 554, 115.967373853);
  delay(128.852637615);
  delay(61.849266055);
  tone(buzzer
, 554, 125.244763761);
  delay(139.160848624);
  delay(87.619793578);

  tone(buzzer
, 554, 106.689983945);
  delay(118.544426606);
  delay(87.619793578);
  tone(buzzer
, 493, 153.076933486);
  delay(170.085481651);
  delay(46.3869495413);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);

  delay(159.777270642);
  tone(buzzer
, 587, 41.7482545872);
  delay(46.3869495413);
  delay(30.9246330275);
  tone(buzzer
, 587, 41.7482545872);
  delay(46.3869495413);
  delay(30.9246330275);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 391, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 32.4708646789);
  delay(36.0787385321);
  delay(36.0787385321);
  tone(buzzer
, 587, 32.4708646789);
  delay(36.0787385321);
  delay(46.3869495413);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 493, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 659, 55.6643394495);
  delay(61.849266055);
  delay(25.7705275229);
  tone(buzzer
, 659, 51.0256444954);
  delay(56.6951605505);
  delay(10.3082110092);

  tone(buzzer
, 659, 46.3869495413);
  delay(51.5410550459);
  delay(25.7705275229);
  tone(buzzer
, 659, 41.7482545872);
  delay(46.3869495413);
  delay(30.9246330275);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(20.6164220183);
  tone(buzzer
, 587, 51.0256444954);
  delay(56.6951605505);
  delay(20.6164220183);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);

  tone(buzzer
, 659, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 659, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(159.777270642);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 554, 264.405612385);
  delay(293.784013761);
  delay(15.4623165138);
  tone(buzzer
, 554, 51.0256444954);
  delay(56.6951605505);
  delay(20.6164220183);
  tone(buzzer
, 554, 51.0256444954);
  delay(56.6951605505);
  delay(20.6164220183);
  tone(buzzer
, 554, 273.683002294);
  delay(304.092224771);

  delay(5.15410550459);
  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 554, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 493, 134.52215367);
  delay(149.469059633);
  delay(5.15410550459);
  tone(buzzer
, 587, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 554, 129.883458716);
  delay(144.314954128);
  delay(10.3082110092);
  tone(buzzer
, 369, 134.52215367);
  delay(149.469059633);

  delay(5.15410550459);
  tone(buzzer
, 369, 273.683002294);
  delay(304.092224771);
  delay(5.15410550459);
  tone(buzzer
, 369, 273.683002294);
  delay(304.092224771);
}