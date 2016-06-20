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
	void wsConnected(WebSocket& socket);
	void wsMessageReceived(WebSocket& socket, const String& message);
	void wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size);
	void wsDisconnected(WebSocket& socket);
protected:
	virtual void _loop(); // Application main loop function goes here
};



#endif /* INCLUDE_APP_H_ */
