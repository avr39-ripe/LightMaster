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

void BinStateClass::onChange(onStateChangeDelegate delegateFunction, uint8_t polarity)
{
//	OnStateChange onStateChange = {delegateFunction, polarity};
	_onChange.add({delegateFunction, polarity});
}

void BinStateClass::_callOnChangeDelegates()
{
	for (uint8_t i = 0; i < _onChange.count(); i++)
	{
		_onChange[i].delegateFunction(get() ? _onChange[i].polarity : !_onChange[i].polarity);
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

// BinStateHttpClass
BinStateHttpClass::BinStateHttpClass(HttpServer& webServer, BinStateClass* outState, String name, uint8_t uid, BinStateClass* inState)
: _webServer(webServer), _outState(outState), _name(name), _uid(uid), _inState(inState)
{
	_updateLength();
	_outState->onChange(onStateChangeDelegate(&BinStateHttpClass::wsSendStateAll, this));
};

void BinStateHttpClass::_updateLength()
{
	_nameLength = 0;
	char* strPtr = (char*)_name.c_str();

	while ( strPtr[_nameLength] )
	{
//		Serial.printf("strPtr[%u] = %u\n", _nameLength, strPtr[_nameLength]);
		_nameLength++;
	}
//	Serial.printf("strLen = %u\n",_nameLength);
}

void BinStateHttpClass::wsBinGetter(WebSocket& socket, uint8_t* data, size_t size)
{
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scBinStateGetName:
		wsSendName(socket);
		break;
	case wsBinConst::scBinStateGetState:
		wsSendState(socket);
		break;
	}
}

void BinStateHttpClass::_fillNameBuffer(uint8_t* buffer)
{
	buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
	buffer[wsBinConst::wsSysId] = sysId;
	buffer[wsBinConst::wsSubCmd] = wsBinConst::scBinStateGetName;

	os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &_uid, sizeof(_uid));
	os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 1]), _name.c_str(), _nameLength);

}

void BinStateHttpClass::_fillStateBuffer(uint8_t* buffer)
{
	buffer[wsBinConst::wsCmd] = wsBinConst::getResponse;
	buffer[wsBinConst::wsSysId] = sysId;
	buffer[wsBinConst::wsSubCmd] = wsBinConst::scBinStateGetState;

	uint8_t tmpState = getState();

	os_memcpy((&buffer[wsBinConst::wsPayLoadStart]), &_uid, sizeof(_uid));
	os_memcpy((&buffer[wsBinConst::wsPayLoadStart + 1]), &tmpState, sizeof(tmpState));
}
void BinStateHttpClass::wsSendName(WebSocket& socket)
{
	uint8_t* buffer = new uint8_t[wsBinConst::wsPayLoadStart + 1 + _nameLength];
	_fillNameBuffer(buffer);

	socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 1 + _nameLength);
	delete buffer;
}

void BinStateHttpClass::wsSendState(WebSocket& socket)
{
	uint8_t* buffer = new uint8_t[wsBinConst::wsPayLoadStart + 1 + 1];

	_fillStateBuffer(buffer);

	socket.sendBinary(buffer, wsBinConst::wsPayLoadStart + 1 + 1);

	delete buffer;
}

void BinStateHttpClass::wsSendStateAll(uint8_t state)
{
	uint8_t* buffer = new uint8_t[wsBinConst::wsPayLoadStart + 1 + 1];

	_fillStateBuffer(buffer);

	WebSocketsList &clients = _webServer.getActiveWebSockets();
	for (uint8_t i = 0; i < clients.count(); i++)
	{
		clients[i].sendBinary(buffer, wsBinConst::wsPayLoadStart + 1 + 1);
	}

	delete buffer;
}

void BinStateHttpClass::setState(uint8_t state)
{
//	Serial.printf("Inside setState!!!\n");

	if (_inState)
	{
//		Serial.printf("inState set\n");
		_inState->set(state);
	}
//	else
//	{
////		Serial.printf("outState set\n");
//		_outState->set(state);
//	}
};

void BinStatesHttpClass::wsBinGetter(WebSocket& socket, uint8_t* data, size_t size)
{
	uint8_t* buffer = nullptr;
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scBinStatesGetAll:
		for (uint8_t i = 0; i < _binStatesHttp.count(); i++)
		{
			_binStatesHttp.valueAt(i)->wsSendName(socket);
			_binStatesHttp.valueAt(i)->wsSendState(socket);
		}
		break;
	case wsBinConst::scBinStateGetState:
		if (_binStatesHttp.contains(data[wsBinConst::wsGetSetArg]))
		{
			_binStatesHttp[data[wsBinConst::wsGetSetArg]]->wsSendState(socket);
		}
		break;
	}
}

void BinStatesHttpClass::wsBinSetter(WebSocket& socket, uint8_t* data, size_t size)
{
	uint8_t* buffer = nullptr;
//	Serial.printf("BinStatesHttp -> wsBinSetter -> wsGetSetArg = %d\n", data[wsBinConst::wsGetSetArg]);
	switch (data[wsBinConst::wsSubCmd])
	{
	case wsBinConst::scBinStateSetState:
		if (_binStatesHttp.contains(data[wsBinConst::wsGetSetArg]))
		{
//			Serial.printf("BinStatesHttp -> wsBinSetter -> wsGetSetArg -> ELEMENT FOUND!\n");
			_binStatesHttp[data[wsBinConst::wsGetSetArg]]->setState(data[wsBinConst::wsPayLoadStartGetSetArg]);
		}
		break;
	}
}
//BinStateSharedDeferredClass

void BinStateSharedDeferredClass::set(uint8_t state)
{
	if ( !state && _consumers > 0 )
	{
		_consumers--;
		Serial.printf("Decrease consumers = %d\n", _consumers);
	}

	if ( _consumers == 0 && ( (state && _trueDelay > 0) || (!state && _falseDelay > 0) ) )
	{
		_setDeferredState(state);
		_delayTimer.initializeMs( (state ? _trueDelay : _falseDelay) * 1000, TimerDelegate(&BinStateSharedDeferredClass::_deferredSet, this)).start(false);
		Serial.printf("Arm deferred %s\n", state ? "True" : "False");
	}

	if ( _consumers == 0 && ( (state && _trueDelay == 0) || (!state && _falseDelay == 0) ) )
	{
		Serial.printf("Fire nodelay %s\n", state ? "true" : "false");
		_delayTimer.stop();
		BinStateClass::set(state, false);
	}

	if ( state )
	{
		_consumers++;
		Serial.printf("Increase consumers = %d\n", _consumers);
	}
}

void BinStateSharedDeferredClass::_deferredSet()
{
	Serial.printf("Fire deferred %s\n", _getDefferedState() ? "true" : "false");
	BinStateClass::set(_getDefferedState(), false);
}
