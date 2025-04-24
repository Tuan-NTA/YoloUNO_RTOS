#ifndef HUMIDIFIER_H
#define HUMIDIFIER_H

#define D7 10
#define D8 17

#define init 0
#define idle 1
#define active_red 2
#define active_yellow 3
#define active_green 4

extern int humidifier_status;

void task_humidifier();

#endif 