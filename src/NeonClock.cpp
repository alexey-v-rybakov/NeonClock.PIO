#include <Wire.h>                           // Подключаем библиотеку Wire                           
#include <TimeLib.h>                        // Подключаем библиотеку TimeLib
#include <DS1307RTC.h> 
#include "indicate.h"
#include "encoder.h"
#include "led_indicate.h"
#include <Arduino.h>




volatile uint16_t isrCounter = 0;
volatile uint16_t isrDivider = 200; // по умолчанию 200 Гц => 1 раз в секунду

// Вызывается пользователем: задаёт частоту от 1 до 200 Гц
void setLogicalFrequency(uint16_t freq) {
  if (freq < 1) freq = 1;
  if (freq > 200) freq = 200;
  noInterrupts();
  isrDivider = 200 / freq; // пример: 200/2 = каждые 100 тиков => 2 Гц
  isrCounter = 0;          // сброс счётчика
  interrupts();
}

void initTimer2_200Hz() {
  noInterrupts();

  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // Настраиваем таймер на частоту 200 Гц
  // Частота CPU: 16_000_000
  // Предделитель: 128 → 16_000_000 / 128 = 125000
  // Нужно 200 Гц → 125000 / 200 = 625 → OCR2A = 624

  OCR2A = (uint8_t)624;
  TCCR2A |= (1 << WGM21); // CTC режим
  TCCR2B |= (1 << CS22) | (1 << CS20); // делитель 128
  TIMSK2 |= (1 << OCIE2A); // Разрешаем прерывания
  interrupts();
}

typedef enum INDICATION_MODE
  {
    HOUR_MIN   = 0,
    DAY_MONTH     ,
    YEAR          ,
    MODE          ,
    BRIGHTNESS    ,
    COLOR         ,
    SPEED         ,
    NUM_INDC_MODE ,
    SET_HOUR      ,
    SET_MIN       ,
    SET_DAY       ,
    SET_MONTH     ,
    SET_YEAR      ,
    NUM_SET_MODE
  } INDICATION_MODE;

typedef enum WORK_MODE
 {
    TIME = 0,
    CONFIGURE,
    NUM_WORK_MODE
 } WORK_MODE;

typedef enum CONFIG_ORDER
{
  
} CONFIG_ORDER;




// пин светодиода 
int dot_pin = 11;
// переменная для сохранения времения
tmElements_t tm;
// период с которым пересчитывается время с RTC
#define TIME_CHANGE  350
unsigned long last_time_change = 0;
// Что показываем на экране
INDICATION_MODE m_indic_mode = INDICATION_MODE::HOUR_MIN;
// Режим работы
WORK_MODE m_work_mode = WORK_MODE::TIME;
// Мигание
bool blink_mode[] = {false, false, false, false};
//
bool led_blink = false;
bool led_state = false;
 
void update_indication_mode()
{
  for (int i = 0; i < 4; i++)
    blink_mode[i] = false;
  led_blink = false;

  switch (m_indic_mode)
    {
      case INDICATION_MODE::SET_HOUR:
      case INDICATION_MODE::SET_DAY:
        blink_mode[0] = true;
        blink_mode[1] = true;
      break;
      case INDICATION_MODE::SET_MIN:
      case INDICATION_MODE::SET_MONTH:
        blink_mode[2] = true;
        blink_mode[3] = true;
      break;
      case INDICATION_MODE::SET_YEAR:
        blink_mode[0] = true;
        blink_mode[1] = true;
        blink_mode[2] = true;
        blink_mode[3] = true;
      break;
      default:
        led_blink = true;
      break;
    }

  if (led_blink == false)
    digitalWrite(dot_pin,HIGH);  

  set_blink(blink_mode);  
}

ISR(TIMER2_COMPA_vect) {

  isrCounter++;
  if (isrCounter >= isrDivider) 
  {
    isrCounter = 0;
    indicate_ext();
  
  }
}

void setup() 
{
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);



// put your setup code here, to run once:
  // задаем частоту ШИМ на 9 выводе 30кГц
  TCCR1B=(TCCR1B&0b11111000)|0x01;
  
  analogWrite(9,130);

  init_encoder();   
  init_indicate();

  pinMode(dot_pin, OUTPUT);
  digitalWrite(dot_pin,LOW);
      
  tm.Day = 16;
  tm.Month = 05;
  tm.Year = CalendarYrToTm(2025);
  
  tm.Hour = 22;
  tm.Minute = 17;
  tm.Second = 00;
  Serial.println("Go");
  unsigned char r = RTC.isRunning();
  if (r)
   {
      Serial.println("running yes");
   }
   else
    {
        Serial.println("running no");
    }


  /*if (RTC.write(tm)) 
   {
    Serial.println("Yes");
   }
   else
   Serial.println("No");*/
  update_indication_mode();

  initTimer2_200Hz(); // Фиксированная частота таймера
  setLogicalFrequency(30); // Пользовательская логика — 2 Гц (мигание 2 раза в секунду)

  init_led();


}



 void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

 unsigned long last_show_time = 0;
 


