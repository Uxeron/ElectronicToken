#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdio.h>

#define minDelay 0
#define maxDelay 20
#define deltaDelay 4

Adafruit_SSD1306 display(4);
unsigned long int counter = 200;
short delayTime = maxDelay;
short currDelay = 0;
short displayIndex = 2;
bool pressed4 = false;
bool pressed5 = false;


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  display.setTextColor(WHITE);

  displayCounter();
}


void loop() {
  if (currDelay > 0) {
    currDelay--;
  } else {
    if (!digitalRead(2)) {
      counter++;
      displayCounter();
      currDelay = delayTime;
      if (delayTime > minDelay) {
        delayTime -= deltaDelay;
      }
    } else if (!digitalRead(3)) {
      if (counter > 0) {
        counter--;
        displayCounter();
        currDelay = delayTime;
        if (delayTime > minDelay) {
          delayTime -= deltaDelay;
        }
      }
    } else {
      delayTime = maxDelay;
      currDelay = 0;
    }
  }
  if (!digitalRead(4)) {
    if (!pressed4) {
      pressed4 = true;
      counter *= 2;
      displayCounter();
    }
  } else {
    pressed4 = false;
  }
  if (!digitalRead(5)) {
    if (!pressed5) {
      pressed5 = true;
      counter = 0;
      displayCounter();
    }
  } else {
    pressed5 = false;
  }
  delay(10);
}

void displayCounter() {
  display.clearDisplay();
  //display.drawTriangle(22, 0, 20, 2, 24, 2, 1);
  //display.drawTriangle(22, 7, 20, 5, 24, 5, 1);
  display.setCursor(0, 0);
  display.setTextSize(1);
  //char c[11];
  //sprintf(c, "  Sick: %6lu", counter);
  display.print("  Sick:*");
  display.println(counter);

  display.setCursor(0, 8);
  display.println("Tapped:*100");

  display.setCursor(0, 16);
  display.println("Active:*2304");

  display.setCursor(0, 56);
  display.println("        *4050/4050");

  display.setCursor(98, 0);
  display.println("Other");
  
  display.setCursor(104, 8);
  display.println("*120");
  
  display.setCursor(80, 24);
  display.println("Flag [ ]");
  
  display.setCursor(0, 38);
  display.setTextSize(2);
  display.println(" Rat Token");
  display.display();
}

