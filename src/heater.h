#ifndef HEATER_H
#define HEATER_H

// Pin definitions
#define D3 6
#define D4 7

// Heater status states
#define INIT 0
#define SAFE 1
#define RISKY 2
#define DANGER 3

// Declare the heater status variable
extern int heater_status;

// Function prototype for heater task
void task_heater();

#endif // HEATER_H