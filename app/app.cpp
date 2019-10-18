/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>
#include <lightmaster.h>

void AppClass::init()
{
	char zoneNames[][27] = {{"Кухня вход"},{"Кухня стол"},{"Кухня"},{"Коридор"},{"Улица"},{"Холл 1 лево"},{"Холл 1 право"}, {"Холл 1 низ"}, \
							{"Холл 2 лево"},{"Холл 2 право"},{"Холл 2 низ"},{"Спальня"},{"Спальня лево"},{"Спальня право"},{"Санузел"},\
							{"С/у зеркало"},{"С/у вентилятор"}, {"Котельная"}};

	ApplicationClass::init();


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
	Wire.pins(14,13);
	mcp000 = new MCP23017;
	mcp001 = new MCP23017;
	mcp002 = new MCP23017;
	mcp000->begin(0x000);
	mcp001->begin(0x001);
	mcp002->begin(0x002);

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

	Serial.printf(_F("Pre ARRAY Free Heap: %d\n"), system_get_free_heap_size());

	BinInClass* inputs[18];
	BinHttpButtonClass* httpButtons[18];

	antiTheft = new AntiTheftClass(outputs, 99);

	wsAddBinGetter(antiTheft->sysId, WebsocketBinaryDelegate(&AntiTheftClass::wsBinGetter,antiTheft));
	wsAddBinSetter(antiTheft->sysId, WebsocketBinaryDelegate(&AntiTheftClass::wsBinSetter,antiTheft));


	Serial.printf(_F("Post ARRAY Free Heap: %d\n"), system_get_free_heap_size());

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

		input->state.onChange([output](uint8_t state){output->state.toggle (state);});
		httpButton->state.onChange([output](uint8_t state){output->state.toggle (state);});
		allOff->onChange([output](uint8_t state){output->state.setFalse(state);});
		input->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
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
		input->state.onChange([output](uint8_t state){output->state.toggle (state);});
		httpButton->state.onChange([output](uint8_t state){output->state.toggle (state);});
		allOff->onChange([output](uint8_t state){output->state.setFalse(state);});
		input->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
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

		BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 16 + i, zoneNames[16 + i], &output->state);
		httpButtons[16 + i] = httpButton;
		input->state.onChange([output](uint8_t state){output->state.toggle (state);});
		httpButton->state.onChange([output](uint8_t state){output->state.toggle (state);});
		allOff->onChange([output](uint8_t state){output->state.setFalse(state);});
		input->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
		httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
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
	BinStateHttpClass* allOffState = new BinStateHttpClass(webServer, allOff, "Выкл. все", 0);
	binStatesHttp->add(allOffState);
	BinHttpButtonClass* httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 25, "Выкл. все", allOff);
	httpButton->state.onChange([allOff](uint8_t state){allOff->toggle(state);});

	allOff->persistent(0);

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 26, "Я дома!");
	httpButton->state.onChange([allOff](uint8_t state){allOff->setFalse(state);});
	httpButton->state.onChange([=](uint8_t state){outputs[0]->state.setTrue(state);});
	httpButton->state.onChange([=](uint8_t state){outputs[3]->state.setTrue(state);});
	httpButton->state.onChange([=](uint8_t state){outputs[4]->state.setTrue(state);});

	httpButton = new BinHttpButtonClass(webServer, *binStatesHttp, 27, "Антивор!", &antiTheft->state);
	httpButton->state.onChange([=](uint8_t state){antiTheft->state.set(state);});

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
	Serial.printf("Free Heap: %d WS count: %d\n", system_get_free_heap_size(), WebsocketConnection::getActiveWebsockets().count());
}

