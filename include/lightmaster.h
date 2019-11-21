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
const uint8_t mcpCount = 1; //Number of MCP23*17 ICs

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
#include <antitheft.h>

extern NtpClient* ntpClient;

const uint8_t ioCount = 8;
const uint8_t outputsCount = ioCount;
const uint8_t inputsCount = ioCount;
const uint8_t allOffId = 7;

extern BinOutClass* outputs[outputsCount];

extern BinInPollerClass binInPoller;
extern AntiTheftClass* antiTheft;

