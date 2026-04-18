#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ozadjeV2.h"
#include "slika.h"
#include "slika0.h"
#include "slika1.h"
#include "slika2.h"
#include "slika3.h"
#include "slika4.h"
#include "slika5.h"
#include "slika6.h"
#include "slika7.h"
#include "slika8.h"
#include "slika9.h"
#include "slika10.h"
#include "slika11.h"
#include "backg.h"
#include "backgsmall.h"
#include "gps.h"

#include "compass.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite backgroundS = TFT_eSprite(&tft);
TFT_eSprite line = TFT_eSprite(&tft);

static GpsService *gps = nullptr;

int d = 40;
int ys = 119;
int xs = 119;
int x = xs;
int y = xs;
float degree;

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    
    background.createSprite(238,238);
    background.setSwapBytes(true);
    background.pushImage(0,0,238,238,back);
    //background.fillScreen(TFT_BLACK);
    
    backgroundS.createSprite(82,240);
    backgroundS.setSwapBytes(true);
    backgroundS.pushImage(0,0,82,240,backs);

    line.createSprite(160,160);
    line.setSwapBytes(true);
    line.pushImage(0,0,160,160,poz11);
    
    gps = &gpsSetup();

    Serial.begin(9600);
}
int getCycle(){
    

    if(345 <= d && d <= 15){
        return 0;
    }

    if(15 <= d && d <= 45){
        return 1;
    }
 
    if(45 <= d && d <= 75){
        return 2; 
    }

    if(75 <= d && d <= 105){
        return 3;
    }

    if(105 <= d && d <= 135){
        return 4; 
    }

    if(135 <= d && d <= 165){
        return 5;
    }

    if(165 <= d && d <= 195){
        return 6;
    }

    if(195 <= d && d <= 225){
        return 7; 
    }
 
    if(225 <= d && d <= 255){
        return 8;
    }

    if(255 <= d && d <= 285){
        return 9; //aka indeks od dol
    }

    if(285 <= d && d <= 315){
        return 10;
    }

    if(315 <= d && d <= 345){
        return 11;
    }
    return 0;
}

void draw(int n){
    switch(n){
        case 0: 
        line.pushImage(0,0,160,160,poz0);
        line.pushToSprite(&background,39,38); 
        break;
        case 1: 
        line.pushImage(0,0,160,160,poz1);
        line.pushToSprite(&background,39,38); 
        break;
        case 2: 
        line.pushImage(0,0,160,160,poz2);
        line.pushToSprite(&background,39,38); 
        break;
        case 3: 
        line.pushImage(0,0,160,160,poz3);
        line.pushToSprite(&background,39,38); 
        break;
        case 4: 
        line.pushImage(0,0,160,160,poz4);
        line.pushToSprite(&background,39,38); 
        break;
        case 5: 
        line.pushImage(0,0,160,160,poz5);
        line.pushToSprite(&background,39,38); 
        break;
        case 6: 
        line.pushImage(0,0,160,160,poz6);
        line.pushToSprite(&background,39,38); 
        break;
        case 7: 
        line.pushImage(0,0,160,160,poz7);
        line.pushToSprite(&background,39,38); 
        break;
        case 8: 
        line.pushImage(0,0,160,160,poz8);
        line.pushToSprite(&background,39,38); 
        break;
        case 9: 
        line.pushImage(0,0,160,160,poz9);
        line.pushToSprite(&background,39,38); 
        break;
        case 10: 
        line.pushImage(0,0,160,160,poz10);
        line.pushToSprite(&background,39,38); 
        break;
        case 11: 
        line.pushImage(0,0,160,160,poz11);
        line.pushToSprite(&background,39,38); 
        break;
    }
}

void loop() {

    if (gps == nullptr) {
		return;
    }
    
    draw(getCycle());
    background.pushSprite(1,1);
    backgroundS.pushSprite(239,0);

    //x = xs + cos((float)a * 3.14 /180 )*109;
    //y = ys + sin((float)a * 3.14 /180 )*109;
    
     d = 360 - compassHeadingDeg(); 
    
    static unsigned long lastReportMs = 0;
	unsigned long now = millis();

	if (now - lastReportMs >= 1000) {
		lastReportMs = now;
		gps->printStatus();
	}
}