#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <dart_config.h>

#define FPS_TO_MPS 0.3047999902464f
#define CALC_MJOULE(fps, mass) (500*fps*fps*FPS_TO_MPS*FPS_TO_MPS*mass) // 0.5*mv^2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);


void drawTopDisplay();
void drawStats();
void updateProperties();

unsigned long spt1, spt2, dpt1, dpt2;
double fps;
double mass;
double length;
DART_TYPE type;

#endif