/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>

#include <lightmaster.h>

//char zoneNames[][20] = {{"Прихожая"},{"Холл"},{"Столовая"},{"Кухня"},{"Низ холла"},{"Спальня 1"},{"Спальня 2"}};
//AppClass
AppClass::AppClass()
:ApplicationClass()
{
	_wsBinSetters[sysId] = WebSocketBinaryDelegate(&AppClass::wsBinSetter,this);
	_wsBinGetters[sysId] = WebSocketBinaryDelegate(&AppClass::wsBinGetter,this);
}
void AppClass::init()
{
	char zoneNames[][27] = {{"Холл споты"},{"Холл перед"},{"Холл середина"},{"Холл лево"},{"Стол"},{"Кухня вход"},{"Кухня стол"},{"Кухня"},{"Коридор"},{"Улица"},{"Холл 1 лево"},{"Холл 1 право"}, {"Холл 1 низ"}, \
							{"Холл 2 лево"},{"Холл 2 право"},{"Холл 2 низ"},{"Спальня"},{"Спальня лево"},{"Спальня право"},{"Санузел"},\
							{"С/у зеркало"},{"С/у вентилятор"}, {"Котельная"}};
	system_update_cpu_freq(SYS_CPU_160MHZ);

	ApplicationClass::init();
	_loadConfig();

	ntpClient = new NtpClient("pool.ntp.org", 300);
	SystemClock.setTimeZone(Config.timeZone);
	Serial.printf("Time zone: %d\n", Config.timeZone);

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	_wsBinGetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp);
	_wsBinSetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp);

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);
	mcp001 = new MCP(0x001, mcp23s17_cs);
	mcp002 = new MCP(0x002, mcp23s17_cs);
	mcp000->begin();
	mcp001->begin();
	mcp002->begin();
#endif

#ifdef GPIO_MCP23017
	Wire.pins(14,13);
	mcp000 = new MCP23017;
	mcp001 = new MCP23017;
	mcp002 = new MCP23017;
	mcp000->begin(0x000);
	mcp001->begin(0x001);
	mcp002->begin(0x002);

//	mcp000->pinMode(0x0000); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
////	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
//	mcp000->digitalWrite(0x0000); //Set all PORTA to 0xFF for simple relay which is active LOW
//
//	mcp001->pinMode(0x0000); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
////	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
//	mcp001->digitalWrite(0x0000); //Set all PORTA to 0xFF for simple relay which is active LOW
//
//	Serial.printf("DONE!\n");
//	return
#endif

#if defined(MCP23S17) || defined(GPIO_MCP23017)
//	mcp000->begin();
	mcp000->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp000->digitalWrite(0x0000); //Set all PORTA to 0xFF for simple relay which is active LOW

//	mcp001->begin();
	mcp001->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp001->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp001->digitalWrite(0x0000); //Set all PORTA to 0xFF for simple relay which is active LOW

//	mcp002->begin();
	mcp002->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp002->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp002->digitalWrite(0x0000); //Set all PORTA to 0xFF for simple relay which is active LOW

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

//	BinOutClass* outputs[18];
	BinInClass* inputs[24];
	BinHttpButtonClass* httpButtons[24];

	antiTheft = new AntiTheftClass(outputs, 99);
	_wsBinGetters[antiTheft->sysId] = WebSocketBinaryDelegate(&AntiTheftClass::wsBinGetter,antiTheft);
	_wsBinSetters[antiTheft->sysId] = WebSocketBinaryDelegate(&AntiTheftClass::wsBinSetter,antiTheft);


	Serial.printf("Post ARRAY Free Heap: %d\n", system_get_free_heap_size());

	BinStateClass* allOff = new BinStateClass();

