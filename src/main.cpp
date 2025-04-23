#include <Arduino.h>
#include <scheduler.h>
#include <heater.h>
#include <cooler.h>
#include <humidifier.h>
#include <sensor.h>
#include <software_timer.h>

void TIMER_ISR(void *pvParameters) {
  while(1) {
    SCH_Update();
    vTaskDelay(10);
  }
}

void setup() {
  SCH_Init();
  SCH_Add_Task(Timer_Run, 0, 1);
  SCH_Add_Task(task_cooler, 0, 1);
  SCH_Add_Task(task_heater, 0, 1);
  SCH_Add_Task(task_humidifier, 0, 1);
  SCH_Add_Task(readSensor, 0, 100);
  SCH_Add_Task(printTnH, 0, 500);
}

void loop() {
  SCH_Dispatch_Tasks();
}