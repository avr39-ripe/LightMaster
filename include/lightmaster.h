/*
 * dragonmaster.h
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */

#ifndef INCLUDE_LIGHTMASTER_H_
#define INCLUDE_LIGHTMASTER_H_
//output mode selector: GPIO or MCP23S17
//#define MCP23S17
#define GPIO_MCP23017

#include <SmingCore/SmingCore.h>
#ifdef MCP23S17 //use MCP23S17
#include <Libraries/MCP23S17/MCP23S17.h>
#endif
#ifdef GPIO_MCP23017 //use MCP23017
#include <Libraries/MCP23017/MCP23017.h>
#endif
#include <wsbinconst.h>
#include <binin.h>
#include <binout.h>
#include <lightgroup.h>
#include <binhttpbutton.h>
#include <lightsystem.h>
#include <bincycler.h>
#include <antitheft.h>

extern NtpClient* ntpClient;

#ifdef MCP23S17 //use MCP23S17
const uint8_t mcp23s17_cs = 15;
extern MCP* mcp000;
extern MCP* mcp001;
extern MCP* mcp002;
#endif

#ifdef GPIO_MCP23017 //use MCP23017
extern MCP23017* mcp000;
extern MCP23017* mcp001;
extern MCP23017* mcp002;
#endif

extern BinOutClass* outputs[24];

extern BinInPollerClass binInPoller;
extern AntiTheftClass* antiTheft;
#endif /* INCLUDE_LIGHTMASTER_H_ */
