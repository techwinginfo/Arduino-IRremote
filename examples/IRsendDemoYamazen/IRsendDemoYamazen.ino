/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */


#include <IRremote.h>

IRsend irsend;

void setup()
{
}

void loop() {
  int khz = 38; // 38kHz carrier frequency for the NEC protocol
  irsend.sendYAMAZEN(0x60C5F708, 32);
  delay(10000); //10 second delay between each signal burst
  irsend.sendYAMAZEN(0x60C5F707, 32);
  delay(10000); //10 second delay between each signal burst
}
