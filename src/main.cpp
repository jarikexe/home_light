#include <FastLED.h>
#include <BH1750.h>
#include <Wire.h>
#include <WiFi.h>
#include "time.h"


BH1750 lightMeter;

/*PIN OUT CONFIG*/
#define LED_PIN 18

/*GENERAL CONFIG*/
#define NUM_LEDS 173
#define MINIMAL_DAYLIGHT_BRIGHTNESS 20
const char* ssid = "jarik’s iPhone";
const char* password = "33333333";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
int hours = 0;
int minutes = 0;
CRGB leds[NUM_LEDS];

/*FUNCTIONS DECLARATION*/
void initWiFi();
void clear();

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  // initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}

void initWiFi() {
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  float lux_original = lightMeter.readLightLevel();
  int lux_prepared = (int)round(lux_original);

  if(lux_prepared < MINIMAL_DAYLIGHT_BRIGHTNESS) {

    int map_lux_to_rgb_val = map(lux_prepared, MINIMAL_DAYLIGHT_BRIGHTNESS, 0, 1, 255);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, random(50, 111), 1);
    }


    Serial.println(map_lux_to_rgb_val);
    FastLED.setBrightness(map_lux_to_rgb_val);
    FastLED.show();
  } else {
    clear();
  }
  delay(100);
}
void clear() {
  Serial.println("clean up");
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
   FastLED.show();
}

