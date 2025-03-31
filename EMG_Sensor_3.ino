int grabPin = A0;    // Forearm sensor
int elbowPin = A1;   // Bicep sensor

// Baseline values (calibrate these when muscles are relaxed!)
const int grabBaseline = 300;
const int elbowBaseline = 300;

// Bandpass filter variables
float alpha_high = 0.95;   // High-pass coefficient (~20 Hz cutoff)
float alpha_low = 0.1;     // Low-pass coefficient (~500 Hz cutoff)

// Filter state variables (keep track between loop iterations)
float prev_high_grab = 0, prev_low_grab = 0;
float prev_high_elbow = 0, prev_low_elbow = 0;

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Read raw values and remove DC offset
  int rawGrab = analogRead(grabPin) - grabBaseline;
  int rawElbow = analogRead(elbowPin) - elbowBaseline;

  // Bandpass filter for grab sensor
  float high_pass_grab = rawGrab - prev_high_grab + alpha_high * prev_high_grab;
  prev_high_grab = high_pass_grab;
  float low_pass_grab = alpha_low * high_pass_grab + (1 - alpha_low) * prev_low_grab;
  prev_low_grab = low_pass_grab;

  // Bandpass filter for elbow sensor
  float high_pass_elbow = rawElbow - prev_high_elbow + alpha_high * prev_high_elbow;
  prev_high_elbow = high_pass_elbow;
  float low_pass_elbow = alpha_low * high_pass_elbow + (1 - alpha_low) * prev_low_elbow;
  prev_low_elbow = low_pass_elbow;

  // Use filtered grab value for thresholding
  if (low_pass_grab > 10) {  // Adjusted threshold for filtered signal
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }

  // Print filtered values
  Serial.print("Grab: ");
  Serial.print(low_pass_grab);
  Serial.print(" | Elbow: ");
  Serial.println(low_pass_elbow);
  Serial.print(analogRead(A2));

  Serial.print(",200,400");

  Serial.println();

  delay(10);  // Reduced delay for better filtering (now ~100Hz sampling)
}
