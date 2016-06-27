/*
 * binstate.cpp
 *
 *  Created on: 18 апр. 2016 г.
 *      Author: shurik
 */
#include <binstate.h>

BinStateClass::BinStateClass(uint8_t polarity, uint8_t toggleActive)
{
	polarity ? _state |= BinState::polarityBit : _state &= ~(BinState::polarityBit);
	toggleActive ? _state |= BinState::toggleActiveBit : _state &= ~(BinState::toggleActiveBit);
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
		_onSet(get()); //Call some external delegate on setting ANY state
	}

	if (get() != getPrev() || forceDelegatesCall)
	{
		_callOnChangeDelegates(); //Call external delegates on state CHANGE

		if ( (_state & BinState::persistentBit) != 0 )
		{
			_saveBinConfig();
		}
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
		_onChange[i](get());
	}

	for (uint8_t i = 0; i < _onChangeInverse.count(); i++)
	{
		_onChangeInverse[i](!get());
	}
}

void BinStateClass::_saveBinConfig()
{
	Serial.printf("Try to save bin cfg..\n");
	file_t file = fileOpen(String(".state" + _uid), eFO_CreateIfNotExist | eFO_WriteOnly);
	fileWrite(file, &_state, sizeof(_state));
	fileClose(file);
}

void BinStateClass::_loadBinConfig()
{
	uint8_t tempState = 0;
	Serial.printf("Try to load bin cfg..\n");
	if (fileExist(String(".state" + _uid)))
	{
		Serial.printf("Will load bin cfg..\n");
		file_t file = fileOpen(String(".state" + _uid), eFO_ReadOnly);
		fileSeek(file, 0, eSO_FileStart);
		fileRead(file, &tempState, sizeof(tempState));
		fileClose(file);

		set( tempState & BinState::stateBit ? getPolarity() : !getPolarity() ); //To properly call all delegates and so on
	}
}

void BinStateClass::persistent(uint8_t uid)
{
	_uid = uid;
	_loadBinConfig();
	_state |= BinState::persistentBit;
}
