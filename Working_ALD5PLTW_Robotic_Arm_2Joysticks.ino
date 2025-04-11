#include <SoftwareSerial.h>

// Define the pins for the joystick
const int joystick1XPin = A0;
const int joystick1YPin = A1;
const int joystick2XPin = A2;
const int joystick2YPin = A3;
const int joystick1ButtonPin = 2; // Will be null with EMG sensor

// Define the servo channels on the SSC-32U
const int baseChannel = 0;
const int shoulderChannel = 1;
const int elbowChannel = 2; //For EMG sensor later
const int wristChannel = 3;
const int gripperChannel = 4; //For EMG sensor later
const int wristrotationChannel = 5;

// Define the initial positions for the servos
int basePos = 1500;
int shoulderPos = 1500;
int elbowPos = 1500;
int wristrotationPos = 1500;
int wristPos = 1500;
int gripperPos = 1500;

// Joystick deadzone
const int DEADZONE = 50;

// Define the sensitivity of the joystick
const int sensitivity = 30;

void setup() {
  // Initialize Serial communication with the SSC-32U
  Serial.begin(9600);

  // Set the joystick button pin as input
  pinMode(joystick1ButtonPin, INPUT_PULLUP);
}

void loop() {
  // Read the joystick values
  int joystick1X = analogRead(joystick1XPin);
  int joystick1Y = analogRead(joystick1YPin);
  int joystick2X = analogRead(joystick2XPin);
  int joystick2Y = analogRead(joystick2YPin);
  int joystick1Button = digitalRead(joystick1ButtonPin);

  // Map the joystick values to servo positions
  if (abs(joystick1X - 512) > DEADZONE) {
    basePos += map(joystick1X, 0, 1023, -sensitivity, sensitivity);
  }
  
  if (abs(joystick1Y - 512) > DEADZONE) { 
    shoulderPos += map(joystick1Y, 0, 1023, -sensitivity, sensitivity);
  }

  elbowPos = 1500;

  if (abs(joystick2X - 512) > DEADZONE) { 
    wristrotationPos += map(joystick2X, 0, 1023, sensitivity, -sensitivity);
  }
  if (abs(joystick2Y - 512) > DEADZONE) { 
    wristPos += map(joystick2Y, 0, 1023, sensitivity, -sensitivity);
  }
  // Constrain the servo positions to avoid out-of-range values
  basePos = constrain(basePos, 500, 2500);
  shoulderPos = constrain(shoulderPos, 500, 2500);
  wristrotationPos = constrain(wristrotationPos, 500, 2500);
  wristPos = constrain(wristPos, 500, 2500);
  
  // Send the servo positions to the SSC-32U
  moveServo(baseChannel, basePos);
  moveServo(shoulderChannel, shoulderPos);
  moveServo(elbowChannel, elbowPos);
  moveServo(wristrotationChannel, wristrotationPos);
  moveServo(wristChannel, wristPos);

  // Check if the joystick button is pressed
  if (joystick1Button == LOW) {
    // Toggle the gripper
    gripperPos = (gripperPos == 500) ? 2000 : 500;
    moveServo(gripperChannel, gripperPos);
    delay(500); // Debounce delay
  }

  // Small delay to stabilize the loop
  delay(20);
}

void moveServo(int channel, int position) {
  // Send the servo position command to the SSC-32U
  Serial.print("#");
  Serial.print(channel);
  Serial.print("P");
  Serial.print(position);
  Serial.print("T100\r\n"); // 100ms time for the movement
}
