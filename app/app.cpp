/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>
#include <lightmaster.h>

void shuttersClose(uint8_t state);

void AppClass::init()
{
//	char zoneNames[28][27] = {{"Кухня вход"},{"Кухня стол"},{"Кухня"},{"Коридор"},{"Улица"},{"Холл 1 лево"},{"Холл 1 право"}, {"Холл 1 низ"}, \
							{"Холл 2 лево"},{"Холл 2 право"},{"Холл 2 низ"},{"Спальня"},{"Спальня лево"},{"Спальня право"},{"Санузел"},\
							{"С/у зеркало"},{"С/у вентилятор"}, {"Котельная"}, "1", "2","3","4","5","6","7","8","9","10"};

	ApplicationClass::init();

	webServer.paths.set("/",HttpPathDelegate(&AppClass::_httpOnIndex,this));

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	wsAddBinGetter(binStatesHttp->sysId, WebsocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp));
	wsAddBinSetter(binStatesHttp->sysId, WebsocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp));

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);
	mcp001 = new MCP(0x001, mcp23s17_cs);
	mcp002 = new MCP(0x002, mcp23s17_cs);
	mcp000->begin();
	mcp001->begin();
	mcp002->begin();
#endif

#ifdef GPIO_MCP23017
	Wire.pins(13,14);
	mcp000 = new MCP23017;
	mcp001 = new MCP23017;
	mcp002 = new MCP23017;
	mcp003 = new MCP23017;
	mcp000->begin(0x000);
	mcp001->begin(0x001);
	mcp002->begin(0x002);
	mcp003->begin(0x003);

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

//	mcp002->begin();
	mcp003->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp003->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp003->digitalWrite(0x0000); //Set all PORTA to 0xFF for simple relay which is active LOW

	Serial.printf(_F("Pre ARRAY Free Heap: %d\n"), system_get_free_heap_size());

	BinInClass* inputs[28];
	BinHttpButtonClass* httpButtons[28];

	antiTheft = new AntiTheftClass(outputs, 99);

	wsAddBinGetter(antiTheft->sysId, WebsocketBinaryDelegate(&AntiTheftClass::wsBinGetter,antiTheft));
	wsAddBinSetter(antiTheft->sysId, WebsocketBinaryDelegate(&AntiTheftClass::wsBinSetter,antiTheft));


	Serial.printf(_F("Post ARRAY Free Heap: %d\n"), system_get_free_heap_size());

	BinStateClass* allOff = new BinStateClass();
	auto allOffsetFalseFunc = [allOff](uint8_t state){allOff->setFalse(state);};

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

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, i, /*zoneNames[i],*/ &output->state);
		httpButtons[i] = httpButton;
		auto togglerFunc = [output](uint8_t state){output->state.toggle(state);};

		//input->state.onChange([output](uint8_t state){output->state.toggle(state);});
		//httpButton->state.onChange([output](uint8_t state){output->state.toggle(state);});
		input->state.onChange(togglerFunc);
		httpButton->state.onChange(togglerFunc);

		allOff->onChange([output](uint8_t state){output->state.setFalse(state);});
		//input->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		//httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		input->state.onChange(allOffsetFalseFunc);
		httpButton->state.onChange(allOffsetFalseFunc);
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

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 8 + i, /*zoneNames[8 + i],*/ &output->state);
		httpButtons[8 + i] = httpButton;
		auto togglerFunc = [output](uint8_t state){output->state.toggle(state);};

//		input->state.onChange([output](uint8_t state){output->state.toggle (state);});
//		httpButton->state.onChange([output](uint8_t state){output->state.toggle (state);});
		input->state.onChange(togglerFunc);
		httpButton->state.onChange(togglerFunc);

		allOff->onChange([output](uint8_t state){output->state.setFalse(state);});
//		input->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
//		httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		input->state.onChange(allOffsetFalseFunc);
		httpButton->state.onChange(allOffsetFalseFunc);
	}

	for (uint8_t i = 0; i < 2; i++)
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

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 16 + i, /*zoneNames[16 + i],*/ &output->state);
		httpButtons[16 + i] = httpButton;
		auto togglerFunc = [output](uint8_t state){output->state.toggle(state);};

//		input->state.onChange([output](uint8_t state){output->state.toggle (state);});
//		httpButton->state.onChange([output](uint8_t state){output->state.toggle (state);});
		input->state.onChange(togglerFunc);
		httpButton->state.onChange(togglerFunc);

		allOff->onChange([output](uint8_t state){output->state.setFalse(state);});
//		input->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
//		httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		input->state.onChange(allOffsetFalseFunc);
		httpButton->state.onChange(allOffsetFalseFunc);
	}

#ifdef MCP23S17
		BinOutClass* output = new BinOutMCP23S17Class(*mcp002,2,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp002,2,0);
#endif
#ifdef GPIO_MCP23017
		BinOutClass* output = new BinOutMCP23017Class(*mcp002,2,0);
		BinInClass* input = new BinInMCP23017Class(*mcp002,2,0);
