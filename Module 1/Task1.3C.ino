// Define pins
#define SOIL_MOISTURE_PIN A0
#define PIR_SENSOR_PIN 3
#define LED_PIN 4

// Define variables
volatile bool motion_detected = false;

void setup() {
  // Set pin modes
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  // Attach interrupt to PIR sensor pin
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), motionDetected, RISING);
}

void loop() {
  // Read soil moisture level
  int soil_moisture = analogRead(SOIL_MOISTURE_PIN);
  Serial.println(soil_moisture);
  // Check if soil moisture level is low
  if (soil_moisture < 500) {
    // Turn on LED
    digitalWrite(LED_PIN, HIGH);
    Serial.println("MOTION DETECTED");
  } else {
    // Turn off LED if motion is not detected
    if (!motion_detected) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("NO MOTION DETECTED");
    }
  }
  delay(1000);
}

void motionDetected() {
  // Set motion detected flag
  motion_detected = true;
  // Turn on LED
  digitalWrite(LED_PIN, HIGH);
  // Wait for 10 seconds
  delay(10000);
  // Reset motion detected flag
  motion_detected = false;
  // Turn off LED if soil moisture level is high
  int soil_moisture = analogRead(SOIL_MOISTURE_PIN);
  if (soil_moisture >= 500) {
    digitalWrite(LED_PIN, LOW);
  }
}
