/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>

#include <lightmaster.h>

//AppClass
//void monitor(HttpRequest &request, HttpResponse &response); // Monitor via json some important params

void AppClass::init()
{
	ntpClient = new NtpClient("pool.ntp.org", 300);
	//TODO: add config param for TZ!
	SystemClock.setTimeZone(3);

	httpButtons = new BinHttpButtonsClass();

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);
#endif

#ifndef MCP23S17 //use GPIO
//Nothing here
#else
	for (uint8_t i = 0; i < 8; i++)
	{
		input[i] = new BinInMCP23S17Class(*mcp000,i,0);
		binInPoller.add(input[i]);
	}
#endif

#ifndef MCP23S17 //use GPIO
//Nothing here
#else
	for (uint8_t i = 0; i < 8; i++)
	{
		output[i] = new BinOutMCP23S17Class(*mcp000,i,0);
		output[i]->state.set(false);
//		lightGroup[i] = new LightGroupClass(*output[i]);
//		lightGroup[i]->addInput(*input[i]);
		input[i]->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output[i]->state));
		BinHttpButtonClass* button = new BinHttpButtonClass(i, "Btn" + String(i), output[i]);
		button->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output[i]->state));
		httpButtons->add(*button);
	}

//	httpButton = new BinHttpButtonClass(0);
//	httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output[0]->state));

#endif
	ApplicationClass::init();
	webServer.addPath("/button",HttpPathDelegate(&BinHttpButtonsClass::onHttp,httpButtons));
//	webServer.addPath("/monitor",monitor);
//	Serial.printf("AppClass init done!\n");
}

void AppClass::start()
{
	ApplicationClass::start();
	binInPoller.start();
#ifdef MCP23S17 //use MCP23S17
	mcp000->begin();
	mcp000->pinMode(0xFF00); // Set PORTA to OUTPUT 0x00, PORTB to INPUT 0xFF
	mcp000->pullupMode(0xFF00); // turn on internal pull-up for PORTB 0xFF
	mcp000->digitalWrite(0x00FF); //Set all PORTA to 0xFF for simple relay which is active LOW
#endif
//	Serial.printf("AppClass start done!\n");
}

void AppClass::_loop()
{
	DateTime nowTime = SystemClock.now();

	ApplicationClass::_loop();
//	Serial.printf("AppClass loop\n");
}

//void monitor(HttpRequest &request, HttpResponse &response)
//{
//	JsonObjectStream* stream = new JsonObjectStream();
//	JsonObject& json = stream->getRoot();
//
//	response.setHeader("Access-Control-Allow-Origin", "*");
//	response.sendJsonObject(stream);
//}
