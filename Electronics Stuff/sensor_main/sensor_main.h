//Header file: declares all functions; also include all libraries to be used
//Think of it like this: the header declares functions, then pulls their actual definitions from the source file. Main program then pulls all of header (including the definitions the header itself pulled).
//Why not just decalre and define everything in the header? Because defining functions in a separate .cpp file lowers compilation time significantly!

#include <WiFi.h>
#include <time.h>
#include <esp_now.h>
#include <esp_wifi.h> //fix suggested by CLAUDE

extern const int envelopePin; //trying to pull these from the main program so they can be used in a function in the souce file
extern String database_to_send;
extern const int samplingHz;
extern const int samplingDur;


void wifi_init(); // function to connect to WiFi

void wifi_disconnect(); //function to disconnect from WiFi

void printLine();

int readEnvelope(const int enPin);

void senddata(const int envPin, String database, const int Hz, const int Dur);



//Sender functions
void OnDataSent(const wifi_tx_info_t *txInfo, esp_now_send_status_t status);

void initialize_esp_sender();

void send_sync_signal();



//Receiver functions

void OnDataRecv(const esp_now_recv_info *recvInfo, const uint8_t *data, int len);

void initialize_esp_receiver();

extern bool received_sync;