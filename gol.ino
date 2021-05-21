// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define btn 2

#define CELLSIZE 4
#define FRAME_WIDTH  (SCREEN_WIDTH/CELLSIZE)
#define FRAME_HEIGHT  (SCREEN_HEIGHT/CELLSIZE)
uint8_t frame[FRAME_WIDTH][FRAME_HEIGHT];

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void reset();
void countNeighbors();
void nextGen();
void draw();

void setup(){
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    
    pinMode(btn, INPUT_PULLUP);

    display.display();
    delay(1000);
    display.clearDisplay();

    reset();
    delay(500);
}

void loop(){
    if(digitalRead(btn) == LOW){
        reset();
    }

    display.clearDisplay();
    nextGen();
    draw();
    display.display();
    
    delay(50);
}

// ====================
void reset(){
    display.clearDisplay();
    for(uint8_t i = 0; i < FRAME_WIDTH; i++){
        for(uint8_t j = 0; j < FRAME_HEIGHT; j++){
            frame[i][j] = random(2);
        }
    }
    draw();
    display.display();
}
void draw(){
    for(uint8_t i = 0; i < FRAME_WIDTH; i++){
        for(uint8_t j = 0; j < FRAME_HEIGHT; j++){
            if(frame[i][j])
                display.fillRect(i*CELLSIZE, j*CELLSIZE, CELLSIZE, CELLSIZE, SSD1306_WHITE);
                // display.drawPixel(i, j, SSD1306_WHITE);
        }
    }
}
uint8_t countNeighbors(uint8_t x, uint8_t y){
    uint8_t sum = 0;
    for(int8_t i = -1; i < 2; i++){
        for(int8_t j = -1; j < 2; j++){
            if(i==0 && j==0){  continue; }
            sum += frame[(x+i+FRAME_WIDTH)%FRAME_WIDTH] [(y+j+FRAME_HEIGHT)%FRAME_HEIGHT];
        }
    }
    return sum;
}

void nextGen(){
    uint8_t nextFrame[FRAME_WIDTH][FRAME_HEIGHT];
    for(uint8_t i = 0; i < FRAME_WIDTH; i++){
        for(uint8_t j = 0; j < FRAME_HEIGHT; j++){
            uint8_t cnt = countNeighbors(i, j);

            if (frame[i][j] == 0 && cnt == 3) {
                nextFrame[i][j] = 1;
            } else if (frame[i][j] == 1 && (cnt < 2 || cnt > 3)) {
                nextFrame[i][j] = 0;
            } else {
                nextFrame[i][j] = frame[i][j];
            }
        }
    }
    for(uint8_t i = 0; i < FRAME_WIDTH; i++){
        for(uint8_t j = 0; j < FRAME_HEIGHT; j++){
            frame[i][j] = nextFrame[i][j];
        }
    }
}