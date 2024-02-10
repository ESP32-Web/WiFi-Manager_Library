#include "ESP32Web.h"


// <========================> User Configuration Varaibles <============================> //

// Website Data
const char* title_type = "text-image"; // "text-only" or "text-image"
const char* main_title = "ESP32 Web";
const char* sub_title = "WiFi Manager";
const char* footer_tag = "Powered by ESP32 Web";

// WiFi HotSpot Details
const char* ap_ssid = "ESP32 Web"; // Hotspot WiFi Name
const char* ap_password = "esp32web"; // Hotspot WiFi Password
// How long WiFi Hotspot stays open for 
// -- Hotspot is energy consuming & chip can get hot
unsigned long apDuration = 200000; // 3 mins = 180000, 2 mins = 120000, 1 min = 60000

// Local Website URL
const char* local_URL = "esp32web"; //  accesible on "ESP32Web.local"

// ============================ Other Varaibles ============================ //


// Network Details
const char* ssid;
const char* password;
String ssid_connected;


// For WiFi Scan function
int wifiNum = -1;                // number of networks
String wifi_array[20] = {}; // all netwrks in object

unsigned long previousMillis = 0;
unsigned long interval = 10000;


// AP timer: How long access points stays open for
unsigned long apStartTime;
// unsigned long apDuration = 10000;

// AP time remainging: to send to website
unsigned long apTimerRemaining;

AsyncWebServer server(80);

// Data recieved from website
String receivedData;  // Variable to store the received data

// Tracks ESP32 mode
String mode; // "station" or "access-point"

// Timers
SimpleTimer t0_AP_Mode;
SimpleTimer t1_stopAP;
SimpleTimer t2;

// Flags
bool wifiScanActive = false;

//State
State currentState = Initiating;


// <========================> Global Function Declared <============================> //

// Code for each state;
void handleState() {

   if (wifiScanActive) {
      wifiScan();
   }

   if (currentState == Idle) {}



   if (currentState == Initiating) {}



   if (currentState == AP_Mode) {
      t0_AP_Mode.run();
   }



   if (currentState == STA_Mode) {}
}



// Handles State Change
void changeState(State fromtState, State toState) {

   // Idle       00
   // Initiating 01
   // AP_Mode    10
   // STA_Mode   11

   State var1 = fromtState;
   State var2 = toState;

   int combinedValue = (var1 << 2) | var2;
   Serial.print("conbinedValue: "); Serial.println(combinedValue);

   switch (combinedValue) {

      /// AP_Mode -> Idle
   case (AP_Mode << 2) | Idle:
      currentState = Idle;
      digitalWrite(2, LOW);
      break;
   }

}


// Scans WiFi Networks -> saves SSID's in global var
void wifiScan() {

   Serial.println("Scaning for WiFi options");

   // WiFi.scanNetworks will return the number of networks found
   int n = WiFi.scanNetworks();
   wifiNum = n;

   // unique SSID count
   int uniqueCount = 0;

   // n is the number of networks found
   if (n == 0) Serial.println("no networks found");
   else {

      Serial.print(n); Serial.println(" networks found");

      for (int i = 0; i < n; i++) {

         // assume the SSID is unique to begin with
         bool isUnique = true;

         // Check if the SSID is already in the wifi_array
         for (int j = 0; j < uniqueCount; j++) {
            if (WiFi.SSID(i) == wifi_array[j]) {
               isUnique = false;
               break;
            }
         }

         // If the SSID is unique, add it to wifi_array and print
         if (isUnique) {
            wifi_array[uniqueCount++] = WiFi.SSID(i);
            Serial.println(WiFi.SSID(i));
         }

         // Limit the unique SSIDs to a maximum number to avoid overflow
         if (uniqueCount >= 20) {
            break;
         }


      }

      Serial.print("Unique SSIDs Count:"); Serial.println(uniqueCount);

      // Update the global variable to the unique count
      wifiNum = uniqueCount;

   }

   Serial.println("Scan Complete");

   // Set the flag to false
   wifiScanActive = false;

}



