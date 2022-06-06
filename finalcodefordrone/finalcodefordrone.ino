// ---------------------------------------------------------------------------
#include <Servo.h>
#include <ArduinoJson.h>
// ---------------------------------------------------------------------------
#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs
#define DEFAULT_PULSE 1055 
#define ACTIVE_PULSE 1150
// ---------------------------------------------------------------------------
Servo motA, motB, motC, motD;
int speeds[4] = {MIN_PULSE_LENGTH, MIN_PULSE_LENGTH, MIN_PULSE_LENGTH, MIN_PULSE_LENGTH}; 
DynamicJsonDocument data(1024);
String json = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
// ---------------------------------------------------------------------------
unsigned long last_read = 0;
int read_interval = 1000;
bool is_connected = false;
/**
 * Initialisation routine
 */
void setup() {
    Serial.begin(9600);
    //calibration of esc's and motors
    motA.attach(4, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motB.attach(7, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motC.attach(5, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motD.attach(6, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    delay(1000);
    motA.writeMicroseconds(MAX_PULSE_LENGTH);
    motB.writeMicroseconds(MAX_PULSE_LENGTH);
    motC.writeMicroseconds(MAX_PULSE_LENGTH);
    motD.writeMicroseconds(MAX_PULSE_LENGTH);   
    delay(5000); 
    motA.writeMicroseconds(MIN_PULSE_LENGTH);
    motB.writeMicroseconds(MIN_PULSE_LENGTH);
    motC.writeMicroseconds(MIN_PULSE_LENGTH);
    motD.writeMicroseconds(MIN_PULSE_LENGTH);
    delay(1000);  
}
/**
 * Main function
 */
void loop() {
    if (Serial.available() && millis() > (last_read + read_interval)) {
      if(!is_connected){ // Bluetooth is connected and signal has been sent from phone to arduino
        motA.writeMicroseconds(DEFAULT_PULSE);
        motB.writeMicroseconds(DEFAULT_PULSE);
        motC.writeMicroseconds(DEFAULT_PULSE);
        motD.writeMicroseconds(DEFAULT_PULSE); 
        is_connected = true;
        delay(1000); 
      }
        json = Serial.readStringUntil('\n');
        deserializeJson(data, json);
        for(int i = 0; i < 4; i++){
          speeds[i] = DEFAULT_PULSE;
        }
        //forward and backward
        if(data["pitch"].as<int>() < 0){
          speeds[0] += data["pitch"].as<int>() * -7;
          speeds[1] += data["pitch"].as<int>() * -5;
        } else if(data["pitch"].as<int>() > 0){
            speeds[2] += data["pitch"].as<int>() * 7;
            speeds[3] += data["pitch"].as<int>() * 7;
        }
        //left and right
        if(data["roll"].as<int>() < 0){
          speeds[0] += data["roll"].as<int>() * -7;
          speeds[3] += data["roll"].as<int>() * -7;
        } else if(data["roll"].as<int>() > 0){
            speeds[1] += data["roll"].as<int>() * 5;
            speeds[2] += data["roll"].as<int>() * 7;
        }
        motA.writeMicroseconds(speeds[0]);
        motB.writeMicroseconds(speeds[1]);
        motC.writeMicroseconds(speeds[2]);
        motD.writeMicroseconds(speeds[3]);

        while(Serial.available()) Serial.read();
        last_read = millis();
  }
}
