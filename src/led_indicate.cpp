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


const int numColor = sizeof(colorList) / sizeof(colorList[0]); // Количество функций в массиве

unsigned long m_last_indicate = 0;

// текущий цвет
int m_color = 0;
// текущий режим
int m_led_mode = 0;
// текущая яркость
int m_led_brightness = 50;
// процент скорости
int m_speed = 50;

//
// Функции отображения
bool single_color()
{
    // не надо каждый раз заполнять если уже один раз заполнили
    if (m_last_indicate != 0)
        return true;

    fill_solid(leds, NUM_LEDS, colorList[m_color]);
    FastLED.show();
    return true;
}

bool rainbow() 
{
  int tout = (30 * (50 + m_speed))/100;  
  if ((millis() - m_last_indicate) < tout)  
    return false;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * 10 + millis() / 10) % 255, 255, 255);
  }
  FastLED.show();
  return true;
}

bool color_chase() 
{
  int tout = (50 * (50 + m_speed))/100;  
  if ((millis() - m_last_indicate) < tout)  
    return false;

  static int pos = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[pos] = colorList[m_color];
  FastLED.show();
  pos = (pos + 1) % NUM_LEDS;

  return true;
}

bool fire_effect() 
{
    int tout = (30 * (50 + m_speed))/100;
    if ((millis() - m_last_indicate) < tout)  
        return false;

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
  return true;
}

bool single_dot_fade() 
{
    int tout = (20 * (50 + m_speed))/100;
      if ((millis() - m_last_indicate) < tout)  
    return false;

  static int pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 40);
  leds[pos] = colorList[m_color];
  FastLED.show();
  
  pos = (pos + 1) % NUM_LEDS;

  return true;
}

bool rainbow_wave() 
{
    int tout = (20 * (50 + m_speed))/100;
    if ((millis() - m_last_indicate) < tout)  
    return false;
  
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + i * 5, 255, 255);
  }
  FastLED.show();
  hue++;
  return true;
}

//
// иницициализировать ленту
void init_led()
 {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness((uint8_t)m_led_brightness);
  /*for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;//colorList[m_color];
   }
  FastLED.show();*/
  single_color();
 }

typedef bool (*LedprocessFunctionPtr)();
// Массив указателей на функции
LedprocessFunctionPtr LedProcessFunctions[] = 
{
    single_color,      
    rainbow,
    color_chase,
    fire_effect,
    single_dot_fade,
    rainbow_wave
};

const int numLedFunctions = sizeof(LedProcessFunctions) / sizeof(LedProcessFunctions[0]); // Количество функций в массиве
// 
// Установить режим работы
int led_switch_color(int dir)
 {
    if (dir == 0)
        return m_color;
    m_last_indicate = 0;
    m_color += dir;
    if (m_color >= numColor) m_color = 0;
    if (m_color < 0) m_color = (numColor - 1); 
    
    m_last_indicate = 0;

    return m_color;
 }
//
// Установить цвет
int led_switch_mode(int dir)
 {
    if (dir == 0)
        return m_led_mode;
    m_speed = 50;
    m_last_indicate = 0;
    m_led_mode += dir;
    if (m_led_mode >= numLedFunctions) m_led_mode = 0;
    if (m_led_mode < 0) m_led_mode = (numLedFunctions - 1); 

    return m_led_mode;
 }

int led_set_speed(int dir)
{
     if (dir == 0)
        return m_speed;

    m_speed += dir;
    if (m_speed > 100) m_speed = 0;
    if (m_speed < 0)   m_speed = 100; 

    return m_speed;

}

int led_set_brightness(int dir)
 {
    if (dir == 0)
        return m_led_brightness;

    m_led_brightness += dir;
    if (m_led_brightness > 100) m_led_brightness = 0;
    if (m_led_brightness < 0)   m_led_brightness = 100; 

    FastLED.setBrightness((uint8_t)m_led_brightness);
    FastLED.show();

    return m_led_brightness;
 }





 //
// отобразить текущий режим
void led_process()
{
  if (LedProcessFunctions[m_led_mode]())
    m_last_indicate = millis();

}



