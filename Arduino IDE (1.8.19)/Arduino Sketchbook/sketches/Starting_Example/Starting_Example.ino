#include <ESP32Web.h>
// For documentation on ESP32Web.h, see: https://esp32web.com/docs


// LED Pin
#define LED_PIN 2

void setup() {
   Serial.begin(115200);
   delay(4000); // not always needed. Some boards need a delay to allow Serial to be ready

   pinMode(LED_PIN, OUTPUT);
   digitalWrite(LED_PIN, LOW);

   // Looping Functions: 
   t0_AP_Mode.setInterval(1000, ledBlink); // timer for LED blink
   t0_AP_Mode.setInterval(1000, stopAP); // timer for AP mode (WiFi Hostpot)

   // check config file -> run AP or STA mode -> start web server
   checkWiFiConfig();

   Serial.println("Setup done");
}



void loop() {

   handleState(); // handle the state of the web server (AP or STA)

   // ----- Write your looping code below -------- //



}
