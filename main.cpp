#include "mbed.h"
#include "uLCD_4DGL.h"
AnalogOut Aout(PA_4);
AnalogOut Sout(PA_5);
InterruptIn left_btn(A0);
InterruptIn mid_btn(A1);
InterruptIn right_btn(A2);
AnalogIn Ain(A3);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
float freq = 200;
bool mode = 0;
Thread t;
Thread q;
Thread r;
float period = 1;
int sample = 100;
float ADCdata[100];
Timer T;
float sel = 1/8;

void OutputA(){
  float p = period - 33;
  float i;
  int j = 0;

  while (j < sample)
  {
    for(i=0.0f; i < 1.0f && j < sample; i += 0.25f) {
      // T.start();
      ADCdata[j] = Ain;
      Sout = i;
      wait_us(p);

      j=j+1;
      // T.stop();
      // auto us=T.elapsed_time().count();
      // printf("%d\n", j);
      // T.reset();
    }
    for(i=0.834f; i  > 0.004f && j < sample; i -= 0.166f) {
      // T.start();
      ADCdata[j] = Ain;
      Sout = i;
      wait_us(p);
      j=j+1;
      // T.stop();
      // auto us=T.elapsed_time().count();
      // printf("%d\n", j);
      // T.reset();
    }
  }
  printf("%f\r\n", freq*10);
  for (int k = 20; k < 60; k++){
      printf("%f\r\n", ADCdata[k]);
      wait_us(100);
  } 
}
void refresh_uLCD(){
  uLCD.cls(); 
  uLCD.locate(1,5);
  if(mode == 1) {
    uLCD.color(RED);
  } else {
    uLCD.color(GREEN);
  }
  uLCD.printf("%3f hz",sel);
}
void left_btn_pushed() {
  mode = 0;
  if(sel < 1) {
    sel = sel * 2;
  }
  refresh_uLCD();
}
void left_btn_isr() {
  queue.call(left_btn_pushed);
}
void right_btn_pushed() {
  mode = 0;
  if(sel > 1/8) {
    sel = sel / 2;
  }
  refresh_uLCD();
}
void right_btn_isr() {
  queue.call(right_btn_pushed);
}
void mid_btn_pushed() {
  mode = 1;
  refresh_uLCD();
  period = 1 / freq *100000;
  r.start(&OutputA);
  // q.start(&ADC);
}
void mid_btn_isr() {
  queue.call(mid_btn_pushed);
}
int main(){
  refresh_uLCD();
  Aout = 1;
  Sout = 0;
  t.start(callback(&queue, &EventQueue::dispatch_forever));
  left_btn.rise(left_btn_isr);
  right_btn.rise(right_btn_isr);
  mid_btn.rise(mid_btn_isr);
}