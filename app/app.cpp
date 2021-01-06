/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>
#include <lightmaster.h>

namespace std
{
	void __throw_length_error(char const*)
	{
		while (true) {}
	}
}

void groupSet(const uint8_t* group, uint8_t state);

Timer imHomeTimer;

void AppClass::init()
{
	ApplicationClass::init();

	webServer.paths.set("/",HttpPathDelegate(&AppClass::_httpOnIndex,this));

	BinStatesHttpClass* binStatesHttp = new BinStatesHttpClass();
	wsAddBinGetter(binStatesHttp->sysId, WebsocketBinaryDelegate(&BinStatesHttpClass::wsBinGetter,binStatesHttp));
	wsAddBinSetter(binStatesHttp->sysId, WebsocketBinaryDelegate(&BinStatesHttpClass::wsBinSetter,binStatesHttp));

#ifdef MCP23S17 //use MCP23S17
	MCP* mcp[mcpCount];
	for (int i=0; i < mcpCount; i++)
	{
		mcp[i] = new MCP(i, mcp23s17_cs);
		mcp[i]->begin();
	}
#endif

#ifdef GPIO_MCP23017
	MCP23017* mcp[mcpCount];
	Wire.pins(13,14); //Setup I2C pins

	for (int i=0; i < mcpCount; i++)
	{
		mcp[i] = new MCP23017;
		mcp[i]->begin(i);
	}
#endif

#if defined(MCP23S17) || defined(GPIO_MCP23017)
	for (int i=0; i < mcpCount; i++)
	{
		mcp[i]->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
		mcp[i]->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
		mcp[i]->digitalWrite(0x0000); //Set all PORTA to 0xFF for simple relay which is active LOW
	}

	BinInClass* inputs[inputsCount];
	BinHttpButtonClass* httpButtons[inputsCount];


	Serial.printf(_F("Initial Free Heap: %d\n"), system_get_free_heap_size());

	BinStateClass* allOff = new BinStateClass();
	auto allOffsetFalseFunc = [allOff](uint8_t state){allOff->setFalse(state);};

// MainControl setup
	for(uint8_t i{0}; i < mainCount; ++i)
	{
		uint8_t mcpId{(uint8_t)(i >> 3)}; //mcp IC number from linear io number, math eq i / 8
		uint8_t pinId{(uint8_t)(i ^ mcpId << 3)}; //mcp IC pin number from linear io number, math eq i ^ (mcpId*8)

#ifdef MCP23S17
		outputs[i] = new BinOutMCP23S17Class(*mcp[mcpId],pinId,0);
		inputs[i] = new BinInMCP23S17Class(*mcp[mcpId],pinId,0);
#endif
#ifdef GPIO_MCP23017
		outputs[i] = new BinOutMCP23017Class(*mcp[mcpId],pinId,0);
		inputs[i] = new BinInMCP23017Class(*mcp[mcpId],pinId,0);
#endif
		binInPoller.add(inputs[i]);

		httpButtons[i] = new BinHttpButtonClass(webServer, *binStatesHttp, i, &outputs[i]->state);
		auto togglerFunc = [i](uint8_t state){outputs[i]->state.toggle(state);};
		inputs[i]->state.onChange(togglerFunc);

		httpButtons[i]->state.onChange(togglerFunc);

		allOff->onChange([i](uint8_t state){outputs[i]->state.setFalse(state);});

		inputs[i]->state.onChange(allOffsetFalseFunc);
		httpButtons[i]->state.onChange(allOffsetFalseFunc);
	}
// AdditionalControl setup
	for(uint8_t i{mainCount}; i < mainCount+additionalCount; ++i)
	{
		uint8_t mcpId{(uint8_t)(i >> 3)}; //mcp IC number from linear io number, math eq i / 8
		uint8_t pinId{(uint8_t)(i ^ mcpId << 3)}; //mcp IC pin number from linear io number, math eq i ^ (mcpId*8)

#ifdef MCP23S17
		outputs[i] = new BinOutMCP23S17Class(*mcp[mcpId],pinId,0);
#endif
#ifdef GPIO_MCP23017
		outputs[i] = new BinOutMCP23017Class(*mcp[mcpId],pinId,0);
#endif

		httpButtons[i] = new BinHttpButtonClass(webServer, *binStatesHttp, i, &outputs[i]->state);
		auto togglerFunc = [i](uint8_t state){outputs[i]->state.toggle(state);};

		httpButtons[i]->state.onChange(togglerFunc);

		allOff->onChange([i](uint8_t state){outputs[i]->state.setFalse(state);});

		httpButtons[i]->state.onChange(allOffsetFalseFunc);
	}

// NightControl setup
	for(uint8_t i{mainCount+additionalCount}; i < mainCount+additionalCount+nightCount; ++i)
	{
		uint8_t mcpId{(uint8_t)(i >> 3)}; //mcp IC number from linear io number, math eq i / 8
		uint8_t pinId{(uint8_t)(i ^ mcpId << 3)}; //mcp IC pin number from linear io number, math eq i ^ (mcpId*8)

#ifdef MCP23S17
		outputs[i] = new BinOutMCP23S17Class(*mcp[mcpId],pinId,0);
#endif
#ifdef GPIO_MCP23017
		outputs[i] = new BinOutMCP23017Class(*mcp[mcpId],pinId,0);
#endif

		httpButtons[i] = new BinHttpButtonClass(webServer, *binStatesHttp, i, &outputs[i]->state);
		auto togglerFunc = [i](uint8_t state){outputs[i]->state.toggle(state);};

		httpButtons[i]->state.onChange(togglerFunc);

		allOff->onChange([i](uint8_t state){outputs[i]->state.setFalse(state);});

		httpButtons[i]->state.onChange(allOffsetFalseFunc);
	}

//Additional buttons/settings

// AllOff + ShuttersAllOff

	for (uint8_t allOffOut: {allOffId, shuttersAllOffId})
	{
		uint8_t mcpId{(uint8_t)(allOffOut >> 3)}; //mcp IC number from linear io number, math eq i / 8
		uint8_t pinId{(uint8_t)(allOffOut ^ mcpId << 3)}; //mcp IC pin number from linear io number, math eq i ^ (mcpId*8)

	#ifdef MCP23S17
			outputs[allOffOut] = new BinOutMCP23S17Class(*mcp[mcpId],pinId,0);
	#endif
	#ifdef GPIO_MCP23017
			outputs[allOffOut] = new BinOutMCP23017Class(*mcp[mcpId],pinId,0);
	#endif
		allOff->onChange([allOffOut](uint8_t state){outputs[allOffOut]->state.set(state);});
	}

	auto shuttersAllOffState = new BinStateHttpClass(webServer, &outputs[shuttersAllOffId]->state, 1);//"Выкл. все жалюзи"
	binStatesHttp->add(shuttersAllOffState);

	auto allOffState = new BinStateHttpClass(webServer, allOff, 0);//"Выкл. все"
	binStatesHttp->add(allOffState);
	auto httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, allOffId, allOff); //"Выкл. все"
	httpButton->state.onChange([allOff](uint8_t state){allOff->toggle(state);});
	allOff->persistent(0);

