#include <Arduino.h>

// выводы для дешифратора
int out1 = A3;
int out2 = A1;
int out4 = A0;
int out8 = A2;
// выводы для транзисторных ключей
int key1= 4;
int key2 = 6;
int key3 = 7;
int key4 = 8;

// Период мигания
#define BLINK_PERIOD  300

// период с которым будут обновляться индикаторы 
#define DIGIT_CHANGE 3
unsigned long last_indc_time;

int m_d1 = 0;
int m_d2 = 0;
int m_d3 = 0;
int m_d4 = 0;

unsigned long blink_millis[] = {0,0,0,0};
 
void set_blink(bool* b_enable)
{
  for (int i = 0; i < 4; i++)
    if (b_enable[i])
      blink_millis[i] = millis();
    else
      blink_millis[i] = 0;
}     



void init_indicate()
 {


    //задаем режим работы выходов микроконтроллера
  pinMode(out1,OUTPUT);
  pinMode(out2,OUTPUT);
  pinMode(out4,OUTPUT);
  pinMode(out8,OUTPUT);
 
  pinMode(key1,OUTPUT);
  pinMode(key2,OUTPUT);
  pinMode(key3,OUTPUT);
  pinMode(key4,OUTPUT);

  // время последней индикации
  last_indc_time = millis();
 }

 void setNumber(int num) 
{
  switch (num)
  {
    case 0:
    digitalWrite (out1,LOW);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 1:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 2:
    digitalWrite (out1,LOW);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 3:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 4:
    digitalWrite (out1,LOW);
    digitalWrite (out2,LOW);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 5:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,LOW);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 6:
    digitalWrite (out1,LOW);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 7:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 8:
    digitalWrite (out1,LOW);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,HIGH);
    break;
    case 9:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,HIGH);
    break;
  }
}


void set_indicate_digits(int d1, int d2, int d3, int d4)
 {
    m_d1 = d1;
    m_d2 = d2;
    m_d3 = d3;
    m_d4 = d4;
 }




void indicate()
 {
  return;
  unsigned long t = millis() - last_indc_time;
  t = t / DIGIT_CHANGE;
  if (t > 3)
   {
    last_indc_time = millis();
    t = 0;
   }

  bool on = true; 

  if (blink_millis[t] != 0)
  {
    if ((millis() - blink_millis[t]) < BLINK_PERIOD)
      on = false;
    if ((millis() - blink_millis[t]) >= (BLINK_PERIOD*2))
      blink_millis[t] = millis();
  }
 
  switch (t)
   {
    case 0:
        setNumber(m_d1);
        digitalWrite(key1,on);
        digitalWrite(key2,LOW);
        digitalWrite(key3,LOW);
        digitalWrite(key4,LOW);
    break;
    case 1:
        setNumber(m_d2);
        digitalWrite(key1,LOW);
        digitalWrite(key2,on);
        digitalWrite(key3,LOW);
        digitalWrite(key4,LOW);
    break;
    case 2:
        setNumber(m_d3);
        digitalWrite(key1,LOW);
        digitalWrite(key2,LOW);
        digitalWrite(key3,on);
        digitalWrite(key4,LOW);
    break;
    case 3:
        setNumber(m_d4);
        digitalWrite(key1,LOW);
        digitalWrite(key2,LOW);
        digitalWrite(key3,LOW);
        digitalWrite(key4,on); 
    break;
   }; 
 }

static int m_indc_digit = 0;

 void indicate_ext()
 {
  int t = m_indc_digit;



  bool on = true; 

  if (blink_millis[t] != 0)
  {
    if ((millis() - blink_millis[t]) < BLINK_PERIOD)
      on = false;
    if ((millis() - blink_millis[t]) >= (BLINK_PERIOD*2))
      blink_millis[t] = millis();
  }
 
  switch (t)
   {
    case 0:
        setNumber(m_d1);
        digitalWrite(key1,on);
        digitalWrite(key2,LOW);
        digitalWrite(key3,LOW);
        digitalWrite(key4,LOW);
    break;
    case 1:
        setNumber(m_d2);
        digitalWrite(key1,LOW);
        digitalWrite(key2,on);
        digitalWrite(key3,LOW);
        digitalWrite(key4,LOW);
    break;
    case 2:
        setNumber(m_d3);
        digitalWrite(key1,LOW);
        digitalWrite(key2,LOW);
        digitalWrite(key3,on);
        digitalWrite(key4,LOW);
    break;
    case 3:
        setNumber(m_d4);
        digitalWrite(key1,LOW);
        digitalWrite(key2,LOW);
        digitalWrite(key3,LOW);
        digitalWrite(key4,on); 
    break;
   }; 

   m_indc_digit++;
   if (m_indc_digit > 3)
    m_indc_digit = 0;
 }