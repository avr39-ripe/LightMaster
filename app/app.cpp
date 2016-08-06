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
	system_update_cpu_freq(SYS_CPU_160MHZ);

	ApplicationClass::init();

//	ntpClient = new NtpClient("pool.ntp.org", 300);
	SystemClock.setTimeZone(Config.timeZone);
	Serial.printf("Time zone: %d\n", Config.timeZone);
	lightSystem = new LightSystemClass();

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	_wsBinGetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp);
	_wsBinSetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp);

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);

	mcp000->begin();
	mcp000->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp000->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW
#endif

#ifndef MCP23S17 //use GPIO
//Nothing here
	BinInGPIOClass* thStatWarmFloor = new BinInGPIOClass(12,0); // Start button
	BinInGPIOClass* thStatBedroom = new BinInGPIOClass(13,0); // Stop button
	binInPoller.add(thStatWarmFloor);
	binInPoller.add(thStatBedroom);
#else
//	Serial.printf("PRE Free Heap: %d\n", system_get_free_heap_size());
//	for (uint8_t i = 0; i < 7; i++)
//	{
//		//test binState
//		binStates[i] = new BinStateClass();
////		BinStateHttpClass* binStateHttp = new BinStateHttpClass(webServer, *binStates[i], String(zoneNames[i]), i);
////		binStatesHttp->add(binStateHttp);
//	}
//	Serial.printf("POST Free Heap: %d\n", system_get_free_heap_size());
//	Serial.printf("Size is ,%d\n", sizeof(BinStateClass));

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
#endif

	BinOutGPIOClass* caldronOut = new BinOutGPIOClass(15,0);
	BinStateSharedDeferredClass* caldron = new BinStateSharedDeferredClass();
	caldron->setTrueDelay(10);
	caldron->setFalseDelay(0);
	caldron->onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)&caldronOut->state));

	BinOutGPIOClass* warmFloorPumpOut = new BinOutGPIOClass(4,0);
	BinStateSharedDeferredClass* warmFloorPump = new BinStateSharedDeferredClass();
	warmFloorPump->setTrueDelay(10);
	warmFloorPump->setFalseDelay(5);
	warmFloorPump->onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)&warmFloorPumpOut->state));

	BinOutGPIOClass* bedroomHeadOut = new BinOutGPIOClass(5,0);
	BinStateClass* bedroomHead = &bedroomHeadOut->state;
//	BinStateClass* bedroomHead = new BinStateClass;

	BinStateHttpClass* caldronState = new BinStateHttpClass(webServer, caldron, "Котел", 0);
	binStatesHttp->add(caldronState);

	BinStateHttpClass* warmFloorPumpState = new BinStateHttpClass(webServer, warmFloorPump, "Насос т. пола", 1);
	binStatesHttp->add(warmFloorPumpState);

	BinStateHttpClass* bedroomHeadState = new BinStateHttpClass(webServer, bedroomHead, "Спальня", 2);
	binStatesHttp->add(bedroomHeadState);

//	httpButton->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::toggle, (BinStateClass*)caldron));

	thStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)warmFloorPump));
	thStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));

	thStatBedroom->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)bedroomHead));
	thStatBedroom->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));


	//WEB THERMOSTAT MOCKUP
	BinHttpButtonClass* webThStatWarmFloor = new BinHttpButtonClass(webServer, *binStatesHttp, 2, "Тстат т. пол", warmFloorPump);
	BinHttpButtonClass* webThStatBedroom = new BinHttpButtonClass(webServer, *binStatesHttp, 3, "Тстат спальня", bedroomHead);

	webThStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)warmFloorPump));
	webThStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));

	webThStatBedroom->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)bedroomHead));
	webThStatBedroom->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));
	//WEB THERMOSTAT MOCKUP
	BinOutGPIOClass* ventOut = new BinOutGPIOClass(16,0);
	BinStateClass* vent = &ventOut->state;
//	BinStateClass* vent = new BinStateClass;
	BinStateHttpClass* ventState = new BinStateHttpClass(webServer, vent, "Вентиляция", 3);
	binStatesHttp->add(ventState);

	BinStateClass* ventMan = new BinStateClass;
	ventMan->onChange(onStateChangeDelegate(&BinStateClass::set , vent));

	binCycler = new BinCyclerClass(*vent, 15, 20);
//	binCycler->state.set(true);
	BinHttpButtonClass* ventAutoButton = new BinHttpButtonClass(webServer, *binStatesHttp, 0, "Вент. автомат", &binCycler->state);
	BinHttpButtonClass* ventManButton = new BinHttpButtonClass(webServer, *binStatesHttp, 1, "Вент. ручной", ventMan);

	ventAutoButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse , ventMan)); // Order *IS METTER! firstly turn of mutual state!*
	ventAutoButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle , &binCycler->state));
	ventManButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse , &binCycler->state)); // Order *IS METTER! firstly turn of mutual state!*
	ventManButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle , ventMan));


//	binStateHttp = new BinStateHttpClass(webServer, output->state, "Тушите свет!", 7);
//	binStatesHttp->add(binStateHttp);

//	lightSystem->addAllOffGroup(output, input, httpButton);
//	BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, 0, "Выкл. все");
//	lightSystem->addAllOffGroup(httpButton);
//	httpButton = new BinHttpButtonClass(webServer, 3, "Антивор");
//
////	httpButton->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::toggle, (BinStateClass*)caldron));
//	lightSystem->addRandomButton(httpButton);

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
//	Serial.print("DateTime: ");Serial.println(nowTime.toFullDateTimeString());

//	for (uint8_t i=0; i<7; i++)
//	{
//		binStates[i]->toggle();
//	}
}

