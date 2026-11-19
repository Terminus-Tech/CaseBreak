#include <Wire.h>
#include "SparkFun_Qwiic_Twist_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Twist
#include <Servo.h>
#include "SparkFun_Qwiic_Keypad_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_keypad

// Component Addresses
TWIST twist; 
Servo myservo;
KEYPAD keypad1;
const word cipherPin[] = {A0, A1, A2, A3};

const int targetCombo[] = {5, -4, 8}; // Correct combination
int currentStep = 0;
long lastCount = 0;
int dialPos = 0; // Current position
int pos = 0;


const int keyCode[] = {1, 2, 3, 4}; // the correct keyCode.
const int cipher[2][4] = {{1, 2, 1, 5},
                          {3, 1, 1, 2}};
int userEntry[] = {0, 0, 0, 0}; // used to store the presses coming in from user
int userEntryIndex = 0; // used to keep track of where we are in the userEntry, incremented on each press, reset on timeout.

boolean userIsActive = false; // used to know when a user is active and therefore we want to engage timeout stuff
#define TIMEOUT 300 // 100s of milliseconds, used to reset input. 30 equates to 3 second.
byte timeOutCounter = 0; // variable this is incremented to keep track of timeouts.




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Case Break");

  // Safe Setup
  if (twist.begin(Wire1, 0x3f) == false){
    Serial.println("Twist does not appear to be connected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  twist.setColor(255, 255, 255); 
  lastCount = twist.getCount();
  myservo.attach(9);

  // Cipher Setup
  if (keypad1.begin(Wire1, 0x4b) == false)
  {
    Serial.println("Keypad does not appear to be connected. Please check wiring. Freezing...");
    while (1);
  }
  //Serial.print("Initialized. Firmware Version: ");
  //Serial.println(keypad1.getVersion());
  //Serial.println("\n\r\n\rPlease type in the correct 4 digit KeyCode (Press '#' to enter or '*' to clear):");
}

void loop() {
  comboEntry();
  keypadEntry();
  
}

// Run code for safe
void comboEntry(){
  // Get position
  int currentCount = twist.getCount();
  int diff = currentCount - lastCount;
  if(diff != 0){
    dialPos += diff;
    lastCount = currentCount;
    Serial.println(dialPos);
    Serial.println(currentStep);
  }

  if (abs(dialPos) >= 72){
    int reset = 5;
    Serial.println("Resetting... Please move knob to 0.");
    while (reset > 0){
      twist.setColor(255, 255, 0);
      delay(500);
      twist.setColor(0, 0, 0);
      delay(500);
      reset--;
    }
    currentCount = twist.getCount();
    lastCount = currentCount;
    diff = 0;
    dialPos = 0;
    currentStep = 0;
    Serial.println("Reset!");
    twist.setColor(255, 255, 255);
  }
  else {
    // Hot or cold feedback
    int colorDif = map(constrain(abs(dialPos - targetCombo[currentStep]), 0, 24), 0, 24, 255, 0);
    twist.setColor(255, 255 - colorDif/2, 255 - colorDif);
  }

  // Check Input
  if(twist.isPressed()){
    Serial.print("Pressed ");

    // Correct Entry
    if (abs(dialPos - targetCombo[currentStep]) < 1){
      Serial.println("Step correct!");
      currentStep++;
      twist.setColor(0, 255, 255); // Cyan for progress
      delay(300);
      if(currentStep >= sizeof(targetCombo)/sizeof(targetCombo[0])){
        Serial.println("UNLOCKED!");
        twist.setColor(0, 255, 0); // Green = Unlocked
        delay(5000); // Keep unlocked for 5 seconds
        for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        // Reset lock
        currentStep = 0;
        twist.setColor(255, 255, 255); // Back to White
      }
    }
    // Incorrect Entry
    else {
      Serial.println("Incorrect");
      currentStep = 0;
      twist.setColor(255, 0, 0); // Red error
      delay(1000);
    }
    // Wait for button release
    while (twist.isPressed()) {
      delay(10);
    }
  }
}

// Get the current cipher combination
int testCipher(int index)
{
  //Serial.println(index);
  int cable = 0;

  for (int i = 0; i < 4; i++)
  {
    int echo = analogRead(cipherPin[i]);

    if (echo < 900)
    {
      if (cable == 0)
      {
        cable = 1;
        if (index == 0)
        {
          //Serial.println("Index 0, Cable 0");
          return (cipher[0][i]);
          break;
        }
        else if (index == 1)
        {
          //Serial.println("Index 1, Cable 0");
          if (echo <= 5)
          {
            return (cipher[1][0]);
          }
          else if (echo >= 15 && echo <= 30)
          {
            return (cipher[1][1]);
          }
          else if (echo >= 100 && echo <= 200)
          {
            return (cipher[1][2]);
          }
          else if (echo >= 500 && echo <= 700)
          {
            return (cipher[1][3]);
          }
          break;
        }
      }
      else if (cable == 1)
      {
        if (index == 2)
        {
          //Serial.println("Index 2, Cable 1");
          return (cipher[0][i]);
        }
        else if (index == 3)
        {
          //Serial.println("Index 3, Cable 1");
          if (echo <= 5)
          {
            return (cipher[1][0]);
          }
          else if (echo >= 15 && echo <= 30)
          {
            return (cipher[1][1]);
          }
          else if (echo >= 100 && echo <= 200)
          {
            return (cipher[1][2]);
          }
          else if (echo >= 500 && echo <= 700)
          {
            return (cipher[1][3]);
          }
        }
        break;
      }
    }
  }
  return (0);
}

// Process keypad entry
void keypadEntry()
{
  keypad1.updateFIFO();  // necessary for keypad to pull button from stack to readable register
  char button = keypad1.getButton();
  int buttonInt = 0;

  if (button == -1)
  {
    Serial.println();
    Serial.println("No keypad detected");
    delay(1000);
  }
  else if (button != 0)
  {
    if (button == '*')
    {
      userIsActive = false; // don't display timeout stuff.
      userEntryIndex = 0; // reset
      clearEntry();
      printEntry();
    }
    else if (userEntryIndex == sizeof(keyCode)/sizeof(keyCode[0]) && button == '#')
    {
      if (memcmp(userEntry, keyCode, sizeof(keyCode)) == 0)
      {
        Serial.println();
        Serial.println("Keycode correct. Wahooooooooooo!");
        delay(1000);
      }
      else
      {
        Serial.println();
        Serial.println("Keycode INCORRECT.");
        delay(1000);
      }
      userIsActive = false; // don't display timeout stuff.
      userEntryIndex = 0; // reset
      clearEntry();
    }
    else if (userEntryIndex < sizeof(keyCode)/sizeof(keyCode[0]) && button != '#')
    {
      switch (button)
      {
        case '1':
          buttonInt = 1;
          break;
        case '2':
          buttonInt = 2;
          break;
        case '3':
          buttonInt = 3;
          break;
        case '4':
          buttonInt = 4;
          break;
        case '5':
          buttonInt = 5;
          break;
        case '6':
          buttonInt = 6;
          break;
        case '7':
          buttonInt = 7;
          break;
        case '8':
          buttonInt = 8;
          break;
        case '9':
          buttonInt = 9;
          break;
      }
      userEntry[userEntryIndex] = buttonInt * testCipher(userEntryIndex); // store button into next spot in array, note, index is incremented later
      printEntry();
      userIsActive = true; // used to only timeout when user is active
      userEntryIndex++;
    }
    timeOutCounter = 0; // reset with any new presses.
  }

  delay(10); //10 is good, more is better, note this effects total timeout time
  
  timeOutCounter++;
  
  if ((timeOutCounter == TIMEOUT) && (userIsActive == true)) // this means the user is actively inputing
  {
    Serial.println("\n\r\n\rTimed out... try again.");
    timeOutCounter = 0; // reset
    userEntryIndex = 0; // reset
    clearEntry();
    //printEntry();
    userIsActive = false; // so we don't continuously timeout while inactive.
  }
}

// check user entry against keyCode array.
// if they all match up, then respond with true.
boolean checkEntry()
{
  return (memcmp(userEntry, keyCode, sizeof(keyCode)) == 0);
}

// "clear" entry with all spaces
void clearEntry()
{
  for (int i = 0 ; i < sizeof(userEntry)/sizeof(userEntry[0]) ; i++)
  {
    userEntry[i] = 0; // fill with spaces
  }
}

void printEntry()
{
  Serial.println();
  Serial.print("UserEntry:");
  for (int i = 0 ; i < sizeof(userEntry)/sizeof(userEntry[0]) ; i++)
  {
    Serial.print(" ");
    Serial.print(userEntry[i]);
  }
  //Serial.println();
}
