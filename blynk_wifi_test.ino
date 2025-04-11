#define BLYNK_TEMPLATE_ID "TMPL36uBh0Fzo"
#define BLYNK_TEMPLATE_NAME "Fall Detection System using IoT"
#define BLYNK_AUTH_TOKEN "tkDRNsL7fft3aj391vxyboGBBVZwDT1F"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define LED_PIN 2  // Built-in LED on ESP32

char ssid[] = "OnePlus";
char pass[] = "1234512345@";

void checkConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi Disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
  } else {
    Serial.println("✅ WiFi Connected!");
  }

  if (!Blynk.connected()) {
    Serial.println("⚠️ Blynk Disconnected! Reconnecting...");
    Blynk.connect();
  } else {
    Serial.println("✅ Blynk Connected!");
  }

  digitalWrite(LED_PIN, (WiFi.status() == WL_CONNECTED && Blynk.connected()) ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  Serial.println("ESP32 Connection Check Started...");
}

void loop() {
  Blynk.run();
  checkConnections();
  delay(5000);  // Check every 5 seconds
}