// I'm home!

	uint8_t mcpId{(uint8_t)(imHomeInputId >> 3)}; //mcp IC number from linear io number, math eq i / 8
	uint8_t pinId{(uint8_t)(imHomeInputId ^ mcpId << 3)}; //mcp IC pin number from linear io number, math eq i ^ (mcpId*8)

#ifdef MCP23S17
	inputs[imHomeInputId] = new BinInMCP23S17Class(*mcp[mcpId],pinId,0);
#endif
#ifdef GPIO_MCP23017
	inputs[imHomeInputId] = new BinInMCP23017Class(*mcp[mcpId],pinId,0);
#endif
	binInPoller.add(inputs[imHomeInputId]);

	auto imHomeFunc{
		[](uint8_t state)
		{
			if (state)
			{
				groupSet(imHomeGroup, true);
				imHomeTimer.initializeMs(imHomeDuration*1000, [=](){groupSet(imHomeGroup, false);}).start(false);
			}

		}
	};

	inputs[imHomeInputId]->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
	inputs[imHomeInputId]->state.onChange(imHomeFunc);

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, imHomeId); //"Я дома!"
	httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
	httpButton->state.onChange(imHomeFunc);




//	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 29, &antiTheft->state);//"Антивор!"
//	httpButton->state.onChange([](uint8_t state){antiTheft->state.toggle(state);});

//Night magic group
//	auto cmnNightGrp = new BinStateSharedDeferredClass(); // Add Shared state to turn on/off shared zones
//	// Shared zones will follow cmnNightGrp state.
//	// First time it turns on they will turn on, last time it turns off they will turn off.
//	uint8_t magicNightOutputs[]{ 1 }; // Output id that will form Magic night group
//	uint8_t magicNightActivators[]{ 3, 7, 8}; // Outputs id that will trigger Magic Night Group
//
//	for (const auto& activatorId : magicNightActivators)
//	{
//		outputs[activatorId]->state.onChange([cmnNightGrp](uint8_t state){cmnNightGrp->set(state);});
//		outputs[activatorId]->state.onChange([magicNightOutputs](uint8_t state)
//				{
//					for (const auto& mnOutputId : magicNightOutputs)
//					{
//						outputs[mnOutputId]->state.setTrue(state);
//					};
//				});
//	}
//
//	cmnNightGrp->onChange([magicNightOutputs](uint8_t state)
//				{
//					for (const auto& mnOutputId : magicNightOutputs)
//					{
//						outputs[mnOutputId]->state.set(state);
//					}
//				});

#endif
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
	Serial.printf("Free Heap: %d WS count: %d Counter: %d\n", system_get_free_heap_size(), WebsocketConnection::getActiveWebsockets().count(), _counter);
}

IMPORT_FSTR(flash_indexhtmlgz, PROJECT_DIR "/files/index.html.gz");
void AppClass::_httpOnIndex(HttpRequest &request, HttpResponse &response)
{
	response.setCache(86400, true); // It's important to use cache for better performance.
//	response.sendFile("index.html");
    response.headers[HTTP_HEADER_CONTENT_ENCODING] = _F("gzip");
    auto stream = new FlashMemoryStream(flash_indexhtmlgz);
    response.sendDataStream(stream, MIME_HTML);
}


// groupSet stuff

void groupSet(const uint8_t* group, uint8_t state)
{
	while (*group != groupEnd)
	{
		outputs[*group++]->state.set(state);
	}
}
