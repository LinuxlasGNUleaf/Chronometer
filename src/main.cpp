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
  type_index = DART_NORMAL;
  button_press_start = 0;
  button_previously_pressed = false;
  discard_button_release = false;
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
  Serial.println("\nReady.");

  arm();
}

void loop()
{
  // if measurement is complete, start computation
  if (state == ALL_SENSORS_FIRED)
    finalize();

  // if measurement has started, return
  if (GET_BIT(state, 0) == 1)
    return;

  current_ms = millis();

  handleButtonInput();

  if (current_ms - last_blink_ms > DISPLAY_BLINK_INTERVAL)
  {
    blink_state = !blink_state;
    update_display = true;
    last_blink_ms = current_ms;
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
  sprintf(buf_j, "%4.0f", constrain(energy * 1000, 0.0f, 9999.0f));

  // big fps display
  // only draw FPS count when not armed or blink_state is true
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

  //==========> LOWER HALF OF DISPLAY
  // draw dart type
  u8g2.setFont(u8g2_font_12x6LED_tf);
  switch (type_index)
  {
  case DART_NORMAL:
    u8g2.drawStr(32 + 7, 43 + 7, "S");
    u8g2.drawXBM(2, 43 + 9, dart_bmp_width, dart_bmp_height, dart_normal);
    break;
  case DART_MEGA:
    u8g2.drawStr(32 + 7, 43 + 7, "M");
    u8g2.drawXBM(2, 43 + 9, dart_bmp_width, dart_bmp_height, dart_mega);
    break;
  case DART_SHORT:
    u8g2.drawStr(32 + 7, 43 + 7, "H");
    u8g2.drawXBM(2, 43 + 9, dart_bmp_width, dart_bmp_height, dart_short);
    break;
  }
  u8g2.setFont(u8g2_font_5x8_mf);
  u8g2.drawStr(1, 42, "projectile:");

  u8g2.drawLine(56, 33 + 9, 56, 63);
  u8g2.drawPixel(56 + 2, 33 + 7);

  // draw measurements
  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.drawStr(58, 42, "RAW (fps):");
  char buf[20];
  sprintf(buf, "spt:%03.0f %03.0f", min(vs1, 999.0), min(vs2, 999.0));
  u8g2.drawStr(58, 42 + 8, buf);
  sprintf(buf, "dpt:%03.0f %03.0f", min(vd1, 999.0), min(vd2, 999.0));
  u8g2.drawStr(58, 42 + 16, buf);

  u8g2.drawLine(103, 42, 103, 63);
  u8g2.drawPixel(103 + 2, 33 + 7);

  // draw state
  u8g2.setFont(u8g2_font_10x20_tf);
  if (state == IDLE || blink_state)
  {
    u8g2.drawRFrame(109, 44, 14, 20, 4);
    if (state == IDLE)
      u8g2.drawStr(111, 46, "I");
    else
      u8g2.drawStr(111, 46, "A");
  }
  else
  {
    u8g2.drawRBox(109, 44, 14, 20, 4);
    u8g2.setDrawColor(0);
    u8g2.drawStr(111, 46, "A");
    u8g2.setDrawColor(1);
  }
}

void updateProperties()
{
  // retrieve current dart specifications
  switch (type_index)
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
  // calculate deltas
  spt1 = (ts1_exit - ts1_entry) * 1e-6;
  spt2 = (ts2_exit - ts2_entry) * 1e-6;
  dpt1 = (ts2_entry - ts1_entry) * 1e-6;
  dpt2 = (ts2_exit - ts1_exit) * 1e-6;

  // calculate individual velocity measurements
  vs1 = (length / spt1) * MPS_TO_FPS;
  vs2 = (length / spt2) * MPS_TO_FPS;
  vd1 = (sensor_distance / dpt1) * MPS_TO_FPS;
  vd2 = (sensor_distance / dpt2) * MPS_TO_FPS;

  updateProperties();

  fps = (vs1 + vs2 + vd1 + vd2) * 0.25f;
  energy = (0.5f * fps * fps * FPS_TO_MPS * FPS_TO_MPS * mass); // 0.5*mv^2
  button_press_start = 0;
  button_previously_pressed = false;
  update_display = true;

  arm();
}

void arm()
{
  ts1_entry = 0, ts1_exit = 0, ts2_entry = 0, ts2_exit = 0;
  sensor1_level = digitalRead(sensorPin1);
  sensor2_level = digitalRead(sensorPin2);
  old_sensor1_level = sensor1_level;
  old_sensor2_level = sensor2_level;
  update_display = true;

  state = ARMED;
}

void handleButtonInput()
{
  // button not pressed
  if (digitalRead(buttonPin))
  {
    if (!button_previously_pressed)
      return;

    if (!discard_button_release)
    {
      type_index = (type_index + 1) % dart_type_count;
      update_display = true;
    }
    arm();
    button_press_end = current_ms;
    discard_button_release = false;
    button_previously_pressed = false;
  }
  // button pressed
  else
  {
    state = IDLE;
    if (button_previously_pressed)
    {
      // if button is held longer than the threshhold value, cycle the dart and reset the timer
      if (current_ms - button_press_start > BUTTON_PRESS_THRESHHOLD)
      {
        button_press_start = current_ms;
        type_index = (type_index + 1) % dart_type_count;
        update_display = true;
        discard_button_release = true;
      }
    }
    else if (current_ms - button_press_end > BUTTON_DEBOUNCE_MIN_MS)
    {
      button_press_start = current_ms;
      button_previously_pressed = true;
    }
  }
}