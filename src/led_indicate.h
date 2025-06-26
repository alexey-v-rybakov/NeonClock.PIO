#ifndef _LED_INDICATE_
#define _LED_INDICATE_


//
// Инициализировать ленту
void init_led();
//
// для loop (отображение яркости)
void led_process();
//
// Установить яркость
void led_set_brightness(unsigned int br);
// 
// Установить режим работы
int led_switch_mode(int dir);
//
// Установить цвет
int led_switch_color(int dir);

#endif