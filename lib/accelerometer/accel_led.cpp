#include "accel_led.h"

// This function lights up LEDs on the side of the board facing most toward the
// ground (meant to somewhat mimic shifting liquid around inside a container)
void light_LEDs() {
  accelerometer_data accel_data = Yboard.get_accelerometer();
  float x = accel_data.x;
  float y = accel_data.y;
  float z = accel_data.z;

  float xp = (x < 0) ? (0) : (x > 1000) ? (1000) : (x);
  float yp = (y < 0) ? (0) : (y > 1000) ? (1000) : (y);
  float zp = (z < 0) ? (0) : (z > 1000) ? (1000) : (z);
  float xn = (x < -1000) ? (1000) : (x > 0) ? (0) : (-x);
  float yn = (y < -1000) ? (1000) : (y > 0) ? (0) : (-y);
  float zn = (z < -1000) ? (1000) : (z > 0) ? (0) : (-z);
  float xi, yi, zi;

  for (int i = 1; i <= LED_COUNT; i++) {
    if (3 < i && i < 11)
      xi = xn;
    else
      xi = xp;

    if (i == 7 || i == 17)
      yi = (yp > yn) ? (yp) : (yn);
    else if (7 < i && i < 17)
      yi = yn;
    else
      yi = yp;

    zi = zn;

    if (Yboard.get_switch(1))
      xi = 0;
    if (Yboard.get_switch(2))
      yi = 0;

    Yboard.set_led_color(i, map(xi, 0, 1000, 0, 255), map(yi, 0, 1000, 0, 255),
                         map(zi, 0, 1000, 0, 255));
  }
}