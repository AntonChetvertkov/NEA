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
    oled.clear();
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    oled.println("NEO-7M GPS Test");
    oled.println("---------------");
    oled.update();
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

        oled.println("Satellites: ", gps.satellites.value());
        oled.println("Fix valid: ", gps.location.isValid() ? "YES" : "NO");

        if (gps.location.isValid()) {
            oled.println("Lat: ", gps.location.lat());
            oled.println("Lng: ", gps.location.lng());
            oled.println("Alt: ", gps.altitude.meters());
            oled.println("Speed: ", gps.speed.kmph());
            oled.println("HDOP: ", gps.hdop.hdop());
        } else {
            oled.println("Waiting for fix...");
        }
        oled.println("---------------");
        oled.update();
    }
}