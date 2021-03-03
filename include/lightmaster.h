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
const uint8_t mcpCount = 3; //Number of MCP23*17 ICs

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

extern NtpClient* ntpClient;

const uint8_t groupEnd{255}; // end marker for light zones group array

const uint8_t ioCount = 24;
const uint8_t outputsCount = ioCount;
const uint8_t inputsCount = ioCount;
const uint8_t mainCount{22u};
const uint8_t maxLightId{20u};

const uint8_t allOffId{23u};

const uint8_t imHomeId{24u};
//const uint8_t imHomeInputId{mainCount};
const uint8_t imHomeGroup[]{3, 10, 14, groupEnd};


const uint8_t sleepModeId{25};

extern BinOutClass* outputs[outputsCount];

extern BinInPollerClass binInPoller;


