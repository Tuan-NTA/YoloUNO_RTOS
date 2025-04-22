#ifndef HUMIDIFIER_H
#define HUMIDIFIER_H


#define INIT 0
#define ACTIVATED 1
#define IDLE 2

extern int humidifier_status;

void task_humidifier();

#endif 