#define CMD_BUFFER_SIZE 32
char cmdBuffer[CMD_BUFFER_SIZE];
byte cmdPos = 0;


void processSerialCommand(const char* cmd) {
  if (strcmp(cmd, "M+") == 0) {
    led_switch_mode(1);
  } else if (strcmp(cmd, "M-") == 0) {
    led_switch_mode(-1);
  } else if (strcmp(cmd, "C+") == 0) {
    led_switch_color(1);
  } else if (strcmp(cmd, "C-") == 0) {
    led_switch_color(-1);
  } else if (cmd[0] == 'B') {
    int value = atoi(cmd + 1);
    led_set_brightness(value);
  } else if (cmd[0] == 'S') {
    int value = atoi(cmd + 1);
    led_set_speed(value);
  } else {
    Serial.println("Unknown command");
  }
}

void loop() 
  {


while (Serial.available() > 0) {
    char ch = Serial.read();

    if (ch == '\n' || ch == '\r') {
      if (cmdPos > 0) {
        cmdBuffer[cmdPos] = '\0'; // завершение строки
        processSerialCommand(cmdBuffer);
         Serial.write(cmdBuffer);
        cmdPos = 0;
      }
    } else {
      if (cmdPos < CMD_BUFFER_SIZE - 1) {
        cmdBuffer[cmdPos++] = ch;
      }
    }
  }
    indicate();
    process_encoder();
    led_process();

    int enc_state;
    int enc_delta;
  
    BT_STATE      bt_state;
    unsigned char bt_pushed_count;
    //
    // нажатие кнопки
    if (get_button_state(bt_state, bt_pushed_count))
    {
      Serial.print("BUTTON STATE : ");
      Serial.print(bt_state, DEC);
      Serial.print(" PUSHED COUNT : ");
      Serial.println(bt_pushed_count, DEC);
      //
      if (m_work_mode == WORK_MODE::TIME)
      { 
       if (bt_state == BT_STATE::BT_CLICKED)
        {
            m_indic_mode = (INDICATION_MODE)(m_indic_mode + 1);
            if (m_indic_mode >= INDICATION_MODE::NUM_INDC_MODE)
              m_indic_mode = INDICATION_MODE::HOUR_MIN;
            update_indication_mode();
            last_show_time = 0;
        }
        if ((bt_state == BT_STATE::BT_PUSHED)&&(bt_pushed_count >= 3))
        {
          m_work_mode   = WORK_MODE::CONFIGURE;
          m_indic_mode  = INDICATION_MODE::SET_HOUR;
          update_indication_mode();
        }

        //

      } 
      else
      if (m_work_mode == WORK_MODE::CONFIGURE)
      {
          Serial.println("WORK_MODE::CONFIGURE");
          if ((bt_state == BT_STATE::BT_PUSHED)&&(bt_pushed_count >= 3))
          {
            // возвращаемся обратно в режим отображения времени
            m_work_mode = WORK_MODE::TIME;
            m_indic_mode = INDICATION_MODE::HOUR_MIN;
            update_indication_mode();
            //
            // TODO: записать время в RTC  
          }
          if (bt_state == BT_STATE::BT_CLICKED)
          {
             m_indic_mode = (INDICATION_MODE)(m_indic_mode + 1);
            if (m_indic_mode >= INDICATION_MODE::NUM_SET_MODE)
              m_indic_mode = INDICATION_MODE::SET_HOUR;
            tm.Second = 0;
            RTC.write(tm);
            update_indication_mode();           
          }

      }
    }
    //
    // поворот ручки энкодера
    if ((enc_delta = get_encoder_state(enc_state)) != 0)
    {
      Serial.print("ENCODER STATE : ");
      Serial.print(enc_state, DEC);
      Serial.print(" delta : ");
      Serial.println(enc_delta, DEC);
      //
      if (m_work_mode == WORK_MODE::CONFIGURE)
        {
          Serial.println("WORK_MODE::CONFIGURE");

          if (enc_delta)
          {
            if (m_indic_mode == SET_HOUR)
            {
              tm.Hour = tm.Hour + enc_delta;
              Serial.println(tm.Hour, DEC);
              if (((signed char)tm.Hour) < 0)  tm.Hour = 23;
              if (tm.Hour > 23) tm.Hour = 0;
              Serial.println(tm.Hour, DEC);
            } 
            else
            if (m_indic_mode == SET_MIN)
            {
              tm.Minute = tm.Minute + enc_delta;
              Serial.println(tm.Minute, DEC);
              if (((signed char)tm.Minute) < 0)  tm.Hour = 59;
              if (tm.Minute > 59) tm.Minute = 0;
              Serial.println(tm.Minute, DEC);
            }
            else
            if (m_indic_mode == SET_DAY)
            {
              tm.Day = tm.Day + enc_delta;
              Serial.println(tm.Day, DEC);
              if (((signed char)tm.Day) < 1)  tm.Day = 31;
              if (tm.Day > 31) tm.Day = 1;
              Serial.println(tm.Day, DEC);
            }
            else
            if (m_indic_mode == SET_MONTH)
            {
              tm.Month = tm.Month + enc_delta;
              Serial.println(tm.Month, DEC);
              if (((signed char)tm.Month) < 1)  tm.Month = 12;
              if (tm.Month > 12) tm.Month = 1;
              Serial.println(tm.Month, DEC);
            }
            else
            if (m_indic_mode == SET_YEAR)
            {
              tm.Year = tm.Year + enc_delta;
              Serial.println(tm.Year, DEC);
            }

            last_show_time = 0;
          }  
      }

      if (m_work_mode == WORK_MODE::TIME)
        {
          if (enc_delta)
          {
            if (m_indic_mode == INDICATION_MODE::MODE)
            {
              led_switch_mode(enc_delta);
            }
            else
            if (m_indic_mode == INDICATION_MODE::BRIGHTNESS)
            {
              led_set_brightness(enc_delta);
            }
            else
            if (m_indic_mode == INDICATION_MODE::COLOR)
            {
              led_switch_color(enc_delta);
            } 
            else
            if (m_indic_mode == INDICATION_MODE::SPEED)
            {
              led_set_speed(enc_delta);
            }
            last_show_time = 0;
          }  
      }
    }
        
  
      if ((millis() - last_show_time) > 1000)
      {
      last_show_time = millis();

      if ((led_blink)&&(false))
      {
          if (led_state)
          {
            led_state = false;   
            digitalWrite(dot_pin,LOW);
          }
          else
          {
            led_state = true;   
            digitalWrite(dot_pin,HIGH);
          }

      }

      if (m_work_mode != WORK_MODE::CONFIGURE)
      if (RTC.read(tm) ) {

      Serial.print("Ok, Time = ");
      print2digits(tm.Hour);
      Serial.write(':');
      print2digits(tm.Minute);
      Serial.write(':');
      print2digits(tm.Second);
      Serial.print(", Date (D/M/Y) = ");
      Serial.print(tm.Day);
      Serial.write('/');
      Serial.print(tm.Month);
      Serial.write('/');
      Serial.print(tmYearToCalendar(tm.Year));
      Serial.println();
      }
      switch (m_indic_mode)
      {
        case INDICATION_MODE::HOUR_MIN:
        case INDICATION_MODE::SET_HOUR:
        case INDICATION_MODE::SET_MIN:
          set_indicate_digits(tm.Hour/10, tm.Hour%10, tm.Minute/10, tm.Minute%10);
        break;
        case INDICATION_MODE::DAY_MONTH:
        case INDICATION_MODE::SET_DAY:
        case INDICATION_MODE::SET_MONTH:
          set_indicate_digits(tm.Day/10, tm.Day%10, tm.Month/10, tm.Month%10);
        break;
        case INDICATION_MODE::YEAR:
        case INDICATION_MODE::SET_YEAR:
          set_indicate_digits((tmYearToCalendar(tm.Year)/1000)%10, (tmYearToCalendar(tm.Year)/100)%10, (tmYearToCalendar(tm.Year)/10)%10, tmYearToCalendar(tm.Year)%10);
        break;
        case INDICATION_MODE::MODE:
          set_indicate_digits(0, 1, (led_switch_mode(0)/10)%10, led_switch_mode(0)%10);
        break;
        case INDICATION_MODE::BRIGHTNESS:
          set_indicate_digits(0, 2, (led_set_brightness(0)/10)%10, led_set_brightness(0)%10);
        break;
        case INDICATION_MODE::COLOR:
          set_indicate_digits(0, 3, (led_switch_color(0)/10)%10, led_switch_color(0)%10);
        break;
        case INDICATION_MODE::SPEED:
          set_indicate_digits(0, 4, (led_set_speed(0)/10)%10, led_set_speed(0)%10);
        break;

      default:
        break;
      }
      



    } 
    /*else {
      if (RTC.chipPresent()) {
        Serial.println("The DS1307 is stopped.  Please run the SetTime");
        Serial.println("example to initialize the time and begin running.");
        Serial.println();
      } else {
        Serial.println("DS1307 read error!  Please check the circuitry.");
        Serial.println();
      }
    
    }*/
      
  }


