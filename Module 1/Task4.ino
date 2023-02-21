#define triggerPin 7      //Define IO pins
#define echoPin 8
#define LED 4

long duration;
double distance;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(triggerPin, OUTPUT);   //Define pin
  pinMode(echoPin, INPUT);
  Serial.begin(9600);           //Starts the serial communication
}

void loop()
{
  digitalWrite(triggerPin, LOW);   //Reset the trigger pin
  delay(1000);
  digitalWrite(triggerPin, HIGH);     //Create a 10 micro second pulse
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH); //Read the pulse travel time in microseconds.
  distance= duration*0.034/2;        //Calculate the distance - speed of sound is 0.034 cm per microsecond
  Serial.print("Distance: ");        //Display the distance on the serial monitor
  Serial.println(distance);
  if(distance > 50)
    digitalWrite(LED, HIGH);
  else
    digitalWrite(LED, LOW);
}
