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
const uint8_t mcpCount = 4; //Number of MCP23*17 ICs

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

const uint8_t ioCount = 27;
const uint8_t outputsCount = ioCount;
const uint8_t inputsCount = ioCount;
const uint8_t allOffId = 27;

const uint8_t shuttersDuration = 5; //Shutters motor duration to edge position in seconds
enum class shutters : int {open,close}; // Constants for access array
const uint8_t shuttersOutputs[][2]{ {15,16}, {17,18}, {19,20}, {21,22}, {23,24}, {25,26}}; // Outputs id of shutters

extern BinOutClass* outputs[outputsCount];

extern BinInPollerClass binInPoller;
extern AntiTheftClass* antiTheft;

