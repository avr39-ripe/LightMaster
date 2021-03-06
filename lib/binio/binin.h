/*
 * binaryinput.h
 *
 *  Created on: 1 апр. 2016 г.
 *      Author: shurik
 */

#ifndef LIB_BINIO_BINARYINPUT_H_
#define LIB_BINIO_BINARYINPUT_H_
#include <SmingCore/SmingCore.h>
#include <binstate.h>
#include <Libraries/MCP23S17/MCP23S17.h>

class BinInClass
{
public:
	BinInClass(uint8_t unitNumber, uint8_t polarity);
	virtual ~BinInClass() {};
	uint8_t getUnitNumber() { return _unitNumber; };
	void setUnitNumber(uint8_t unitNumber) { _unitNumber = unitNumber; };
	BinStateClass state;
protected:
	uint8_t _unitNumber = 0;
	void _readState();
	virtual uint8_t _readUnit() = 0;

friend class BinInPollerClass;
};

class BinInGPIOClass : public BinInClass
{
public:
	BinInGPIOClass(uint8_t unitNumber, uint8_t polarity);
	virtual ~BinInGPIOClass() {};
	void setUnitNumber(uint8_t unitNumber);
protected:
	virtual uint8_t _readUnit();
};

class BinInMCP23S17Class : public BinInClass
{
public:
	BinInMCP23S17Class(MCP &mcp, uint8_t unitNumber, uint8_t polarity);
	virtual ~BinInMCP23S17Class() {};
	void setUnitNumber(uint8_t unitNumber);
protected:
	virtual uint8_t _readUnit();
	MCP *_mcp;
};

class BinInPollerClass
{
public:
	BinInPollerClass(uint16_t refresh = 500) { _refresh = refresh; };
	virtual ~BinInPollerClass() {};
	void add(BinInClass* binIn);
	void start();
	void stop();
private:
	void _pollState();
	Vector<BinInClass*> _binIn;
	uint16_t _refresh;
	Timer _refreshTimer;
};

#endif /* LIB_BINIO_BINARYINPUT_H_ */
