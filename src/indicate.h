#ifndef _INDICATE_H_
#define _INDICATE_H_

void init_indicate();

void indicate();
void indicate_ext();


void set_indicate_digits(int d1, int d2, int d3, int d4);

void set_blink(bool* b_enable);

#endif