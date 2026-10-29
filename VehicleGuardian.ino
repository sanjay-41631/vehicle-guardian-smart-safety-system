/*
 Vehicle Guardian starter integration — ESP32 DevKit V1
 Verify wiring before use. Keep motor battery disconnected for initial tests.
 HC-SR04 ECHO must be level shifted to 3.3 V.
 Add compatible WAV files to data/ and upload LittleFS.
*/
#include <Wire.h>
#include <math.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>
#include <LittleFS.h>
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

constexpr int TRIG_PIN=23, ECHO_PIN=34, IR_PIN=35;
constexpr int SDA_PIN=21, SCL_PIN=22;
constexpr int I2S_BCLK=17, I2S_LRC=16, I2S_DOUT=5;
constexpr int A_IN1=14, A_IN2=13, A_IN3=18, A_IN4=19;
constexpr int B_IN1=25, B_IN2=26, B_IN3=32, B_IN4=33;
constexpr float DANGER_CM=25.0f, SAFE_CM=30.0f;
constexpr float IMPACT_THRESHOLD_MS2=25.0f; // preliminary only; calibrate
constexpr unsigned long IMPACT_COOLDOWN_MS=5000;

HardwareSerial GPSSerial(2);
TinyGPSPlus gps;
Adafruit_MPU6050 mpu;
bool mpuOnline=false, collisionLatched=false, doorWasBlocked=false, gpsAnnounced=false;
unsigned long lastImpactMs=0, lastPrintMs=0, lastDistanceMs=0;
float distanceCm=-1.0f;

AudioGeneratorWAV *wav=nullptr;
AudioFileSourceLittleFS *audioFile=nullptr;
AudioOutputI2S *audioOut=nullptr;
enum Alert : uint8_t { NONE, COLLISION, DOOR, ACCIDENT, GPS_ALERT };
Alert pending=NONE;

void stopMotors(){
  digitalWrite(A_IN1,LOW); digitalWrite(A_IN2,LOW);
  digitalWrite(A_IN3,LOW); digitalWrite(A_IN4,LOW);
  digitalWrite(B_IN1,LOW); digitalWrite(B_IN2,LOW);
  digitalWrite(B_IN3,LOW); digitalWrite(B_IN4,LOW);
}
float readDistanceCm(){
  digitalWrite(TRIG_PIN,LOW); delayMicroseconds(3);
  digitalWrite(TRIG_PIN,HIGH); delayMicroseconds(10); digitalWrite(TRIG_PIN,LOW);
  unsigned long us=pulseIn(ECHO_PIN,HIGH,30000UL);
  return us ? (us*0.0343f/2.0f) : -1.0f;
}
void requestAlert(Alert a){ if(pending==NONE) pending=a; }

void serviceAudio(){
  if(wav && wav->isRunning()){
    if(!wav->loop()){
      wav->stop(); delete wav; wav=nullptr;
      delete audioFile; audioFile=nullptr;
    }
    return;
  }
  if(wav){delete wav; wav=nullptr;}
  if(audioFile){delete audioFile; audioFile=nullptr;}
  if(pending==NONE || !audioOut) return;
  const char *path=nullptr;
  switch(pending){
    case COLLISION: path="/collision_guard_warning.wav"; break;
    case DOOR: path="/door_guard_warning.wav"; break;
    case ACCIDENT: path="/accident_guard_warning.wav"; break;
    case GPS_ALERT: path="/gps_status.wav"; break;
    default: break;
  }
  pending=NONE;
  if(!path) return;
  audioFile=new AudioFileSourceLittleFS(path);
  if(!audioFile || !audioFile->isOpen()){
    Serial.print("Missing/unopenable WAV: "); Serial.println(path);
    delete audioFile; audioFile=nullptr; return;
  }
  wav=new AudioGeneratorWAV();
  if(!wav || !wav->begin(audioFile,audioOut)){
    Serial.print("Could not start WAV: "); Serial.println(path);
    delete wav; wav=nullptr; delete audioFile; audioFile=nullptr;
  } else {
    Serial.print("Playing: "); Serial.println(path);
  }
}

void setup(){
  Serial.begin(115200); delay(300);
  pinMode(TRIG_PIN,OUTPUT); pinMode(ECHO_PIN,INPUT); pinMode(IR_PIN,INPUT);
  const int mp[]={A_IN1,A_IN2,A_IN3,A_IN4,B_IN1,B_IN2,B_IN3,B_IN4};
  for(int p:mp) pinMode(p,OUTPUT);
  stopMotors(); // no drive commands in this starter sketch

  Wire.begin(SDA_PIN,SCL_PIN);
  mpuOnline=mpu.begin(0x68,&Wire);
  if(mpuOnline){
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("MPU6050 online at 0x68");
  } else Serial.println("MPU6050 offline; check wiring");

  GPSSerial.begin(9600,SERIAL_8N1,27,4);
  if(!LittleFS.begin(false)) Serial.println("LittleFS mount failed; not formatted");
  else Serial.println("LittleFS mounted");

  audioOut=new AudioOutputI2S();
  audioOut->SetPinout(I2S_BCLK,I2S_LRC,I2S_DOUT);
  audioOut->SetGain(1.0);
  Serial.println("Vehicle Guardian ready. Motor outputs held LOW.");
}

void loop(){
  while(GPSSerial.available()) gps.encode(GPSSerial.read());
  unsigned long now=millis();

  if(now-lastDistanceMs>=120){
    lastDistanceMs=now; distanceCm=readDistanceCm();
    if(distanceCm>0 && distanceCm<=DANGER_CM && !collisionLatched){
      collisionLatched=true; requestAlert(COLLISION);
      Serial.println("Collision Guard: obstacle detected"); stopMotors();
    } else if(distanceCm>SAFE_CM) collisionLatched=false;
  }

  bool blocked=(digitalRead(IR_PIN)==LOW); // documented logic: LOW = object detected
  if(blocked && !doorWasBlocked){
    requestAlert(DOOR);
    Serial.println("Door Guard: simulated door opening blocked");
  }
  doorWasBlocked=blocked;

  if(mpuOnline){
    sensors_event_t a,g,t;
    mpu.getEvent(&a,&g,&t);
    float mag=sqrtf(a.acceleration.x*a.acceleration.x+
                     a.acceleration.y*a.acceleration.y+
                     a.acceleration.z*a.acceleration.z);
    if(mag>IMPACT_THRESHOLD_MS2 && now-lastImpactMs>=IMPACT_COOLDOWN_MS){
      lastImpactMs=now; requestAlert(ACCIDENT);
      Serial.println("Accident Guard: impact threshold exceeded"); stopMotors();
    }
  }

  if(gps.location.isValid() && !gpsAnnounced){
    gpsAnnounced=true; requestAlert(GPS_ALERT);
    Serial.println("GPS fix acquired");
    Serial.print("Latitude: "); Serial.println(gps.location.lat(),6);
    Serial.print("Longitude: "); Serial.println(gps.location.lng(),6);
  }

  serviceAudio();
  if(now-lastPrintMs>=1000){
    lastPrintMs=now;
    Serial.print("Distance: ");
    if(distanceCm>0){Serial.print(distanceCm,1);Serial.println(" cm");}
    else Serial.println("No echo");
    Serial.print("Door simulation: "); Serial.println(blocked?"BLOCKED":"UNBLOCKED");
    Serial.print("MPU6050: "); Serial.println(mpuOnline?"ONLINE":"OFFLINE");
    Serial.print("GPS: "); Serial.println(gps.location.isValid()?"FIX":"Waiting for valid location fix");
  }
}
