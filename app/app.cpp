/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>

#include <lightmaster.h>

char zoneNames[][20] = {{"Прихожая"},{"Холл"},{"Столовая"},{"Кухня"},{"Низ холла"},{"Спальня 1"},{"Спальня 2"}};
//AppClass
AppClass::AppClass()
:ApplicationClass()
{
	_wsBinSetters[sysId] = WebSocketBinaryDelegate(&AppClass::wsBinSetter,this);
	_wsBinGetters[sysId] = WebSocketBinaryDelegate(&AppClass::wsBinGetter,this);
}
void AppClass::init()
{
	ApplicationClass::init();
//	ntpClient = new NtpClient("pool.ntp.org", 300);
	SystemClock.setTimeZone(Config.timeZone);
	Serial.printf("Time zone: %d\n", Config.timeZone);
	lightSystem = new LightSystemClass();

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);

	mcp000->begin();
	mcp000->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp000->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW
#endif

#ifndef MCP23S17 //use GPIO
//Nothing here
#else
	for (uint8_t i = 0; i < 7; i++)
	{
		BinOutClass* output = new BinOutMCP23S17Class(*mcp000,i,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp000,i,0);
		binInPoller.add(input);
		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, i, String(zoneNames[i]), &output->state);
		lightSystem->addLightGroup(output, input, httpButton);
	}
	BinOutClass* output = new BinOutMCP23S17Class(*mcp000,7,0);
	BinInClass* input = new BinInMCP23S17Class(*mcp000,7,0);
	binInPoller.add(input);
	BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, 7, "Выкл. все");
	lightSystem->addAllOffGroup(output, input, httpButton);
	httpButton = new BinHttpButtonClass(webServer, 8, "Антивор");
	lightSystem->addRandomButton(httpButton);
//	lightSystem->randomLight(true);

#endif

//	webServer.addPath("/button",HttpPathDelegate(&BinHttpButtonsClass::onHttp,httpButtons));
	// Web Sockets configuration
	webServer.enableWebSockets(true);
	webServer.setWebSocketConnectionHandler(WebSocketDelegate(&AppClass::wsConnected,this));
	webServer.setWebSocketMessageHandler(WebSocketMessageDelegate(&AppClass::wsMessageReceived,this));
	webServer.setWebSocketBinaryHandler(WebSocketBinaryDelegate(&AppClass::wsBinaryReceived,this));
	webServer.setWebSocketDisconnectionHandler(WebSocketDelegate(&AppClass::wsDisconnected,this));
//	Serial.printf("AppClass init done!\n");
}

//WebSocket handling
void AppClass::wsConnected(WebSocket& socket)
{
	Serial.printf("Websocket CONNECTED!\n");
}

void AppClass::wsMessageReceived(WebSocket& socket, const String& message)
{
	Serial.printf("WebSocket message received:\r\n%s\r\n", message.c_str());
	String msg = message;
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(msg);
	//Uncomment next line for extra debuginfo
//	root.prettyPrintTo(Serial);
	String command = root["command"];
//	Serial.printf("Command str: %s\n", command.c_str());

	if (command == "setButton")
	{
		lightSystem->onWSReceiveButton(root);
	}

	if (command == "getButtons")
	{
		lightSystem->onWSGetButtons(socket);
	}

	if (command == "setRandom")
	{
		lightSystem->onWSReceiveRandom(root);
	}

	if (command == "getRandom")
	{
		lightSystem->onWSGetRandom(socket);
	}

//	if (command == "setTime")
//	{
//		onWSSetTime(root);
//	}
//
//	if (command == "getAppState")
//	{
//		onWSGetAppState(socket);
//	}

}

