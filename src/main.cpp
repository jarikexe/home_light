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
LightMode lingtMorningTime(7, 8);
LightMode lingtModeDayTime(23, 7);  
LightMode lingtModeEvening(16, 11);
LightMode lingtModePrepareForSleepTime(23, 0);
LightMode lingtModeNight(0, 7);
/*FUNCTIONS DECLARATION*/
void initWiFi();
void clear();
bool isInTimeShift(int hour, LightMode mode);
void morningModeEffect();
void dayModeEffect();

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
  if(isInTimeShift(now.hour(), lingtMorningTime)) {
    morningModeEffect();
  }
  else if(isInTimeShift(now.hour(), lingtModeDayTime)) {
    dayModeEffect();
  }


  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
}

bool isInTimeShift(int hour, LightMode mode) {
    if (mode.startTime <= mode.endTime) {
        return (hour >= mode.startTime && hour < mode.endTime);
    } else {
        return (hour >= mode.startTime || hour < mode.endTime);
    }
}

void clear() {
  Serial.println("clean up");
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
   FastLED.show();
}

void dayModeEffect() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(99, 255, 164);
  }
    int i = 50;
    bool mode = true;
  while(true) {
    if(mode && i < 255){
      i++;
    } else {
      i--;
    }
    if(i == 255 || i == 50) {
      mode = !mode;
    }  
    delay(10);
    Serial.println(i);
    FastLED.setBrightness(i);
    FastLED.show();
  }
}

void morningModeEffect() {
  static uint8_t brightness = 255;
  static bool increasing = false;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(random(256), 255, 255); 
  }

  if (increasing) {
    brightness += 30;
    if (brightness >= 255) {
      brightness = 255;
      increasing = false;
    }
  } else {
    brightness -= 30;
    if (brightness == 0) {
      increasing = true;
    }
  }

  FastLED.setBrightness(brightness);
  FastLED.show();
  delay(10);
}

