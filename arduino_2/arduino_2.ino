//Voltage Sensor
const int voltageSensorPin = A3;          // sensor pin
float vIn;                                // measured voltage (3.3V = max. 16.5V, 5V = max 25V)
float vOut;
float voltageSensorVal;                   // value on pin A3 (0 - 1023)
const float factor = 5.0;               // reduction factor of the Voltage Sensor shield
const float vCC = 5.00;                   // Arduino input voltage (measurable by voltmeter)




//Fuel level
const int sensorPin = A1;
int sensorValue = 0; // fuel resistance
float Vin = 5;
float Vout = 0;
float Rref = 350;//value of Rref resistor 
float R = 0;


//Rotary Encoder
#define encoderPinA 2
#define encoderPinB 3
// Variables
volatile int encoderPos = 0;
volatile int prevEncoderPos = 0;
unsigned long prevTime = 0;
unsigned long angleChangeTime = 0;
const unsigned long interval = 1000;    // Time interval in milliseconds
const int angleThreshold = 1600;        // 1600 degrees threshold
const unsigned long notOkInterval = 5000; // 15 seconds


//thermistors
int ThermistorPin = A4;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

//payload
#include <TimerOne.h>

const int potentiometerPin = A0;
const int IRSensorPin = 6;
const int debounceDelay = 5;

int inputState, lastInputState = LOW;
long lastDebounceTime = 0, endTime, startTime, time;
int RPM;
float lnTime = 0;


void setup() {
  // put your setup code here, to run once:
  //payload
  pinMode(IRSensorPin, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  delay(600);
  endTime = 0;
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timerIsr);

  //Voltage  and fuel
  Serial.begin(9600);

  //Rotary encoder
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);

  prevTime = millis(); // Record initial time
  angleChangeTime = millis(); // Record initial time for angle change check

  
}
void loop() {


  
  //payload
  time = millis();
  int currentSwitchState = digitalRead(IRSensorPin);

  if (currentSwitchState != lastInputState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentSwitchState != inputState) {
      inputState = currentSwitchState;
      digitalWrite(13, inputState == LOW ? LOW : HIGH);
      if (inputState == LOW) {
        calculateRPM();
      }
    }
  }
  lastInputState = currentSwitchState;
  

  
    //Voltage sensor
  voltageSensorVal = analogRead(voltageSensorPin);    // read the current sensor value (0 - 1023) 
  vOut = (voltageSensorVal / 1024) * vCC;             // convert the value to the real voltage on the analog pin
  vIn =  vOut * factor ;                               // convert the voltage on the source by multiplying with the factor




  //Fuel sensor
  sensorValue = analogRead(sensorPin);
  Vout = (Vin*sensorValue)/1023;
  R = (Rref*(1/((Vin/Vout) -1)))/7;

  //Thermistor
  //Thermistors
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;

  if(R>101){
    R=0.0;
  }
  Serial.print("FuelLevel:");
  Serial.print(R);
  Serial.print(",");
  Serial.print("Voltage:");
  Serial.print(vIn);
  Serial.print(",");
  Serial.print("temperature:"); 
  Serial.print(Tc);
  Serial.println(",");



  //Rotary encoder
   unsigned long currentTime = millis();
  
  // Calculate time difference
  unsigned long elapsedTime = currentTime - prevTime;
  
  if (elapsedTime >= interval) {
    // Calculate the angle change
    int angleChange = encoderPos - prevEncoderPos;
    
    // Calculate angular velocity in degrees per second
    float angularVelocity = (float)angleChange / elapsedTime * 1000; // Multiply by 1000 to convert to degrees per second
    Serial.print("encoder:");
    // Check if the change in angle exceeds the threshold
    if (angularVelocity > angleThreshold) {
      Serial.print("1,");
      angleChangeTime = currentTime; // Reset timer for angle change
    }
    else{
    Serial.print("0,");
  }
    if(angularVelocity > 100){
      angleChangeTime = currentTime;
    }
    
    // Reset variables for the next interval
    prevEncoderPos = encoderPos;
    prevTime = currentTime;
  }
  
  // Check if angle has not changed for 15 seconds
  if ((currentTime - angleChangeTime) >= notOkInterval) {
    // Serial.println("Not OK: Angle not changing for 15 seconds");
    angleChangeTime = currentTime; // Reset timer for angle change
  }
  
  
  delay(1000);

}


//Rotary encoder function
void updateEncoder() {
  static int previousA = 0;
  static int previousB = 0;
  int currentA = digitalRead(encoderPinA);
  int currentB = digitalRead(encoderPinB);
  
  if ((previousA == LOW && previousB == LOW) || (previousA == HIGH && previousB == HIGH)) {
    if (currentA != currentB) {
      encoderPos++;
    } else {
      encoderPos--;
    }
  } else {
    if (currentA == currentB) {
      encoderPos++;
    } else {
      encoderPos--;
    }
  }
  
  previousA = currentA;
  previousB = currentB;
}
//payload
void calculateRPM() {
  startTime = lastDebounceTime;
  lnTime = startTime - endTime;
  RPM = (startTime - endTime != 0) ? 60000 / (startTime - endTime) : 0;
  endTime = startTime;
}

void timerIsr() {
  time = millis() / 1000;
  int sensorValue = analogRead(potentiometerPin);
  delay(500);
  
  Serial.print("payload:");
  Serial.print((RPM <= 10 * (sensorValue - 350) + 500 && RPM >= 10 * (sensorValue - 350) - 500) ? "1," : (RPM >= 10 * (sensorValue - 350) + 500) ? "0," : "2,");
  RPM = 0;
}
