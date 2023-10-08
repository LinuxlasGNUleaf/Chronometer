#include <main.h>

void setup()
{
  type = DART_NORMAL;
  updateProperties();
  // put your setup code here, to run once:
  u8g2.begin();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

int i = 0;
void loop()
{
  fps = i++ % 1000;
  u8g2.clearBuffer();
  drawTopDisplay();
  drawStats();
  u8g2.sendBuffer();
  delay(150);
}

void drawStats()
{
  // draw dart type
  u8g2.setFont(u8g2_font_12x6LED_tf);
  switch (type)
  {
  case DART_NORMAL:
    u8g2.drawStr(32 + 7, 43 + 7, "N");
    u8g2.drawXBM(2, 43 + 9, dart_bmp_width, dart_bmp_height, dart_normal);
    break;
  case DART_MEGA:
    u8g2.drawStr(32 + 7, 43 + 7, "M");
    u8g2.drawXBM(2, 43 + 9, dart_bmp_width, dart_bmp_height, dart_mega);
    break;
  case DART_SHORT:
    u8g2.drawStr(32 + 7, 43 + 7, "S");
    u8g2.drawXBM(2, 43 + 9, dart_bmp_width, dart_bmp_height, dart_short);
    break;
  }
  u8g2.setFont(u8g2_font_5x8_mf);
  u8g2.drawStr(1, 42, "projectile:");

  // draw timings
  u8g2.drawLine(56, 33 + 9, 56, 63);
  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.drawStr(58, 42, "TIMINGS (ms):");
  u8g2.drawStr(58, 42 + 8, "spt: xx | xx");
  u8g2.drawStr(58, 42 + 16, "dpt: xx | xx");

  // draw state
}

void drawTopDisplay()
{
  char *buf_fps;
  char *buf_j;
  buf_fps = new char[4];
  buf_j = new char[5];
  sprintf(buf_fps, "%03.0f", constrain(fps, 0, 999));
  sprintf(buf_j, "%4.0f", constrain(CALC_MJOULE(fps, mass), 0.0f, 9999.0f));

  // big fps display
  u8g2.setFont(u8g2_font_inr33_mf);
  u8g2.drawStr(0, 0, buf_fps);
  u8g2.setFont(u8g2_font_inr16_mf);
  u8g2.drawStr(28 * 3, 33 - 16, "fps");

  // small energy display
  u8g2.setFont(u8g2_font_5x8_mf);
  u8g2.drawStr(28 * 3 + 4, 2, buf_j);
  u8g2.drawStr(28 * 3 + 4 + 5 * 5, 2, "mJ");

  // seperators
  u8g2.drawLine(0, 33 + 6, 127, 33 + 6);
  u8g2.drawLine(0, 33 + 8, 127, 33 + 8);
}

void updateProperties()
{
  switch (type)
  {
  case DART_NORMAL:
    mass = mass_dart_normal;
    length = length_dart_normal;
    break;
  case DART_SHORT:
    mass = mass_dart_short;
    length = length_dart_short;
    break;
  case DART_MEGA:
    mass = mass_dart_mega;
    length = length_dart_mega;
    break;
  }
}