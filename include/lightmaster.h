/*
 * dragonmaster.h
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */

#ifndef INCLUDE_LIGHTMASTER_H_
#define INCLUDE_LIGHTMASTER_H_
#include <SmingCore/SmingCore.h>
#include <Libraries/OneWire/OneWire.h>
#include <Libraries/MCP23S17/MCP23S17.h>
#include <wsbinconst.h>
#include <binin.h>
#include <binout.h>
#include <lightgroup.h>
#include <binhttpbutton.h>
#include <lightsystem.h>
#include <bincycler.h>
#include <tempsensors.h>
#include <twvalve.h>

//output mode selector: GPIO or MCP23S17
//#define MCP23S17

extern NtpClient* ntpClient;

#ifdef MCP23S17 //use MCP23S17
const uint8_t mcp23s17_cs = 15;
extern MCP* mcp000;
#endif

extern TWValveClass* TWValve;
extern BinInPollerClass binInPoller;

//OneWire stuff
const uint8_t onewire_pin = 2;
extern OneWire ds;
extern TempSensors* tempSensor;

extern BinStateSharedDeferredClass* caldron;
extern BinCyclerClass* binCycler;
#endif /* INCLUDE_LIGHTMASTER_H_ */
