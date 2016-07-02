/*
 * app.h
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */

#ifndef INCLUDE_APP_H_
#define INCLUDE_APP_H_
#include <application.h>

namespace wsBinConst
{
//Frame header offsets
	const uint8_t wsCmd = 0; //Command type
	const uint8_t wsSysId = 1; //target sysId
	const uint8_t wsSubCmd = 2; //sub-command type
	const uint8_t wsPayLoadStart = 3;

	const uint8_t reservedCmd = 0;
	const uint8_t getCmd = 1;
	const uint8_t setCmd = 2;
	const uint8_t getResponse = 3;
	const uint8_t setAck = 4;
	const uint8_t setNack =5;

// sub-commands
	const uint8_t scAppSetTime = 1;
	const uint8_t scAppGetStatus = 2;
};

class AppClass : public ApplicationClass
{
public:
	AppClass();
	virtual void init(); // Application initialization
	virtual void start(); // Application main-loop start/restart
	void wsConnected(WebSocket& socket);
	void wsMessageReceived(WebSocket& socket, const String& message);
	void wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size);
	void wsDisconnected(WebSocket& socket);
	void onWSSetTime(JsonObject& jsonRoot);
	void onWSGetAppState(WebSocket& socket);
	void wsBinSetter(WebSocket& socket, uint8_t* data, size_t size);
	void wsBinGetter(WebSocket& socket, uint8_t* data, size_t size);
	static const uint8_t sysId = 1;
	HashMap<uint8_t,WebSocketBinaryDelegate> _wsBinSetters;
	HashMap<uint8_t,WebSocketBinaryDelegate> _wsBinGetters;
protected:
	virtual void _loop(); // Application main loop function goes here
};



#endif /* INCLUDE_APP_H_ */
