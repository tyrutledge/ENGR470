int grabPin = A0;    // setting analog input for the forearm sensor
int elbowPin = A1;    // setting analog input for the bicep sensor
int grabValue = 0;    //setting initial values to 0
int elbowValue = 0;

void setup() {
  pinMode(grabPin, INPUT);
  pinMode(elbowPin, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  // read the values from the sensor:
  grabValue = analogRead(grabPin);
  Serial.println(grabValue);
  elbowValue = analogRead(elbowPin);
  Serial.println(elbowValue);
    Serial.print(analogRead(A2));
  Serial.print(",0,1023");
  Serial.println();
  delay(2);
}