// Handles POST requests
void handlePostRequest(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {

   // Clear the received data for the next request
   receivedData = "";

   // Store the received data in the variable "recievedData"
   for (size_t i = 0; i < len; i++) {
      receivedData += (char)data[i];
   }

   Serial.println("Recieved Data: ");
   Serial.println(receivedData);
   Serial.print("\n");

   // Parse the received data as a JSON object
   DynamicJsonDocument jsonDocument(1024);                                    // creates a json object with 1024 bytes initial capcity
   DeserializationError error = deserializeJson(jsonDocument, receivedData);  // JSON data in "receivedData" is parsed into "jsondocument"

   // Check if parsing succeeded
   if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
   }

   // Access specific values from the JSON object
   const char* requestType = jsonDocument["request"];          // can get value for each key in jsondocument

   // Do something with the values
   // Serial.print("request: ");
   // Serial.println(requestType);

   if (String(requestType) == "get-user-data") {

      Serial.println(title_type);
      Serial.println(main_title);
      Serial.println(sub_title);
      Serial.println(footer_tag);

      // Send a custom json message back
      DynamicJsonDocument responseJson(256);

      responseJson["title_type"] = title_type;
      responseJson["main_title"] = main_title;
      responseJson["sub_title"] = sub_title;
      responseJson["footer_tag"] = footer_tag;

      // String variable created
      String jsonResponse;
      // json object (responseJson) is then serialised in a json formatted string (jsonResponse)
      serializeJson(responseJson, jsonResponse);
      request->send(200, "application/json", jsonResponse);
      return;
   }


   if (String(requestType) == "get-ssid") {

      Serial.print("ssid: ");
      Serial.println(ssid_connected);

      // Send a custom json message back
      DynamicJsonDocument responseJson(150);

      responseJson["ssid"] = ssid_connected;

      // String variable created
      String jsonResponse;
      // json object (responseJson) is then serialised in a json formatted string (jsonResponse)
      serializeJson(responseJson, jsonResponse);
      request->send(200, "application/json", jsonResponse);
      return;
   }


   if (String(requestType) == "wifi-scan") {

      Serial.print("wifiNum: "); Serial.println(wifiNum);
      Serial.print("wifiScanActive: "); Serial.println(wifiScanActive);

      // If newtworks found are 0, then start a new scan
      if (wifiNum == -1 || wifiScanActive == true) {
         // Activate flag to start a new scan
         wifiScanActive = true;

         // Acknowledge the request
         request->send(200, "text/plain", "Request received, processing...");
         Serial.println("scan request acknowledged");
         Serial.print("\n");
         return;

      }
      // Scan data is available
      if (wifiNum != -1 && wifiScanActive == false) {

         // Send a custom json message back
         DynamicJsonDocument responseJson(256);

         responseJson["wifiNum"] = String(wifiNum); // 1st key-value pair

         Serial.print("wifiNum:");
         Serial.println(wifiNum);

         for (int i = 0; i < wifiNum; i++) {

            String key = String(i);
            responseJson[key] = wifi_array[i];   // the rest key-value pairs
            Serial.println(wifi_array[i]);
         }

         Serial.println("scan data prepared");

         // String variable created
         String jsonResponse;
         // json object (responseJson) is then serialised in a json formatted string (jsonResponse)
         serializeJson(responseJson, jsonResponse);
         request->send(200, "application/json", jsonResponse);

         Serial.println("scan data sent");

         // Reset the wifiNum to -1
         wifiNum = -1;
         return;


      }


   }


   if (String(requestType) == "wifi-setup") {

      // view data recieved
      const char* recieved_ssid = jsonDocument["username"];
      const char* recieved_password = jsonDocument["password"];

      Serial.print("SSID: ");
      Serial.println(recieved_ssid);

      Serial.print("Password: ");
      Serial.println(recieved_password);

      // ----------

      // Create a JSON object
      StaticJsonDocument<100> wifi_doc;

      // Add key-value pairs to json object
      wifi_doc["ssid"] = recieved_ssid;
      wifi_doc["password"] = recieved_password;

      // Convert the JSON object to a string
      String wifi_jsonString;
      serializeJson(wifi_doc, wifi_jsonString);

      // Print the JSON string
      Serial.println(wifi_jsonString);

      // ----------

      // Save the JSON string to a file
      File file = LittleFS.open("/wifi-config.json", "w+");
      if (!file) {
         Serial.println("Failed to create file");
         return;
      }
      file.print(wifi_jsonString);
      file.close();

      //------------

      // Reply back to the website

      request->send(200, "text/plain", "WiFi Setup Data Recieved & Configured");
      delay(1000);

      // ----------

      // Restart ESP
      ESP.restart();

   }


   if (String(requestType) == "current-mode") {

      Serial.println(mode);

      // Reply back to the website
      DynamicJsonDocument responseJson(150);

      responseJson["mode"] = mode; // 1st key-value pair

      // String variable created
      String jsonResponse;
      // json object (responseJson) is then serialised in a json formatted string (jsonResponse)
      serializeJson(responseJson, jsonResponse);
      request->send(200, "application/json", jsonResponse);
      return;

   }


   if (String(requestType) == "get-ap-timer") {

      // Reply back to the website
      DynamicJsonDocument responseJson(150);

      responseJson["timer"] = apTimerRemaining - 20; // 1st key-value pair

      // String variable created
      String jsonResponse;
      // json object (responseJson) is then serialised in a json formatted string (jsonResponse)
      serializeJson(responseJson, jsonResponse);
      Serial.print("timer json response:");
      Serial.println(jsonResponse);
      request->send(200, "application/json", jsonResponse);
      return;

   }


   if (String(requestType) == "reset-esp32") {

      // view data recieved

      // ----------

      // Create a JSON object
      StaticJsonDocument<100> wifi_doc;

      // Add key-value pairs to json object
      wifi_doc["ssid"] = "Hyperoptic Fibre 4AF7";
      wifi_doc["password"] = "ETVYEQP?";

      // Convert the JSON object to a string
      String wifi_jsonString;
      serializeJson(wifi_doc, wifi_jsonString);

      // Print the JSON string
      Serial.println(wifi_jsonString);

      // ----------

      // Save the JSON string to a file
      File file = LittleFS.open("/wifi-config.json", "w+");
      if (!file) {
         Serial.println("Failed to create file");
         return;
      }
      file.print(wifi_jsonString);
      file.close();


      // Reply back to the website


      request->send(200, "text/plain", "ESP32 Wifi Data Reset");



      // ----------
      // Restart ESP32
      ESP.restart();

   }


   if (String(requestType) == "connection-check") {

      // Reply back to the website
      request->send(200, "text/plain", "connection-good");
      return;
   }
}