#endif
	allOff->onChange([output](uint8_t state){output->state.set(state);});
	binInPoller.add(input);
	input->state.onChange([allOff](uint8_t state){allOff->toggle(state);});
	BinStateHttpClass* allOffState = new BinStateHttpClass(webServer, allOff, /*"Выкл. все",*/ 0);
	binStatesHttp->add(allOffState);
	BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 25, /*"Выкл. все",*/ allOff);
	httpButton->state.onChange([allOff](uint8_t state){allOff->toggle(state);});

	allOff->persistent(0);

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 26/*, "Я дома!"*/);
	httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
	httpButton->state.onChange([=](uint8_t state){outputs[0]->state.setTrue(state);});
	httpButton->state.onChange([=](uint8_t state){outputs[3]->state.setTrue(state);});
	httpButton->state.onChange([=](uint8_t state){outputs[4]->state.setTrue(state);});

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 27, /*"Антивор!",*/ &antiTheft->state);
	httpButton->state.onChange([=](uint8_t state){antiTheft->state.toggle(state);});

	///TEST
	for (uint8_t i = 0; i < 8; i++)
	{
#ifdef MCP23S17
		BinOutClass* output = new BinOutMCP23S17Class(*mcp000,i,0);
		BinInClass* input = new BinInMCP23S17Class(*mcp000,i,0);
#endif
#ifdef GPIO_MCP23017
		BinOutClass* output = new BinOutMCP23017Class(*mcp003,i,0);
		BinInClass* input = new BinInMCP23017Class(*mcp003,i,0);
#endif
		outputs[18+i] = output;
		inputs[18+i] = input;
		binInPoller.add(input);
		antiTheft->addOutputId(18+i);

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 28+i, /*zoneNames[18+i],*/ &output->state);
		httpButtons[28+i] = httpButton;
		auto togglerFunc = [output](uint8_t state){output->state.toggle(state);};

		//input->state.onChange([output](uint8_t state){output->state.toggle(state);});
		//httpButton->state.onChange([output](uint8_t state){output->state.toggle(state);});
		input->state.onChange(togglerFunc);
		httpButton->state.onChange(togglerFunc);

		auto setFalseFunc = [allOff](uint8_t state){allOff->setFalse(state);};

		allOff->onChange([output](uint8_t state){output->state.setFalse(state);});
		//input->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		//httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		input->state.onChange(setFalseFunc);
		httpButton->state.onChange(setFalseFunc);
	}

	//Night magic group
	auto cmnNightGrp = new BinStateSharedDeferredClass(); // Add Shared state to turn on/off shared zones
	// Shared zones will follow cmnNightGrp state.
	// First time it turns on they will turn on, last time it turns off they will turn off.
	cmnNightGrp->onChange([=](uint8_t state){outputs[0]->state.set(state);});
	cmnNightGrp->onChange([=](uint8_t state){outputs[1]->state.set(state);});

	for (uint8_t i = 24; i < 26; i++)
	{
		// As soon as individual zone from "Night magic group" turns on/off, turn on/off,
		// add/remove consumer in fact, cmnNightGrp state.
		outputs[i]->state.onChange([cmnNightGrp](uint8_t state){cmnNightGrp->set(state);});
		// If some zone from shared "Night magic group" isn't on, turn it on.
		outputs[i]->state.onChange([=](uint8_t state){outputs[0]->state.setTrue(state);});
		outputs[i]->state.onChange([=](uint8_t state){outputs[1]->state.setTrue(state);});
	}

	//shuttersControl
	allOff->onChange(shuttersClose); // Close all shutters on True and open on False
	// Make shutters closers/openers mutual exclusive
	outputs[18]->state.onChange([=](uint8_t state){outputs[21]->state.setFalse(state);});
	outputs[19]->state.onChange([=](uint8_t state){outputs[22]->state.setFalse(state);});
	outputs[20]->state.onChange([=](uint8_t state){outputs[23]->state.setFalse(state);});

	outputs[21]->state.onChange([=](uint8_t state){outputs[18]->state.setFalse(state);});
	outputs[22]->state.onChange([=](uint8_t state){outputs[19]->state.setFalse(state);});
	outputs[23]->state.onChange([=](uint8_t state){outputs[20]->state.setFalse(state);});

	///TEST
#endif

//	Serial.printf("AppClass init done!\n");
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
	Serial.printf("Free Heap: %d WS count: %d Counter: %d\n", system_get_free_heap_size(), WebsocketConnection::getActiveWebsockets().count(), _counter);
}

IMPORT_FSTR(flash_indexhtmlgz, PROJECT_DIR "/files/index.html.gz");
void AppClass::_httpOnIndex(HttpRequest &request, HttpResponse &response)
{
	response.setCache(86400, true); // It's important to use cache for better performance.
//	response.sendFile("index.html");
    response.headers[HTTP_HEADER_CONTENT_ENCODING] = _F("gzip");
    auto stream = new FlashMemoryStream(flash_indexhtmlgz);
    response.sendDataStream(stream, MimeType::MIME_HTML);
}

// Shutters callbacks and other stuff
Timer shuttersTimer;
const uint8_t shuttersDuration = 5; //Shutters motor duration to edge position

void shuttersCtrl(uint8_t close, uint8_t state)
{
	if (close)
	{
		outputs[18]->state.set(state);
		outputs[19]->state.set(state);
		outputs[20]->state.set(state);
	}
	else
	{
		outputs[21]->state.set(state);
		outputs[22]->state.set(state);
		outputs[23]->state.set(state);
	}
}

void shuttersClose(uint8_t state)
{
	if (shuttersTimer.isStarted()) { shuttersTimer.stop(); } //stop defered timer
	shuttersCtrl(state, true); // Based on state turn on all shutters closers(T) or openers(F)
	shuttersTimer.initializeMs(shuttersDuration*1000, [state](){shuttersCtrl(state,false);}).start(false);
}
