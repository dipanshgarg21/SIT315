int led1 = 8;      // assign the LED pin number to a variable
int motion_detection1 = 2;    // assign the PIR sensor input pin number to a variable
int current_motion_state1 = LOW;  // initialize the PIR state as LOW (no motion detected)
int led2 = 9;     // assign the LED pin number to a variable
int motion_detection2 = 3;    // assign the PIR sensor input pin number to a variable
int current_motion_state2 = LOW;  // initialize the PIR state as LOW (no motion detected)
int led3 = 11;     // assign the LED pin number to a variable
int motion_detection3 = 4;    // assign the PIR sensor input pin number to a variable
int current_motion_state3 = LOW;  // initialize the PIR state as LOW (no motion detected)
int led4 = 12;     // assign the LED pin number to a variable

void setup() {
  pinMode(led1, OUTPUT);   // set the LED pin as an output
  pinMode(motion_detection1, INPUT);  // set the PIR sensor pin as an input
  pinMode(led2, OUTPUT);   // set the LED pin as an output
  pinMode(motion_detection2, INPUT);  // set the PIR sensor pin as an input
  pinMode(led3, OUTPUT);   // set the LED pin as an output
  pinMode(motion_detection3, INPUT);  // set the PIR sensor pin as an input
  pinMode(led4, OUTPUT);   // set the LED pin as an output
  Serial.begin(9600);         // initialize serial communication at 9600 baud

  pciSetup(motion_detection1);      // enable interrupt for pin 2
  pciSetup(motion_detection2);      // enable interrupt for pin 3
  pciSetup(motion_detection3);      // enable interrupt for pin 4

  startTimer(0.5);    // calling finction "startTimer" with parameter 0.5 which is for 2 secinds
}

void loop() {
  // nothing to do in the loop function
}
void startTimer(double timerFrequency) {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // frequency = clock_speed / (prescaler * (1 + OCR1A))
  // OCR1A = (clock_speed / (prescaler * frequency)) - 1
  OCR1A = round(16000000 / (1024 * timerFrequency)) - 1;
  
  // Prescaler 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  // CTC mode
  TCCR1B |= (1 << WGM12);
  interrupts();
}
ISR(TIMER1_COMPA_vect) {
  digitalWrite(led4, digitalRead(led4) ^ 1);
}

void pciSetup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR |= bit(digitalPinToPCICRbit(pin));                    // clear any outstanding interrupt
  PCICR |= bit(digitalPinToPCICRbit(pin));                    // enable interrupt for the group
}

ISR(PCINT2_vect)  // handle pin change interrupt for D0 to D7 here
{
  motion_detected1();
  motion_detected2();
  motion_detected3();
}


void motion_detected1() {
  int val = digitalRead(motion_detection1);                // read the PIR sensor input and store the value in val
  if (val == HIGH) {                               // check if motion is detected
    // digitalWrite(led1, digitalRead(led1) ^ 1);
    digitalWrite(led1, HIGH);                   // turn on the LED
    if (current_motion_state1 == LOW) {                        // check if motion was not detected previously
      Serial.println("Motion detected Sensor1!");  // print message to serial monitor
      current_motion_state1 = HIGH;                            // set the PIR state to HIGH (motion detected)
    }
  } else {
    // digitalWrite(led1, digitalRead(led1) ^ 1);
    digitalWrite(led1, LOW);                 // turn off the LED
    if (current_motion_state1 == HIGH) {                    // check if motion was detected previously
      Serial.println("Motion ended Sensor1!");  // print message to serial monitor
      current_motion_state1 = LOW;                          // set the PIR state to LOW (no motion detected)
    }
  }
}
void motion_detected2() {
  int val = digitalRead(motion_detection2);                // read the PIR sensor input and store the value in val
  if (val == HIGH) { 
    // digitalWrite(led2, digitalRead(led2) ^ 1);                              // check if motion is detected
    digitalWrite(led2, HIGH);                   // turn on the LED
    if (current_motion_state2 == LOW) {                        // check if motion was not detected previously
      Serial.println("Motion detected Sensor2!");  // print message to serial monitor
      current_motion_state2 = HIGH;                            // set the PIR state to HIGH (motion detected)
    }
  } else {
    // digitalWrite(led2, digitalRead(led2) ^ 1);
    digitalWrite(led2, LOW);                 // turn off the LED
    if (current_motion_state2 == HIGH) {                    // check if motion was detected previously
      Serial.println("Motion ended Sensor2!");  // print message to serial monitor
      current_motion_state2 = LOW;                          // set the PIR state to LOW (no motion detected)
    }
  }
}
void motion_detected3() {
  int val = digitalRead(motion_detection3);                // read the PIR sensor input and store the value in val
  if (val == HIGH) {   
    // digitalWrite(led3, digitalRead(led3) ^ 1);                            // check if motion is detected
    digitalWrite(led3, HIGH);                   // turn on the LED
    if (current_motion_state3 == LOW) {                        // check if motion was not detected previously
      Serial.println("Motion detected Sensor2!");  // print message to serial monitor
      current_motion_state3 = HIGH;                            // set the PIR state to HIGH (motion detected)
    }
  } else {
    // digitalWrite(led3, digitalRead(led3) ^ 1);
    digitalWrite(led3, LOW);                 // turn off the LED
    if (current_motion_state3 == HIGH) {                    // check if motion was detected previously
      Serial.println("Motion ended Sensor2!");  // print message to serial monitor
      current_motion_state3 = LOW;                          // set the PIR state to LOW (no motion detected)
    }
  }
}
