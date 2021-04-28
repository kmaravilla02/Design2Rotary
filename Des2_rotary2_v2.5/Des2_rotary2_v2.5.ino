#include<Wire.h>
#include <LiquidCrystal_I2C.h>
#include <rotary.h>                 // rotary handler

int nAccel, nSpeed;
const int LED = 13;
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define PINA 2
#define PINB 3
#define PUSHB 4

// Initialize the Rotary object
// Rotary(Encoder Pin 1, Encoder Pin 2, Button Pin) Attach center to ground
Rotary r = Rotary(PINA, PINB, PUSHB);        // there is no must for using interrupt pins !!

int columnsLCD = 20;
String MenuLine[] = {"Tid Vol", "I:E         1:", "Execute"};
int MenuItems = 3;
int CursorLine = 0;

volatile unsigned char vol_result;
volatile unsigned char ie_result;

bool rowCheck = false;

int row_initial = 1;
int prevRow = 0;

//int backlightPin = 10;   //PWM pin
//int brightness = 255;
//int fadeAmount = 5;

byte z;
byte iecnt = 2; //IE counter

int menuCounter = 0; //counts the clicks of the rotary encoder between menu items (0-3 in this case)
int TidV_Val = 300; //value within menu 1
int IE_Val = 2; //value within menu 2
//int menu3_Value = 2; //value within menu 2

int tdvl = 300;
int TidCnt = 1;
int TidPass;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;  //the value is a number of milliseconds

void setup ()
{
  //  digitalWrite (PINA, HIGH);     // enable pull-ups
  //  digitalWrite (PINB, HIGH);
  //  digitalWrite (PUSHB, HIGH);

  //  pinMode(backlightPin, OUTPUT);          //backlightPin as an output
  //  digitalWrite(backlightPin, HIGH);

  //Serial.begin(9600);
  lcd.begin();
  lcd.clear(); // go home

  lcd.setCursor(5, 0);
  lcd.print("Automated");
  lcd.setCursor(3, 1);
  lcd.print("Bag Valve Mask");
  lcd.setCursor(6, 2);
  lcd.print("WARNING:");
  lcd.setCursor(0, 3);
  lcd.print("USE ADULT MASK ONLY!");
  delay(1000); //wait 2 sec

  lcd.clear();

  startMillis = millis();  //initial start time
  print_menu();
} //End of setup

void loop ()
{
  volatile unsigned char result = r.process();
  //currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  print_menu();

  if (result) {
    result == DIR_CCW ? row_initial = row_initial - 1, rowCheck = true : row_initial = row_initial + 1, rowCheck = true;
    if (row_initial < 1) {
      row_initial = 3; // roll over to last item }
    }
    else if (row_initial > 3) {
      row_initial = 1;                 // roll over to first item
    }
    print_arrow();
  } //End if result


  if (r.buttonPressedReleased(25)) {
    //init_backlight();     //wake up LCD...
    //lcd.setCursor(0, 0);  //(col, row)
    //lcd.print("You selected:");
    //lcd.setCursor(0, row_initial);  //(col, row)
    selection();
    //print_menu();

  } //endif buttonPressedReleased

  updateValueLCD();
} //End loop()

/************FUNCTIONS**************/

void print_menu()
{
  //lcd.clear();
  lcd.setCursor(4, 0);     //(col, row)
  lcd.print("BVM Settings");
  lcd.setCursor(1, 1);    //2nd row
  //lcd.print("<"); lcd.setCursor(columnsLCD - 1, 1); lcd.print(">");
  lcd.setCursor(1, 1);
  lcd.print(MenuLine[0]);
  lcd.setCursor(1, 2);
  lcd.print(MenuLine[1]);
  lcd.setCursor(1, 3);
  lcd.print(MenuLine[2]);
}


void print_arrow() {
  lcd.setCursor(0, row_initial);
  lcd.print(">");
  delay(1);

  lcd.setCursor(0, row_initial);
  lcd.print(" ");
  delay(1);
}

void selection()
{
  switch (row_initial) {
    case 0:
      break;

    case 1:
      lcd.setCursor(0, 1);
      lcd.print("X");
      lcd.setCursor(1, 1);
      lcd.print("Tid Vol");
      vol_result = r.process();
      if (vol_result) {
        vol_result == DIR_CCW ? TidV_Val = TidV_Val - 50, TidCnt-- : TidV_Val = TidV_Val + 50, TidCnt++;
        if (TidV_Val < 300) {
          TidV_Val = 600; // roll over to last item
          TidCnt = 7;
        }
        if (TidV_Val > 600) {
          TidV_Val = 300;                 // roll over to first item
          TidCnt = 1;
        }
        //set a flag or do something....
        if (r.buttonPressedReleased(25)) {
          lcd.setCursor(0, 2);
          lcd.print(" ");
          break;
        }
      }


    case 2:
      lcd.setCursor(0, 2);
      lcd.print("X");
      lcd.setCursor(1, 2);
      lcd.print("I:E         1:");
      ie_result = r.process();
      if (ie_result) {
        ie_result == DIR_CCW ? IE_Val = IE_Val - 1, iecnt-- : IE_Val = IE_Val + 1, iecnt++;
        if (IE_Val < 2) {
          IE_Val = 4; // roll over to last item
          iecnt = 4;
        }
        if (IE_Val > 4) {
          IE_Val = 2;                 // roll over to first item
          iecnt = 2;
        }
        //set a flag or do something....
        if (r.buttonPressedReleased(25)) {
          lcd.setCursor(0, 2);
          lcd.print(" ");
          break;
        }
      }


    case 3:
      lcd.setCursor(0, 3);
      lcd.print("X");
      lcd.setCursor(1, 3);
      lcd.print("Execute");
      //set a flag or do something....
      digitalWrite(LED, HIGH);
      z = iecnt;

      Wire.beginTransmission(4);
      Wire.write(z);
      Wire.write(byte(TidPass));
      Wire.endTransmission();
      break;

    default:
      break;
  } //end switch

  delay(1000);
  row_initial = 1;     // reset to start position
} //End selection



void updateValueLCD() {
  lcd.setCursor(19, 3); //1st line, 18th block
  lcd.print(menuCounter); //counter (0 to 3)

  lcd.setCursor(17, 2);
  lcd.print("   ");
  lcd.setCursor(17, 2);
  lcd.print(iecnt); //

  lcd.setCursor(15, 1);
  lcd.print("   ");
  lcd.setCursor(15, 1);
  lcd.print(tdvl); //
}
