#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ozadjeV2.h"
#include "headerZaSlike.h"
#include "gps.h"

#include "compass.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite status = TFT_eSprite(&tft);
TFT_eSprite backgroundS = TFT_eSprite(&tft);
TFT_eSprite line = TFT_eSprite(&tft);
TFT_eSprite ura = TFT_eSprite(&tft);

static GpsService *gps = nullptr;

static constexpr int kSaveButtonPin = 25;
static constexpr int kBuiltinLedPin = 2;
static constexpr unsigned long kDebounceMs = 40;
static constexpr unsigned long kReportPeriodMs = 300;
static constexpr int kButtonPressedLevel = HIGH;
static constexpr double kDistanceAlertThresholdM = 10.0;

int d = 40;
int ys = 119;
int xs = 119;
int x = xs;
int y = xs;
float degree;
int cas = 0;
bool compas = true;

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    
    background.createSprite(238,238);
    background.setSwapBytes(true);
    background.pushImage(0,0,238,238,back);
    
    backgroundS.createSprite(82,240);
    backgroundS.setSwapBytes(true);
    backgroundS.setPivot(30,70);
    backgroundS.pushImage(0,0,82,240,backs);

    pinMode(kSaveButtonPin, INPUT);
    pinMode(kBuiltinLedPin, OUTPUT);
    digitalWrite(kBuiltinLedPin, LOW);

    line.createSprite(160,160);
    line.setSwapBytes(true);
    line.pushImage(0,0,160,160,poz11);
    
    ura.createSprite(100,50);
    ura.setSwapBytes(true);
    ura.setRotation(1);
    ura.setTextSize(3);
    ura.setTextColor(0x404a8b, TFT_BLACK);

    gps = &gpsSetup();

    Serial.begin(9600);

    backgroundS.pushSprite(239,0);
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

struct SavedCoords {
    bool valid;
    double latitude;
    double longitude;
};

static SavedCoords saved = {
    .valid = false,
    .latitude = 0.0,
    .longitude = 0.0,
};

const char *cardinalFromAngleDeg(double angleDeg) {
    if (angleDeg >= 45.0 && angleDeg < 135.0) {
        return "north";
    }
    if (angleDeg >= 135.0 && angleDeg < 225.0) {
        return "west";
    }
    if (angleDeg >= 225.0 && angleDeg < 315.0) {
        return "south";
    }
    return "east";
}

bool checkButtonPressedDebounced() {
    static int lastReading = HIGH;
    static int stableState = HIGH;
    static unsigned long lastChangeMs = 0;

    const int reading = digitalRead(kSaveButtonPin);
    const unsigned long now = millis();

    if (reading != lastReading) {
        lastChangeMs = now;
        lastReading = reading;
    }

    if ((now - lastChangeMs) >= kDebounceMs && reading != stableState) {
        stableState = reading;
        if (stableState == kButtonPressedLevel) {
            return true;
        }
    }

    return false;
}

void saveCurrentGpsCoords() {
    if (gps == nullptr || !gps->data.hasFix) {
        Serial.println("Save failed: no GPS fix.");
        return;
    }

    saved.valid = true;
    saved.latitude = gps->data.latitude;
    saved.longitude = gps->data.longitude;
    Serial.printf("Saved coords in RAM: lat=%.8f lon=%.8f\n", saved.latitude, saved.longitude);
}

void loop() {

    if (gps == nullptr) {
		return;
    }
    
    draw(getCycle());
   background.pushSprite(1,1);
   
   int gps_time = (2+gps->data.hour) * 60 + gps->data.minute;
   //Serial.println(gps_time);
   if(cas != gps_time){
       cas = gps_time;
        backgroundS.fillScreen(TFT_BLACK);
        backgroundS.pushImage(0,0,82,240,backs);
        
        ura.drawString(String(2+gps->data.hour)+":"+String(gps->data.minute), 0, TFT_BLACK);
        ura.pushRotated(&backgroundS, 90, TFT_BLACK);
        
        backgroundS.pushSprite(238, 1);
        
    
   }

    //status.pushSprite(1,1);

    if(compas) d = 360 - compassHeadingDeg(); 
    static unsigned long lastReportMs = 0;
	unsigned long now = millis();

    if (checkButtonPressedDebounced()) {
		saveCurrentGpsCoords();
	}

	if (now - lastReportMs < kReportPeriodMs) {
		return;
	}
	lastReportMs = now;

	const double currentAngleDeg = static_cast<double>(compassHeadingDeg());
    /*
    Serial.print("current angle: ");
    if (isnan(currentAngleDeg)) {
        Serial.println("n/a");
    } else {
        Serial.println(currentAngleDeg, 1);
    }
    */

        Serial.print("satellites: ");
        Serial.println(gps->data.satellites);

        Serial.print("current lat/lon: ");
        if (gps->data.hasFix) {
            Serial.print(gps->data.latitude, 8);
            Serial.print(", ");
            Serial.println(gps->data.longitude, 8);
        } else {
            Serial.println("no fix");
        }

    /*    Serial.print("saved lat/lon: ");
        if (saved.valid) {
            Serial.print(saved.latitude, 8);
            Serial.print(", ");
            Serial.println(saved.longitude, 8);
        } else {
            Serial.println("not saved");
        }
    */
    if (!saved.valid || !gps->data.hasFix) {
        digitalWrite(kBuiltinLedPin, LOW);
        Serial.println("distance: n/a");
        Serial.println("cardinal direction: n/a");
        Serial.println("relative angle: n/a");
        Serial.println();
        return;
    }

    const double angleDeg = gps->angleToDegrees(saved.latitude, saved.longitude);
    const double distanceM = gps->distanceToMeters(saved.latitude, saved.longitude);
    const double relativeAngleDeg = gps->relativeAngleToDegrees(saved.latitude, saved.longitude);

    //    Serial.print("distance: ");
    if (isnan(distanceM)) {
    //      Serial.println("n/a");
        digitalWrite(kBuiltinLedPin, LOW);
    //    Serial.println("cardinal direction: n/a");
    } else {
    //       Serial.print(distanceM, 2);
    //     Serial.println(" m");

        const bool alert = distanceM > kDistanceAlertThresholdM;
        digitalWrite(kBuiltinLedPin, alert ? HIGH : LOW);

     /*   Serial.print("cardinal direction: ");
        if (alert && !isnan(angleDeg)) {
            Serial.println(cardinalFromAngleDeg(angleDeg));
        } else {
            Serial.println("within 10m");
        }
    */
    }

        //Serial.print("relative angle: ");
    if (isnan(relativeAngleDeg)) {
        Serial.println("n/a");
        compas = true;
    } else {
        Serial.println(relativeAngleDeg, 1);
        compas = false;
        d = 360 - relativeAngleDeg;
    }

	Serial.println();
    //--------
    /*
	Serial.print("current angle: ");
	if (isnan(currentAngleDeg)) {
		Serial.println("n/a");
	} else {
		Serial.println(currentAngleDeg, 1);
        gps->printStatus();
	}
        */
}