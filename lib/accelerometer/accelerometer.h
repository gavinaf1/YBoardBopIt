#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

/////////////////////////////////////////////////////////////////////////////////////

#include "SparkFun_LIS2DH12.h"
#include "yboard.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/////////////////////////////////////////////////////////////////////////////////////

// LEDs
#define LED_COUNT 20
#define LED_PIN 5

// Screen Constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_TITLE_HEIGHT 16
#define SCREEN_BODY_HEIGHT 48

// Screen Orientation
#define ZERO_DEG 0
#define NINETY_DEG 1
#define ONE_EIGHTY_DEG 2
#define TWO_SEVENTY_DEG 3

// Screen Brightness
#define BRIGHTNESS_DAMPER 0.8 // 0 is brightest
#define REFRESH_RATE 50       // Measured in ms

// Colors
#define OFF 0
#define ON 1

// Accelerometer
#define ACCEL_ADDR 0x19
#define ACCEL_SCALE_FACTOR 0.02

// 3D Rendering
// #define PI 3.1415926535
#define NUM_VERTICES 5
#define NUM_AXES_VERTICES 7
#define WIREFRAME_CENTER_OFFSET 0
#define FOCAL_LENGTH 40.0

// On-Screen Info
#define TEXT_WIDTH 5
#define TEXT_HEIGHT 8

//////////////////////// Try Editing These!! ////////////////////////

#define ROT_X (PI + PI / 8)
#define ROT_Y (-PI / 4)
#define ROT_Z (PI / 2)

#define AXES_LENGTH 10.0

#define TEXT_X_POS 0
#define TEXT_Y_POS_X SCREEN_TITLE_HEIGHT

#define BAR_X_POS (TEXT_X_POS + TEXT_WIDTH + 1)
#define BAR_Y_POS_X SCREEN_TITLE_HEIGHT

#define BAR_SCALE_FACTOR 2.0

/////////////////////////////////////////////////////////////////////////////////////

#define TEXT_Y_POS_Y (TEXT_Y_POS_X + TEXT_HEIGHT)
#define TEXT_Y_POS_Z (TEXT_Y_POS_Y + TEXT_HEIGHT + 1)
#define BAR_Y_POS_Y (TEXT_Y_POS_X + TEXT_HEIGHT)
#define BAR_Y_POS_Z (TEXT_Y_POS_Y + TEXT_HEIGHT + 1)

/////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  float x;
  float y;
} point2_t;

typedef struct {
  float x;
  float y;
  float z;
} point3_t;

typedef struct {
  float x;
  float y;
  float z;
} angle3_t; // Measure in radians

// Basically "main"
void defaultProgram();

// Coordinate Management Functions
void setVertices(point3_t *r, int arrLen);
void setup_axes();
void setAxes(point3_t *ax, int arrLen);
void rotate(angle3_t theta, point3_t *r, int arrLen);
void projTo2D(point3_t *r, point2_t *v, int arrLen);

// Display Functions
void setup_display();
void drawMagnitude(point2_t *v, int arrLen);
void drawAxes(point2_t *ax2, int arrLen);
void drawInfo(point3_t *v, int arrLen);

/////////////////////////////////////////////////////////////////////////////////////

#endif // ACCELEROMETER_H