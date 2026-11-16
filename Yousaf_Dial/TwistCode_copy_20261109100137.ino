#include "SparkFun_Qwiic_Twist_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Twist
#include <Servo.h>
TWIST twist; 
Servo myservo;
const int targetCombo[] = {5, -4, 8}; // Example delta counts
int currentStep = 0;
long lastCount = 0;
int accumulatedMove = 0;                             //Create instance of this object
int pos = 0; 
void setup() {
  Serial.begin(9600);
  Serial.println("Qwiic Twist Example");
  if (twist.begin(Wire1) == false){
    Serial.println("Twist does not appear to be connected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  twist.setColor(255, 255, 255); 
  lastCount = twist.getCount();
  // put your setup code here, to run once:
  myservo.attach(9);
}
void loop() {
  long currentCount = twist.getCount();
  long diff = currentCount - lastCount;
  if(diff != 0){
    accumulatedMove += diff;
    lastCount = currentCount;
    Serial.println(accumulatedMove);
    Serial.println(currentStep);
  }
  lockCode();
}
void lockCode(){
  if(accumulatedMove == 4 || accumulatedMove == 5 || accumulatedMove == 6){
    if(currentStep == 0){
      twist.setColor(255, 165, 0);
    }
  } else if (accumulatedMove == -4 || accumulatedMove == -5 || accumulatedMove == -6){
    if(currentStep == 1){
      twist.setColor(255, 165, 0);
    }
  } else if (accumulatedMove == 7 || accumulatedMove == 8 || accumulatedMove == 9){
    if(currentStep == 2){
      twist.setColor(255, 165, 0);
    }
  } else {
    twist.setColor(255, 255, 255);
  }
   if(twist.isPressed()){
    Serial.print("Pressed");
    if (abs(accumulatedMove - targetCombo[currentStep]) < 1){
      Serial.println("Step correct!");
      currentStep++;
      twist.setColor(0, 255, 255); // Cyan for progress
      delay(300);
      if(currentStep >= 3){
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
    } else {
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