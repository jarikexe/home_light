#include <FastLED.h>
#include <BH1750.h>
#include <Wire.h>
#include <WiFi.h>
#include "time.h"
#include "RTClib.h"
#include "LightMode.cpp"


BH1750 lightMeter(0x23);
RTC_DS3231 rtc;


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
CRGB leds[NUM_LEDS];
LightMode lightMode1(23, 0);  
LightMode lightMode2(0, 7);
// LightMode lightMode1(11, 12);  

/*FUNCTIONS DECLARATION*/
void initWiFi();
void clear();

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  if (! rtc.begin()) {
    Serial.println("Could not find RTC! Check circuit.");
    while (1);
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));
  // Serial.print("RRSI: ");
  // Serial.println(WiFi.RSSI());
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
  
  DateTime now = rtc.now();
  float lux_original = lightMeter.readLightLevel();
  int lux_prepared = (int)round(lux_original);
  if(lux_prepared < MINIMAL_DAYLIGHT_BRIGHTNESS) {

    int map_lux_to_rgb_val = map(lux_prepared, MINIMAL_DAYLIGHT_BRIGHTNESS, 0, 1, 255);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, random(50, 111), 1);
    }
    if(now.hour() >= lightMode1.startTime && now.hour() != lightMode1.endTime) {
      if(now.hour() >= lightMode1.startTime && now.hour() != lightMode1.endTime) {
        Serial.println(now.minute() * 60 + now.second());
        int seconds_to_brightnes = map(now.minute() * 60 + now.second(), 0, 3600, 255, 0);
        Serial.println(seconds_to_brightnes);
        FastLED.setBrightness(seconds_to_brightnes);
        Serial.println(seconds_to_brightnes);
      }
      if(now.hour() >= lightMode2.startTime && now.hour() <= lightMode2.endTime) {
        clear();
      }
    } else  {
      FastLED.setBrightness(map_lux_to_rgb_val);
    }
    FastLED.show();
  } else {
    clear();
  }

  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());


  delay(2000);
}
void clear() {
  Serial.println("clean up");
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
   FastLED.show();
}