//	BinStateClass* imHome = new BinStateClass();

	for (uint8_t i = 0; i < 8; i++)
	{
#ifdef MCP23S17
		BinOutClass* output = new BinOutMCP23S17Class(*mcp000,i,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp000,i,0);
#endif
#ifdef GPIO_MCP23017
		BinOutClass* output = new BinOutMCP23017Class(*mcp000,i,0);
		BinInClass* input = new BinInMCP23017Class(*mcp000,i,0);
#endif
		outputs[i] = output;
		inputs[i] = input;
		binInPoller.add(input);
		antiTheft->addOutputId(i);

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, i, zoneNames[i], &output->state);
		httpButtons[i] = httpButton;
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));

		allOff->onChange(onStateChangeDelegate(&BinStateClass::setFalse, &output->state));
		input->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	}

	for (uint8_t i = 0; i < 8; i++)
	{
#ifdef MCP23S17
		BinOutClass* output = new BinOutMCP23S17Class(*mcp001,i,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp001,i,0);
#endif
#ifdef GPIO_MCP23017
		BinOutClass* output = new BinOutMCP23017Class(*mcp001,i,0);
		BinInClass* input = new BinInMCP23017Class(*mcp001,i,0);
#endif
		outputs[8 + i] = output;
		inputs[8 + i] = input;
		binInPoller.add(input);
		antiTheft->addOutputId(8 + i);

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 8 + i, zoneNames[8 + i], &output->state);
		httpButtons[8 + i] = httpButton;
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));

		allOff->onChange(onStateChangeDelegate(&BinStateClass::setFalse, &output->state));
		input->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	}

	for (uint8_t i = 0; i < 7; i++)
	{
#ifdef MCP23S17
		BinOutClass* output = new BinOutMCP23S17Class(*mcp002,i,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp002,i,0);
#endif
#ifdef GPIO_MCP23017
		BinOutClass* output = new BinOutMCP23017Class(*mcp002,i,0);
		BinInClass* input = new BinInMCP23017Class(*mcp002,i,0);
#endif
		outputs[16 + i] = output;
		inputs[16 + i] = input;
		binInPoller.add(input);
		antiTheft->addOutputId(16 + i);

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 16 + i, zoneNames[16 + i], &output->state);
		httpButtons[16 + i] = httpButton;
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));

		allOff->onChange(onStateChangeDelegate(&BinStateClass::setFalse, &output->state));
		input->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	}
//	BinOutGPIOClass* ventOut = new BinOutGPIOClass(16,0);
//	BinStateClass* vent = &ventOut->state;

#ifdef MCP23S17
		BinOutClass* output = new BinOutMCP23S17Class(*mcp002,7,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp002,7,0);
#endif
#ifdef GPIO_MCP23017
		BinOutClass* output = new BinOutMCP23017Class(*mcp002,7,0);
		BinInClass* input = new BinInMCP23017Class(*mcp002,7,0);
#endif
	allOff->onChange(onStateChangeDelegate(&BinStateClass::set, &output->state));
	binInPoller.add(input);
	input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, allOff));

	BinStateHttpClass* allOffState = new BinStateHttpClass(webServer, allOff, "Выкл. все", 0);
	binStatesHttp->add(allOffState);
	BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 25, "Выкл. все", allOff);
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, allOff));

	allOff->persistent(0);

//	BinStateHttpClass* imHomeState = new BinStateHttpClass(webServer, imHome, "Я дома!", 2);
//	binStatesHttp->add(imHomeState);
	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 26, "Я дома!");
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[0]->state));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[3]->state));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[4]->state));

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 27, "Антивор!", &antiTheft->state);
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &antiTheft->state));

//	Serial.printf("Pre DEALLOCATE ARRAY Free Heap: %d\n", system_get_free_heap_size());

//	delete[] outputs;
//	delete[] inputs;
//	delete[] httpButtons;

//	Serial.printf("Post DEALLOCATE ARRAY Free Heap: %d\n", system_get_free_heap_size());
#endif

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
	antiTheft->start();

//	Serial.printf("AppClass start done!\n");
}

void AppClass::_loop()
{
	ApplicationClass::_loop();
//	Serial.printf("AppClass loop\n");
	Serial.printf("Free Heap: %d WS count: %d\n", system_get_free_heap_size(), webServer.getActiveWebSockets().count());
}

