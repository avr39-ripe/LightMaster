/*
 * dragonmaster.h
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */

#ifndef INCLUDE_LIGHTMASTER_H_
#define INCLUDE_LIGHTMASTER_H_
#include <SmingCore/SmingCore.h>
#include <Libraries/MCP23S17/MCP23S17.h>
#include <wsbinconst.h>
#include <binin.h>
#include <binout.h>
#include <lightgroup.h>
#include <binhttpbutton.h>
#include <lightsystem.h>
#include <bincycler.h>

//output mode selector: GPIO or MCP23S17
//#define MCP23S17

extern NtpClient* ntpClient;

#ifdef MCP23S17 //use MCP23S17
const uint8_t mcp23s17_cs = 15;
extern MCP* mcp000;
#endif

extern BinInPollerClass binInPoller;
extern LightSystemClass* lightSystem;

extern BinStateClass* binStates[7];
extern BinCyclerClass* binCycler;
#endif /* INCLUDE_LIGHTMASTER_H_ */
