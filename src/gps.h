#pragma once

#include <Arduino.h>

struct GpsPinConfig {
	int rxPin;
	int txPin;
	uint32_t baud;
};

struct GpsData {
	bool dataSeen;
	bool hasFix;
	bool hasTimeDate;
	double latitude;
	double longitude;
	double altitudeM;
	double speedKmph;
	uint32_t satellites;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint32_t charsProcessed;
};

class GpsService {
public:
	GpsData data;

	void begin(const GpsPinConfig &pins);
	void updateFromModule();
	void printStatus() const;
	double distanceToMeters(double latitude, double longitude) const;
	double angleToDegrees(double latitude, double longitude) const;
	double relativeAngleToDegrees(double latitude, double longitude) const;

private:
	friend GpsService &gpsSetup();

	GpsService();
	TaskHandle_t taskHandle_;
};

GpsService &gpsSetup();