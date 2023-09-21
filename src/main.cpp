#include <FastLED.h>
#include <BH1750.h>
#include <Wire.h>
#include <WiFi.h>
#include "time.h"
#include "RTClib.h"
#include "LightMode.cpp"
#include <Keypad.h>

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

/*LIGHT MODES DECLARATION*/
LightMode lingtMorningTime(7, 8);
LightMode lingtModeDayTime(8, 18);  
LightMode lingtModeEvening(18, 0);
LightMode lingtModePrepareForSleepTime(0, 1);
LightMode lingtModeNight(0, 7);

/*FUNCTIONS DECLARATION*/
void initWiFi();
void clear();
bool isInTimeShift(int hour, LightMode mode);
void morningModeEffect();
void dayModeEffect();
void prepareForSleepEffect(DateTime now);
void evningEffect();
void romanticMoodEffect();

CRGBPalette16 currentPalette = PartyColors_p;
CRGBPalette16 targetPalette = PartyColors_p;
TBlendType    currentBlending = LINEARBLEND;        

const byte ROWS = 1;
const byte COLS = 4;
int customMode = 0;

char keys[ROWS][COLS] = {
{1,2,3,4},
};
byte rowPins[ROWS] = {23};
byte colPins[COLS] = {2, 4, 5, 19};

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

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

  int key = kpd.getKey();
  if(key) {
    Serial.print(key);
    customMode = key;
    dayModeEffect();
  }

  DateTime now = rtc.now();
  float lux_original = lightMeter.readLightLevel();
  int lux_prepared = (int)round(lux_original);
  if(customMode == 0) {
    if(isInTimeShift(now.hour(), lingtMorningTime)) {
        morningModeEffect();
      }
      else if(isInTimeShift(now.hour(), lingtModeDayTime)) {
        dayModeEffect();
      }
      else if(isInTimeShift(now.hour(), lingtModeEvening)) {
        evningEffect();
      }

      else if(isInTimeShift(now.hour(), lingtModeNight)) {
        clear();
      }
      else if(isInTimeShift(now.hour(), lingtModePrepareForSleepTime)) {
        prepareForSleepEffect(now);
      }
  } else {
    switch (customMode)
    {
    case 1:
      dayModeEffect();
      break;
    case 2:
      romanticMoodEffect();
      break;
    case 3:
      morningModeEffect();
    case 4:
      clear();  
      break;  
    default:
      customMode = 0;
    }
  }

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

void evningEffect() {
  float lux_original = lightMeter.readLightLevel();
  int lux_prepared = (int)round(lux_original);

  if(lux_prepared < MINIMAL_DAYLIGHT_BRIGHTNESS) {

    int map_lux_to_rgb_val = map(lux_prepared, MINIMAL_DAYLIGHT_BRIGHTNESS, 0, 1, 255);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, random(50, 111), 1);
    }


    FastLED.setBrightness(map_lux_to_rgb_val);
    FastLED.show();
    delay(1000);
  } else {
    clear();
  }
}

void prepareForSleepEffect(DateTime now) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, random(100, 160), 50);
    }

    int seconds_to_brightnes = map(now.minute() * 60 + now.second(), 0, 3600, 255, 0);
    Serial.println(seconds_to_brightnes);
    FastLED.setBrightness(seconds_to_brightnes);
    FastLED.show();
    delay(500);
}

void dayModeEffect() {
 #define scale 30
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = inoise8(i*scale, millis()/10+i*scale);
    leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);
  }
 
  EVERY_N_MILLIS(10) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, 48);
  }
 
  EVERY_N_SECONDS(5) {                                                      
    uint8_t baseC=random8();
    targetPalette = CRGBPalette16(CHSV(baseC+random8(32), 255, random8(128,255)),
                                  CHSV(baseC+random8(64), 255, random8(128,255)),
                                  CHSV(baseC+random8(96), 192, random8(128,255)),
                                  CHSV(baseC+random8(16), 255, random8(128,255)));
  }
  LEDS.show();             
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

void romanticMoodEffect() {
 static int ripplePos[5] = {0};
  static int rippleSize[5] = {0};
  
  // Move and expand each ripple
  for (int i = 0; i < 5; i++) {
    ripplePos[i]++;
    rippleSize[i]++;
    

    // Create the ripple with red and pink colors
    for (int j = 0; j < NUM_LEDS; j++) {
      int distance = abs(j - ripplePos[i]);
      if (distance <= rippleSize[i]) {
        // Vary hue to simulate a rainbow effect (red to pink)
        int hue = map(distance, 0, rippleSize[i], 0, 128);
        leds[j] = CHSV(hue, 255, 255);
      }
    }
    
    // Fade out the ripple
    if (rippleSize[i] >= NUM_LEDS / 2) {
      rippleSize[i] = 0;
    }
  }
  
  FastLED.show();
  delay(30);
}