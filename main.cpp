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
int sample = 1000;
float ADCdata[1000];
Timer T;
float sel = 0.125;
float fs = 1000;

void OutputA(){
  // float p = period - 33;
  float i;
  float j = 0;
  float k = 80000 * (1 - sel);
  float x = 80000 * (1 - sel);
  float step = 1 / (80*sel);
  int c = 0;
  int index = 0;
  // printf("%f\n", step);
  while (index < sample)
  {
    j = 0;
    for(c = 0; c < 80 && index < sample; c++) {
      // T.start();
      Sout = j;
      wait_us(1000 - 25);
      // T.start();
      ADCdata[index] = Ain;
      index = index + 1;
      j= j + step;
      if(j > 1){
        j = 1;
      }
      // T.stop();
      // auto us=T.elapsed_time().count();
      // printf ("Timer time: %llu us\n", us);
      // T.reset();
    }
    for(c = 0; c < 80 && index < sample; c++) {
      ADCdata[index] = Ain;
      index = index + 1;
      wait_us(1000 - 25);
    }
    k = x;
    for(;k >0 && index < sample; k-=1000) {
      ADCdata[index] = Ain;
      index = index + 1;
      wait_us(1000 - 25);
    }
    for(i=1.0f;i > 0.0f && index < sample; i -= step) {
      Sout = i;
      ADCdata[index] = Ain;
      index = index + 1;
      wait_us(1000 - 25);
    }
  }
  printf("%f\r\n", fs);
  for (int k = 0; k < sample; k++){
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
  uLCD.printf("%3f",sel);
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
  if(sel > 0.125) {
    sel = sel / 2;
  } else {
    sel = 0.125;
  }
  refresh_uLCD();
}
void right_btn_isr() {
  queue.call(right_btn_pushed);
}
void mid_btn_pushed() {
  mode = 1;
  refresh_uLCD();
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