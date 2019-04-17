#ifndef __CONTROL_H
#define __CONTROL_H

#include "stdint.h"

void go_anywhere(float angel, int speed);
void set_direction(uint16_t angel,uint16_t speed);
void move_time(uint16_t angel, uint16_t speed, uint16_t time);
void move_distance(uint16_t angel, uint16_t speed, uint16_t distance);
void go_left(uint16_t angel, uint32_t sudu);
void go_right(uint16_t angel, uint32_t speed);
uint16_t test_distance(void);
int get_distance(int num);
int gengsui(void);
int gengsui1(void);
void move_ahead(uint16_t direction ,uint8_t changedirection,uint16_t speed,uint16_t distance);
void move_ahead1(uint16_t direction ,uint8_t changedirection,uint16_t speed,uint16_t distance);

#endif
