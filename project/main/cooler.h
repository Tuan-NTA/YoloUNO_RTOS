#ifndef COOLER_H
#define COOLER_H


#define D5 8
#define D6 9

#define init 0
#define active 1
#define temp_check 2

extern int cooler_status;


void task_cooler();

#endif