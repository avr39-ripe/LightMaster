/*
 * lightsystem.cpp
 *
 *  Created on: 22 июня 2016 г.
 *      Author: shurik
 */

#include <lightsystem.h>

LightSystemClass::LightSystemClass()
{
	_loadBinConfig();
	_allOffState = new BinStateClass();
	_allOffState->onChange(onStateChangeDelegate(&LightSystemClass::toggleAllOff, this));
	_allOffState->persistent(0);
}


void LightSystemClass::addLightGroup(BinOutClass* output, BinInClass* input, BinHttpButtonClass* httpButton)
{
	if (output)
	{
		_outputs.add(output);
	}
	if (input)
	{
		_inputs.add(input);
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));
	}
	if (httpButton)
	{
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, &output->state));
	}
}

void LightSystemClass::addAllOffGroup(BinOutClass* output, BinInClass* input, BinHttpButtonClass* httpButton)
{
	if (output)
	{
		_allOffOutput = output;
		if (_allOffState)
		{
			delete _allOffState;
			_allOffState = &output->state;
			_allOffState->onChange(onStateChangeDelegate(&LightSystemClass::toggleAllOff, this));
			_allOffState->persistent(1);
		}
	}

	if (input)
	{
		_inputs.add(input);
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, _allOffState));
	}
	if (httpButton)
	{
		httpButton->addOutState(_allOffState);
//		_turnAllState.onChange(onStateChangeDelegate(&BinHttpButtonClass::wsSendButton, httpButton));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, _allOffState));
	}
}
void LightSystemClass::toggleAllOff(uint8_t state)
{
	if (!state)
	{
//		Serial.println("TurnPREVAll");
//		for (uint8_t i = 0; i < _outputs.count(); i++)
//		{
//			_outputs[i]->state.set(_outputs[i]->state.getPrev());
//		}
//		Serial.printf("TurnAllState: %s\n", _allOffState.get() ? "true" : "false");
	}
	else
	{
		Serial.println("TurnOFFAll");
		for (uint8_t i = 0; i < _outputs.count(); i++)
		{
			_outputs[i]->state.set(false);
		}
		Serial.printf("TurnAllState: %s\n", _allOffState->get() ? "true" : "false");
	}
}

uint16_t LightSystemClass::getRandom(uint16_t min, uint16_t max)
{
//	float interval = max - min;
//	long rand = os_random();
//	return min + lround((labs(rand) * interval) / LONG_MAX);
////	Serial.printf("Random long: %d, Random idx: %d\n", rand, idx);
////	Serial.println((labs(rand) * 7.0 ) / LONG_MAX);
	return random(min,max);
}

void LightSystemClass::_randomTurnOn()
{
	_randomLightGroupIdx = getRandom(0, _outputs.count());
	_outputs[_randomLightGroupIdx]->state.set(true);
	uint8_t onTime = getRandom(_minOn, _maxOn);
	Serial.printf("Random ON, %d: for %d seconds\n", _randomLightGroupIdx, onTime);
	_randomTimer.initializeMs(onTime * 60000, TimerDelegate(&LightSystemClass::_randomTurnOff, this)).start(true);
}

void LightSystemClass::_randomTurnOff()
{
	_outputs[_randomLightGroupIdx]->state.set(false);
	uint8_t offTime = getRandom(_minOff, _maxOff);
	Serial.printf("Random OFF, %d: wait for %d seconds\n", _randomLightGroupIdx, offTime);
	_randomTimer.initializeMs(offTime * 60000, TimerDelegate(&LightSystemClass::_randomTurnOn, this)).start(true);
}

void LightSystemClass::randomLight(uint8_t state)
{
	if (state)
	{
		Serial.printf("Random ON!!!");
		_randomTurnOn();
	}
	else
	{
		Serial.printf("Random OFF!!!");
		if (_randomTimer.isStarted())
		{
			_outputs[_randomLightGroupIdx]->state.set(false);
			_randomTimer.stop();
		}

	}
}

void LightSystemClass::addRandomButton(BinHttpButtonClass* httpButton)
{
	if (httpButton)
	{
		_randomState = new BinStateClass();
		_randomState->onChange(onStateChangeDelegate(&LightSystemClass::_randomEnabler, this));
		_randomState->persistent(2);

		httpButton->addOutState(_randomState);
//		_turnAllState.onChange(onStateChangeDelegate(&BinHttpButtonClass::wsSendButton, httpButton));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, _randomState));
	}
}

