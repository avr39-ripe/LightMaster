/*
 * binstate.cpp
 *
 *  Created on: 18 апр. 2016 г.
 *      Author: shurik
 */
#include <binstate.h>

BinStateClass::BinStateClass(uint8_t polarity, uint8_t toggleActive)
{
	polarity ? _state |= polarityBit : _state &= ~(polarityBit);
	toggleActive ? _state |= toggleActiveBit : _state &= ~(toggleActiveBit);
}

void BinStateClass::set(uint8_t state, uint8_t forceDelegatesCall)
{
//	uint8_t prevState = _state;

	_setPrev(get());

//	_state = state;
	state ? _setState(getPolarity()) : _setState(!getPolarity());

	Serial.printf("State set to: %s\n", get() ? "true" : "false");
	Serial.printf("prevState set to: %s\n", getPrev() ? "true" : "false");
	if (_onSet)
	{
		_onSet(getRawState()); //Call some external delegate on setting ANY state
	}

	if (get() != getPrev() || forceDelegatesCall)
	{
		_callOnChangeDelegates(); //Call external delegates on state CHANGE
	}
}

void BinStateClass::toggle(uint8_t state)
{
	Serial.println("Toggle called\n");
	if (state == getToggleActive())
	{
		set(!get());
		Serial.println("Toggle TOGGLED!\n");
	}
}

void BinStateClass::onSet(onStateChangeDelegate delegateFunction)
{
	_onSet = delegateFunction;
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
		_onChange[i](getRawState());
	}

	for (uint8_t i = 0; i < _onChangeInverse.count(); i++)
	{
		_onChangeInverse[i](!getRawState());
	}
}

