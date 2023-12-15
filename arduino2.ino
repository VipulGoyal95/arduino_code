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
float Rref = 450;//value of Rref resistor 
int R = 0;


//Rotary Encoder
const int encoderPin1 = 2;
const int encoderPin2 = 3 ;
const int stepsPerRevolution = 360; // Set the number of steps per revolution for your encoder
volatile long encoderValue = 0;
float angle = 0.0;





void setup() {
  // put your setup code here, to run once:
  //Voltage  and fuel
  Serial.begin(9600);

  //Rotary encoder
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  digitalWrite(encoderPin1, HIGH); // Turn pull-up resistor on
  digitalWrite(encoderPin2, HIGH); // Turn pull-up resistor on
  attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder, CHANGE);

  
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

  Serial.print("Voltaqe:");             
  Serial.println(vIn);
  Serial.print("Fuel Level:");
  Serial.println(R);




  //Rotary encoder
  // Calculate the angle based on the encoder value and steps per revolution
  angle = (encoderValue * 360.0) / (stepsPerRevolution *2.32);
  //  new_angle=map(angle,org min,org max,new_min,new_max);
  Serial.println(angle);
//  delay(1000); // Just here to slow down the output and show it will work even during a delay


}


//Rotary encoder function
void updateEncoder() {
  if (digitalRead(encoderPin1) == digitalRead(encoderPin2))
    encoderValue++;
  else
    encoderValue--;
}
