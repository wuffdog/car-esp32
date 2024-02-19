// -----------------------------------------------------
// Pin define
#define PLSER D2 // Left servo: Orange
#define PRSER D3 // Right servo: Yellow
#define PBUZZ D12 // Buzzer

#define MSGLEN 16

// -----------------------------------------------------
// Display
#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

// -----------------------------------------------------
// Servo
#include <ESP32Servo.h>
Servo lSer;
Servo rSer;

// -----------------------------------------------------
// Buzzer
#include <ESP32PWM.h>
ESP32PWM buzz;

// -----------------------------------------------------
// Xbox controller
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

int joySpd = 0;
int joyDir = 0;

// -----------------------------------------------------
// Servo stop state calibration
// Value: 90(technical stop) + Tested value
const int lStop = 90 + 2.5;
const int rStop = 90 + 2.5;

// Servo speed
// Initially stop (0)
int lSpeed = 0;
int rSpeed = 0;

// -----------------------------------------------------
// Display constant
String lStrCst = "L ";
String rStrCst = "R ";

// Display dynamic
String strDyn;

// Display output
char disp[MSGLEN];

// -----------------------------------------------------
void setup() {
  
  // Display setup
  u8g2.begin();

  // Display startup message
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_chargen_92_me);
  u8g2.drawStr(0,24,"Gara Dev 7");
  u8g2.sendBuffer();

  // Buzzer setup
  buzz.attachPin(PBUZZ, 1000);
  buzz.writeScaled(0);

  // Servo pin attachment
  lSer.attach(PLSER); 
  rSer.attach(PRSER); 
  
  // Servo speed initialization
  lSer.write(lStop - lSpeed);
  rSer.write(rStop - rSpeed);

  // Xbox controller setup
  xboxController.begin();

  // Buzzer startup sound
  buzz.writeTone(800);
  delay(300);
  buzz.writeTone(900);
  delay(300);
  buzz.writeTone(1200);
  delay(400);
  buzz.writeScaled (0);

}

void loop() {
 
  // Read Xbox controller input
  xboxController.onLoop();
  joySpd = (-1) * (xboxController.xboxNotif.joyLVert - 32767);
  joyDir = xboxController.xboxNotif.joyRHori - 32767;

  // Xbox controller steady state tolerance
  if(joySpd > -2500 && joySpd < 2500) joySpd = 0;
  if(joyDir > -2500 && joyDir < 2500) joyDir = 0;

  // Xbox controller input scaling
  joySpd /= 1000;
  joyDir /= 2000;

  // Convert to servo speed
  lSpeed = joySpd + joyDir;
  rSpeed = joySpd - joyDir;
  
  // Set servo speed
  lSer.write(lStop + lSpeed);
  rSer.write(rStop + rSpeed);

  // Display begin ------------------------------------------------------------
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_chargen_92_me);

  // Display line 1
  strDyn = lStrCst + lSpeed;
  strDyn.toCharArray(disp, MSGLEN);
  u8g2.drawStr(0,15,disp);

  // Display line 2
  strDyn = rStrCst + rSpeed;
  strDyn.toCharArray(disp, MSGLEN);
  u8g2.drawStr(0,31,disp);

  u8g2.sendBuffer();

  // Display end --------------------------------------------------------------

}
