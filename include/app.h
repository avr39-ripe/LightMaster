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
public:
	virtual void init(); // Application initialization
	virtual void start(); // Application main-loop start/restart
//	virtual void userSTAGotIP(IpAddress ip, IpAddress mask, IpAddress gateway); // Runs when Station got ip from access-point
protected:
	virtual void _loop(); // Application main loop function goes here
	void _httpOnIndex(HttpRequest &request, HttpResponse &response);
};
