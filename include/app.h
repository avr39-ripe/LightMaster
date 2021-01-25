/*
 * app.h
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */

#pragma once
#include <application.h>

class AppClass : public ApplicationClass
{
	static uint16_t shuttersDuration; //Shutters motor duration to edge position in seconds
	static uint16_t ventDuration; // Ventilation turn on duration in seconds.

	void _loadAppConfig(file_t& file) override;
	void _saveAppConfig(file_t& file) override;
	bool _extraConfigReadJson(JsonObject& json) override;
	void _extraConfigWriteJson(JsonObject& json) override;
public:
	virtual void init(); // Application initialization
	virtual void start(); // Application main-loop start/restart
	static uint16_t getShuttersDuration(){return shuttersDuration;};
	static uint16_t getVentDuration(){return ventDuration;};
//	virtual void userSTAGotIP(IpAddress ip, IpAddress mask, IpAddress gateway); // Runs when Station got ip from access-point
protected:
	virtual void _loop(); // Application main loop function goes here
	void _httpOnIndex(HttpRequest &request, HttpResponse &response);
};