// Connects to WiFi with custom local domain
void startWiFi() {

   Serial.println(ssid);
   Serial.println(password);

   ssid_connected = ssid;
   Serial.print("ssid_connected: ");
   Serial.println(ssid_connected);

   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
   Serial.print("Connecting to WiFi ..");
   unsigned long startAttemptTime = millis();


   while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);

      // Check if connection attempt timed out (20 sec)
      if (millis() - startAttemptTime > 10000) {
         Serial.println("\nFailed to connect to WiFi. Check credentials or move closer to the access point.");
         setupWiFi();
         return;
      }
   }

   // To send to the react website for correct component rendering
   mode = "station";

   Serial.println("\nConnection Successful");
   Serial.println("");
   Serial.print("Connected to ");
   Serial.println(ssid);
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());

   // Allocating a custom local URL
   if (MDNS.begin(local_URL)) {  // ESP32Web.local/
      Serial.println("MDNS responder started");
   }

   // get wifi strength
   Serial.print("RRSI: ");
   Serial.println(WiFi.RSSI());

   // Blue LED on 
   digitalWrite(2, HIGH);

   // -----------------
   currentState = STA_Mode;
}



// Start a Access Point Connection
void startAP() {

   Serial.println("Initialising Soft Access Point...");

   // // Initialize the ESP32 WiFi library
   // WiFi.mode(WIFI_AP);

   // // http://192.168.4.1/ 

   // // Set your desired IP address, subnet mask, and gateway
   // IPAddress ip(192, 168, 4, 1);  // Change this to your desired IP
   // IPAddress subnet(255, 255, 255, 0);
   // IPAddress gateway(192, 168, 4, 1);  // Should match the IP address above

   // // Configure the softAP with the custom IP settings
   // WiFi.softAPConfig(ip, gateway, subnet);

   // Initialising ESP32 AP
   WiFi.softAP(ap_ssid, ap_password);
   Serial.println("softap");
   Serial.println("");
   Serial.println(WiFi.softAPIP()); // prints ip address

   // To send to the react website for correct component rendering
   mode = "access-point";

   // Allocating a custom local URL
   // 196.168.4.1
   if (MDNS.begin(local_URL)) {  // Raptor.local/
      Serial.println("MDNS responder started");
   }

   // Stop Server After 2 mins: var registers start time
   apStartTime = millis();

   // -----------------
   currentState = AP_Mode;

}