void LightSystemClass::onWSReceiveRandom(JsonObject& jsonRoot)
{
	uint8_t needSave = false;

	if (jsonRoot["startTime"].success())
	{
		_randomStartTime = jsonRoot["startTime"];
		needSave = true;
	}
	if (jsonRoot["stopTime"].success())
	{
		_randomStopTime = jsonRoot["stopTime"];
		needSave = true;
	}
	if (jsonRoot["minOn"].success())
	{
		_minOn = jsonRoot["minOn"];
		needSave = true;
	}
	if (jsonRoot["maxOn"].success())
	{
		_maxOn = jsonRoot["maxOn"];
		needSave = true;
	}
	if (jsonRoot["minOff"].success())
	{
		_minOff = jsonRoot["minOff"];
		needSave = true;
	}
	if (jsonRoot["maxOff"].success())
	{
		_maxOff = jsonRoot["maxOff"];
		needSave = true;
	}

	if (needSave)
	{
		_saveBinConfig();
	}
}

void LightSystemClass::onWSGetRandom(WebSocket& socket)
{
	DynamicJsonBuffer jsonBuffer;
	String buf;
	JsonObject& root = jsonBuffer.createObject();
	root["response"] = "getRandom";

	root["startTime"] = _randomStartTime;
	root["stopTime"] = _randomStopTime;
	root["minOn"] = _minOn;
	root["maxOn"] = _maxOn;
	root["minOff"] = _minOff;
	root["maxOff"] = _maxOff;

	root.printTo(buf);
	socket.sendString(buf);

}

void LightSystemClass::_saveBinConfig()
{
	Serial.printf("Try to save bin cfg..\n");
	file_t file = fileOpen("lightSys.cfg", eFO_CreateIfNotExist | eFO_WriteOnly);
	fileWrite(file, &_randomStartTime, sizeof(_randomStartTime));
	fileWrite(file, &_randomStopTime, sizeof(_randomStopTime));
	fileWrite(file, &_minOn, sizeof(_minOn));
	fileWrite(file, &_maxOn, sizeof(_maxOn));
	fileWrite(file, &_minOff, sizeof(_minOff));
	fileWrite(file, &_maxOff, sizeof(_maxOff));
	fileClose(file);
}

void LightSystemClass::_loadBinConfig()
{
	Serial.printf("Try to load bin cfg..\n");
	if (fileExist("lightSys.cfg"))
	{
		Serial.printf("Will load bin cfg..\n");
		file_t file = fileOpen("lightSys.cfg", eFO_ReadOnly);
		fileSeek(file, 0, eSO_FileStart);
		fileRead(file, &_randomStartTime, sizeof(_randomStartTime));
		fileRead(file, &_randomStopTime, sizeof(_randomStopTime));
		fileRead(file, &_minOn, sizeof(_minOn));
		fileRead(file, &_maxOn, sizeof(_maxOn));
		fileRead(file, &_minOff, sizeof(_minOff));
		fileRead(file, &_maxOff, sizeof(_maxOff));
		fileClose(file);
	}
}

void LightSystemClass::_randomEnabler(uint8_t state)
{
	if (state)
	{
		_randomEnablerTimer.initializeMs(60000, TimerDelegate(&LightSystemClass::_randomEnablerCheck, this)).start(true);
		_randomEnablerCheck();
	}
	else
	{
		randomLight(false);
		_randomEnablerTimer.stop();
	}
}

void LightSystemClass::_randomEnablerCheck()
{
	DateTime _now = SystemClock.now(eTZ_Local);
	uint16_t _now_minutes = _now.Hour * 60 + _now.Minute;

	if ((_now_minutes >= _randomStartTime) && (_now_minutes <= _randomStopTime))
	{
		Serial.println("RandomChecker in range!");
		if (!_randomTimer.isStarted())
		{
			Serial.print(_now.toFullDateTimeString()); Serial.println(" Turn Random ON! ");
			randomLight(true);
		}
	}
	else
	{
		Serial.println("RandomChecker out of range!");
		if (_randomTimer.isStarted())
		{
			Serial.print(_now.toFullDateTimeString()); Serial.println(" Sleep time for Random! ");
			randomLight(false);
		}
	}
}
