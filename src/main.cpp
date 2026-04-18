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

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite line = TFT_eSprite(&tft);


int a = 40;
int ys = 119;
int xs = 119;
int x = xs;
int y = xs;

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_WHITE);
    
    background.createSprite(238,238);
    background.setSwapBytes(true);
    background.pushImage(0,0,238,238,ozadje);
    //background.fillScreen(TFT_BLACK);
    
    line.createSprite(160,160);
    line.setSwapBytes(true);
    line.pushImage(0,0,160,160,poz11);
    
}
int getCycle(){
    

    if(345 <= a && a <= 15){
        return 0; //aka indeks od naravnost desno
    }

    if(15 <= a && a <= 45){
        return 1; //aka indeks od desno malo dol
    }
 
    if(45 <= a && a <= 75){
        return 2; //aka indeks od desno dol kr
    }

    if(75 <= a && a <= 105){
        return 3; //aka indeks od dol
    }

    if(105 <= a && a <= 135){
        return 4; 
    }

    if(135 <= a && a <= 165){
        return 5; //aka indeks od naravnost desno
    }

    if(165 <= a && a <= 195){
        return 6; //aka indeks od naravnost desno
    }

    if(195 <= a && a <= 225){
        return 7; //aka indeks od desno malo dol
    }
 
    if(225 <= a && a <= 255){
        return 8; //aka indeks od desno dol kr
    }

    if(255 <= a && a <= 285){
        return 9; //aka indeks od dol
    }

    if(285 <= a && a <= 315){
        return 10;
    }

    if(315 <= a && a <= 345){
        return 11;
    }
    return 0;
}

void draw(int n){
    a = a + 1;
    a = a%360;
    switch(n){
        case 0: 
        line.pushImage(0,0,160,160,poz0);
        line.pushSprite(0,0); 
        break;
        case 1: 
        line.pushImage(0,0,160,160,poz1);
        line.pushSprite(0,0); 
        break;
        case 2: 
        line.pushImage(0,0,160,160,poz2);
        line.pushSprite(0,0); 
        break;
        case 3: 
        line.pushImage(0,0,160,160,poz3);
        line.pushSprite(0,0); 
        break;
        case 4: 
        line.pushImage(0,0,160,160,poz4);
        line.pushSprite(0,0); 
        break;
        case 5: 
        line.pushImage(0,0,160,160,poz5);
        line.pushSprite(0,0); 
        break;
        case 6: 
        line.pushImage(0,0,160,160,poz6);
        line.pushSprite(0,0); 
        break;
        case 7: 
        line.pushImage(0,0,160,160,poz7);
        line.pushSprite(0,0); 
        break;
        case 8: 
        line.pushImage(0,0,160,160,poz8);
        line.pushSprite(0,0); 
        break;
        case 9: 
        line.pushImage(0,0,160,160,poz9);
        line.pushSprite(0,0); 
        break;
        case 10: 
        line.pushImage(0,0,160,160,poz10);
        line.pushSprite(0,0); 
        break;
        case 11: 
        line.pushImage(0,0,160,160,poz11);
        line.pushSprite(0,0); 
        break;
    }
}

void loop() {
    
    draw(getCycle());

    x = xs + cos((float)a * 3.14 /180 )*109;
    y = ys + sin((float)a * 3.14 /180 )*109;
    
}