//Main program: code sent to ESP32, pulls what it needs from header file
//can change sensor ID, envelope pin, database information (any variables passed as parameters can be changed in main program)

#include <kane_functions.h>

//Sensor ID, envelope pin, database data is sent to (ALL CAN BE CHANGED)
String sensor_ID = "3";
const int envelopePin = 3;
String database_to_send = "Kane"; //either "Landan" or "Kane"
const int samplingHz = 100; //readings per second
const int samplingDur = 10; //duration of sampling in seconds

const int IRpowerPin = 5; //pin providing power to IR receiver
const int IRsignalPin = 10; //pin connected to IR revceiver OUT
int sync_time = 0;

const int time_between_readings = 30000; //30 seconds between readings
int time_elapsed = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Hello! This program will collect ambient sound data.");
  Serial.println();
  syncBoard(IRsignalPin, IRpowerPin);

  sync_time = millis(); //sets sync_time to time the board was synced
  Serial.print("The millis() time this board was synced was ");
  Serial.println(sync_time);

  delay(3000); //delay to allow voltage to adjust for accurate sound reading

}

void loop() {
  // put your main code here, to run repeatedly:
  time_elapsed = (millis() - sync_time);

  if (time_elapsed % time_between_readings == 0){ //millis() is independent of the Hz set delays
    senddata(sensor_ID, envelopePin, database_to_send, samplingHz, samplingDur);
    delay(1); //must include or else this will repeat 3 times
  }
}