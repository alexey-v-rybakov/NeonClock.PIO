#include "led_indicate.h"
#include <FastLED.h>

#define NUM_LEDS 7
#define DATA_PIN 10

// Предположительно WS2812B-совместимая лента (если не работает — см. ниже)
#define LED_TYPE WS2812B
#define COLOR_ORDER RGB

CRGB leds[NUM_LEDS];

const CRGB colorList[] = {
    CRGB::Red,
    CRGB::Green,
    CRGB::Blue,
    CRGB::Yellow,
    CRGB::Orange,
    CRGB::Purple,
    CRGB::Aqua,
    CRGB::Teal,
    CRGB::White,
    CRGB::Black,      // off
    CRGB::Gray,
    CRGB::Pink,
    CRGB::Fuchsia,
    CRGB::Lavender,
    CRGB::Maroon,
    CRGB::Navy,
    CRGB::Olive,
    CRGB::Cyan,
    CRGB::Magenta,
    CRGB::Gold,
    CRGB::Crimson
  };




 
typedef void (*LedprocessFunctionPtr)();
// Массив указателей на функции
LedprocessFunctionPtr LedProcessFunctions[] = 
{
    single_color,      
    rainbow,
    color_chase,
    fire_effect,
    single_dot_fade,
    rainbowwave
};

const int numLedFunctions = sizeof(LedProcessFunctions) / sizeof(LedProcessFunctions[0]); // Количество функций в массиве


// текущий цвет
unsigned int m_color = 0;
// текущий режим
unsigned int m_led_mode = 0;
// текущая яркость
uint8_t      m_led_brightness = 50;

//
// Функции отображения
void single_color()
{


}

void rainbow() 
{
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * 10 + millis() / 10) % 255, 255, 255);
  }
  FastLED.show();
  delay(30);
}

void colorChase(CRGB color) {
  static int pos = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[pos] = color;
  FastLED.show();
  delay(50);
  pos = (pos + 1) % NUM_LEDS;
}

void fireEffect() {
  static byte heat[NUM_LEDS];
  const byte COOLING = 55;
  const byte SPARKING = 120;

  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j] = HeatColor(heat[j]);
  }

  FastLED.show();
  delay(30);
}

void singleDotFade(CRGB color) {
  static int pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 40);
  leds[pos] = color;
  FastLED.show();
  delay(20);
  pos = (pos + 1) % NUM_LEDS;
}

void rainbowWave() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + i * 5, 255, 255);
  }
  FastLED.show();
  hue++;
  delay(20);
}

//
// иницициализировать ленту
void init_led()
 {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(m_led_brightness);
   for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = colorList[m_color];
   }
  FastLED.show();
 }

//
// отобразить текущий режим
void led_process()
{


}

void set_brightness(unsigned int br)
 {
    FastLED.setBrightness(br);
    FastLED.show();
 }

/*              if (m_color < 0)  m_color = (sizeof(colorList) / sizeof(CRGB)) - 1;
              if (m_color > (int)((sizeof(colorList) / sizeof(CRGB)) - 1)) m_color = 0;
               fill_solid(leds, NUM_LEDS, colorList[m_color]);
               FastLED.show(); */







