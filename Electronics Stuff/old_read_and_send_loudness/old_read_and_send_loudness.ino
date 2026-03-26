//include WiFi library
#include <WiFi.h>

//wifi ssid and password (using wahoo)
const char *ssid = "wahoo";
const char *pwd = "";

//Set envelope pin and initialize envelope value
const int enPin = 3;
int enValue = 0;

void wifi_init() // function to connect to WiFi
{
  WiFi.mode(WIFI_STA); //Acting as a client

  //Get MAC Address:
  Serial.print("Board MAC Address: ");
  Serial.println(WiFi.macAddress());

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

//domain, port, and directory variables (SPECIFIC TO DATABASE)
const char * hostDomain = "54.234.33.164"; //IP address of AWS server
const int hostPort = 80; //port used for HTTP
const char * hostDir = "/soundapp/esp32_sound/test_add2database.php"; //directory and file called in HTTP request

void printLine()//printline function to print dashes until something completes
{
  Serial.println();
  for (int i=0; i<30; i++)
    Serial.print("-");
  Serial.println();
}

int readEnvelope() //function to read and serial print analog sound level, delays 0.1 seconds to make reading 10 Hz (ADJUSTABLE)
{
  enValue = analogRead(enPin);
  Serial.println(enValue);
  return enValue;
  delay(100); //delay 0.1 s to next read
}

void senddata(const char * host, uint8_t port, const char * directory) //takes sound readings and makes url string, THEN connects to WiFi, THEN DISCONNECTS from WiFi (this is necessary to ensure the sound level is accurate)
{
  //read sound and construct GET request
  Serial.println("Reading ambient sound...");

  int loudness = 0;
  String get_request = "GET " + String(directory) + "?reading=";
  loudness = readEnvelope();
  get_request += String(loudness);

  int i = 0;
  while (i < 249) {
    readEnvelope();
    i++;
  }

  i = 0;
  while (i < 99) { //takes 100 total readings (ADJUSTABLE)
    loudness = readEnvelope();
    get_request += ";" + String(loudness);
    i++;
  }
  delay(1000);

  //connect to WiFi
  wifi_init();

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Hello! This program will collect 10 seconds of ambient sound data every 30 seconds and send it to the database.");
  Serial.println();
  delay(3000); //delay to allow for accurate sound reading

}

void loop() {
  // put your main code here, to run repeatedly:
  senddata(hostDomain, hostPort, hostDir);
  delay(20000); //delay 20 seconds - makes total period 30 seconds (ADJUSTABLE)
}
