/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>

#include <lightmaster.h>

//AppClass
AppClass::AppClass()
:ApplicationClass()
{
	_wsBinSetters[sysId] = WebSocketBinaryDelegate(&AppClass::wsBinSetter,this);
	_wsBinGetters[sysId] = WebSocketBinaryDelegate(&AppClass::wsBinGetter,this);
}
void AppClass::init()
{
	char zoneNames[][20] = {{"Прихожая"},{"Холл"},{"Столовая"},{"Кухня"},{"Низ холла"},{"Спальня 1"},{"Спальня 2"}};
	system_update_cpu_freq(SYS_CPU_160MHZ);

	ApplicationClass::init();
//	_loadConfig();

	ntpClient = new NtpClient("pool.ntp.org", 300);
//	SystemClock.setTimeZone(Config.timeZone);
//	Serial.printf("Time zone: %d\n", Config.timeZone);

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	_wsBinGetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp);
	_wsBinSetters[binStatesHttp->sysId] = WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp);

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);
	mcp000->begin();
#endif

#ifdef GPIO_MCP23017
	Wire.pins(14,13);
	mcp000 = new MCP23017;
	mcp000->begin(0x000);
#endif

#if defined(MCP23S17) || defined(GPIO_MCP23017)
	mcp000->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp000->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW

//	BinOutClass* outputs[18];
	BinInClass* inputs[7];
	BinHttpButtonClass* httpButtons[7];

	antiTheft = new AntiTheftClass(outputs, 99);
	_wsBinGetters[antiTheft->sysId] = WebSocketBinaryDelegate(&AntiTheftClass::wsBinGetter,antiTheft);
	_wsBinSetters[antiTheft->sysId] = WebSocketBinaryDelegate(&AntiTheftClass::wsBinSetter,antiTheft);

	BinStateClass* allOff = new BinStateClass();

//	BinStateClass* imHome = new BinStateClass();

	for (uint8_t i = 0; i < 7; i++)
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

#ifdef MCP23S17
		BinOutClass* output = new BinOutMCP23S17Class(*mcp000,7,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp000,7,0);
#endif
#ifdef GPIO_MCP23017
		BinOutClass* output = new BinOutMCP23017Class(*mcp000,7,0);
		BinInClass* input = new BinInMCP23017Class(*mcp000,7,0);
#endif
	allOff->onChange(onStateChangeDelegate(&BinStateClass::set, &output->state));
	binInPoller.add(input);
	input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, allOff));

	BinStateHttpClass* allOffState = new BinStateHttpClass(webServer, allOff, "Выкл. все", 0);
	binStatesHttp->add(allOffState);
	BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 25, "Выкл. все", allOff);
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, allOff));

	allOff->persistent(0);

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 26, "Я дома!");
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse, allOff));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[0]->state));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[3]->state));
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::setTrue, &outputs[4]->state));

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 27, "Антивор!", &antiTheft->state);
	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &antiTheft->state));
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

//void AppClass::_loadConfig()
//{
//		Serial.printf("Try to load App bin cfg..\n");
//	if (fileExist("app.conf"))
//	{
//		Serial.printf("Will load App bin cfg..\n");
//		file_t file = fileOpen("app.conf", eFO_ReadOnly);
//		fileSeek(file, 0, eSO_FileStart);
//		fileRead(file, &ventCycleDuration, sizeof(ventCycleDuration));
//		fileRead(file, &ventCycleInterval, sizeof(ventCycleInterval));
//		fileRead(file, &caldronOnDelay, sizeof(caldronOnDelay));
//		fileClose(file);
//	}
//}
//
//void AppClass::_saveConfig()
//{
//	Serial.printf("Try to save App bin cfg..\n");
//	file_t file = fileOpen("app.conf", eFO_CreateIfNotExist | eFO_WriteOnly);
//	fileWrite(file, &ventCycleDuration, sizeof(ventCycleDuration));
//	fileWrite(file, &ventCycleInterval, sizeof(ventCycleInterval));
//	fileWrite(file, &caldronOnDelay, sizeof(caldronOnDelay));
//	fileClose(file);
//}

void AppClass::wsBinSetter(WebSocket& socket, uint8_t* data, size_t size)
{
	ApplicationClass::wsBinSetter(socket, data, size);

//	switch (data[wsBinConst::wsSubCmd])
//	{
//	case wsBinConst::scAppSetTime:
//	{
//		uint32_t timestamp = 0;
//		os_memcpy(&timestamp, (&data[wsBinConst::wsPayLoadStart]), 4);
//		if (Config.timeZone != data[wsBinConst::wsPayLoadStart + 4])
//		{
//			Config.timeZone = data[wsBinConst::wsPayLoadStart + 4];
//			Config.save();
//			SystemClock.setTimeZone(Config.timeZone);
//		}
//		SystemClock.setTime(timestamp, eTZ_UTC);
//		break;
//	}
//	case wsBinConst::scAppConfigSet:
//	{
//		os_memcpy(&ventCycleDuration, (&data[wsBinConst::wsPayLoadStart]), 2);
//		os_memcpy(&ventCycleInterval, (&data[wsBinConst::wsPayLoadStart + 2]), 2);
//		os_memcpy(&caldronOnDelay, (&data[wsBinConst::wsPayLoadStart + 4]), 2);
//
//		binCycler->setDuration(ventCycleDuration);
//		binCycler->setInterval(ventCycleInterval);
//		caldron->setTrueDelay(caldronOnDelay);
//		_saveConfig();
//		break;
//	}
//	}
}

void AppClass::wsBinGetter(WebSocket& socket, uint8_t* data, size_t size)
{
	ApplicationClass::wsBinGetter(socket, data, size);

//	uint8_t* buffer;
//	switch (data[wsBinConst::wsSubCmd])
//	{
//	case wsBinConst::scAppGetStatus:
//	{
//		buffer = new uint8_t[wsBinConst::wsPayLoadStart + 4 + 4];
//		buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
//		buffer[wsBinConst::wsSysId] = sysId;
//		buffer[wsBinConst::wsSubCmd] = wsBinConst::scAppGetStatus;
//
//		DateTime now = SystemClock.now(eTZ_UTC);
//		uint32_t timestamp = now.toUnixTime();
//		os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &_counter, sizeof(_counter));
//		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 4]), &timestamp, sizeof(timestamp));
//		socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 4 + 4);
//		delete buffer;
//		break;
//	}
//	case wsBinConst::scAppConfigGet:
//	{
//		buffer = new uint8_t[wsBinConst::wsPayLoadStart + 2 + 2 + 2];
//		buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
//		buffer[wsBinConst::wsSysId] = sysId;
//		buffer[wsBinConst::wsSubCmd] = wsBinConst::scAppConfigGet;
//
//		os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &ventCycleDuration, sizeof(ventCycleDuration));
//		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 2]), &ventCycleInterval, sizeof(ventCycleInterval));
//		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 4]), &caldronOnDelay, sizeof(caldronOnDelay));
//
//		socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 2 + 2 + 2);
//		delete buffer;
//		break;
//	}
//	}
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

