#ifndef COOLER_H
#define COOLER_H


#define D5
#define D6

#define INIT 0
#define ACTIVATE 1
#define TEMP_CHECK 2

extern int cooler_status;


void task_cooler();

#endif