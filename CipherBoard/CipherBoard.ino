#include <Wire.h>
#include "SparkFun_Qwiic_Keypad_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_keypad
KEYPAD keypad1; //Create instance of this object
const word cipherPin[] = {A0, A1, A2, A3};

const int keyCode[] = {1, 2, 3, 4}; // the correct keyCode - change to your own unique set of keys if you like.
int userEntry[] = {0, 0, 0, 0}; // used to store the presses coming in from user
boolean userIsActive = false; // used to know when a user is active and therefore we want to engage timeout stuff

const int cipher[2][4] = {{1, 2, 1, 5},
                          {3, 1, 1, 2}};

#define TIMEOUT 300 // 100s of milliseconds, used to reset input. 30 equates to 3 second.
byte timeOutCounter = 0; // variable this is incremented to keep track of timeouts.

int userEntryIndex = 0; // used to keep track of where we are in the userEntry, incremented on each press, reset on timeout.

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Case Break");

  if (keypad1.begin(Wire1) == false) 	// Note, using begin() like this will use default I2C address, 0x4B.
    // You can pass begin() a different address like so: keypad1.begin(Wire, 0x4A).
  {
    Serial.println("Keypad does not appear to be connected. Please check wiring. Freezing...");
    while (1);
  }
  Serial.print("Initialized. Firmware Version: ");
  Serial.println(keypad1.getVersion());
  Serial.println("\n\r\n\rPlease type in the correct 4 digit KeyCode (Press '#' to enter or '*' to clear):");
}

void loop(void)
{
  keypadEntry();
  delay(100);
}

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