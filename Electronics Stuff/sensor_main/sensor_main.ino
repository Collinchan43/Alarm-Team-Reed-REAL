//README: MOFIDY APPROPRIATE PARAMETERS BEFORE UPLOADING CODE TO BOARD

//Main program: code sent to ESP32, pulls what it needs from header file
//Can change pins, database information, and sampling information (any variables passed as parameters can be changed in main program)

#include <sensor_main.h>

//MODIFY THESE PARAMETERS BEFORE UPLOADING CODE TO BOARD
const int envelopePin = 3;
String database_to_send = "Kane"; //either "Landan" or "Kane"
const int samplingHz = 100; //readings per second
const int samplingDur = 10; //duration of sampling in seconds
const double minutes_between_readings = 0.75; //time between readings in minutes; CHANGE VAR TYPE FOR INT VALUES
String sensor = "sender"; //either "sender" (sets the sync time) or "receiver" (follows sender's sync time)

//Sensor ID is automatically detected by the board using its MAC Address. If the labels or sensors are changed, update sensor_main.cpp accordingly

//initialize variables
const int time_between_readings = ((minutes_between_readings*60)*1000);
int time_last_read = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Hello! This program will collect ambient sound data.");
  Serial.println();
  WiFi.mode(WIFI_STA);

 if (sensor == "sender"){
    wifi_init();
    wifi_disconnect(); //gets board ot same radio channel as wahoo
    initialize_esp_sender();
    Serial.println("This board is a sender.");
  }

  else if (sensor == "receiver") {
    wifi_init();
    wifi_disconnect(); //gets board to same radio channel as wahoo
    initialize_esp_receiver();
    Serial.println("This board is a receiver.");
  }

  Serial.println("WiFi channel: ");
  Serial.println(WiFi.channel());
 // senddata(envelopePin, database_to_send, samplingHz, samplingDur); //this is here for when minutes between readings goes back up to 10
}

void loop() {
  // put your main code here, to run repeatedly:
  if (sensor == "sender") {
    if ((millis() - time_last_read) > time_between_readings) {
      time_last_read = millis();
      send_sync_signal();
      Serial.println("Board would be reading values right now");
      senddata(envelopePin, database_to_send, samplingHz, samplingDur);
      initialize_esp_sender(); //fix suggested by CLAUDE
      //wifi_init();
      //wifi_disconnect();
    }
  }

  if (received_sync) { //must use this if statement and boolean because cannot conncet to WiFi within OnDataRecv
    //Serial.println("Second dummy function indicating sync signal received.");
    senddata(envelopePin, database_to_send, samplingHz, samplingDur);
    received_sync = false;
    initialize_esp_receiver(); //FIX SUGGESTED BY CLAUDE
    
    //wifi_init();
    //wifi_disconnect();
  }


}