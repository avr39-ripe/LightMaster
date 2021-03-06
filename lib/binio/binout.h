/*
 * binout.h
 *
 *  Created on: 2 апр. 2016
 *      Author: shurik
 */

#ifndef LIB_BINIO_BINOUT_H_
#define LIB_BINIO_BINOUT_H_
#include <SmingCore/SmingCore.h>
#include <binstate.h>
#include <Libraries/MCP23S17/MCP23S17.h>

class BinOutClass
{
public:
	BinOutClass(uint8_t unitNumber, uint8_t polarity);
	virtual ~BinOutClass() {};
	void setUnitNumber(uint8_t unitNumber) { _unitNumber = unitNumber; };
	BinStateClass state;
protected:
	uint8_t _unitNumber = 0;
	virtual void _setUnitState(uint8_t state) = 0;
};

class BinOutGPIOClass : public BinOutClass
{
public:
	BinOutGPIOClass(uint8_t unitNumber, uint8_t polarity);
	virtual ~BinOutGPIOClass() {};
	void setUnitNumber(uint8_t unitNumber);
protected:
	virtual void _setUnitState(uint8_t state);
};

class BinOutMCP23S17Class : public BinOutClass
{
public:
	BinOutMCP23S17Class(MCP &mcp, uint8_t unitNumber, uint8_t polarity);
	virtual ~BinOutMCP23S17Class() {};
	void setUnitNumber(uint8_t unitNumber);
protected:
	virtual void _setUnitState(uint8_t state);
	MCP *_mcp;
};

#endif /* LIB_BINIO_BINOUT_H_ */
