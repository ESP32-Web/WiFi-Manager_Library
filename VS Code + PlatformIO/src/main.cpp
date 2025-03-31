#include <Arduino.h>
#include <ESP32Web.h>
// For documentation on ESP32Web.h, see: https://esp32web.com/docs



void setup() {
   Serial.begin(115200);
   delay(4000); // not always needed. Some boards need a delay to allow Serial to be ready

   // check config file -> run AP or STA mode -> start web server
   checkWiFiConfig();

   Serial.println("Setup done");
}



void loop() {

   handleState(); // handle the state of the web server (AP or STA)

   // ----- Write your looping code below -------- //


   // The code below is an example of how to use the currentState variable & restart the AP Mode
   // You may errase all of the below - it's just some guidance on conditional statements

   // if connected to a WiFi network
   if (currentState == STA_Mode) {
      Serial.println("STA Mode");
      // Your code here
   }


   // if not connected to a WiFi network & in AP mode
   if (currentState == AP_Mode) {
      Serial.println("AP Mode");
      // Your code here
   }


   // if in idle mode (AP mode timed out)
   if (currentState == Idle) {
      Serial.println("Idle Mode");
      // Your code here

      // Restart the AP mode after 5 seconds or based on your requirement
      delay(5000);
      checkWiFiConfig();
   }

   delay(2000); // delay to prevent flooding the serial monitor

}