// Stop Access Point
void stopAP() {
   // Serial.println("stopAP");

   if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
      apTimerRemaining = (apDuration - (millis() - apStartTime)) / 1000;
      (apTimerRemaining > 240000) ? apTimerRemaining = 0 : apTimerRemaining = apTimerRemaining;
      Serial.println(apTimerRemaining);
   }

   // AP Timeout
   if ((WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) && millis() - apStartTime >= apDuration) {
      // Disconnect and stop the soft AP
      WiFi.softAPdisconnect(true);
      Serial.println("Soft AP disconnected");

      // ----------
      changeState(AP_Mode, Idle);
   }
}



// Starts up the React website
void startWebserver() {

   // Initialize LittleFS
   if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
      Serial.println("LittleFS Mount Failed");
      return;
   }

   // Read "Data" Direcotry 
   listDir(LittleFS, "/", 0);

   // Serve the static files from LittleFS
   server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

   // Configure the server to handle the data request
   server.on(
      "/endpoint", HTTP_POST,
      [](AsyncWebServerRequest* request) {
         // Handle the POST request by calling the separate function
         // handlePostRequest(request, NULL, 0, 0, 0);
      },
      NULL,
      handlePostRequest
      );

   // serves the site on subpages
   server.onNotFound([](AsyncWebServerRequest* request) {
      if (!request->url().startsWith("/api/")) {
         request->send(LittleFS, "/index.html", "text/html");
      }
      });



   // Start the server
   server.begin();

   // Configure the server to handle the data request
   server.on(
      "/endpoint", HTTP_POST,
      [](AsyncWebServerRequest* request) {
         // Handle the POST request by calling the separate function
         // handlePostRequest(request, NULL, 0, 0, 0);
      },
      NULL,
      handlePostRequest
      );



   // Start the server
   server.begin();
}


// -------not used------
void wifi_StatusCheck() {
   unsigned long currentMillis = millis();
   // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
   if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
      Serial.print(millis());
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
      previousMillis = currentMillis;
   }
   else {
      Serial.print("Wifi Status: ");
      Serial.println(WiFi.status());
   }
}



// Launch a AP browser to enter & save WiFi details
void setupWiFi() {

   // end server & wifi
   server.end();
   WiFi.disconnect(true);


   // start AP and restart server
   startAP();
   startWebserver();

}



// Check file data to start wifi 
void checkWiFiConfig() {
   Serial.println("Check WiFi Config Running...");

   // Mount LittleFS
   if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
      Serial.println("LittleFS Mount Failed");
      return;
   }

   // Reading LitteFS Files
   listDir(LittleFS, "/", 0);
   readFile(LittleFS, "/wifi-config.json");

   // Open "wifi-config.json" file
   File jsonFile = LittleFS.open("/wifi-config.json", "r");  // use "r" for read mode & "w" for overide mode, "a" for append to end mode

   // No file -> Then let's create one
   if (!jsonFile) {
      Serial.println("Failed to open data.json file");
      setupWiFi(); // enter wifi details via hosted webpage
      return;
   }

   // File exists -> Good, lets use the data in it to start WiFi
   else {

      // Read the JSON data
      String jsonData = jsonFile.readString();

      // Close the file
      jsonFile.close();

      // Parse the JSON data
      const size_t capacity = JSON_OBJECT_SIZE(20);  // Object size refers to the number of key vlaue pairs
      DynamicJsonDocument jsonDoc(capacity);

      DeserializationError error = deserializeJson(jsonDoc, jsonData);
      if (error) {
         Serial.println("Failed to parse JSON");
         return;
      }
      else {
         Serial.println("Parse Complete");
      }

      // Access JSON values
      ssid = jsonDoc["ssid"];
      password = jsonDoc["password"];

      // ------------------

      startWiFi();
      startWebserver();
   }



}



void ledBlink() {

   digitalWrite(2, !digitalRead(2));

}


