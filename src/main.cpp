#include <main.h>

void setup()
{
  // put your setup code here, to run once:
  u8g2.begin();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void loop()
{
  // put your main code here, to run repeatedly:
  for (int i = 0; i <= 99; i++)
  {
    u8g2.clearBuffer();
    drawFPS(i);
    u8g2.sendBuffer();
    delay(150);
  }
}

void drawSpeed(float fps)
{
  char *buf_fps;
  char *buf_kmh;
  buf_fps = new char[3];
  buf_kmh = new char[4];
  sprintf(buf_fps, "%02d", constrain(round(fps),0,99));
  sprintf(buf_kmh, "%03d", constrain(round(fps*fps_to_kmh),0,999));

  u8g2.setFont(u8g2_font_inr42_mf);
  u8g2.drawStr(2, 2, buf_fps);
  u8g2.setFont(u8g2_font_inr21_mf);
  u8g2.drawStr(36 * 2 + 2, 42-21, "fps");
}