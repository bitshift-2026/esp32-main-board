#include "gps.h"

#include "compass.h"

#include <cmath>
#include <TinyGPS++.h>

/* MAIN EXAMPLE:
#include <Arduino.h>

#include "gps.h"

static GpsService *gps = nullptr;

void setup() {
	gps = &gpsSetup();
}

void loop() {
	if (gps == nullptr) {
		return;
	}

	static unsigned long lastReportMs = 0;
	unsigned long now = millis();

	if (now - lastReportMs >= 1000) {
		lastReportMs = now;
		gps->printStatus();
	}
}
*/

namespace {

static constexpr GpsPinConfig kDefaultPins = {
	.rxPin = 16,
	.txPin = 17,
	.baud = 9600,
};

static constexpr double kRelativeAngleMinDistanceM = 10.0;

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

void gpsTask(void *parameter) {
	auto *service = static_cast<GpsService *>(parameter);

	for (;;) {
		service->updateFromModule();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

}  // namespace

GpsService::GpsService() : data{}, taskHandle_(nullptr) {}

void GpsService::begin(const GpsPinConfig &pins) {
	Serial.begin(115200);

	gpsSerial.begin(pins.baud, SERIAL_8N1, pins.rxPin, pins.txPin);

	if (taskHandle_ == nullptr) {
		xTaskCreatePinnedToCore(gpsTask, "gps", 4096, this, 1, &taskHandle_, 1);
	}
}

void GpsService::updateFromModule() {
	while (gpsSerial.available() > 0) {
		gps.encode(gpsSerial.read());
	}

	data.dataSeen = gps.charsProcessed() > 0;
	data.hasFix = gps.location.isValid();
	data.hasTimeDate = gps.time.isValid() && gps.date.isValid();
	data.latitude = gps.location.lat();
	data.longitude = gps.location.lng();
	data.altitudeM = gps.altitude.meters();
	data.speedKmph = gps.speed.kmph();
	data.satellites = gps.satellites.value();
	data.year = gps.date.year();
	data.month = gps.date.month();
	data.day = gps.date.day();
	data.hour = gps.time.hour();
	data.minute = gps.time.minute();
	data.second = gps.time.second();
	data.charsProcessed = gps.charsProcessed();
}

void GpsService::printStatus() const {
	if (data.satellites == 0) {
		Serial.println("No satellites");
		return;
	}

	Serial.printf(
		"GPS %s | sats:%lu | chars:%lu\n",
		data.hasFix ? "fix" : "search",
		data.satellites,
		data.charsProcessed
	);

	if (data.hasFix) {
		Serial.printf("  lat/lon: %.6f, %.6f\n", data.latitude, data.longitude);
	}

	if (data.hasTimeDate) {
		Serial.printf(
			"  utc: %04d-%02d-%02d %02d:%02d:%02d\n",
			data.year,
			data.month,
			data.day,
			data.hour,
			data.minute,
			data.second
		);
	}
}

double GpsService::distanceToMeters(double latitude, double longitude) const {
	if (!data.hasFix) {
		return NAN;
	}

	static constexpr double kPi = 3.14159265358979323846;
	static constexpr double kDegToRad = kPi / 180.0;
	static constexpr double kMetersPerDegLat = 111320.0;

	const double avgLatRad = (data.latitude + latitude) * 0.5 * kDegToRad;
	const double dxMeters = (longitude - data.longitude) * kMetersPerDegLat * cos(avgLatRad);
	const double dyMeters = (latitude - data.latitude) * kMetersPerDegLat;

	return sqrt(dxMeters * dxMeters + dyMeters * dyMeters);
}

/// @return Angle in degrees from current GPS location to the given latitude/longitude, or NAN if no GPS fix.
/// The angle is in the range [0, 360),
/// where 0 means "to the right" (east),
/// 90 means "up" (north),
/// 180 means "to the left" (west),
/// and 270 means "down" (south).
double GpsService::angleToDegrees(double latitude, double longitude) const {
	if (!data.hasFix) {
		return NAN;
	}

	static constexpr double kPi = 3.14159265358979323846;
	static constexpr double kDegToRad = kPi / 180.0;
	static constexpr double kRadToDeg = 180.0 / kPi;
	static constexpr double kMetersPerDegLat = 111320.0;

	const double avgLatRad = (data.latitude + latitude) * 0.5 * kDegToRad;
	const double dxMeters = (longitude - data.longitude) * kMetersPerDegLat * cos(avgLatRad);
	const double dyMeters = (latitude - data.latitude) * kMetersPerDegLat;

	double angleDeg = atan2(dyMeters, dxMeters) * kRadToDeg;
	if (angleDeg < 0.0) {
		angleDeg += 360.0;
	}

	return angleDeg;
}

double GpsService::relativeAngleToDegrees(double latitude, double longitude) const {
	const double distanceM = distanceToMeters(latitude, longitude);
	if (isnan(distanceM) || distanceM < kRelativeAngleMinDistanceM) {
		return NAN;
	}

	const double worldAngleDeg = angleToDegrees(latitude, longitude);
	if (isnan(worldAngleDeg)) {
		return NAN;
	}

	const double headingDeg = static_cast<double>(compassHeadingDeg());
	if (isnan(headingDeg)) {
		return NAN;
	}

	// Relative angle: 0째 = target ahead, 90째 = target right, 180째 = target behind, 270째 = target left
	double relativeDeg = headingDeg - worldAngleDeg;
	if (relativeDeg < 0.0) {
		relativeDeg += 360.0;
	}
	if (relativeDeg >= 360.0) {
		relativeDeg -= 360.0;
	}

	return relativeDeg;
}

GpsService &gpsSetup() {
	static GpsService service;
	service.begin(kDefaultPins);
	return service;
}