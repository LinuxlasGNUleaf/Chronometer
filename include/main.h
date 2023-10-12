#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <dart_config.h>

#define FPS_TO_MPS 0.3047999902464f
#define MPS_TO_FPS 3.28084f

#define CALC_MJOULE(fps, mass) (500*fps*fps*FPS_TO_MPS*FPS_TO_MPS*mass) // 0.5*mv^2

// HARDWARE CONFIG
const int buttonPin = D7;
const int sensorPin1 = D5;
const int sensorPin2 = D6;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
const double sensor_distance = 5e-2;

// SOFTWARE CONFIG
#define ALL_SENSORS_FIRED 0b10001111
#define ARMED_BIT         7
#define ARMED             0b10000000
#define IDLE              0b00000000

#define MIN_US_DELAY 360
#define DISPLAY_BLINK_INTERVAL 500

#define GET_BIT(data, n) (1 & (data >> n))
#define SET_BIT(data, n, value) (value ? data |= (1 << n) : data &= ~(1 << n))
#define IS_ARMED() GET_BIT(state, ARMED_BIT)

// methods
void drawDisplayBuffer();
void updateProperties();
void finalize();
void arm();

// interrupt routines
void IRAM_ATTR SENSOR1_ISR();
void IRAM_ATTR SENSOR2_ISR();

// measurement variables
double spt1, spt2, dpt1, dpt2;
volatile unsigned long ts1_entry, ts1_exit, ts2_entry, ts2_exit;
volatile byte state;
volatile bool sensor1_level, sensor2_level, old_sensor1_level, old_sensor2_level;

// result variables
double vs1, vs2, vd1, vd2;
double fps;
double mass;
double length;
DART_TYPE type;

// misc variables
bool update_display;
bool blink_state;
unsigned long last_display_blink;
unsigned long current_ms;
#endif