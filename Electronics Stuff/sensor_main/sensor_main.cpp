//Source file: defines all functions, declares and defines all variables to be used within them
//If you want to access a variable from the main program, you must extern declare it from header as well then define it here! Main program can only pull from hedaer!

#include <sensor_main.h>

//declare variables to be used in functions
const char *ssid = "wahoo";
const char *pwd = "";

int enValue = 0;

const char* ntpServer = "pool.ntp.org";

char * host = "";
int port = 80;
char * directory = "";

int delay_time = 0;
int reading_total = 0;

String sensor_ID = "";


//Sender functions and variables

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //this broadcasts to all MAC addresses

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

void OnDataSent(const wifi_tx_info_t *txInfo, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void initialize_esp_sender() {
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE); //fix suggested by CLAUDE

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void send_sync_signal() {
  strcpy(myData.a, "THIS IS A CHAR");
  myData.b = random(1,20);
  myData.c = 1.2;
  myData.d = false;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }

  else {
    Serial.println("Error sending the data");
  }
  
}



//Receiver functions

bool received_sync = false;

void OnDataRecv(const esp_now_recv_info *recvInfo, const uint8_t *data, int len) {
  memcpy(&myData, data, sizeof(myData));
  Serial.println("This printed line represents a dummy function executed upon the the board receiving an ESP NOW signal."); //could do an if statement to make it do different stuff depending on what the signal was...
  //senddata(envelopePin, database_to_send, samplingHz, samplingDur);
  received_sync = true;
}

void initialize_esp_receiver() {
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE); //fix suggested by CLAUDE
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv); //board is now always listening

}



void wifi_init() // function to connect to WiFi
{
  //WiFi.mode(STA); //Acting as a client

  //Get MAC Address:
  Serial.print("Board MAC Address: ");
  Serial.println(WiFi.macAddress());

  //Identify sensor; CHANGE IF LABELS OR SENSORS ARE CHANGED
  String mac_address = String(WiFi.macAddress());
  if (mac_address == "58:CF:79:D9:8D:7C"){
    sensor_ID = "1";
  }

  else if (mac_address == "58:CF:79:D8:AF:68"){
    sensor_ID = "2";
  }

  else if (mac_address == "58:CF:79:D9:89:00"){
    sensor_ID = "3";
  }

  else if (mac_address == "58:CF:79:D8:78:40"){
    sensor_ID = "4";
  }

  else {
    sensor_ID = "0"; //using 0 for unknown sensor
  }

  //Start a WiFi connection:
  WiFi.begin(ssid, pwd);
  Serial.println("\nConnecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void wifi_disconnect() // function to disconnect from WiFi
{
  Serial.println("Disconnecting from WiFi");
  WiFi.disconnect();

  while (WiFi.status() == WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nDisconnected from WiFi network");
}

void printLine()//printline function to print dashes until something completes
{
  Serial.println();
  for (int i=0; i<30; i++)
    Serial.print("-");
  Serial.println();
}

int readEnvelope(const int enPin) //function to read and serial print analog sound level
{
  enValue = analogReadMilliVolts(enPin);
  Serial.println(enValue);
  return enValue;
}

//need to have unique Sensor ID referred to in this function, so including it has a parameter rather than defining it and the referring to it. This way, it can be changed in the main program.
void senddata(const int envPin, String database, const int Hz, const int Dur) //takes sound readings and makes url string, THEN connects to WiFi, THEN DISCONNECTS from WiFi (this is necessary to ensure the sound level is accurate)
{
  //read sound and construct GET request

  //DATABASE INFORMATION
  if (database == "Landan") {
    host = "54.234.33.164"; //IP address of AWS server
    port = 80; //port used for HTTP
    directory = "/soundapp/esp32_sound/test_add2database2.php"; //directory and file called in HTTP request
  }

  else if (database == "Kane") {
    host = "54.209.25.97"; //IP address of AWS server
    port = 80; //port used for HTTP
    directory = "/rodman/test_directory/test_add2database.php"; //directory and file called in HTTP request
  }

  //SAMPLING INFORMATION
  delay_time = (1.0/(Hz))*1000; //time between readings in ms; must use 1.0 in division to obtain a decimal value
  reading_total = Hz*Dur; //total number of readings; used for loop counters

  //CALIBRATION AND GET REQUEST SETUP
  Serial.println("Calibrating sensor to surrounding sound level...");

  int loudness = 0;
  String request_append = "&reading=";

  int i = 0;
  while (i < (reading_total/2)) { //adjusts for half of sampling time
    readEnvelope(envPin);
    i++;
    delay(delay_time);
  }

  //TAKE READING FOR UPLOAD TO DATABASE

  Serial.println("Reading ambient sound...");

  loudness=readEnvelope(envPin);
  request_append += String(loudness);
  delay(delay_time);
  
  i = 1;
  while (i < reading_total) { //takes readings for upload
    Serial.print("Reading number ");
    Serial.print(i);
    Serial.print(": ");
    loudness = readEnvelope(envPin);
    request_append += ";" + String(loudness);
    i++;
    delay(delay_time);
  }

  Serial.println("Sound level measured. Preparing to upload data...");
  delay(1000);

  //connect to WiFi
  wifi_init();
  
  //obtain UTC
  configTime(0, 0, ntpServer);

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for NTP sync...");
    delay(1000);
  }

  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
  String timestamp = String(buf);
  timestamp.replace(" ", "%20");
  timestamp.replace(":", "%3A");
  Serial.println(&timeinfo, "UTC: %Y-%m-%d %H:%M:%S");

  //construct get_request
  String get_request = "GET " + String(directory) + "?time=";
  get_request += timestamp;
  get_request += ("&sensorid=" + sensor_ID);
  get_request += request_append;

  Serial.println(get_request);

  //connect to server
  printLine();
  Serial.println("Connecting to domain: " + String(host));

  //use WifiClient class to connect to server
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("connection failed");
    return;
  }

  Serial.println("Connected!");
  printLine();

  //Actually do GET request
  client.print(get_request);  
  client.println(" HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();
  client.println();
  client.stop();

  //server replies after receives HTTP GET request
  //print all lines of reply to Serial
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  //Close connection to server
  Serial.println();
  Serial.println("Closing connection");
  client.stop();

  //Disconnect from WiFi
  wifi_disconnect();
}