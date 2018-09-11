#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MathHelpers.h>
#include <math.h>

#define minDelay 0
#define maxDelay 5
#define deltaDelay 1

Adafruit_SSD1306 display(4);

// 0.sick, 1.tapped, 2.active, 3.other, 4.flag, 5.damage, 6.life
double counts[7] = {0, 0, 0, 0, 0, 1, 1};
//                               sick,   tapped,   active,   other,    flag,     damage,   life
byte positions[7][2] =         {{48, 0}, {48, 8}, {48, 16}, {98, 8}, {116, 24}, {26, 56}, {60, 56}};
byte SelectorPositions[7][2] = {{42, 0}, {42, 8}, {42, 16}, {92, 8}, {105, 24}, {26, 56}, {60, 56}};

byte index = 3;
byte delayTime = maxDelay;
byte currDelay = 0;

bool pressed2 = false;
bool pressed3 = false;
bool pressed6 = false;
bool pressed7 = false;
bool pressed8 = false;


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(2, INPUT_PULLUP); // Up
  pinMode(3, INPUT_PULLUP); // Down
  pinMode(4, INPUT_PULLUP); // Increase
  pinMode(5, INPUT_PULLUP); // Decrease
  pinMode(6, INPUT_PULLUP); // New Turn
  pinMode(7, INPUT_PULLUP); // Double
  pinMode(8, INPUT_PULLUP); // Wipe

  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.setTextWrap(false);

  drawLabels();
}


void loop() {
    if (currDelay > 0) {
        currDelay--;
    } else {
        if (!digitalRead(4)) {
            if (index != 4) {
                counts[index]++;
                if (index == 2 and counts[1] > 0) {
                    counts[1]--;
                    index = 1;
                    writeCounts();
                    index = 2;
                }

                if (index == 1 and counts[2] > 0) {
                    counts[2]--;
                    index = 2;
                    writeCounts();
                    index = 1;
                }

                writeCounts();
                writeDL();
            } else {
                counts[index] = !counts[index];
                writeFlag();
            }
            currDelay = delayTime;
            if (delayTime > minDelay) {
                delayTime -= deltaDelay;
            }
            display.display();
            return;
        } else if (!digitalRead(5)) {
            if (index != 4) {
                if (counts[index] > 0) {
                    counts[index]--;
                    writeCounts();
                    writeDL();
                }
            } else {
                counts[index] = !counts[index];
                writeFlag();
            }
            currDelay = delayTime;
            if (delayTime > minDelay) {
                delayTime -= deltaDelay;
            }
            display.display();
            return;
        } else {
            delayTime = maxDelay;
            currDelay = 0;
        }
    }
    if (!digitalRead(2)) {
        if (!pressed2) {
            pressed2 = true;
            if (index == 0)
                index = 6;
            else
                index--;
            drawSelector();
        }
    } else {
        pressed2 = false;
    }
    if (!digitalRead(3)) {
        if (!pressed3) {
            pressed3 = true;
            if (index == 6)
                index = 0;
            else
                index++;
            drawSelector();
        }
    } else {
        pressed3 = false;
    }
    if (!digitalRead(6)) {
        if (!pressed6) {
            pressed6 = true;
            counts[2] += counts[0] + counts[1];
            counts[0] = 0;
            counts[1] = 0;
            writeAllCounts();
        }
    } else {
        pressed6 = false;
    }
    if (!digitalRead(7)) {
        if (!pressed7) {
            double all;
            pressed7 = true;

            if (counts[0] > 0)
                counts[0]--;
            else if (counts[1] > 0)
                counts[1]--;
            else if (counts[2] > 0)
                counts[2]--;
            else if (counts[3] > 0)
                counts[3]--;
            else
                return;

            all = counts[0] + counts[1] + counts[2] + counts[3];
            if (index < 3)
                counts[index] += all;
            else
                counts[0] += all;

            writeAllCounts();
        }
    } else {
        pressed7 = false;
    }
    if (!digitalRead(8)) {
        if (!pressed8) {
            pressed8 = true;
            for(byte i = 0; i < 4; i++)
                counts[i] = 0;
            writeAllCounts();
        }
    } else {
        pressed8 = false;
    }
    display.display();
}


void drawLabels() {
    display.clearDisplay();

    display.setCursor(0, 0);
    display.print("  Sick: 0");

    display.setCursor(0, 8);
    display.println("Tapped: 0");

    display.setCursor(0, 16);
    display.println("Active: 0");

    display.setCursor(98, 0);
    display.println("Other");

    display.setCursor(98, 8);
    display.println("0");

    display.setCursor(80, 24);
    display.println("Flag [ ]");

    display.setCursor(12, 38);
    display.setTextSize(2);
    display.println("Rat Token");
    display.setTextSize(1);

    display.setCursor(60, 56);
    display.println("1/1");

    drawSelector();
    display.display();
}


void writeCounts() {
    display.setCursor(positions[index][0], positions[index][1]);
    display.print("       ");
    display.setCursor(positions[index][0], positions[index][1]);
    print(counts[index]);
}


void writeAllCounts() {
    byte save = index;
    for(byte i = 0; i < 4; i++) {
        index = i;
        writeCounts();
    }
    index = save;
    writeDL();
}


void writeFlag() {
    display.setCursor(positions[4][0], positions[4][1]);
    if (counts[4]) display.print("X");
    else      display.print(" ");
    writeDL();
}


void writeDL() {
    double sum;
    if (counts[4])
        sum = counts[0] + counts[1] + counts[2] + counts[3];
    else
        sum = 0;

    display.setCursor(positions[5][0], positions[5][1]);

    if (sum < 999999.0) {
        byte len = 6 - (byte) floor(log10(sum + counts[5]));
        for (byte i = 0; i < len; i++) {
            display.print(" ");
        }
    }

    if (isinf(sum)) display.print("    ");

    if (index == 5)
        display.print("*");
    else
        display.print(" ");

    print(sum + counts[5]);
    display.print("/");
    print(sum + counts[6]);
    if (index == 6)
        display.print("*");
    else
        display.print(" ");
    display.print("             ");
}

void drawSelector() {
    byte ind;

    ind = !index ? 6 : index - 1;
    display.setCursor(SelectorPositions[ind][0], SelectorPositions[ind][1]);
    display.print(" ");

    ind = index;
    display.setCursor(SelectorPositions[ind][0], SelectorPositions[ind][1]);
    display.print("*");

    ind = index == 6 ? 0 : index + 1;
    display.setCursor(SelectorPositions[ind][0], SelectorPositions[ind][1]);
    display.print(" ");
    writeDL();
}

void print(double number) {
    if (number > 999999.0)
        display.print(sci(number, 1));
    else
        display.print(number, 0);
}