void AppClass::wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size)
{
	Serial.printf("Websocket binary data recieved, size: %d\r\n", size);
	Serial.printf("Opcode: %d\n", data[0]);

	if ( data[wsBinConst::wsCmd] == wsBinConst::setCmd)
	{
		Serial.printf("wsSetCmd\n");
		if (_wsBinSetters.contains(data[wsBinConst::wsSysId]))
		{
			Serial.printf("wsSysId = %d setter called!\n", data[wsBinConst::wsSysId]);
			_wsBinSetters[data[wsBinConst::wsSysId]](socket, data, size);
		}
	}

	if ( data[wsBinConst::wsCmd] == wsBinConst::getCmd)
	{
		Serial.printf("wsGetCmd\n");
		if (_wsBinGetters.contains(data[wsBinConst::wsSysId]))
		{
			Serial.printf("wsSysId = %d getter called!\n", data[wsBinConst::wsSysId]);
			_wsBinGetters[data[wsBinConst::wsSysId]](socket, data, size);
		}
	}
}

void AppClass::wsBinSetter(WebSocket& socket, uint8_t* data, size_t size)
{
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scAppSetTime:
		uint32_t timestamp = 0;
		os_memcpy(&timestamp, (&data[wsBinConst::wsPayLoadStart]), 4);
		if (Config.timeZone != data[wsBinConst::wsPayLoadStart + 4])
		{
			Config.timeZone = data[wsBinConst::wsPayLoadStart + 4];
			Config.save();
			SystemClock.setTimeZone(Config.timeZone);
		}
		SystemClock.setTime(timestamp, eTZ_UTC);
		break;
	}
}

void AppClass::wsBinGetter(WebSocket& socket, uint8_t* data, size_t size)
{
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scAppGetStatus:
		uint8_t* buffer = new uint8_t[wsBinConst::wsPayLoadStart + 4 + 4];
		buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
		buffer[wsBinConst::wsSysId] = sysId;
		buffer[wsBinConst::wsSubCmd] = wsBinConst::scAppGetStatus;

		DateTime now = SystemClock.now(eTZ_UTC);
		uint32_t timestamp = now.toUnixTime();
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &_counter, sizeof(_counter));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 4]), &timestamp, sizeof(timestamp));
		socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 4 + 4);
		delete buffer;
		break;
	}
}

void AppClass::wsDisconnected(WebSocket& socket)
{
	Serial.printf("Websocket DISCONNECTED!\n");
}

//void AppClass::onWSSetTime(JsonObject& jsonRoot)
//{
//	if (jsonRoot["timeZone"].success())
//	{
//		Config.timeZone = jsonRoot["timeZone"];
//		Config.save();
//		SystemClock.setTimeZone(Config.timeZone);
//		DateTime dateTime;
//
//		dateTime.Second = jsonRoot["Second"];
//		dateTime.Minute = jsonRoot["Minute"];
//		dateTime.Hour = jsonRoot["Hour"];
//		dateTime.DayofWeek = jsonRoot["Wday"];
//		dateTime.Day = jsonRoot["Day"];
//		dateTime.Month = jsonRoot["Month"];
//		dateTime.Year = jsonRoot["Year"];
//
//		SystemClock.setTime(dateTime.toUnixTime(), eTZ_UTC);
//		randomSeed(dateTime.toUnixTime());
//
//		dateTime = SystemClock.now(eTZ_Local);
//		Serial.print("Time synced to: ");Serial.println(dateTime.toFullDateTimeString());
//	}
//}
//
//void AppClass::onWSGetAppState(WebSocket& socket)
//{
//	DynamicJsonBuffer jsonBuffer;
//	String buf;
//	JsonObject& root = jsonBuffer.createObject();
//	root["response"] = "getAppState";
//
//	root["counter"] = _counter;
//	String _date_time_str = SystemClock.getSystemTimeString();
//	root["dateTime"] = _date_time_str.c_str();
//
//	root.printTo(buf);
//	socket.sendString(buf);
//}
void AppClass::start()
{
	ApplicationClass::start();
	binInPoller.start();

//	Serial.printf("AppClass start done!\n");
}

void AppClass::_loop()
{
	DateTime nowTime = SystemClock.now(eTZ_Local);

	ApplicationClass::_loop();
//	Serial.printf("AppClass loop\n");
	Serial.printf("Free Heap: %d WS count: %d\n", system_get_free_heap_size(), webServer.getActiveWebSockets().count());
//	Serial.printf("Random: %d\n", lightSystem->getRandom(8,25));
	Serial.print("DateTime: ");Serial.println(nowTime.toFullDateTimeString());
}

