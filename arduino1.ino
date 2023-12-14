
//Speed
#include <TimerOne.h>
const int analogPin = A0;
int inputState;
int lastInputState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 5;
long time;
long endTime;
long startTime;
int RPM ; 
double trip = 0;
double kkbanspd = 0.00140;
float lnTime = 0;


//MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu;

void setup() {
  
  Serial.begin(9600);

  //Speed
  pinMode(13,OUTPUT);
  delay(10);
  endTime = 0;
  Timer1.initialize(500000);  // Set the timer to 60 rpm, 1,000,000 microseconds (1 second)
  Timer1.attachInterrupt(timerIsr);  // Attach the service routine here
  analogReference(EXTERNAL);


  //MPU6050
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  Serial.println("");
  delay(100);
}

void loop() {
  
  //Speed
  int analogValue = analogRead(analogPin); // Read analog value from A0
  float voltage = analogValue * (5.0 / 1023.0);
  time = millis();
  int currentSwitchState;
  if (analogValue>10){
  currentSwitchState=1;
  }
  if (analogValue<10){
    currentSwitchState=0;
  }
  // Serial.println(currentSwitchState);
  if (currentSwitchState != lastInputState) { //or if IR pin is high
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentSwitchState != inputState) {
      inputState = currentSwitchState;
      if (inputState == 1) {
        digitalWrite(13, LOW);
        calculateRPM(); // Real RPM from sensor
        // Serial.print(input state);
      }
      else {
        digitalWrite(13, HIGH);
      }
    }
  }
  lastInputState = currentSwitchState;


  //MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
//  Serial.print("Acceleration X: ");
float acclx=a.acceleration.x;
float accly=a.acceleration.y;
float acclz=a.acceleration.z;
if(acclx <-30.0 || accly < -30.0 || acclz < -30.0){
  Serial.println("Accident detected");
}

if(g.gyro.x >10.0 || g.gyro.y > 10.0 || g.gyro.z > 10.0 || g.gyro.x <-10.0 || g.gyro.y <-10.0 || g.gyro.z < -10.0){
  Serial.println("Accident detected"); 
}
//  Serial.print(a.acceleration.x);
//  Serial.print(", Y: ");
//  Serial.print(a.acceleration.y);
//  Serial.print(", Z: ");
//  Serial.print(a.acceleration.z);
//  Serial.println(" m/s^2");
//
//  Serial.print("Rotation X: ");
//  Serial.print(g.gyro.x);
//  Serial.print(", Y: ");
//  Serial.print(g.gyro.y);
//  Serial.print(", Z: ");
//  Serial.print(g.gyro.z);
//  Serial.println(" rad/s");
//
//  Serial.println("");
//  delay(500);
}

//Speed function-1
void calculateRPM() {
  startTime = lastDebounceTime;
  lnTime = startTime - endTime;
  RPM = 60000 / (startTime - endTime);
  endTime = startTime;
  trip++;
}

//Speed function-2
void timerIsr()
{
  // Print RPM every second
  // RPM based on timer
  
  time = millis() / 1000;

 Serial.print("Spd:");
// //  lcd.setCursor(5, 0);
 Serial.print((RPM * kkbanspd) * 20);
// //  lcd.setCursor(12, 0);
 Serial.println("Km/h");
// //  
  RPM = 0;
}
