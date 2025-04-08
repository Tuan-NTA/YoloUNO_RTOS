#define D3 6
#define D4 7
#define D5 8
#define D6 9

#include "scheduler.h"

void TIMER_ISR(void *pvParameters) {
  
  while(1) {
    SCH_Update();
    vTaskDelay(10);
  }
}

int led_status1 = 0;
int led_status2 = 0;
int count_light1 = 1;
int count_light2 = 6;


void turn_red1()    { digitalWrite(D3, HIGH);digitalWrite(D4, HIGH); }
void turn_yellow1() { digitalWrite(D3, LOW);digitalWrite(D4, HIGH); }
void turn_green1()  { digitalWrite(D3, HIGH);digitalWrite(D4, LOW); }

void turn_red2()    { digitalWrite(D5, HIGH);digitalWrite(D6, HIGH); }
void turn_yellow2() { digitalWrite(D5, LOW);digitalWrite(D6, HIGH); }
void turn_green2()  { digitalWrite(D5, HIGH);digitalWrite(D6, LOW); }

void display_light1() {
    if (count_light1 <= 5) turn_red1();
    else if (count_light1 <= 8) turn_green1();
    else turn_yellow1();
    count_light1 = (count_light1 % 10) + 1;
}

void display_light2() {
    if (count_light2 <= 5) turn_red2();
    else if (count_light2 <= 8) turn_green2();
    else turn_yellow2();
    count_light2 = (count_light2 % 10) + 1;
}

void setup()
{
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);

  xTaskCreate(TIMER_ISR, "TIMER_ISR", 2048, NULL, 2, NULL);

  SCH_Init();
  SCH_Add_Task(display_light1, 0, 100);
  SCH_Add_Task(display_light2, 0, 100);
  
} 

void loop(){
  SCH_Dispatch_Tasks();
}