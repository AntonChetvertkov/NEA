#include <FastIMU.h>
#include <TinyGPS++.h>
#include <GyverOLED.h>
#include "bitmaps.h"

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
int ui_mode = 1;
byte selected_index = 0;
String options[4] = {" Just Row ", " Workout ", " Log ", " About "};

#define tUp 26
#define tOk 25
#define tDown 33

#define IMU_ADDRESS 0x68
const float GYRO_THRESHOLD  = 30.0f;
const float GYRO_RESET      = 10.0f;
const unsigned long MIN_STROKE_MS  = 400;
const unsigned long SPM_TIMEOUT_MS = 5000;
MPU6500 imu;
GyroData gyro;
calData cal = { 0 };
bool aboveThreshold = false;
unsigned long lastStrokeTime = 0;
float spm = 0.0f;
float spmSum = 0.0f;
int   spmCount = 0;
float avgSpm = 0.0f;
unsigned long rowingTimerMs = 0;
unsigned long timerAccum = 0;
bool timerRunning = false;
bool okWasPressed = false;
unsigned long okPressedSince = 0;
const unsigned long HOLD_MS = 800;
float maxAxis(GyroData &g) {
    float ax = fabsf(g.gyroX);
    float ay = fabsf(g.gyroY);
    float az = fabsf(g.gyroZ);
    if (ax >= ay && ax >= az) return g.gyroX;
    if (ay >= ax && ay >= az) return g.gyroY;
    return g.gyroZ;
}

#define GNSS_RX 15
#define GNSS_TX 14
TinyGPSPlus gnss;
HardwareSerial gnssSerial(2);

String calcSplit(float velocityKmh) {
    if (velocityKmh <= 0.0f) {
        return "--:--";
    }

    float velocityMs = velocityKmh / 3.6f;
    float splitSeconds = 500.0f / velocityMs;

    int minutes = (int)splitSeconds / 60;
    int seconds = (int)splitSeconds % 60;

    String result = String(minutes) + ":";
    if (seconds < 10) result += "0";
    result += String(seconds);

    return result;
}

void setup() {
  oled.init();
  oled.setContrast(255);
  oled.setScale(2);

  pinMode(26, INPUT);
  pinMode(25, INPUT);
  pinMode(33, INPUT);

  Wire.begin();
  imu.init(cal, IMU_ADDRESS);

  gnssSerial.begin(9600, SERIAL_8N1, GNSS_RX, GNSS_TX);
}

