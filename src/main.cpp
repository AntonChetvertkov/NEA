#include <HardwareSerial.h>
#include <TinyGPS++.h>

#define GPS_RX 15
#define GPS_TX 14
//test
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    Serial.println("NEO-7M GPS Test");
    Serial.println("---------------");
}

void loop() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    static unsigned long last = 0;
    if (millis() - last > 1000) {
        last = millis();

        Serial.printf("Satellites: %d\n", gps.satellites.value());
        Serial.printf("Fix valid:  %s\n", gps.location.isValid() ? "YES" : "NO");

        if (gps.location.isValid()) {
            Serial.printf("Lat:   %.6f\n", gps.location.lat());
            Serial.printf("Lng:   %.6f\n", gps.location.lng());
            Serial.printf("Alt:   %.2f m\n", gps.altitude.meters());
            Serial.printf("Speed: %.2f km/h\n", gps.speed.kmph());
            Serial.printf("HDOP:  %.2f\n", gps.hdop.hdop());
        } else {
            Serial.println("Waiting for fix...");
        }
        Serial.println("---------------");
    }
}