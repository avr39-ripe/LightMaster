/*
 * app.h
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */

#ifndef INCLUDE_APP_H_
#define INCLUDE_APP_H_
#include <application.h>

class AppClass : public ApplicationClass
{
public:
	virtual void init(); // Application initialization
	virtual void start(); // Application main-loop start/restart
protected:
	virtual void _loop(); // Application main loop function goes here
	virtual void wsBinSetter(WebSocket& socket, uint8_t* data, size_t size);
	virtual void wsBinGetter(WebSocket& socket, uint8_t* data, size_t size);
private:
	void _loadConfig();
	void _saveConfig();
//	char confFileName[9] = "app.conf";
//Application specific global configuration variables
	uint16_t	ventCycleDuration = 1;
	uint16_t	ventCycleInterval = 1;
	uint16_t	caldronOnDelay = 1;
};



#endif /* INCLUDE_APP_H_ */
