#include <main.h>

void setup()
{
  // put your setup code here, to run once:
  u8g2.begin();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

int i = 0;
void loop()
{
  u8g2.clearBuffer();
  drawSpeed(i++ % 1000);
  u8g2.drawLine(0, 33+6, 127, 33+6);
  drawStats()
  u8g2.sendBuffer();
  delay(150);
}

void drawSpeed(float fps)
{
  char *buf_fps;
  char *buf_kmh;
  buf_fps = new char[4];
  buf_kmh = new char[4];
  sprintf(buf_fps, "%03.0f", constrain(fps, 0, 999));
  sprintf(buf_kmh, "%03.0f", constrain(fps * fps_to_kmh, 0, 999));

  // big fps display
  u8g2.setFont(u8g2_font_inr33_mf);
  u8g2.drawStr(0, 0, buf_fps);
  u8g2.setFont(u8g2_font_inr16_mf);
  u8g2.drawStr(28 * 3 + 2, 33 - 16, "fps");

  // small km/h display
  u8g2.setFont(u8g2_font_5x8_mf);
  u8g2.drawStr(28 * 3 + 4, 2, buf_kmh);
  u8g2.drawStr(28 * 3 + 4 + 5 * 4, 2, "km/h");
}