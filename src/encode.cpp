#include <Arduino.h>
#include "encoder.h"

int encoder_state = 0;
int old_encoder_state = 0;



int get_encoder_state(int &state)
 {

  //noInterrupts();
  if (encoder_state != old_encoder_state)
  {
    int d = encoder_state - old_encoder_state;
    old_encoder_state = encoder_state;
    state = encoder_state;
    return d;
    }
  //interrupts();
  return 0;
 }
    
void init_encoder()
 {
// для энкодера
  pinMode(3,INPUT_PULLUP); // ENC-A
  pinMode(5,INPUT_PULLUP); // ENC-B
  pinMode(2,INPUT_PULLUP); // ENC-B
 // PCICR =  0b00000100; // PCICR |= (1<<PCIE1); Включить прерывание PCINT2
 // PCMSK2 = 0b00101100; // Разрешить прерывание для  A1, A2, A3

 }

 uint8_t last_comb = -1;
int count = 0;
int p[2];


// время через которое определяем что кнопка нажата
#define BT_PUSHED_TIMEOUT   800
// время дребезга кнопки
#define NT_JITTER_TIMEOUT   20
// счетчик нажатой кнопки
unsigned long bt_down_mills = 0;
// когда последний раз сообщали о том, что кнопка нажата
unsigned long bt_last_pushed = 0;
// счетчик нажатий
unsigned char bt_pushed_count = 0;


int bt_state = BT_STATE::BT_RELEASED;
int bt_state_upd = false;

int bt_old_pin_state = 1;


bool get_button_state(BT_STATE &_bt_state, unsigned char &_bt_pushed_count)
 {
  if (bt_state_upd)
   {
      _bt_state = (BT_STATE )bt_state;
      _bt_pushed_count = bt_pushed_count;
      bt_state_upd = false;
      return true;
   }
  return false;
 }


//
// Прерывания от Encoder
//
//ISR (PCINT2_vect) //Обработчик прерывания от пинов A1, A2, A3
void process_encoder()
{
  uint8_t comb = bitRead( PIND, 5)<<1 | bitRead(PIND, 3); //считываем состояние пинов энкодера и кнопки
    
  // обработка состояния кнопки  
  uint8_t bt_pin_state = bitRead(PIND, 2);
  // состояние пина изменилось  
 if (bt_pin_state != bt_old_pin_state)
   {
        // если нажали кнопку
        if (!bt_pin_state)
        {
            bt_down_mills = bt_last_pushed = millis();
            bt_pushed_count = 0;
        }
        // если отжали
        else
        {
          // убераем дребезг
         if ((millis() - bt_down_mills) < NT_JITTER_TIMEOUT)
            goto next;
          if ((millis() - bt_down_mills) < BT_PUSHED_TIMEOUT)
            {
              //Serial.println("CLICKED");
              bt_state = BT_STATE::BT_CLICKED;
              bt_state_upd = true;
            }
        }

   }
  else 
   {
        // кнопка все еще нажата
        if (!bt_pin_state)
        {
            if ((millis() - bt_last_pushed) > BT_PUSHED_TIMEOUT)
             {
                //Serial.println("PUSHED");
                bt_last_pushed = millis();
                bt_state = BT_STATE::BT_PUSHED;
                bt_pushed_count++;
                bt_state_upd = true;

             }
        }
        else
        {

        }

   } 
next:
  bt_old_pin_state = bt_pin_state; 


  if ((comb == last_comb)||(comb == 2))
    return;
  last_comb = comb;
  if (comb == 3)  
    {
      count = 0;
      return;
    }
   p[count] = comb;
   count++;
   if (count >= 2)
    {
      if (p[0] == 0)
        {
          encoder_state++;
        }
        else
        {
          encoder_state--;
        }
      count = 0;
       
    } 
  
}
