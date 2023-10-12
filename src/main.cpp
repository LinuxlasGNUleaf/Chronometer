#include <main.h>

void setup()
{
  // configure sensors
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);

  // reset variables
  state = IDLE;
  update_display = false;
  type = DART_MEGA;
  updateProperties();

  // start display
  u8g2.begin();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  drawDisplayBuffer();
  u8g2.sendBuffer();

  // attach ISRs
  attachInterrupt(sensorPin1, SENSOR1_ISR, CHANGE);
  attachInterrupt(sensorPin2, SENSOR2_ISR, CHANGE);

  Serial.begin(115200);

  arm();
}

void printState()
{
  Serial.print("0b");
  for (int i = 7; i >= 0; i--)
  {
    Serial.print(GET_BIT(state, i));
  }
  Serial.println();
}

byte old_state = IDLE;
void loop()
{
  if (state != old_state)
  {
    printState();
    old_state = state;
  }
  // if measurement is complete, start computation
  if (state == ALL_SENSORS_FIRED)
    finalize();

  // if measurement has started, return
  if (GET_BIT(state, 0) == 1)
    return;

  current_ms = millis();

  if (current_ms - last_display_blink > DISPLAY_BLINK_INTERVAL && IS_ARMED())
  {
    blink_state = !blink_state;
    last_display_blink = current_ms;
    update_display = true;
  }

  // if not armed and update has been requested, redraw screen
  if (update_display)
  {
    drawDisplayBuffer();
    // if measurement hasn't started, continue
    if (GET_BIT(state, 0) == 0)
    {
      u8g2.sendBuffer();
      update_display = false;
    }
  }
}

void IRAM_ATTR SENSOR1_ISR()
{
  sensor1_level = digitalRead(sensorPin1);
  // return when not armed
  if (!IS_ARMED())
  {
    old_sensor1_level = sensor1_level;
    return;
  }

  // entry point trigger when:
  if (
      // - sensor has NOT fired yet
      GET_BIT(state, 0) == false &&
      // - level is HIGH
      sensor1_level == HIGH &&
      // - old_level is low
      old_sensor1_level == LOW)
  {
    ts1_entry = micros();
    SET_BIT(state, 0, true);
  }
  // exit point trigger when:
  else if (
      // - entry point trigger has fired
      GET_BIT(state, 0) == true &&
      // - exit point trigger has NOT fired yet
      GET_BIT(state, 1) == false &&
      // - level is LOW
      sensor1_level == LOW &&
      // - old level is HIGH
      old_sensor1_level == HIGH &&
      // - MIN_DELAY has passed
      (micros() - ts1_entry) > MIN_US_DELAY)
  {
    ts1_exit = micros();
    SET_BIT(state, 1, true);
  }

  old_sensor1_level = sensor1_level;
}

void IRAM_ATTR SENSOR2_ISR()
{
  sensor2_level = digitalRead(sensorPin2);
  // return when not armed OR sensor 1 has not fired yet
  if (!IS_ARMED() || !GET_BIT(state, 0))
  {
    old_sensor2_level = sensor2_level;
    return;
  }

  // entry point trigger when:
  if (
      // - sensor has NOT fired yet
      GET_BIT(state, 2) == false &&
      // - level is HIGH
      sensor2_level == HIGH &&
      // - old_level is low
      old_sensor2_level == LOW)
  {
    ts2_entry = micros();
    SET_BIT(state, 2, true);
  }
  // exit point trigger when:
  else if (
      // - entry point trigger has fired
      GET_BIT(state, 2) == true &&
      // - exit point trigger has NOT fired yet
      GET_BIT(state, 3) == false &&
      // - level is LOW
      sensor2_level == LOW &&
      // - old level is HIGH
      old_sensor2_level == HIGH &&
      // - MIN_DELAY has passed
      (micros() - ts2_entry) > MIN_US_DELAY)
  {
    ts2_exit = micros();
    SET_BIT(state, 3, true);
  }

  old_sensor2_level = sensor2_level;
}

void drawDisplayBuffer()
{
  u8g2.clearBuffer();
  //==========> UPPER HALF OF DISPLAY
  char *buf_fps;
  char *buf_j;
  buf_fps = new char[4];
  buf_j = new char[5];
  sprintf(buf_fps, "%03.0f", constrain(fps, 0, 999));
  sprintf(buf_j, "%4.0f", constrain(CALC_MJOULE(fps, mass), 0.0f, 9999.0f));

  // big fps display
  // only draw FPS count when not armed or blink_state is true
  if (!IS_ARMED() || blink_state)
  {
    u8g2.setFont(u8g2_font_inr33_mf);
    u8g2.drawStr(0, 0, buf_fps);
  }
  u8g2.setFont(u8g2_font_inr16_mf);
  u8g2.drawStr(28 * 3, 33 - 16, "fps");

  // small energy display
  u8g2.setFont(u8g2_font_5x8_mf);
  u8g2.drawStr(28 * 3 + 4, 2, buf_j);
  u8g2.drawStr(28 * 3 + 4 + 5 * 5, 2, "mJ");

  // seperators
  u8g2.drawLine(0, 33 + 6, 127, 33 + 6);
  u8g2.drawLine(0, 33 + 8, 127, 33 + 8);

  //==========> LOWER HALF OF DISPLAY
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

  // draw measurements
  u8g2.drawLine(56, 33 + 9, 56, 63);
  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.drawStr(58, 42, "RAW (fps):");
  char buf[20];
  sprintf(buf, "spt:%03.0f|%03.0f", min(vs1, 999.0), min(vs2, 999.0));
  u8g2.drawStr(58, 42 + 8, buf);
  sprintf(buf, "dpt:%03.0f|%03.0f", min(vd1, 999.0), min(vd2, 999.0));
  u8g2.drawStr(58, 42 + 16, buf);

  // draw state
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

void finalize()
{
  spt1 = (ts1_exit - ts1_entry) * 1e-6;
  spt2 = (ts2_exit - ts2_entry) * 1e-6;
  dpt1 = (ts2_entry - ts1_entry) * 1e-6;
  dpt2 = (ts2_exit - ts1_exit) * 1e-6;

  vs1 = (length / spt1) * MPS_TO_FPS;
  vs2 = (length / spt2) * MPS_TO_FPS;
  vd1 = (sensor_distance / dpt1) * MPS_TO_FPS;
  vd2 = (sensor_distance / dpt2) * MPS_TO_FPS;

  fps = (vs1 + vs2 + vd1 + vd2) * 0.25f;
  update_display = true;
  state = IDLE;
}

void arm()
{
  ts1_entry = 0, ts1_exit = 0, ts2_entry = 0, ts2_exit = 0;
  sensor1_level = digitalRead(sensorPin1);
  sensor2_level = digitalRead(sensorPin2);
  old_sensor1_level = sensor1_level;
  old_sensor2_level = sensor2_level;
  state = ARMED;
  last_display_blink = millis();
  blink_state = true;
}