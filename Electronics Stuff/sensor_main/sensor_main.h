//Header file: declares all functions; also include all libraries to be used
//Think of it like this: the header declares functions, then pulls their actual definitions from the source file. Main program then pulls all of header (including the definitions the header itself pulled).
//Why not just decalre and define everything in the header? Because defining functions in a separate .cpp file lowers compilation time significantly!

#include <WiFi.h>
#include <time.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

void wifi_init(); // function to connect to WiFi

void wifi_disconnect(); //function to disconnect from WiFi

void printLine();

int readEnvelope(const int enPin);

void senddata(const int envPin, String database, const int Hz, const int Dur);

void syncBoard(const int kRecvPin, const int IRreceiverPowerPin);