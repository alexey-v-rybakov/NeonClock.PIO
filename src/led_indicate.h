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
int led_set_brightness(int dir);

// 
// Установить режим работы
int led_switch_mode(int dir);
//
// Установить цвет
int led_switch_color(int dir);
//
// Установить скорость
int led_set_speed(int speed);

#endif