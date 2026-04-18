#include "compass.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <math.h>

namespace {

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

static constexpr int kCompassSdaPin = 21;
static constexpr int kCompassSclPin = 19;
static constexpr float kStaleEpsilonUt = 0.15f;
static constexpr int kStaleSampleLimit = 20;

static constexpr float kMagneticDeclinationDeg = 0.0f;
static constexpr float kHeadingOffsetDeg = 90.0f;
static constexpr bool kSwapCompassAxes = false;
static constexpr bool kInvertCompassX = false;
static constexpr bool kInvertCompassY = false;

bool gInitialized = false;
bool gHasPrevious = false;
float gPreviousX = 0.0f;
float gPreviousY = 0.0f;
float gPreviousZ = 0.0f;
int gStaleCount = 0;

void printCalibrationStatus() {
	Serial.println("Compass calibration settings:");
	Serial.print("  SDA=");
	Serial.print(kCompassSdaPin);
	Serial.print(" SCL=");
	Serial.println(kCompassSclPin);
}

bool ensureCompassReady() {
	if (gInitialized) {
		return true;
	}

	Wire.begin(kCompassSdaPin, kCompassSclPin);

	if (!mag.begin()) {
		Serial.println("HMC5883L not found!");
		return false;
	}

	gInitialized = true;
	printCalibrationStatus();
	Serial.println("HMC5883L ready.");
	return true;
}

float computeHeadingDeg(float x, float y) {
	if (kSwapCompassAxes) {
		const float tmp = x;
		x = y;
		y = tmp;
	}

	if (kInvertCompassX) {
		x = -x;
	}

	if (kInvertCompassY) {
		y = -y;
	}

	float headingRad = atan2f(y, x);
	headingRad += (kMagneticDeclinationDeg + kHeadingOffsetDeg) * (PI / 180.0f);

	if (headingRad < 0.0f) {
		headingRad += 2.0f * PI;
	}
	if (headingRad >= 2.0f * PI) {
		headingRad -= 2.0f * PI;
	}

	return headingRad * (180.0f / PI);
}

void resetCompassState() {
	gInitialized = false;
	gHasPrevious = false;
	gPreviousX = 0.0f;
	gPreviousY = 0.0f;
	gPreviousZ = 0.0f;
	gStaleCount = 0;
}

}  // namespace

float compassHeadingDeg() {
	if (!ensureCompassReady()) {
		return NAN;
	}

	sensors_event_t event;
	mag.getEvent(&event);

	const float x = event.magnetic.x;
	const float y = event.magnetic.y;
	const float z = event.magnetic.z;

	if (gHasPrevious) {
		const bool staleX = fabsf(x - gPreviousX) < kStaleEpsilonUt;
		const bool staleY = fabsf(y - gPreviousY) < kStaleEpsilonUt;
		const bool staleZ = fabsf(z - gPreviousZ) < kStaleEpsilonUt;

		if (staleX && staleY && staleZ) {
			gStaleCount++;
		} else {
			gStaleCount = 0;
		}
	}

	gPreviousX = x;
	gPreviousY = y;
	gPreviousZ = z;
	gHasPrevious = true;

	if (gStaleCount >= kStaleSampleLimit) {
		Serial.println("Compass data looks stale, reinitializing...");
		resetCompassState();
		if (!ensureCompassReady()) {
			return NAN;
		}
		gStaleCount = 0;
	}

	return computeHeadingDeg(x, y);
}
