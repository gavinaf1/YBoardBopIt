#include "accelerometer.h"

////////////////////////////////////////////////////////////////////////////

point3_t r[NUM_VERTICES];
point3_t axes[NUM_AXES_VERTICES];
point2_t ax2[NUM_AXES_VERTICES];

static Adafruit_SSD1306 display(128, 64); // Create display


//////////////////////// High-Level "main" Function ////////////////////////

// This function calls other functions to perform each step of figuring out what
// needs to be displayed every instant based on accelerometer data
void defaultProgram() {
  angle3_t theta = {ROT_X, ROT_Y, ROT_Z};
  point2_t v[NUM_VERTICES];

  // display.clearDisplay();
  setVertices(r, NUM_VERTICES);     // 1. Create 3D Vertices
  rotate(theta, r, NUM_VERTICES);   // 2. Rotate 3D Vertices
  projTo2D(r, v, NUM_VERTICES);     // 3. Project 3D Vertices Into 2D Space
  drawAxes(ax2, NUM_AXES_VERTICES); // 4. Draw Static Axes
  drawMagnitude(v, NUM_VERTICES);   // 5. Draw Magnitude Vectors
  drawInfo(r, NUM_VERTICES);        // 6. Display (x, y, z) Info
  // display.display();
}

///////////////////// Coordinate Management Functions //////////////////////

// This function creates vertices for drawing the 3-dimensional graph off to the
// right
void setVertices(point3_t *r, int arrLen) {
  // Get info from accelerometer
  accelerometer_data accel_data = Yboard.get_accelerometer();
  float x = accel_data.x * ACCEL_SCALE_FACTOR;
  float y = accel_data.y * ACCEL_SCALE_FACTOR;
  float z = accel_data.z * ACCEL_SCALE_FACTOR * 1.2;

  // Scale some axes so it looks more normal to the human eye
  if (x < 0)
    x *= 1.2;
  if (z < 0)
    z *= 0.8;

  // Set vertices for drawing vectors
  r[0] = {0, 0, 0};
  r[1] = {x, 0, 0};
  r[2] = {0, y, 0};
  r[3] = {0, 0, -z};
  r[4] = {x, y, -z};
}

// This function creates the 3D axes that are drawn off to the right
// Note: this is not drawing the acceleration magnitude vector - only the axes
void setup_axes() {
  angle3_t theta = {ROT_X, ROT_Y, ROT_Z};
  setAxes(axes, NUM_AXES_VERTICES);
  rotate(theta, axes, NUM_AXES_VERTICES);
  projTo2D(axes, ax2, NUM_AXES_VERTICES);
}

// This function creates vertices for drawing the axes
void setAxes(point3_t *ax, int arrLen) {
  float x = AXES_LENGTH;
  float y = AXES_LENGTH;
  float z = AXES_LENGTH;

  ax[0] = {0, 0, 0};
  ax[1] = {x, 0, 0};
  ax[2] = {-x, 0, 0};
  ax[3] = {0, y, 0};
  ax[4] = {0, -y, 0};
  ax[5] = {0, 0, -z};
  ax[6] = {0, 0, z};
}

// This function takes in a set of points (vertices) and applies a rotation
// matrix to them so that, once projected into 2D space (the screen), the points
// appear to have rotated. This is what makes the graph appear 3D.
void rotate(angle3_t theta, point3_t *r, int arrLen) {
  float cx = cos(theta.x);
  float cy = cos(theta.y);
  float cz = cos(theta.z);
  float sx = sin(theta.x);
  float sy = sin(theta.y);
  float sz = sin(theta.z);

  point3_t temp;

  // Rotate each vertex in 3D space according to the three theta values
  for (int i = 0; i < arrLen; i++) {
    // Rotation about x-axis
    temp.x = r[i].x;
    temp.y = r[i].y * cx + r[i].z * sx;
    temp.z = r[i].y * -sx + r[i].z * cx;
    r[i] = temp;

    // Rotation about y-axis
    temp.x = r[i].x * cy + r[i].z * -sy;
    temp.y = r[i].y;
    temp.z = r[i].x * sy + r[i].z * cy;
    r[i] = temp;

    // Rotation about z-axis
    temp.x = r[i].x * cz + r[i].y * sz;
    temp.y = r[i].x * -sz + r[i].y * cz;
    temp.z = r[i].z;
    r[i] = temp;
  }
}

// This function takes a set of points in 3D space and maps them into a 2D space
// in such a way that the drawing resulting from such points still appears as if
// it were 3D
void projTo2D(point3_t *r, point2_t *v, int arrLen) {
  for (int i = 0; i < arrLen; i++) {
    v[i].x = FOCAL_LENGTH * r[i].x / (FOCAL_LENGTH + r[i].z);
    v[i].y = FOCAL_LENGTH * r[i].y / (FOCAL_LENGTH + r[i].z);
  }
}

//////////////////////////// Display Functions ////////////////////////////

// This just runs initialization functions to get the screen ready to go
void setup_display() {
  delay(1000); // Display needs time to initialize
  display.begin(SSD1306_SWITCHCAPVCC,
                0x3c); // Initialize display with I2C address: 0x3C
  display.clearDisplay();
  display.setTextColor(ON);
  display.setRotation(ZERO_DEG); // Can be 0, 90, 180, or 270
  display.setTextWrap(false);
  // display.dim(BRIGHTNESS_DAMPER); // Causes some screens to fail
  display.display();
}

