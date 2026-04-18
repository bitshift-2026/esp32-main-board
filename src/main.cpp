#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ozadjeV2.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite rect = TFT_eSprite(&tft);

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLUE);
    
    background.createSprite(238,238);
    background.setSwapBytes(true);
    background.pushImage(0,0,238,238,ozadje);
    //background.fillScreen(TFT_BLACK);
    background.pushSprite(0,0);

}

int a = 0;

int ys = 119;
int xs = 119;

int x = xs;
int y = xs;

void loop() {
    delay(10);
    background.pushSprite(0,0);
    a++;
    x = xs + cos((float)a * 3.14 /180 )*109;
    y = ys + sin((float)a * 3.14 /180 )*109;
    tft.drawLine(x,y,119,119,TFT_RED);
}