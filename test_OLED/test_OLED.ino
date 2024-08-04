#include <Wire.h>
#include <SeeedOLED.h>

void setup() {
  // Initialize the OLED display
  SeeedOled.init();  // Initialize OLED
  SeeedOled.clearDisplay();  // Clear the display
  SeeedOled.setNormalDisplay();  // Set to normal display (not inverted)
  SeeedOled.setPageMode();  // Set display to page mode
  
  // Display a test message
  SeeedOled.setTextXY(0, 0);  // Set cursor to row 0, column 0
  SeeedOled.putString("OLED Display Test");
  
  SeeedOled.setTextXY(2, 0);  // Move to row 2, column 0
  SeeedOled.putString("Arduino Connected!");
  
  SeeedOled.setTextXY(4, 0);  // Move to row 4, column 0
  SeeedOled.putString("Testing...");
}

void loop() {
  // You can add more dynamic tests here if needed
}
