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
	_loadConfig();

//	ntpClient = new NtpClient("pool.ntp.org", 300);
	SystemClock.setTimeZone(Config.timeZone);
	Serial.printf("Time zone: %d\n", Config.timeZone);

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	_wsBinGetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp);
	_wsBinSetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp);

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);

	mcp000->begin();
	mcp000->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp000->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW

	mcp001 = new MCP(0x001, mcp23s17_cs);

	mcp001->begin();
	mcp001->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp001->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp001->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW

	mcp002 = new MCP(0x002, mcp23s17_cs);

	mcp002->begin();
	mcp002->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp002->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp002->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW

#endif

#ifndef MCP23S17 //use GPIO
//Nothing here
	BinInGPIOClass* thStatWarmFloor = new BinInGPIOClass(12,0);
	BinInGPIOClass* thStatBedroom = new BinInGPIOClass(13,0);
	BinInGPIOClass* thStatHall = new BinInGPIOClass(14,0);
	binInPoller.add(thStatWarmFloor);
	binInPoller.add(thStatBedroom);
	binInPoller.add(thStatHall);
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

	Serial.printf("Pre ARRAY Free Heap: %d\n", system_get_free_heap_size());

//	BinOutClass** outputs = new BinOutClass*[16];
//	BinInClass** inputs = new BinInClass*[16];
//	BinHttpButtonClass** httpButtons = new BinHttpButtonClass*[16];
	BinOutClass* outputs[16];
	BinInClass* inputs[16];
	BinHttpButtonClass* httpButtons[16];

	Serial.printf("Post ARRAY Free Heap: %d\n", system_get_free_heap_size());

	BinStateClass* allOff = new BinStateClass();
//	BinStateClass* imHome = new BinStateClass();

	for (uint8_t i = 0; i < 8; i++)
	{
		BinOutClass* output = new BinOutMCP23S17Class(*mcp000,i,0);
		outputs[i] = output;
		BinInClass* input = new BinInMCP23S17Class(*mcp000,i,0);
		inputs[i] = input;
		binInPoller.add(input);
		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, i, String("Комната") + String(i), &output->state);
		httpButtons[i] = httpButton;
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));

		allOff->onChange(onStateChangeDelegate(&BinStateClass::setFalse, &output->state));
		input->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		BinOutClass* output = new BinOutMCP23S17Class(*mcp001,i,0);
		outputs[8 + i] = output;
		BinInClass* input = new BinInMCP23S17Class(*mcp001,i,0);
		binInPoller.add(input);
		inputs[8 + i] = input;
		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 8 + i, String("Комната") + String(8 + i), &output->state);
		httpButtons[8 + i] = httpButton;
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));

		allOff->onChange(onStateChangeDelegate(&BinStateClass::setFalse, &output->state));
		input->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	}
//	BinOutGPIOClass* ventOut = new BinOutGPIOClass(16,0);
//	BinStateClass* vent = &ventOut->state;

	BinStateHttpClass* allOffState = new BinStateHttpClass(webServer, allOff, "Выкл. все", 0);
	binStatesHttp->add(allOffState);
	BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 16, "Выкл. все", allOff);
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, allOff));

//	BinStateHttpClass* imHomeState = new BinStateHttpClass(webServer, imHome, "Я дома!", 2);
//	binStatesHttp->add(imHomeState);
	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 17, "Я дома!");
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[15]->state));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[14]->state));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[0]->state));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[1]->state));

//	Serial.printf("Pre DEALLOCATE ARRAY Free Heap: %d\n", system_get_free_heap_size());

//	delete[] outputs;
//	delete[] inputs;
//	delete[] httpButtons;

//	Serial.printf("Post DEALLOCATE ARRAY Free Heap: %d\n", system_get_free_heap_size());
#endif

