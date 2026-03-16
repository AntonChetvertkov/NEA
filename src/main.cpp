#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <GyverOLED.h>

#define GPS_RX 15
#define GPS_TX 14

HardwareSerial gpsSerial(2);
TinyGPSPlus gps;
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

void setup() {
    Serial.begin(115200);
    oled.init();
    oled.home();
    oled.setContrast(255);
    // oled.clear();
    // gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    // oled.println("NEO-7M GPS Test");
    // oled.println("---------------");
    oled.println("ROWWELL-1-Starting up");
    oled.update();
    delay(10000);


}   

void loop() {
  oled.clear();
  oled.home();
    

  while (gpsSerial.available()) {
      gps.encode(gpsSerial.read());
  }

  static unsigned long last = 0;
  if (millis() - last > 1000) {
      last = millis();

      oled.println("Satellites: "+ String(gps.satellites.value()));
      oled.println("Fix valid: "+ String(gps.location.isValid() ? "YES" : "NO"));

      if (gps.location.isValid()) {
          oled.println("Lat: "+ String(gps.location.lat()));
          oled.println("Lng: "+ String(gps.location.lng()));
          oled.println("Alt: "+ String(gps.altitude.meters()));
          oled.println("Speed: "+ String(gps.speed.kmph()));
          oled.println("HDOP: "+ String(gps.hdop.hdop()));
      } else {
          oled.println("Waiting for fix...");
      }
      oled.println("---------------");
  }
  oled.update();
}