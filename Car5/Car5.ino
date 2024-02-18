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
// WiFi
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
const char *ssid = "Gara Dev";
const char *password = "gara-dev";
WiFiServer server(80);

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
String lStrCst = "Left:  ";
String rStrCst = "Right: ";

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
  u8g2.drawStr(0,24,"Gara Dev 5");
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

  // WiFi setup
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  server.begin();
  
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
 
  // WiFi begin ---------------------------------------------------------------
  WiFiClient client = server.available();
  if (client) { 
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // HTML begin -----------------------------------------------------
            
            client.print("<p style=\"font-size:64px\"><a href=\"/forward\">Forward</a><br></p>");
            client.print("<p style=\"font-size:64px\"><a href=\"/backward\">Backward</a><br></p>");
            client.print("<p style=\"font-size:64px\"><a href=\"/left\">Left</a><br></p>");
            client.print("<p style=\"font-size:64px\"><a href=\"/right\">Right</a><br></p>");
            client.print("<p style=\"font-size:64px\"><a href=\"/stop\">Stop</a><br></p>");

            // HTML end -------------------------------------------------------

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Response check begin -----------------------------------------------

        if (currentLine.endsWith("GET /forward")) {
          lSpeed = 20;
          rSpeed = 20; 
          buzz.writeScaled(0);
        }

        if (currentLine.endsWith("GET /backward")) {
          lSpeed = -20;
          rSpeed = -20; 
          buzz.writeTone(500);
        }

        if (currentLine.endsWith("GET /left")) {
          lSpeed = 10;
          rSpeed = 20; 
          buzz.writeScaled(0);
        }

        if (currentLine.endsWith("GET /right")) {
          lSpeed = 20;
          rSpeed = 10; 
          buzz.writeScaled(0);
        }

        if (currentLine.endsWith("GET /stop")) {
          lSpeed = 0;
          rSpeed = 0; 
          buzz.writeScaled(0);
        }

        // Response check end-- -----------------------------------------------
      }
    }
    client.stop();
  }

  // WiFi end -----------------------------------------------------------------
  
  // Set speed
  lSer.write(lStop - lSpeed);
  rSer.write(rStop - rSpeed);

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