//	BinOutGPIOClass* caldronOut = new BinOutGPIOClass(15,0);
//	caldron = new BinStateSharedDeferredClass();
//	caldron->setTrueDelay(caldronOnDelay);
//	caldron->setFalseDelay(0);
//	caldron->onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)&caldronOut->state));
//
//	BinOutGPIOClass* warmFloorPumpOut = new BinOutGPIOClass(4,0);
//	BinStateClass* warmFloorPump = &warmFloorPumpOut->state;
//
//	BinOutGPIOClass* bedroomHeadOut = new BinOutGPIOClass(5,0);
//	BinStateClass* bedroomHead = &bedroomHeadOut->state;
//
//	BinOutGPIOClass* hallHeadOut = new BinOutGPIOClass(2,0);
//	BinStateClass* hallHead = &hallHeadOut->state;
//
//	BinStateHttpClass* caldronState = new BinStateHttpClass(webServer, caldron, "Котел", 0);
//	binStatesHttp->add(caldronState);
//
//	BinStateHttpClass* warmFloorPumpState = new BinStateHttpClass(webServer, warmFloorPump, "Насос т. пола", 1);
//	binStatesHttp->add(warmFloorPumpState);
//
//	BinStateHttpClass* bedroomHeadState = new BinStateHttpClass(webServer, bedroomHead, "Спальня", 2);
//	binStatesHttp->add(bedroomHeadState);
//
//	BinStateHttpClass* hallHeadState = new BinStateHttpClass(webServer, hallHead, "Холл", 4);
//	binStatesHttp->add(hallHeadState);
//
//
//	thStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)warmFloorPump));
//	thStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::setNow, (BinStateSharedDeferredClass*)caldron));
//
//	thStatBedroom->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)bedroomHead));
//	thStatBedroom->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));
//
//	thStatHall->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)hallHead));
//	thStatHall->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));
//	//WEB THERMOSTAT MOCKUP
//	BinHttpButtonClass* webThStatWarmFloor = new BinHttpButtonClass(webServer, *binStatesHttp, 2, "Тстат т. пол", warmFloorPump);
//	BinHttpButtonClass* webThStatBedroom = new BinHttpButtonClass(webServer, *binStatesHttp, 3, "Тстат спальня", bedroomHead);
//	BinHttpButtonClass* webThStatHall = new BinHttpButtonClass(webServer, *binStatesHttp, 4, "Тстат холл", hallHead);
//
//	webThStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)warmFloorPump));
//	webThStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::setNow, (BinStateSharedDeferredClass*)caldron));
//
//	webThStatBedroom->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)bedroomHead));
//	webThStatBedroom->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));
//
//	webThStatHall->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)hallHead));
//	webThStatHall->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));
//	//WEB THERMOSTAT MOCKUP
//	BinOutGPIOClass* ventOut = new BinOutGPIOClass(16,0);
//	BinStateClass* vent = &ventOut->state;
//	BinStateHttpClass* ventState = new BinStateHttpClass(webServer, vent, "Вентиляция", 3);
//	binStatesHttp->add(ventState);
//
//	BinStateClass* ventMan = new BinStateClass;
//	ventMan->onChange(onStateChangeDelegate(&BinStateClass::set , vent));
//
//	binCycler = new BinCyclerClass(*vent, ventCycleDuration, ventCycleInterval);
//
//	BinHttpButtonClass* ventAutoButton = new BinHttpButtonClass(webServer, *binStatesHttp, 0, "Вент. автомат", &binCycler->state);
//	BinHttpButtonClass* ventManButton = new BinHttpButtonClass(webServer, *binStatesHttp, 1, "Вент. ручной", ventMan);
//
//	ventAutoButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse , ventMan)); // Order *IS METTER! firstly turn of mutual state!*
//	ventAutoButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle , &binCycler->state));
//	ventManButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse , &binCycler->state)); // Order *IS METTER! firstly turn of mutual state!*
//	ventManButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle , ventMan));

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

//	if (command == "setRandom")
//	{
//		lightSystem->onWSReceiveRandom(root);
//	}
//
//	if (command == "getRandom")
//	{
//		lightSystem->onWSGetRandom(socket);
//	}

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

void AppClass::_loadConfig()
{
		Serial.printf("Try to load App bin cfg..\n");
	if (fileExist("app.conf"))
	{
		Serial.printf("Will load App bin cfg..\n");
		file_t file = fileOpen("app.conf", eFO_ReadOnly);
		fileSeek(file, 0, eSO_FileStart);
		fileRead(file, &ventCycleDuration, sizeof(ventCycleDuration));
		fileRead(file, &ventCycleInterval, sizeof(ventCycleInterval));
		fileRead(file, &caldronOnDelay, sizeof(caldronOnDelay));
		fileClose(file);
	}
}

void AppClass::_saveConfig()
{
	Serial.printf("Try to save App bin cfg..\n");
	file_t file = fileOpen("app.conf", eFO_CreateIfNotExist | eFO_WriteOnly);
	fileWrite(file, &ventCycleDuration, sizeof(ventCycleDuration));
	fileWrite(file, &ventCycleInterval, sizeof(ventCycleInterval));
	fileWrite(file, &caldronOnDelay, sizeof(caldronOnDelay));
	fileClose(file);
}

void AppClass::wsBinSetter(WebSocket& socket, uint8_t* data, size_t size)
{
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scAppSetTime:
	{
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
	case wsBinConst::scAppConfigSet:
	{
//		os_memcpy(&ventCycleDuration, (&data[wsBinConst::wsPayLoadStart]), 2);
//		os_memcpy(&ventCycleInterval, (&data[wsBinConst::wsPayLoadStart + 2]), 2);
//		os_memcpy(&caldronOnDelay, (&data[wsBinConst::wsPayLoadStart + 4]), 2);
//
//		binCycler->setDuration(ventCycleDuration);
//		binCycler->setInterval(ventCycleInterval);
//		caldron->setTrueDelay(caldronOnDelay);
//		_saveConfig();
		break;
	}
	}
}

void AppClass::wsBinGetter(WebSocket& socket, uint8_t* data, size_t size)
{
	uint8_t* buffer;
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scAppGetStatus:
	{
		buffer = new uint8_t[wsBinConst::wsPayLoadStart + 4 + 4];
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
	case wsBinConst::scAppConfigGet:
	{
		buffer = new uint8_t[wsBinConst::wsPayLoadStart + 2 + 2 + 2];
		buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
		buffer[wsBinConst::wsSysId] = sysId;
		buffer[wsBinConst::wsSubCmd] = wsBinConst::scAppConfigGet;

		os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &ventCycleDuration, sizeof(ventCycleDuration));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 2]), &ventCycleInterval, sizeof(ventCycleInterval));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 4]), &caldronOnDelay, sizeof(caldronOnDelay));

		socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 2 + 2 + 2);
		delete buffer;
		break;
	}
	}
}

void AppClass::wsDisconnected(WebSocket& socket)
{
	Serial.printf("Websocket DISCONNECTED!\n");
}

void AppClass::start()
{
	ApplicationClass::start();
	binInPoller.start();

//	Serial.printf("AppClass start done!\n");
}

void AppClass::_loop()
{
	ApplicationClass::_loop();
//	Serial.printf("AppClass loop\n");
	Serial.printf("Free Heap: %d WS count: %d\n", system_get_free_heap_size(), webServer.getActiveWebSockets().count());
}

