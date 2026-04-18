#include "gps.h"

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

GpsService &gpsSetup() {
	static GpsService service;
	service.begin(kDefaultPins);
	return service;
}