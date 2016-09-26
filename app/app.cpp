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
void AppClass::init()
{
	system_update_cpu_freq(SYS_CPU_160MHZ);
	ApplicationClass::init();

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	wsAddBinGetter(binStatesHttp->sysId, WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp));
	wsAddBinSetter(binStatesHttp->sysId, WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp));

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);

	mcp000->begin();
	mcp000->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp000->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW
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
	caldron = new BinStateSharedDeferredClass();
	caldron->setTrueDelay(caldronOnDelay);
	caldron->setFalseDelay(0);
	caldron->onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)&caldronOut->state));

	BinOutGPIOClass* warmFloorPumpOut = new BinOutGPIOClass(4,0);
	BinStateClass* warmFloorPump = &warmFloorPumpOut->state;

	BinOutGPIOClass* bedroomHeadOut = new BinOutGPIOClass(5,0);
	BinStateClass* bedroomHead = &bedroomHeadOut->state;

	BinOutGPIOClass* hallHeadOut = new BinOutGPIOClass(2,0);
	BinStateClass* hallHead = &hallHeadOut->state;

	BinStateHttpClass* caldronState = new BinStateHttpClass(webServer, caldron, "Котел", 0);
	binStatesHttp->add(caldronState);

	BinStateHttpClass* warmFloorPumpState = new BinStateHttpClass(webServer, warmFloorPump, "Насос т. пола", 1);
	binStatesHttp->add(warmFloorPumpState);

	BinStateHttpClass* bedroomHeadState = new BinStateHttpClass(webServer, bedroomHead, "Спальня", 2);
	binStatesHttp->add(bedroomHeadState);

	BinStateHttpClass* hallHeadState = new BinStateHttpClass(webServer, hallHead, "Холл", 4);
	binStatesHttp->add(hallHeadState);


	thStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)warmFloorPump));
	thStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::setNow, (BinStateSharedDeferredClass*)caldron));

	thStatBedroom->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)bedroomHead));
	thStatBedroom->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));

	thStatHall->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)hallHead));
	thStatHall->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));
	//WEB THERMOSTAT MOCKUP
	BinHttpButtonClass* webThStatWarmFloor = new BinHttpButtonClass(webServer, *binStatesHttp, 2, "Тстат т. пол", warmFloorPump);
	BinHttpButtonClass* webThStatBedroom = new BinHttpButtonClass(webServer, *binStatesHttp, 3, "Тстат спальня", bedroomHead);
	BinHttpButtonClass* webThStatHall = new BinHttpButtonClass(webServer, *binStatesHttp, 4, "Тстат холл", hallHead);

	webThStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)warmFloorPump));
	webThStatWarmFloor->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::setNow, (BinStateSharedDeferredClass*)caldron));

	webThStatBedroom->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)bedroomHead));
	webThStatBedroom->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));

	webThStatHall->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)hallHead));
	webThStatHall->state.onChange(onStateChangeDelegate(&BinStateSharedDeferredClass::set, (BinStateSharedDeferredClass*)caldron));
	//WEB THERMOSTAT MOCKUP
	BinOutGPIOClass* ventOut = new BinOutGPIOClass(16,0);
	BinStateClass* vent = &ventOut->state;
	BinStateHttpClass* ventState = new BinStateHttpClass(webServer, vent, "Вентиляция", 3);
	binStatesHttp->add(ventState);

	BinStateClass* ventMan = new BinStateClass;
	ventMan->onChange(onStateChangeDelegate(&BinStateClass::set , vent));

	binCycler = new BinCyclerClass(*vent, ventCycleDuration, ventCycleInterval);

	BinHttpButtonClass* ventAutoButton = new BinHttpButtonClass(webServer, *binStatesHttp, 0, "Вент. автомат", &binCycler->state);
	BinHttpButtonClass* ventManButton = new BinHttpButtonClass(webServer, *binStatesHttp, 1, "Вент. ручной", ventMan);

	ventAutoButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse , ventMan)); // Order *IS METTER! firstly turn of mutual state!*
	ventAutoButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle , &binCycler->state));
	ventManButton->state.onChange(onStateChangeDelegate(&BinStateClass::setFalse , &binCycler->state)); // Order *IS METTER! firstly turn of mutual state!*
	ventManButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle , ventMan));

//	Serial.printf("AppClass init done!\n");
}


void AppClass::wsBinSetter(WebSocket& socket, uint8_t* data, size_t size)
{
	ApplicationClass::wsBinSetter(socket, data, size);

	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scAppConfigSet:
	{
		os_memcpy(&ventCycleDuration, (&data[wsBinConst::wsPayLoadStart]), 2);
		os_memcpy(&ventCycleInterval, (&data[wsBinConst::wsPayLoadStart + 2]), 2);
		os_memcpy(&caldronOnDelay, (&data[wsBinConst::wsPayLoadStart + 4]), 2);

		binCycler->setDuration(ventCycleDuration);
		binCycler->setInterval(ventCycleInterval);
		caldron->setTrueDelay(caldronOnDelay);
		saveConfig();
		break;
	}
	}
}

void AppClass::wsBinGetter(WebSocket& socket, uint8_t* data, size_t size)
{
	ApplicationClass::wsBinGetter(socket, data, size);

	uint8_t* buffer;
	switch (data[wsBinConst::wsSubCmd])
	{
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

void AppClass::_loadAppConfig(file_t file)
{
		fileRead(file, &ventCycleDuration, sizeof(ventCycleDuration));
		fileRead(file, &ventCycleInterval, sizeof(ventCycleInterval));
		fileRead(file, &caldronOnDelay, sizeof(caldronOnDelay));
}

void AppClass::_saveAppConfig(file_t file)
{
	fileWrite(file, &ventCycleDuration, sizeof(ventCycleDuration));
	fileWrite(file, &ventCycleInterval, sizeof(ventCycleInterval));
	fileWrite(file, &caldronOnDelay, sizeof(caldronOnDelay));
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

