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
#include <bincycler.h>
#include <antitheft.h>

extern NtpClient* ntpClient;

const uint8_t groupEnd{255}; // end marker for light zones group array

const uint8_t ioCount = 24;
const uint8_t outputsCount = ioCount;
const uint8_t inputsCount = ioCount;
const uint8_t mainCount{14u};
const uint8_t additionalCount{2u};
const uint8_t nightCount{6u};
const uint8_t allOffId{22u};
const uint8_t shuttersAllOffId{23u};
const uint8_t imHomeId{24u};
const uint8_t imHomeInputId{mainCount};
const uint8_t imHomeDuration{10}; // I'm home group turn on duration in seconds
const uint8_t imHomeGroup[]{3, 10, 14, groupEnd};

const uint8_t nightChildrenId{25};
const uint8_t nightChildrenInputId{mainCount+1};
const uint8_t nightBedroomId{26};
const uint8_t nightBedroomInputId{mainCount+2};
const uint8_t nightManualId{27};
const uint8_t nightDuration{10}; // Night group turn on duration in seconds

const uint8_t nightChildrenGroup[]{16,17,18,19, groupEnd};
const uint8_t nightBedroomGroup[]{20,21, groupEnd};

const uint16_t shuttersDuration = 300; //Shutters motor duration to edge position in seconds
enum class shutters : int {open,close}; // Constants for access array
const uint8_t shuttersOutputs[][2]{ {15,16}, {17,18}, {19,20}, {21,22}, {23,24}, {25,26}}; // Outputs id of shutters

extern BinOutClass* outputs[outputsCount];

extern BinInPollerClass binInPoller;


