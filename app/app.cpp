/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>
#include <lightmaster.h>

// Shutters callback
void shuttersClose(uint8_t state);

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

	antiTheft = new AntiTheftClass(outputs, 99);

	wsAddBinGetter(antiTheft->sysId, WebsocketBinaryDelegate(&AntiTheftClass::wsBinGetter,antiTheft));
	wsAddBinSetter(antiTheft->sysId, WebsocketBinaryDelegate(&AntiTheftClass::wsBinSetter,antiTheft));


	Serial.printf(_F("Initial Free Heap: %d\n"), system_get_free_heap_size());

	BinStateClass* allOff = new BinStateClass();
	auto allOffsetFalseFunc = [allOff](uint8_t state){allOff->setFalse(state);};

	for(uint8_t i = 0; i < ioCount; i++)
	{
		uint8_t mcpId = i >> 3; //mcp IC number from linear io number, math eq i / 8
		uint8_t pinId = i ^ mcpId << 3; //mcp IC pin number from linear io number, math eq i ^ (mcpId*8)

#ifdef MCP23S17
		outputs[i] = new BinOutMCP23S17Class(*mcp[mcpId],pinId,0);
		inputs[i] = new BinInMCP23S17Class(*mcp[mcpId],pinId,0);
#endif
#ifdef GPIO_MCP23017
		outputs[i] = new BinOutMCP23017Class(*mcp[mcpId],pinId,0);
		inputs[i] = new BinInMCP23017Class(*mcp[mcpId],pinId,0);
#endif
		binInPoller.add(inputs[i]);
		antiTheft->addOutputId(i);

		httpButtons[i] = new BinHttpButtonClass(webServer, *binStatesHttp, i, &outputs[i]->state);
		auto togglerFunc = [i](uint8_t state){outputs[i]->state.toggle(state);};
		auto inverterFunc = [i](uint8_t state){outputs[i]->state.invert(state);};

		inputs[i]->state.onChange(inverterFunc);
		httpButtons[i]->state.onChange(togglerFunc);

		allOff->onChange([i](uint8_t state){outputs[i]->state.setFalse(state);});

		inputs[i]->state.onChange(allOffsetFalseFunc);
		httpButtons[i]->state.onChange(allOffsetFalseFunc);
	}
//Additional buttons/settings
	uint8_t mcpId = allOffId >> 3; //mcp IC number from linear io number, math eq i / 8
	uint8_t pinId = allOffId ^ mcpId << 3; //mcp IC pin number from linear io number, math eq i ^ (mcpId*8)

#ifdef MCP23S17
		auto output = new BinOutMCP23S17Class(*mcp[mcpId],pinId,0);
		auto input = new BinInMCP23S17Class(*mcp[mcpId],pinId,0);
#endif
#ifdef GPIO_MCP23017
		auto output = new BinOutMCP23017Class(*mcp[mcpId],pinId,0);
		auto input = new BinInMCP23017Class(*mcp[mcpId],pinId,0);
#endif
	allOff->onChange([output](uint8_t state){output->state.set(state);});
	binInPoller.add(input);
	input->state.onChange([allOff](uint8_t state){allOff->toggle(state);});
	auto allOffState = new BinStateHttpClass(webServer, allOff, 0);//"Выкл. все"
	binStatesHttp->add(allOffState);
	auto httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, allOffId, allOff); //"Выкл. все"
	httpButton->state.onChange([allOff](uint8_t state){allOff->toggle(state);});
	allOff->persistent(0);

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 8); //"Вся терасса!"
	httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
	httpButton->state.onChange([](uint8_t state)
			{
				for(uint8_t zoneId: {3,4,5})
				{
					outputs[zoneId]->state.setTrue(state);
				};
			});

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 9, &antiTheft->state);//"Антивор!"
	httpButton->state.onChange([](uint8_t state){antiTheft->state.toggle(state);});

#endif
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
