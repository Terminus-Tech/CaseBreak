#include "SparkFun_Qwiic_Twist_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Twist
#include <Servo.h>
TWIST twist; 
Servo myservo;
const int targetCombo[] = {5, -4, 8}; // Correct combination
int currentStep = 0;
long lastCount = 0;
int dialPos = 0; // Current position
int pos = 0;

void setup() {
  Serial.begin(9600);
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
  
  lockCode();
}
void lockCode(){

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