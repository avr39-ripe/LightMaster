/*
 * dragonmaster.h
 *
 *  Created on: 30 марта 2016
 *      Author: shurik
 */

#pragma once
//output mode selector: GPIO or MCP23S17
//#define MCP23S17
#define GPIO_MCP23017

#if defined(MCP23S17) || defined(GPIO_MCP23017)
const uint8_t mcpCount = 2; //Number of MCP23*17 ICs

#endif
#include <SmingCore.h>
#ifdef MCP23S17 //use MCP23S17
#include <Libraries/MCP23S17/MCP23S17.h>
#endif
#ifdef GPIO_MCP23017 //use MCP23017
#include <Libraries/MCP23017/MCP23017.h>
#endif
#include <wsbinconst.h>
#include <binin.h>
#include <binout.h>
#include <binhttpbutton.h>
#include <bincycler.h>

extern NtpClient* ntpClient;

const uint8_t ioCount = 15;
const uint8_t outputsCount = ioCount;
const uint8_t inputsCount = ioCount;
const uint8_t allOffId = 15;
const uint8_t closeAllId{15};
const uint8_t closeAllWebId{16};
const uint8_t openAllWebId{15};
const uint8_t maxShuttersId{11};

enum class shutters : int {open,close}; // Constants for access array
const uint8_t shuttersOutputs[][2]{ {0,1}, {2,3}, {4,5}, {6,7}, {8,9}, {10,11}}; // Outputs id of shutters

extern BinOutClass* outputs[outputsCount];

extern BinInPollerClass binInPoller;


