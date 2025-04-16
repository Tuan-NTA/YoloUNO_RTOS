#define D3 6
#define D4 7
#define D5 8
#define D6 9
#define D7 10
#define D8 17
#define D9 18
#define D10 21



#include "scheduler.h"

void TIMER_ISR(void *pvParameters) {
  
  while(1) {
    SCH_Update();
    vTaskDelay(10);
  }
}


void turnOnRed(int PIN){
  digitalWrite(PIN,HIGH);
  digitalWrite(PIN+1,HIGH);
}

void turnOnYellow(int PIN){
  digitalWrite(PIN,LOW);
  digitalWrite(PIN+1,HIGH);
}

void turnOnGreen(int PIN){
  digitalWrite(PIN,HIGH);
  digitalWrite(PIN+1,LOW);
}

int second = 0;

void displayOnClock() {
  clearClock();

  int second_led = (second / 5)%12;
  
  switch (second_led){
    case 0:
      turnOnYellow(D3);
      break;
    case 1:
      turnOnGreen(D3);
      break;
    case 2:
      turnOnGreen(D5);
      break;
    case 3:
      turnOnYellow(D5);
      break;
    case 4:
      turnOnRed(D5);
      break;
    case 5:
      digitalWrite(D7,HIGH);
      digitalWrite(D8,LOW);
      break;
    case 6:
      digitalWrite(D7,LOW);
      digitalWrite(D8,HIGH);
      break;
    case 7:
      digitalWrite(D7,HIGH);
      digitalWrite(D8,HIGH);
      break;
    case 8:
      digitalWrite(D9,HIGH);
      digitalWrite(D10,LOW);
      break;
    case 9:
      digitalWrite(D9,LOW);
      digitalWrite(D10,HIGH);
      break;
    case 10:
      digitalWrite(D9,HIGH);
      digitalWrite(D10,HIGH);
      break;
    case 11:
      turnOnRed(D3);
      break;
  }

  second += 1;
  if(second==60){
    second = 0;
  }
}

void clearClock() {
  digitalWrite(D3,LOW);
  digitalWrite(D4,LOW);

  digitalWrite(D5,LOW);
  digitalWrite(D6,LOW);

  digitalWrite(D7,LOW);
  digitalWrite(D8,LOW);

  digitalWrite(D9,LOW);
  digitalWrite(D10,LOW);
}

void setup()
{
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D9, OUTPUT);
  pinMode(D10, OUTPUT);

  xTaskCreate(TIMER_ISR, "TIMER_ISR", 2048, NULL, 2, NULL);

  SCH_Init();
  displayOnClock();
  SCH_Add_Task(displayOnClock, 0, 100);
  
} 

void loop(){
  SCH_Dispatch_Tasks();
}