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

  // Attach interrupt to PIR sensor pin
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), motionDetected, RISING);
}

void loop() {
  // Read soil moisture level
  int soil_moisture = analogRead(SOIL_MOISTURE_PIN);

  // Check if soil moisture level is low
  if (soil_moisture < 500) {
    // Turn on LED
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Turn off LED if motion is not detected
    if (!motion_detected) {
      digitalWrite(LED_PIN, LOW);
    }
  }
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
