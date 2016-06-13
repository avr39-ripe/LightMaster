/*
 * binaryinput.cpp
 *
 *  Created on: 1 апр. 2016 г.
 *      Author: shurik
 */

#include <binin.h>

// BinInClass

BinInClass::BinInClass(uint8_t unitNumber, uint8_t polarity)
{
	_unitNumber = unitNumber;
	_polarity = polarity;
}

void BinInClass::onStateChange(onStateChangeDelegate delegateFunction)
{
	_onChangeState = delegateFunction;
}

void BinInClass::_readState()
{
	uint8_t prevState;
	prevState = _state;
	_state = _readUnit() ? _polarity : !(_polarity);
	if (prevState != _state && _onChangeState)
	{
		Serial.printf("onChangeState Delegate/CB called!\n");
		_onChangeState(_state);
	}
}

//BinInGPIOClass
BinInGPIOClass::BinInGPIOClass(uint8_t unitNumber, uint8_t polarity)
:BinInClass(unitNumber, polarity)
{
	pinMode(_unitNumber, INPUT);
}

void BinInGPIOClass::setUnitNumber(uint8_t unitNumber)
{
	BinInClass::setUnitNumber(unitNumber);
	pinMode(_unitNumber, INPUT);
}

uint8_t BinInGPIOClass::_readUnit()
{
	return digitalRead(_unitNumber);
}

//BinInMCP23S17Class
BinInMCP23S17Class::BinInMCP23S17Class(MCP &mcp, uint8_t unitNumber, uint8_t polarity)
:BinInClass(unitNumber, polarity)
{
	_mcp = &mcp;
//	_mcp->pinMode(_unitNumber, INPUT);
}

void BinInMCP23S17Class::setUnitNumber(uint8_t unitNumber)
{
	BinInClass::setUnitNumber(unitNumber);
//	pinMode(_unitNumber, INPUT);
}

uint8_t BinInMCP23S17Class::_readUnit()
{
	return _mcp->digitalRead(8 + _unitNumber);
}

// BinInPollerClass

void BinInPollerClass::start()
{
	_refreshTimer.initializeMs(_refresh, TimerDelegate(&BinInPollerClass::_pollState, this)).start(true);
}

void BinInPollerClass::stop()
{
	_refreshTimer.stop();
}

void BinInPollerClass::add(BinInClass * binIn)
{
	_binIn.add(binIn);
}

void BinInPollerClass::_pollState()
{
	for (uint8_t id=0; id < _binIn.count(); id++)
	{
		_binIn[id]->_readState();
	}
}
