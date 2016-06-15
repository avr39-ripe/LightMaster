/*
 * app.cpp
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */
#include <app.h>

#include <lightmaster.h>

//AppClass
void monitor(HttpRequest &request, HttpResponse &response); // Monitor via json some important params

void AppClass::init()
{
	ntpClient = new NtpClient("pool.ntp.org", 300);
	//TODO: add config param for TZ!
	SystemClock.setTimeZone(3);

#ifdef MCP23S17 //use MCP23S17
	mcp000 = new MCP(0x000, mcp23s17_cs);
#endif

#ifndef MCP23S17 //use GPIO
	input[0] = new BinInGPIOClass(15,1); // Start button
	input[1] = new BinInGPIOClass(16,0); // Stop button
#else
//	input[0] = new BinInMCP23S17Class(*mcp000,1,0); // Start button
//	input[1] = new BinInMCP23S17Class(*mcp000,2,0); // Stop button
	for (uint8_t i = 0; i < 8; i++)
	{
		input[i] = new BinInMCP23S17Class(*mcp000,i,0);
		binInPoller.add(input[i]);
	}
#endif
//	binInPoller.add(input[0]);
//	binInPoller.add(input[1]);

#ifndef MCP23S17 //use GPIO
	output[0] = new BinOutGPIOClass(12,1); // Fan
	output[1] = new BinOutGPIOClass(13,1); // Pumup
	output[2] = new BinOutGPIOClass(14,1); // O3
#else
	for (uint8_t i = 0; i < 8; i++)
	{
		output[i] = new BinOutMCP23S17Class(*mcp000,i,0);
		output[i]->state.set(false);
//		lightGroup[i] = new LightGroupClass(*output[i]);
//		lightGroup[i]->addInput(*input[i]);
		input[i]->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output[i]->state));
	}
//	output[0] = new BinOutMCP23S17Class(*mcp000,1,0); // Fan
//	output[1] = new BinOutMCP23S17Class(*mcp000,2,0); // Pumup
//	output[2] = new BinOutMCP23S17Class(*mcp000,3,0); // O3
#endif
//	output[0]->setState(false);
//	output[1]->setState(false);
//	output[2]->setState(false);
//	input[0]->onStateChange(onStateChangeDelegate(&BinOutGPIOClass::setState, output[0]));
//	input[1]->onStateChange(onStateChangeDelegate(&BinOutGPIOClass::setState, output[1]));

	ApplicationClass::init();

//	webServer.addPath("/temperature.json",HttpPathDelegate(&TempSensors::onHttpGet,tempSensor));
//	webServer.addPath("/thermostat.fan",HttpPathDelegate(&ThermostatClass::onHttpConfig,thermostats[0]));
//	webServer.addPath("/thermostat.pump",HttpPathDelegate(&ThermostatClass::onHttpConfig,thermostats[1]));
//	webServer.addPath("/fan",HttpPathDelegate(&FanClass::onHttpConfig,fan));
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
//	Serial.printf("GPIO 15: %d GPIO 16: %d\n", input[0]->getState(), input[1]->getState());
}

//void monitor(HttpRequest &request, HttpResponse &response)
//{
//	JsonObjectStream* stream = new JsonObjectStream();
//	JsonObject& json = stream->getRoot();
//
//	response.setHeader("Access-Control-Allow-Origin", "*");
//	response.sendJsonObject(stream);
//}
