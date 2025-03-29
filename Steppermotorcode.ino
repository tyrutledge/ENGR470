// NEMA 17 Stepper Motor Control with A4988 Driver
// Control via Serial Monitor with Microstepping Support

// Define pin connections
const int DIR_PIN = 2;    // Direction pin
const int STEP_PIN = 3;   // Step pin
const int ENABLE_PIN = 4; // Enable pin

// MS pins for microstepping control (connect to A4988 MS1, MS2, MS3)
const int MS1_PIN = 5;
const int MS2_PIN = 6;
const int MS3_PIN = 7;

// Motor configuration
const int STEPS_PER_REVOLUTION = 200;  // NEMA 17 typically has 200 steps per revolution

// Default settings
int currentMicrostep = 1;   // Default to full step
int stepDelay = 1000;       // Default step delay in microseconds
bool motorEnabled = true;   // Motor enabled by default
bool clockwise = true;      // Direction (true = clockwise)

// Command variables
String inputCommand = "";
boolean commandComplete = false;

void setup() {
  // Setup pin modes
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);
  
  // Initialize default settings
  digitalWrite(ENABLE_PIN, LOW);  // Enable driver (LOW = enabled for A4988)
  digitalWrite(DIR_PIN, HIGH);    // Set initial direction
  setMicrostepping(1);            // Start with full stepping
  
  // Initialize serial communication
  Serial.begin(9600);
  printMenu();
}

void loop() {
  // Process serial commands
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      commandComplete = true;
    } else {
      inputCommand += inChar;
    }
  }
  
  // Process completed command
  if (commandComplete) {
    processCommand(inputCommand);
    inputCommand = "";
    commandComplete = false;
  }
}

void processCommand(String command) {
  command.trim();
  
  // Convert command to lowercase for easier processing
  command.toLowerCase();
  
  if (command.startsWith("rotate ")) {
    // Extract number of steps from command
    int steps = command.substring(7).toInt();
    rotateSteps(steps);
  }
  else if (command.startsWith("rev ")) {
    // Extract number of revolutions from command
    float revs = command.substring(4).toFloat();
    int steps = round(revs * STEPS_PER_REVOLUTION * currentMicrostep);
    rotateSteps(steps);
  }
  else if (command == "cw") {
    clockwise = true;
    digitalWrite(DIR_PIN, HIGH);
    Serial.println("Direction: Clockwise");
  }
  else if (command == "ccw") {
    clockwise = false;
    digitalWrite(DIR_PIN, LOW);
    Serial.println("Direction: Counter-Clockwise");
  }
  else if (command.startsWith("delay ")) {
    // Set step delay in microseconds
    stepDelay = command.substring(6).toInt();
    Serial.print("Step delay set to ");
    Serial.print(stepDelay);
    Serial.println(" microseconds");
  }
  else if (command.startsWith("ms ")) {
    // Set microstepping mode (1, 2, 4, 8, or 16)
    int ms = command.substring(3).toInt();
    if (ms == 1 || ms == 2 || ms == 4 || ms == 8 || ms == 16) {
      setMicrostepping(ms);
      Serial.print("Microstepping set to 1/");
      Serial.println(ms);
    } else {
      Serial.println("Invalid microstepping value. Use 1, 2, 4, 8, or 16.");
    }
  }
  else if (command == "enable") {
    motorEnabled = true;
    digitalWrite(ENABLE_PIN, LOW);
    Serial.println("Motor enabled");
  }
  else if (command == "disable") {
    motorEnabled = false;
    digitalWrite(ENABLE_PIN, HIGH);
    Serial.println("Motor disabled");
  }
  else if (command == "status") {
    printStatus();
  }
  else if (command == "menu" || command == "help") {
    printMenu();
  }
  else {
    Serial.println("Unknown command. Type 'menu' for options.");
  }
}

void setMicrostepping(int microsteps) {
  currentMicrostep = microsteps;
  
  // Set MS1, MS2, MS3 pins based on desired microstepping mode
  switch (microsteps) {
    case 1:  // Full step
      digitalWrite(MS1_PIN, LOW);
      digitalWrite(MS2_PIN, LOW);
      digitalWrite(MS3_PIN, LOW);
      break;
    case 2:  // Half step
      digitalWrite(MS1_PIN, HIGH);
      digitalWrite(MS2_PIN, LOW);
      digitalWrite(MS3_PIN, LOW);
      break;
    case 4:  // Quarter step
      digitalWrite(MS1_PIN, LOW);
      digitalWrite(MS2_PIN, HIGH);
      digitalWrite(MS3_PIN, LOW);
      break;
    case 8:  // Eighth step
      digitalWrite(MS1_PIN, HIGH);
      digitalWrite(MS2_PIN, HIGH);
      digitalWrite(MS3_PIN, LOW);
      break;
    case 16: // Sixteenth step
      digitalWrite(MS1_PIN, HIGH);
      digitalWrite(MS2_PIN, HIGH);
      digitalWrite(MS3_PIN, HIGH);
      break;
  }
}

void rotateSteps(int steps) {
  if (!motorEnabled) {
    Serial.println("Motor is disabled. Type 'enable' first.");
    return;
  }
  
  Serial.print("Rotating ");
  Serial.print(steps);
  Serial.print(" steps (");
  Serial.print((float)steps / (STEPS_PER_REVOLUTION * currentMicrostep));
  Serial.println(" revolutions)");
  
  // Perform rotation
  for (int i = 0; i < abs(steps); i++) {
    // If steps is negative, change direction temporarily
    if (steps < 0) {
      digitalWrite(DIR_PIN, !clockwise);
    } else {
      digitalWrite(DIR_PIN, clockwise);
    }
    
    // Send pulse
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelay);
  }
  
  // Restore original direction
  digitalWrite(DIR_PIN, clockwise);
  
  Serial.println("Rotation complete");
}

void printStatus() {
  Serial.println("\n--- Current Status ---");
  Serial.print("Direction: ");
  Serial.println(clockwise ? "Clockwise" : "Counter-Clockwise");
  Serial.print("Microstepping: 1/");
  Serial.println(currentMicrostep);
  Serial.print("Step delay: ");
  Serial.print(stepDelay);
  Serial.println(" microseconds");
  Serial.print("Motor: ");
  Serial.println(motorEnabled ? "Enabled" : "Disabled");
  Serial.println("---------------------");
}

void printMenu() {
  Serial.println("\n=== NEMA 17 Stepper Motor Control ===");
  Serial.println("Available commands:");
  Serial.println("- rotate X    : Rotate X steps");
  Serial.println("- rev X       : Rotate X revolutions (can use decimals)");
  Serial.println("- cw          : Set direction to clockwise");
  Serial.println("- ccw         : Set direction to counter-clockwise");
  Serial.println("- delay X     : Set step delay to X microseconds");
  Serial.println("- ms X        : Set microstepping (1, 2, 4, 8, or 16)");
  Serial.println("- enable      : Enable the motor");
  Serial.println("- disable     : Disable the motor (power saving)");
  Serial.println("- status      : Display current settings");
  Serial.println("- menu/help   : Show this menu");
  Serial.println("=====================================");
}