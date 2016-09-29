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
	virtual void _loadAppConfig(file_t file);
	virtual void _saveAppConfig(file_t file);
private:
//Application specific global configuration variables
	uint16_t	_targetTemp = 3000; //MULTIPLE VALUE BY 100
	uint16_t	_targetTempDelta = 50; //MULTIPLE VALUE BY 100
	uint16_t	_valveStepTime = 10; //seconds
	uint16_t	_valveEdgeTime = 130; //seconds
	uint16_t	_termostatUpdateTime = 30; //seconds
};



#endif /* INCLUDE_APP_H_ */