// This function draws the resultant acceleration vector (accounts for direction
// and magnitude of the board's overall acceleration)
void drawMagnitude(point2_t *v, int arrLen) {
  // Center the projection on-screen
  float x_zero = SCREEN_WIDTH * 3 / 4;
  float y_zero = SCREEN_BODY_HEIGHT / 2 + SCREEN_TITLE_HEIGHT;

  // Draw Component Magnitude Vectors
  // display.drawLine((int)(x_zero + v[0].x), (int)(y_zero + v[0].y),
  //                  (int)(x_zero + v[1].x), (int)(y_zero + v[1].y), ON);
  // display.drawLine((int)(x_zero + v[0].x), (int)(y_zero + v[0].y),
  //                  (int)(x_zero + v[2].x), (int)(y_zero + v[2].y), ON);
  // display.drawLine((int)(x_zero + v[0].x), (int)(y_zero + v[0].y),
  //                  (int)(x_zero + v[3].x), (int)(y_zero + v[3].y), ON);

  // Draw Summation Magnitude Vector
  display.drawLine((int)(x_zero + v[0].x), (int)(y_zero + v[0].y),
                   (int)(x_zero + v[4].x), (int)(y_zero + v[4].y), ON);
  display.fillCircle((int)(x_zero + v[4].x), (int)(y_zero + v[4].y), 1, ON);

  // Debugging Mark -- Origin
  // display.drawCircle((int)(x_zero + v[0].x), (int)(y_zero + v[0].y), 3, ON);
}

// This function draws the axes within which the magnitude vector will be drawn
void drawAxes(point2_t *ax2, int arrLen) {
  // Center the projection on-screen
  float x_zero = SCREEN_WIDTH * 3 / 4;
  float y_zero = SCREEN_BODY_HEIGHT / 2 + SCREEN_TITLE_HEIGHT;

  // X-axis
  display.drawLine((int)(x_zero + ax2[0].x), (int)(y_zero + ax2[0].y),
                   (int)(x_zero + ax2[1].x), (int)(y_zero + ax2[1].y), ON);
  display.drawLine((int)(x_zero + ax2[0].x), (int)(y_zero + ax2[0].y),
                   (int)(x_zero + ax2[2].x), (int)(y_zero + ax2[2].y), ON);

  // Y-axis
  display.drawLine((int)(x_zero + ax2[0].x), (int)(y_zero + ax2[0].y),
                   (int)(x_zero + ax2[3].x), (int)(y_zero + ax2[3].y), ON);
  display.drawLine((int)(x_zero + ax2[0].x), (int)(y_zero + ax2[0].y),
                   (int)(x_zero + ax2[4].x), (int)(y_zero + ax2[4].y), ON);

  // Z-axis
  display.drawLine((int)(x_zero + ax2[0].x), (int)(y_zero + ax2[0].y),
                   (int)(x_zero + ax2[5].x), (int)(y_zero + ax2[5].y), ON);
  display.drawLine((int)(x_zero + ax2[0].x), (int)(y_zero + ax2[0].y),
                   (int)(x_zero + ax2[6].x), (int)(y_zero + ax2[6].y), ON);

  // Debugging Mark -- Origin
  // display.drawCircle((int)(x_zero + ax2[0].x), (int)(y_zero + ax2[0].y), 3,
  // ON);
}

// This function draws everything else on-screen:
// 1. The title bar ("Accelerate!")
// 2. X, Y, and Z listed vertically on the lefthand side of the screen
// 3. Bars representing the magnitude (bar length) and direction (shaded vs.
//    unshaded) of acceleration experienced in each of the coordinate
//    directions: X, Y, and Z
void drawInfo(point3_t *r, int arrLen) {
  uint8_t text_size = 2;

  // Draw Title
  display.setCursor(0, 0);
  display.setTextSize(text_size);
  display.print("Accelerate!");

  // Get Acceleration Info
  // (x, y, z, don't line up because I'm redefining their orientation on-screen)
  accelerometer_data accel_data = Yboard.get_accelerometer();
  float x = -accel_data.x * ACCEL_SCALE_FACTOR;
  float y = accel_data.y * ACCEL_SCALE_FACTOR;
  float z = -accel_data.z * ACCEL_SCALE_FACTOR;
  text_size = 1;

  // Draw Characters
  display.drawChar(TEXT_X_POS, TEXT_Y_POS_X, 'x', ON, OFF, text_size);
  display.drawChar(TEXT_X_POS, TEXT_Y_POS_Y, 'y', ON, OFF, text_size);
  display.drawChar(TEXT_X_POS, TEXT_Y_POS_Z, 'z', ON, OFF, text_size);

  // Draw Acceleration Bars
  if (x > 0) // x
    display.fillRect(BAR_X_POS, BAR_Y_POS_X, x, TEXT_HEIGHT, ON);
  else if (x < 0)
    display.drawRect(BAR_X_POS, BAR_Y_POS_X, -x, TEXT_HEIGHT, ON);

  if (y > 0) // y
    display.fillRect(BAR_X_POS, BAR_Y_POS_Y, y, TEXT_HEIGHT, ON);
  else if (y < 0)
    display.drawRect(BAR_X_POS, BAR_Y_POS_Y, -y, TEXT_HEIGHT, ON);

  if (z > 0) // z
    display.fillRect(BAR_X_POS, BAR_Y_POS_Z, z, TEXT_HEIGHT, ON);
  else if (z < 0)
    display.drawRect(BAR_X_POS, BAR_Y_POS_Z, -z, TEXT_HEIGHT, ON);
}