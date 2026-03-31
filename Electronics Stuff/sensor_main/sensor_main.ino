//README: MOFIDY APPROPRIATE PARAMETERS BEFORE UPLOADING CODE TO BOARD

//Main program: code sent to ESP32, pulls what it needs from header file
//Can change pins, database information, and sampling information (any variables passed as parameters can be changed in main program)

#include <sensor_main.h>

//MODIFY THESE PARAMETERS BEFORE UPLOADING CODE TO BOARD
const int envelopePin = 3;
String database_to_send = "Landan"; //either "Landan" or "Kane"
const int samplingHz = 100; //readings per second
const int samplingDur = 10; //duration of sampling in seconds
const int IRpowerPin = 5; //pin providing power to IR receiver
const int IRsignalPin = 10; //pin connected to IR revceiver OUT
const double minutes_between_readings = 10; //time between readings in minutes; CHANGE VAR TYPE FOR INT VALUES

//Sensor ID is automatically detected by the board using its MAC Address. If the labels or sensors are changed, update sensor_main.cpp accordingly

//initialize variables
int sync_time = 0;
int time_elapsed = 0;
const int time_between_readings = ((minutes_between_readings*60)*1000);
int counter = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Hello! This program will collect ambient sound data.");
  Serial.println();
  syncBoard(IRsignalPin, IRpowerPin);

  sync_time = millis(); //sets sync_time to time the board was synced (in ms relative to when board was last reset)
  Serial.print("The millis() time this board was synced was ");
  Serial.println(sync_time);
}

void loop() {
  // put your main code here, to run repeatedly:
  time_elapsed = (millis() - sync_time); //using millis() because it tracks time since board was reset independently of any program running

  if (time_elapsed >= (counter*time_between_readings)){
    senddata(envelopePin, database_to_send, samplingHz, samplingDur);
    delay(1);
    counter++;
  }
}