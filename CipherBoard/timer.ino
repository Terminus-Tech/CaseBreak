/*
  SerLCD Library - Hello World
  Gaston Williams - August 29, 2018

  This sketch prints "Hello World!" to the LCD
  and shows the time over I2C using the Qwiic system.

  The circuit:
   SparkFun RGB OpenLCD Serial display connected through
   a Sparkfun Qwiic adpater to an Ardruino with a
   Qwiic shield or a Sparkfun Blackboard with Qwiic built in.

  The Qwiic adapter should be attached to the display as follows:
  Display	/ Qwiic Cable Color
 	GND	/	Black
 	RAW	/	Red
 	SDA	/	Blue
 	SCL	/	Yellow

  Note: If you connect directly to a 5V Arduino instead, you *MUST* use
  a level-shifter to convert the i2c voltage levels down to 3.3V for the display.

  This code is based on the LiquidCrystal code originally by David A. Mellis
  and the OpenLCD code by Nathan Seidle at SparkFun.

  License: This example code is in the public domain.

  More info on Qwiic here: https://www.sparkfun.com/qwiic

  AVR-Based Serial Enabled LCDs Hookup Guide
  https://learn.sparkfun.com/tutorials/avr-based-serial-enabled-lcds-hookup-guide
*/

#include <Wire.h>

#include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
SerLCD lcd; // Initialize the library with default I2C address 0x72
int timer = 30;

void setup() {
  Wire1.begin();

  lcd.begin(Wire1); //Set up the LCD for I2C communication

  lcd.setBacklight(255, 0, 0); //Set backlight to bright white

  lcd.print("---CASE BREAK---");

  delay(5000);

  lcd.clear(); //Clear the display - this moves the cursor to home position as well

  lcd.print("Time left...");
}

void loop () {
  lcd.setCursor(0,1);
  lcd.print(timer);
  lcd.print(" ");

  

  if (timer == 0) {
    lcd.clear();
    lcd.print("Boom!");
    while (true) {}
  }

  delay(1000);
  timer--;
}

//void loop() {
  // Set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // Print the number of seconds since reset:
  //lcd.print(timer);
  //delay(1000);
  //timer--;
  //if (timer == 0){
  //lcd.clear();
  //lcd.print("Boom");
  //}
//}
