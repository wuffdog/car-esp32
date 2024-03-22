// Autonomous vehicle with Arduino Nano ESP32
// Ver 9 - Caramel Gara @ Feb 20, 2024 

// -----------------------------------------------------
// Pin define
#define PLSER D2   // Left servo: Orange
#define PRSER D3   // Right servo: Yellow
#define PBUZZ D12  // Buzzer

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

int isCtrl = 0;
int ctrlSpd = 0;
int ctrlDir = 0;
int ctrlA = 0;
int ctrlY = 0;

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
String strDyn1;
String strDyn2;

// Display output
#define MSGLEN 16
char charDisp[MSGLEN];

// -----------------------------------------------------
// Lock status
int isLocked = 1;

// -----------------------------------------------------
// Function: Read Xbox controller input
void ctrlRead() {
  xboxController.onLoop();
  if(xboxController.isConnected()){
    isCtrl = 1;
    ctrlSpd = (-1) * (xboxController.xboxNotif.joyLVert - 32767);
    ctrlDir = xboxController.xboxNotif.joyRHori - 32767;
    ctrlA = xboxController.xboxNotif.btnA;
    ctrlY = xboxController.xboxNotif.btnY;
  }else{
    isCtrl = 0;
  }
}

// -----------------------------------------------------
// Function: Stop everything
void stopMode(){
    lSer.write(lStop);
    rSer.write(rStop);
    sendBuzz(0);
}

// -----------------------------------------------------
// Function: Send to display
void sendDisp(String strLine1, String strLine2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_chargen_92_me);
  strLine1.toCharArray(charDisp, MSGLEN);
  u8g2.drawStr(0, 15, charDisp);
  strLine2.toCharArray(charDisp, MSGLEN);
  u8g2.drawStr(0, 31, charDisp);
  u8g2.sendBuffer();
}

// -----------------------------------------------------
// Function: Send to buzzer library
void sendBuzz(int i) {
  switch (i) {

    // 1. Startup sound
    case 1:
      {
        buzz.writeTone(800);
        delay(250);
        buzz.writeTone(900);
        delay(250);
        buzz.writeTone(1200);
        delay(350);
        buzz.writeScaled(0);
        break;
      }

    // 2. Horn
    case 2:
      {
        buzz.writeTone(900);
        break;
      }

    // 3. Unlock
    case 3:
      {
        buzz.writeTone(800);
        delay(250);
        buzz.writeTone(1200);
        delay(250);
        buzz.writeScaled(0);
        break;
      }

    // 4. Lock
    case 4:
      {
        buzz.writeTone(1200);
        delay(250);
        buzz.writeTone(800);
        delay(250);
        buzz.writeScaled(0);
        break;
      }

    // 0. Mute
    default: buzz.writeScaled(0);  // Mute
  }
}

// -----------------------------------------------------
void setup() {

  // Servo pin attachment
  lSer.attach(PLSER);
  rSer.attach(PRSER);
  // Servo speed initialization
  lSer.write(lStop - lSpeed);
  rSer.write(rStop - rSpeed);

  // Display setup
  u8g2.begin();
  // Display startup message
  sendDisp("Gara Dev", "ver 9.0");

  // Xbox controller setup
  xboxController.begin();

  // Buzzer setup
  // Freq = 800Hz to match the first startup sound.
  buzz.attachPin(PBUZZ, 800);
  // Buzzer startup sound
  sendBuzz(1);

  delay(500);
}

void loop() {
tgHead:

  // Read Xbox controller input
  ctrlRead();

  // If Xbox controller is disconnected
  if(isCtrl == 0){
    sendDisp("Bluetooth", "Hold PAIR");
    stopMode();
    goto tgHead;
  }

  // Locked mode: Push Y on Xbox controller
  while (ctrlY) {
    ctrlRead();
    if (!ctrlY) {
      if (isLocked) {
        isLocked = 0;
        sendBuzz(3);  // Unlock sound
      } else {
        isLocked = 1;
        sendBuzz(4);  // Lock sound
      }
    }
  }

  if (isLocked) {
    // Locked display message
    sendDisp("Locked", "Push Y");
    stopMode();
    // Locked loop
    goto tgHead;
  }

  // Xbox controller steady state tolerance
  if (ctrlSpd > -2500 && ctrlSpd < 2500) ctrlSpd = 0;
  if (ctrlDir > -2500 && ctrlDir < 2500) ctrlDir = 0;

  // Xbox controller input scaling
  ctrlSpd /= 1000;
  ctrlDir /= 2000;

  // Convert to servo speed
  lSpeed = ctrlSpd + ctrlDir;
  rSpeed = ctrlSpd - ctrlDir;

  // Set servo speed
  lSer.write(lStop + lSpeed);
  rSer.write(rStop + rSpeed);

  // Display
  strDyn1 = lStrCst + lSpeed;
  strDyn2 = rStrCst + rSpeed;
  sendDisp(strDyn1, strDyn2);

  // Buzzer: Hold A on Xbox controller
  if (ctrlA) sendBuzz(2);
  else sendBuzz(0);
}
