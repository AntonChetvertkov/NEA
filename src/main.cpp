#include <GyverOLED.h>

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

int ui_mode = 0; //0-just row; 1 menu; 2 - workout; 3 - log; 4 - About
byte selected_index = 0;
String options[4] = {" Just Row", " Workout", " Log", " About"};

#define tUp 26
#define tOk 25
#define tDown 33

void setup() {
  // put your setup code here, to run once:
  oled.init();
  oled.setScale(2);

  pinMode(26, INPUT);
  pinMode(25, INPUT);
  pinMode(33, INPUT);
}

void loop() {
  oled.clear();
  oled.home();
  
  for (int i = 0; i < 4; i++){
    if (i == selected_index) oled.invertText(1);
    else oled.invertText(0);
    oled.println(options[i]);
  }
  oled.update();
  if (digitalRead(tDown) == 1 && selected_index < 3) selected_index ++;
  if (digitalRead(tUp) == 1 && selected_index > 0) selected_index --;
}
