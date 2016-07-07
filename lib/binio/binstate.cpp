/*
 * binstate.cpp
 *
 *  Created on: 18 апр. 2016 г.
 *      Author: shurik
 */
#include <binstate.h>
#include <wsbinconst.h>

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

void BinStateHttpClass::_updateLength()
{
	_nameLength = 0;
	char* strPtr = (char*)_name.c_str();

	while ( strPtr[_nameLength] )
	{
		Serial.printf("strPtr[%u] = %u\n", _nameLength, strPtr[_nameLength]);
		_nameLength++;
	}
	Serial.printf("strLen = %u\n",_nameLength);
}

void BinStateHttpClass::wsBinGetter(WebSocket& socket, uint8_t* data, size_t size)
{
	uint8_t* buffer = nullptr;
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scBinStateGetName:

		buffer = new uint8_t[wsBinConst::wsPayLoadStart + 1 + _nameLength];
		buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
		buffer[wsBinConst::wsSysId] = sysId;
		buffer[wsBinConst::wsSubCmd] = wsBinConst::scBinStateGetName;

		os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &_uid, sizeof(_uid));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 1]), _name.c_str(), _nameLength);
		socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 1 + _nameLength);
		break;
	case wsBinConst::scBinStateGetState:
		buffer = new uint8_t[wsBinConst::wsPayLoadStart + 1 + 1];
		buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
		buffer[wsBinConst::wsSysId] = sysId;
		buffer[wsBinConst::wsSubCmd] = wsBinConst::scBinStateGetState;

		uint8_t tmpState = _state.get();

		os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &_uid, sizeof(_uid));
		os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 1]), &tmpState, sizeof(tmpState));
		socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 1 + 1);
		break;
	}

	if (buffer)
	{
		delete buffer;
	}
}
