//Voltage Sensor
const int voltageSensorPin = A3;          // sensor pin
float vIn;                                // measured voltage (3.3V = max. 16.5V, 5V = max 25V)
float vOut;
float voltageSensorVal;                   // value on pin A3 (0 - 1023)
const float factor = 5.128;               // reduction factor of the Voltage Sensor shield
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





void setup() {
  // put your setup code here, to run once:
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

  
  //Voltage sensor
  voltageSensorVal = analogRead(voltageSensorPin);    // read the current sensor value (0 - 1023) 
  vOut = (voltageSensorVal / 1024) * vCC;             // convert the value to the real voltage on the analog pin
  vIn =  vOut * factor;                               // convert the voltage on the source by multiplying with the factor
  




  //Fuel sensor
  sensorValue = analogRead(sensorPin);
  Vout = (Vin*sensorValue)/1023;
  R = (Rref*(1/((Vin/Vout) -1)))/7;

  
  Serial.print("Fuel Level:");
  Serial.print(R);
  Serial.print(",");
  Serial.print("Voltaqe:");
  Serial.print(vIn);
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
    
    // Check if the change in angle exceeds the threshold
    if (angularVelocity > angleThreshold) {
      Serial.println("Angle Change > 1600 degrees in 1 second: OK");
      angleChangeTime = currentTime; // Reset timer for angle change
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
    Serial.println("Not OK: Angle not changing for 15 seconds");
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
