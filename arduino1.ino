
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


//gps
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = 4, TXPin = 3;// Here we make pin 4 as RX of arduino & pin 3 as TX of arduino 
static const uint32_t GPSBaud = 9600;


TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

//MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu;

//MCP2515
//#include <CAN.h> // the OBD2 library depends on the CAN library
//#include <OBD2.h>
//// array of PID's to print values of
//const int PIDS[] = {
//
//  ENGINE_COOLANT_TEMPERATURE,
//
//};
//
//const int NUM_PIDS = sizeof(PIDS) / sizeof(PIDS[0]);

void setup() {
  
  Serial.begin(9600);

  ss.begin(GPSBaud);

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

//  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
//    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
//  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
//    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
//    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
//    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
//    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
//    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
//    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
//    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
//    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
//  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
//    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
//    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
//    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
//    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
//    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
//    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
//    Serial.println("5 Hz");
    break;
  }
  Serial.println("");
  delay(100);


  //MCP2515
//  while (!Serial);
//  Serial.println(F("OBD2 Key Stats"));
//  while (true) {
//    Serial.print(F("Attempting to connect to OBD2 CAN bus ... "));
//    if (!OBD2.begin()) {
//      Serial.println(F("failed!"));
//
//      delay(1000);
//    } else {
//      Serial.println(F("success"));
//      break;
//    }
//  }
//  Serial.println();
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
if(acclx <-60.0 || accly < -60.0 || acclz < -60.0){
  Serial.println("Accident detected");
}

if(g.gyro.x >8.0 || g.gyro.y > 8.0 || g.gyro.z > 8.0 || g.gyro.x <-8.0 || g.gyro.y <-8.0 || g.gyro.z < -8.0){
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
//  Serial.print("Temperature:");
//  Serial.print(temp.temperature);
//  Serial.println(",");

  //MCP2515

//  for (int i = 0; i < NUM_PIDS; i++) {
//  int pid = PIDS[i];
//
//  printPID(pid);
//  }
//  Serial.println();
//
//  delay(1000);



  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("NANA"));
    while(true);
  }
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

 Serial.print("Speed:");
// //  lcd.setCursor(5, 0);
 Serial.print((RPM * kkbanspd) * 20);
 Serial.print(F(","));
// //  lcd.setCursor(12, 0);
 
// //  
  RPM = 0;

    Serial.print(F("lat:")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(",");
    Serial.print(F("long:"));
    Serial.print(gps.location.lng(), 6);
    Serial.print(",");
    Serial.print("\n");
  }
  else
  {
    Serial.println(F("NA"));
  }
  
//  sensors_event_t a, g, temp;
//  mpu.getEvent(&a, &g, &temp);
//
//  Serial.print("Temperature:");
//  Serial.print(temp.temperature);
//  Serial.println(",");
  
}
  void displayInfo()
{

  }






//mcp2515
//void printPID(int pid) {
//  // print PID name
//  Serial.print(OBD2.pidName(pid));
//  Serial.print(F(" = "));
//
//  // read the PID value
//  float pidValue = OBD2.pidRead(pid);
//
//  if (isnan(pidValue)) {
//    Serial.print("error");
//  } else {
//    // print value with units
//    Serial.print(pidValue);
//    Serial.print(F(" "));
//    Serial.print(OBD2.pidUnits(pid));
//  }
//
//  Serial.println();
//}
