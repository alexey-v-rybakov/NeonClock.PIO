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
unsigned int led_switch_mode(bool f_next);
//
// Установить цвет
unsigned int led_switch_coloe(bool f_next);

#endif