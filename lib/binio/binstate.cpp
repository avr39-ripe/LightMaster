/*
 * binstate.cpp
 *
 *  Created on: 18 апр. 2016 г.
 *      Author: shurik
 */
#include <binstate.h>


void BinStateClass::set(uint8_t state, uint8_t forceDelegatesCall)
{
	uint8_t prevState = _state;
	_state = state;
//	Serial.printf("Thermostat %s: %s\n", _name.c_str(), _state ? "true" : "false");
	if (_state != prevState || forceDelegatesCall)
	{
		_callOnChangeDelegates();
	}
}

void BinStateClass::onChange(onStateChangeDelegate delegateFunction, uint8_t directState)
{
	if (directState)
	{
		_onChange.add(delegateFunction);
	}
	else
	{
		_onChangeInverse.add(delegateFunction);
	}

}

void BinStateClass::_callOnChangeDelegates()
{
	for (uint8_t i = 0; i < _onChange.count(); i++)
	{
		_onChange[i](_state);
	}

	for (uint8_t i = 0; i < _onChangeInverse.count(); i++)
	{
		_onChangeInverse[i](!_state);
	}
}

