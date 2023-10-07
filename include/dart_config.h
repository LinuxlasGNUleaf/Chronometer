#ifndef BITMAPS_H
#define BITMAPS_H
#include <Arduino.h>

enum DART_TYPE
{
   DART_NORMAL,
   DART_SHORT,
   DART_MEGA
};

const unsigned dart_bmp_width = 32;
const unsigned dart_bmp_height = 12;

//=====> NORMAL DART
const double mass_dart_normal = 1.2e-3;
const double length_dart_normal = 7.1e-2;
static unsigned char dart_normal[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x3f,
    0x3c, 0x00, 0x00, 0x20, 0x3c, 0x00, 0x00, 0x20, 0x3c, 0x00, 0x00, 0x20,
    0x3c, 0x00, 0x00, 0x20, 0x3c, 0x00, 0x00, 0x20, 0x3c, 0x00, 0x00, 0x20,
    0xf8, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//=====> MEGA DART
const double mass_dart_mega = 2.7e-3;
const double length_dart_mega = 9.5e-2;
static unsigned char dart_mega[] = {
    0xfc, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0xc0,
    0x3f, 0x00, 0x00, 0xc0, 0x37, 0x00, 0x00, 0xc0, 0x37, 0x00, 0x00, 0xc0,
    0x37, 0x00, 0x00, 0xc0, 0x37, 0x00, 0x00, 0xc0, 0x3f, 0x00, 0x00, 0xc0,
    0x3f, 0x00, 0x00, 0xc0, 0xfe, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff};

//=====> SHORT DART
// TODO: measure values!
const double mass_dart_short = 1.2e-3 / 2;
const double length_dart_short = 7.1e-2 / 2;
static unsigned char dart_short[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xcd, 0x0c,
    0x3c, 0x00, 0x01, 0x20, 0x3c, 0x00, 0x01, 0x20, 0x3c, 0x00, 0x01, 0x00,
    0x3c, 0x00, 0x01, 0x00, 0x3c, 0x00, 0x01, 0x20, 0x3c, 0x00, 0x01, 0x20,
    0xf8, 0xff, 0xcd, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif