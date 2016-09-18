#include <Servo.h>

const int led = 9; //led pin (PWM)
const int servopin = 8; //servo pin (PWM)
const int sampleTime = 50; //for microphone samples; 50ms = 20Hz (f=1/T)
const int window = 1500; //time between claps to register as double clap
unsigned int sample; //where the microphone sample is stored
int pos = 0; //postion of the servo from 0 to 180 deg
long timeDiff = 0; //time spent in the current window 
long curTime = 0; //temp variable to store current time
bool clap[2] = {false, false}; //array to hold most recent registered claps
Servo servo; //creation of servo object

void setup() {
  Serial.begin(9600); //Serial communication start ar 9600 baud
  servo.attach(servopin); //assigning servo to pin
  servo.write(0); //setting servo intial position 0 deg
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int peakToPeak = 0;

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  while ((millis() - startMillis) < sampleTime) {
    sample = analogRead(0);
    if (sample < 1024) {
      if (sample > signalMax) 
        signalMax = sample;
      else if (sample < signalMin)
        signalMin = sample;
    }
  }
  peakToPeak = (signalMax-signalMin);
  double volts = (peakToPeak*3.3)/1024;
  
  int aVal = (volts/2.16)*256;
  analogWrite(led, aVal);

  if (aVal == 255) {
    if (clap[0] == false) { //if this is the first registered clap
      clap[0] = true;
      curTime = millis();
    }
    else {
      clap[1] = true;
    }
  }
  else if (clap[0] == false) {
    curTime = millis();
  }
  
  timeDiff = (millis()-curTime);
  
  if (timeDiff >= window) {
    
    Serial.print(clap[0]);
    Serial.print(", ");
    Serial.println(clap[1]);
    if (clap[0] && clap[1]) {
      pos -= 90;
      servo.write(pos);
    }
    else if (clap[0] && clap[1] != true) {
      pos += 90;
      servo.write(pos); 
    }
    curTime = millis();
    timeDiff = 0;
    clap[0] = clap[1] = false;
  }

  if (aVal == 255) {
    Serial.println(volts);
    Serial.println(aVal);
  }
  
}
