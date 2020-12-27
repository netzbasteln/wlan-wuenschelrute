/**
 * WLAN-WUENSCHELRUTE
 * As used in Netzbasteln 155 - netzbasteln.de/#155
 * Note: for ESP32, you need the lbernstone/Tone32 library.
 */

// Config --------------------------------

// Generic ESP32 (avoid ADC2 pins when using WiFi!):
// #define BUZZER_PIN 3
// #define LED_PIN 5

// Wemos D1 mini ESP8266:
#define BUZZER_PIN D8
// Note: For mysterical reasons on some WemosD1 boards the buzzer sometimes works on D3 only eventhough the Pin is set to D8
#define LED_PIN D4

// ---------------------------------------


#include <Arduino.h>

#if defined(ESP32)
  #include <WiFi.h>
  #include <esp_wifi.h>
  #include <Tone32.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif


String targetSSID;
uint8_t targetChannel;


void setup() {
  Serial.begin(115200);
  Serial.println("Hi.");

  // Initialize pins.
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize WiFi.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Do 1 scan, find nearest AP.
  Serial.println("Searching nearest WiFi ...");
  uint8_t found = 0;
  digitalWrite(LED_PIN, LOW);
  while (found < 1) {
    #if defined(ESP32)
    found = WiFi.scanNetworks(false, false, false, 120);
    #elif defined(ESP8266)
    found = WiFi.scanNetworks(false, false, false);
    #endif
  }
  digitalWrite(LED_PIN, HIGH);
  targetSSID = WiFi.SSID(0);
  targetChannel = WiFi.channel(0);
  Serial.printf("Found: %s on ch %d\r\n", targetSSID.c_str(), targetChannel);
  Serial.println("----------------");
  // Beep-out channel nr.
  for (int i = 0; i < targetChannel; i++) {
    digitalWrite(LED_PIN, LOW);
    #if defined(ESP32)
    tone(BUZZER_PIN, 1337, 200, 0);
    #elif defined(ESP8266)
    tone(BUZZER_PIN, 1337, 200);
    #endif
    digitalWrite(LED_PIN, HIGH);
    delay(300);
  }
  delay(1000);

  // Fake a country with only 1 channel, for faster scanning.
  #if defined(ESP32)
  wifi_country_t country = {"ZZ", targetChannel, targetChannel, 0, WIFI_COUNTRY_POLICY_MANUAL};
  esp_wifi_set_country(&country);
  #elif defined(ESP8266)
  wifi_country_t country = {"ZZ", targetChannel, targetChannel, WIFI_COUNTRY_POLICY_MANUAL};
  wifi_set_country(&country);
  #endif
}


void loop() {
  // Scan.
  #if defined(ESP32)
  found = WiFi.scanNetworks(false, false, false, 120);
  #elif defined(ESP8266)
  uint8_t found = WiFi.scanNetworks(false, false, false);
  #endif

  if (found > 0) {
    for (int i = 0; i < found; i++) {
      // Find own network.
      if (WiFi.SSID(i).equals(targetSSID)) {
        Serial.println(WiFi.RSSI(i));

        digitalWrite(LED_PIN, LOW);

        // Beep.
        long freq = map(WiFi.RSSI(i), -100, 0, 100, 5000);
        long duration = map(WiFi.RSSI(i), -100, 0, 10, 100);
        #if defined(ESP32)
        tone(BUZZER_PIN, freq, duration, 0);
        #elif defined(ESP8266)
        tone(BUZZER_PIN, (int) freq, duration);
        #endif

        digitalWrite(LED_PIN, HIGH);
        break;
      }
    }
  }
}
