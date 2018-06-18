#include "irda.h"
#include <stdio.h>

// #define DEBUG

#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial Serial(0, 2);
#else
#include <TinyWireM.h>
#endif

//
// If you want to use timer0, delay and micros() are affected
// C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\wiring.c
// Reference: https://www.re-innovation.co.uk/docs/fast-pwm-on-attiny85/
//
// #define F_CPU 8000000
// #include <util/delay.h>

//
// Reference: https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
//
#include <avr/interrupt.h>

unsigned char motor = 0;
void on_commit(unsigned long data) {
  unsigned char masked = data & 0xFF;

  if (masked == 0xFA) {
    motor = (motor & 0x0C) | 0x02;
  } else if (masked == 0xFF) {
    motor = (motor & 0x0C) | 0x01;
  } else if (masked == 0xFB) {
    motor = (motor & 0x0C) | 0x00;
  } else if (masked == 0x7B) {
    motor = (motor & 0x03) | 0x08;
  } else if (masked == 0x7E) {
    motor = (motor & 0x03) | 0x04;
  } else if (masked == 0xFE) {
    motor = (motor & 0x03) | 0x00;
  }
#ifdef DEBUG
  char buf[100];
  //sprintf(buf, "OnCommit: %x", data);
  sprintf(buf, "OnCommit: %x", motor);
  Serial.println(buf);
#else
  TinyWireM.beginTransmission(0x20);
  TinyWireM.write(0x12);
  TinyWireM.write(motor);
  TinyWireM.endTransmission();
#endif
}

void on_repeat(unsigned long data) {
}

void on_error(char* msg) {
#ifdef DEBUG
  Serial.println(msg);
#endif
}

IRDA irda(micros(), on_commit, on_repeat, on_error);

void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  // PWM
  pinMode(1, OUTPUT);
  TCCR1 = 0x63;
  OCR1A = 00;

  // Interrupt
  cli();
  GIMSK = 0x20;
  PCMSK = 0x10;
  sei();

#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("setup");
#else
  // GPIO expander
  TinyWireM.begin();
  TinyWireM.beginTransmission(0x20);
  TinyWireM.write(0x00);
  TinyWireM.write(0x00);
  TinyWireM.endTransmission();
  TinyWireM.beginTransmission(0x20);
  TinyWireM.write(0x01);
  TinyWireM.write(0x00);
  TinyWireM.endTransmission();
  TinyWireM.beginTransmission(0x20);
  TinyWireM.write(0x13);
  TinyWireM.write(0x01);
  TinyWireM.endTransmission();
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
#ifdef DEBUG
  static int cnt = 0;
  static int prev = 0;
  int value = digitalRead(4);
  if (!value && prev != value) {
    irda.detect(micros());
  }
  prev = value;

  if (cnt++ % 100000 == 0) {
    calc_pwm(analogRead(3));
  }
  delayMicroseconds(1);
#else
  OCR1A = calc_pwm(analogRead(3));
  delay(1000);
#endif
}

#ifndef DEBUG
ISR(PCINT0_vect) {
  int value = digitalRead(4);
  if (value == 0) {
    irda.detect(micros());
  }
}
#endif

unsigned char calc_pwm(unsigned int half_voltage) {
   unsigned char pwm = (unsigned char) (3.0  / (half_voltage / (float)1024 * 2 * 5) * 256);
#ifdef DEBUG
   char buf[100];
   sprintf(buf, "half_voltage=%d, pwm=%d", half_voltage, pwm);
   Serial.println(buf);
#endif
  return pwm;
}
