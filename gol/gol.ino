// OLED MODULE TIPS
// GND -> GND
// VCC -> 5V
// SCL -> A5
// SDA -> A4

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BitArray.h>

// OLED displayer information START
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// OLED displayer information END

// pin map setting START
#define btn 2
// pin map setting END

// system setting START
#define CELLSIZE 2                             // 1 for 1x1 pixel per cell
#define FRAME_WIDTH  (SCREEN_WIDTH/CELLSIZE)   // number of cells in the width
#define FRAME_HEIGHT (SCREEN_HEIGHT/CELLSIZE)  // number of cells in the height
// system setting END

// global variable START
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BitArray*        Frame = new BitArray();  // (1, FRAME_WIDTH * FRAME_HEIGHT);
// global variable END

// reference of all funcitons START
void reset();
void nextGeneration();
void draw();
void renewFrame(BitArray*);
uint8_t countNeighbors(uint8_t x, uint8_t y);
// reference of all funcitons END


void setup() {
  // Serial init
  Serial.begin(115200);
  Serial.println("Start");

  // OLED Displayer setting init
  if(!Display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  Serial.println("pin");
  // pinMap init
  pinMode(btn, INPUT_PULLUP);

  Serial.println("bitArray begin");
  // First BitArray init
  int ret = (*Frame).begin(1, FRAME_WIDTH * FRAME_HEIGHT);

  Serial.println("display");
  // display()
  // draw somthing special???
  Display.display();
  delay(1000);

  Serial.println("reset");
  // reset()
  reset();
  Display.display();
  delay(500);

  Serial.println("end of setup");
}

void loop() { 
  // readButton
  if(digitalRead(btn) == LOW){
    reset();
  }else {
    // update: 
    //   nextGeneration()
    nextGeneration();
  }

  //   draw()
  Display.clearDisplay();
  draw();

  // display()
  Display.display();
  // delay()
  // delay(5);
}

/**
 * make a random new frame then set to Frame */
void reset() {
    BitArray* tmp = new BitArray();  (*tmp).begin(1, FRAME_WIDTH * FRAME_HEIGHT);
    for(uint8_t i = 0; i < FRAME_HEIGHT; i++) {
        for(uint8_t j = 0; j < FRAME_WIDTH; j++) {
            (*tmp).set(i*FRAME_WIDTH + j, random(2));
        }
    }
    renewFrame(tmp);
}

/**
 * make next generation frame then replace Frame */
void nextGeneration() {
    BitArray* tmp = new BitArray();  (*tmp).begin(1, FRAME_WIDTH * FRAME_HEIGHT);
    for(uint8_t i = 0; i < FRAME_HEIGHT; i++) {
        for(uint8_t j = 0; j < FRAME_WIDTH; j++) {
            uint8_t cnt = countNeighbors(j, i);
            uint8_t p = (*Frame).get(i*FRAME_WIDTH + j);
            if(p == 0 && cnt == 3) {
                (*tmp).set(i*FRAME_WIDTH + j, 1);
            }else if(p == 1 && (cnt < 2 || cnt > 3)) {
                (*tmp).set(i*FRAME_WIDTH + j, 0);
            }else {
                (*tmp).set(i*FRAME_WIDTH + j, p);
            }
        }
    }
    renewFrame(tmp);
}

/**
 * draw the OLED displayer from Frame */
void draw() {
    for(uint8_t i = 0; i < FRAME_HEIGHT; i++) {
        for(uint8_t j = 0; j < FRAME_WIDTH; j++) {
            if((*Frame).get(i*FRAME_WIDTH + j) ==  1) {
                Display.fillRect(j*CELLSIZE, i*CELLSIZE, CELLSIZE, CELLSIZE, SSD1306_WHITE);  // change to drawRect
            }
        }
    }
}

/**
 * delete old Frame and set a new one to Frame */
void renewFrame(BitArray* newFrame){
    delete Frame;
    Frame = newFrame;
}

uint8_t countNeighbors(uint8_t x, uint8_t y) {
    uint8_t cnt = 0;
    for(int8_t i = -1; i < 2; i++) {
        for(int8_t j = -1; j < 2; j++) {
            if(i == 0 && j == 0) {
              continue;
            }
            cnt += (*Frame).get((y+j+FRAME_HEIGHT)%FRAME_HEIGHT * FRAME_WIDTH + (x+i+FRAME_WIDTH)%FRAME_WIDTH);
        }
    }
    return cnt;
}
