#include <Arduino.h>
#include <scheduler.h>
#include <heater.h>
#include <cooler.h>
#include <humidifier.h>
#include <sensor.h>

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
  SCH_Add_Task(printResult, 0, 500);
}

void loop() {
  SCH_Dispatch_Task();
}