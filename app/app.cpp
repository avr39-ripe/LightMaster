/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <lightmaster.h>
#include <app.h>

//#include <lightmaster.h>

char zoneNames[][20] = {{"Прихожая"},{"Холл"},{"Столовая"},{"Кухня"},{"Низ холла"},{"Спальня 1"},{"Спальня 2"}};
//AppClass
void AppClass::init()
{
	system_update_cpu_freq(SYS_CPU_160MHZ);
	ApplicationClass::init();

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	wsAddBinGetter(binStatesHttp->sysId, WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp));
	wsAddBinSetter(binStatesHttp->sysId, WebSocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp));

	tempSensor = new TempSensorsOW(ds, 4000);
	ds.begin();
	tempSensor->addSensor();
	webServer.addPath("/temperature.json",HttpPathDelegate(&TempSensors::onHttpGet,tempSensor));

	BinOutGPIOClass* warmOut = new BinOutGPIOClass(12,0);
	BinOutGPIOClass* coldOut = new BinOutGPIOClass(14,0);

	BinStateHttpClass* warmState = new BinStateHttpClass(webServer, &warmOut->state, "Горячая", 0);
	binStatesHttp->add(warmState);

	BinStateHttpClass* clodState = new BinStateHttpClass(webServer, &coldOut->state, "Холодная", 1);
	binStatesHttp->add(clodState);

	TWValve = new TWValveClass(*tempSensor, 0, warmOut->state, coldOut->state);

	TWValve->setTargetTemp(float(_targetTemp / 100.0));
	TWValve->setTargetTempDelta(float(_targetTempDelta / 100.0));
	TWValve->setStepTime(_valveStepTime);
	TWValve->setEdgeTime(_valveEdgeTime);
	TWValve->setTermostatUpdateTime(_termostatUpdateTime);

	BinStateHttpClass* valveState = new BinStateHttpClass(webServer, &TWValve->state, "Активно", 2);
	binStatesHttp->add(valveState);

	BinInGPIOClass* valveEnableIn = new BinInGPIOClass(16,0);
	valveEnableIn->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)&TWValve->state));
	binInPoller.add(valveEnableIn);

	BinHttpButtonClass* webEnableIn = new BinHttpButtonClass(webServer, *binStatesHttp, 0, "Активно", &TWValve->state);
	webEnableIn->state.onChange(onStateChangeDelegate(&BinStateClass::set, (BinStateClass*)&TWValve->state));

//
////	BinInPollerClass* binInPoller = new BinInPollerClass(100);
//#ifndef MCP23S17 //use GPIO
////Nothing here
//	BinInGPIOClass* thStatWarmFloor = new BinInGPIOClass(12,0);
//	BinInGPIOClass* thStatBedroom = new BinInGPIOClass(13,0);
//	BinInGPIOClass* thStatHall = new BinInGPIOClass(14,0);
//	binInPoller.add(thStatWarmFloor);
//	binInPoller.add(thStatBedroom);
//	binInPoller.add(thStatHall);
//#else
//	for (uint8_t i = 0; i < 7; i++)
//	{
//		BinOutClass* output = new BinOutMCP23S17Class(*mcp000,i,0);
//		BinInClass* input = new BinInMCP23S17Class(*mcp000,i,0);
//		binInPoller.add(input);
//		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, i, String(zoneNames[i]), &output->state);
//		lightSystem.addLightGroup(output, input, httpButton);
//
//
//	}
//	BinOutClass* output = new BinOutMCP23S17Class(*mcp000,7,0);
//	BinInClass* input = new BinInMCP23S17Class(*mcp000,7,0);
//	binInPoller.add(input);
//#endif
//
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

//	Serial.printf("AppClass init done!\n");
}


void AppClass::wsBinSetter(WebSocket& socket, uint8_t* data, size_t size)
{
	ApplicationClass::wsBinSetter(socket, data, size);

	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scAppConfigSet:
	{
		os_memcpy(&_targetTemp, (&data[wsBinConst::wsPayLoadStart]), 2);
		os_memcpy(&_targetTempDelta, (&data[wsBinConst::wsPayLoadStart + 2]), 2);
		os_memcpy(&_valveStepTime, (&data[wsBinConst::wsPayLoadStart + 4]), 2);
		os_memcpy(&_valveEdgeTime, (&data[wsBinConst::wsPayLoadStart + 6]), 2);
		os_memcpy(&_termostatUpdateTime, (&data[wsBinConst::wsPayLoadStart + 8]), 2);

		TWValve->setTargetTemp(float(_targetTemp / 100.0));
		TWValve->setTargetTempDelta(float(_targetTempDelta / 100.0));
		TWValve->setStepTime(_valveStepTime);
		TWValve->setEdgeTime(_valveEdgeTime);
		TWValve->setTermostatUpdateTime(_termostatUpdateTime);

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
		buffer = new uint8_t[wsBinConst::wsPayLoadStart + 2 + 2 + 2 + 2 + 2];
		buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
		buffer[wsBinConst::wsSysId] = sysId;
		buffer[wsBinConst::wsSubCmd] = wsBinConst::scAppConfigGet;

		os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &_targetTemp, sizeof(_targetTemp));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 2]), &_targetTempDelta, sizeof(_targetTempDelta));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 4]), &_valveStepTime, sizeof(_valveStepTime));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 6]), &_valveEdgeTime, sizeof(_valveEdgeTime));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 8]), &_termostatUpdateTime, sizeof(_termostatUpdateTime));

		socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 2 + 2 + 2 + 2 + 2);
		delete buffer;
		break;
	}
	}
}

void AppClass::_loadAppConfig(file_t file)
{
		fileRead(file, &_targetTemp, sizeof(_targetTemp));
		fileRead(file, &_targetTempDelta, sizeof(_targetTempDelta));
		fileRead(file, &_valveStepTime, sizeof(_valveStepTime));
		fileRead(file, &_valveEdgeTime, sizeof(_valveEdgeTime));
		fileRead(file, &_termostatUpdateTime, sizeof(_termostatUpdateTime));
}

void AppClass::_saveAppConfig(file_t file)
{
	fileWrite(file, &_targetTemp, sizeof(_targetTemp));
	fileWrite(file, &_targetTempDelta, sizeof(_targetTempDelta));
	fileWrite(file, &_valveStepTime, sizeof(_valveStepTime));
	fileWrite(file, &_valveEdgeTime, sizeof(_valveEdgeTime));
	fileWrite(file, &_termostatUpdateTime, sizeof(_termostatUpdateTime));
}

void AppClass::start()
{
	ApplicationClass::start();
	binInPoller.start();
	tempSensor->start();

//	Serial.printf("AppClass start done!\n");
}

void AppClass::_loop()
{
	ApplicationClass::_loop();
//	Serial.printf("AppClass loop\n");
	Serial.printf("Free Heap: %d WS count: %d\n", system_get_free_heap_size(), webServer.getActiveWebSockets().count());
}

