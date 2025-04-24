#ifndef HEATER_H
#define HEATER_H

#define D3 6
#define D4 7

#define init 0
#define safe 1
#define risky 2
#define danger 3


extern int heater_status;


void task_heater();

#endif 