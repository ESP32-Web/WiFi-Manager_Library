#ifndef MyFunctions_H
#define MyFunctions_H
#include <Arduino.h>

// // <========================> Libraries Declared <============================> //

#include "FS.h"
#include <LittleFS.h>
#include <time.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include "ArduinoJson.h"
#include <LittleFS_extra.h>
#include <SimpleTimer.h>
#include <DNSServer.h>

// <========================> Gloabal Varibales Declared <============================> //

extern const char* title_type;
extern const char* main_title;
extern const char* sub_title;
extern const char* made_by;


extern const char* ssid;
extern const char* password;
extern String ssid_connected;



extern int wifiNum;
extern String wifi_array[20];


extern unsigned long previousMillis;
extern unsigned long interval;


// AP timer: variables used in startAP() & stopAP()
extern unsigned long apStartTime;
extern unsigned long apDuration;

extern unsigned long apTimerRemaining;

// AsyncWebServer server(80);

extern String receivedData;  // Variable to store the received data


extern String mode;


// Timers
extern SimpleTimer t0_AP_Mode;
extern SimpleTimer t1_stopAP;
extern SimpleTimer t2;

//State
enum State {
   Idle,        // 00
   Initiating,  // 01
   AP_Mode,     // 10
   STA_Mode,    // 11
};

// Flags
extern bool wifiScanActive;
extern bool captiveWebPortal;

extern State currentState;


// <========================> Global Function Declared <============================> //
void handleState();
void changeState(State fromState, State toState);
void wifiScan();
void handlePostRequest(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
void startWiFi();
void startAP();
void stopAP();
void startWebserver();
void wifi_StatusCheck();
void setupWiFi();
void checkWiFiConfig();
void ledBlink();


#endif