void loop() {
  oled.clear();
  oled.home();

  while (gnssSerial.available() > 0) {
    gnss.encode(gnssSerial.read());
  }

  switch (ui_mode){
    case 0:{
      imu.update();
      imu.getGyro(&gyro);

      unsigned long now = millis();

      bool okPressed = digitalRead(tOk) == HIGH;

      if (okPressed && !okWasPressed) {
          okPressedSince = now;
      }

      if (okPressed && (now - okPressedSince >= HOLD_MS)) {
          timerRunning = false;
          timerAccum = 0;
          rowingTimerMs = 0;
          spm = 0.0f;
          spmSum = 0.0f;
          spmCount = 0;
          avgSpm = 0.0f;
          lastStrokeTime = 0;
          aboveThreshold = false;
      } else if (!okPressed && okWasPressed) {
          if (now - okPressedSince < HOLD_MS) {
              if (!timerRunning) {
                  rowingTimerMs = now;
                  timerRunning = true;
              } else {
                  timerAccum += now - rowingTimerMs;
                  timerRunning = false;
              }
          }
      }

      okWasPressed = okPressed;

      float g = maxAxis(gyro);

      if (!aboveThreshold && g > GYRO_THRESHOLD) {
          aboveThreshold = true;
          unsigned long dt = now - lastStrokeTime;
          if (lastStrokeTime != 0 && dt >= MIN_STROKE_MS) {
              spm = 60000.0f / dt;
              spmSum += spm;
              spmCount++;
              avgSpm = spmSum / spmCount;
          }
          lastStrokeTime = now;
      } else if (aboveThreshold && g < GYRO_RESET) {
          aboveThreshold = false;
      }

      if (lastStrokeTime != 0 && (now - lastStrokeTime) > SPM_TIMEOUT_MS) {
          spm = 0.0f;
          lastStrokeTime = 0;
      }

      unsigned long elapsed = timerAccum + (timerRunning ? (now - rowingTimerMs) : 0);
      unsigned int  mins = elapsed / 60000;
      unsigned int  secs = (elapsed % 60000) / 1000;
      unsigned int  ms   = (elapsed % 1000) / 10;

      oled.clear();
      oled.drawBitmap(0, 0, epd_bitmap, 128, 64);

      oled.setScale(1);
      oled.setCursor(86, 1);
      if (mins < 10) oled.print('0');
      oled.print(mins);
      oled.print(':');
      if (secs < 10) oled.print('0');
      oled.print(secs);
      oled.print('.');
      oled.print(ms / 10);

      oled.setCursor(0, 0);
      if (gnss.time.isValid()) {
        char timeBuf[9];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d",
          gnss.time.hour(), gnss.time.minute(), gnss.time.second());
        oled.print(timeBuf);
      } else {
        oled.print("--:--:--");
      }

      oled.setScale(2);
      oled.setCursor(2, 3);
      oled.print((int)spm);
      oled.print("spm");

      oled.setCursor(2, 6);
      oled.print((int)avgSpm);
      oled.print("avg");
      
      oled.setScale(1);
      String split = calcSplit(gnss.speed.kmph());
      oled.setCursor(75, 16);
      oled.print(split);

      oled.update();
      if (digitalRead(tDown) == 1) ui_mode = 1;

      break;
      }
    case 1:{
      oled.setScale(2);
      for (int i = 0; i < 4; i++){
        if (i == selected_index) oled.invertText(1);
        else oled.invertText(0);
        oled.println(options[i]);
      }
      if (digitalRead(tDown) == 1 && selected_index < 3) selected_index ++;
      if (digitalRead(tUp) == 1 && selected_index > 0) selected_index --;
      if (digitalRead(tOk) == 1){
        switch (selected_index){
          case 0:
            ui_mode = 0;
            break;
          case 1:
            ui_mode = 2;
            break;
          case 2:
            ui_mode = 3;
            break;
          case 3:
            ui_mode = 4;
            break;
        }
      }
      break;
    }
    case 4: {
      oled.setScale(1);
      oled.invertText(0);
      oled.println("RowWell-V1");
      oled.println("FW version 0.6");
      oled.println("By Anton Industries");

      char satBuf[14];
      snprintf(satBuf, sizeof(satBuf), "Sats: %d", (int)gnss.satellites.value());
      oled.println(satBuf);

      if (gnss.time.isValid()) {
        char timeBuf[12];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d GMT ",
          gnss.time.hour(), gnss.time.minute(), gnss.time.second());
        oled.println(timeBuf);
      } else {
        oled.println("Time: --:--:--");
      }

      if (gnss.location.isValid()) {
        char coordBuf[22];
        snprintf(coordBuf, sizeof(coordBuf), "%.5f,%.5f",
          gnss.location.lat(), gnss.location.lng());
        oled.println(coordBuf);
      } else {
        oled.println("No fix");
      }
      oled.println("Radio deactivated");
      oled.println("DOWN to exit");
      if (digitalRead(tDown) == 1) ui_mode = 1;
      oled.setScale(2);
      break;
    }
    default:{
        oled.setScale(1);
        oled.invertText(0);
        oled.println("Not implemented yet");
        oled.println("Press DOWN to exit");
        if (digitalRead(tDown) == 1) ui_mode = 1;
        oled.setScale(2);
        break;
    }
  }
  oled.update();
}