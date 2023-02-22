const byte led_pin = 4;
const byte interrupt_pin = 3;
volatile byte state = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(led_pin,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin),interrupt_routine,RISING);
}

void loop() {
  if (state==HIGH){
    digitalWrite(led_pin,HIGH);
    delay(500);
    
  }
  if (state==HIGH) {
    Serial.println("NO MOTION");
    state = LOW;
    digitalWrite(led_pin,LOW);
  }
}

void interrupt_routine(){
  state = HIGH;
  Serial.println("MOTION DETECTED");
}
