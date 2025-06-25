#ifndef _ENCODER_H_
#define _ENCODER_H_

typedef enum BT_STATE
 {
  BT_RELEASED = 0,
  BT_PUSHED      ,
  BT_CLICKED
 } BT_STATE;


void init_encoder();


int get_encoder_state(int& state);
bool get_button_state(BT_STATE &_bt_state, unsigned char &_bt_pushed_count);

void process_encoder();

#endif