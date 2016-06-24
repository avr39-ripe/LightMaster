/*
 * lightsystem.cpp
 *
 *  Created on: 22 июня 2016 г.
 *      Author: shurik
 */

#include <lightsystem.h>

LightSystemClass::LightSystemClass()
{
	_allOffState = new BinStateClass();
	_allOffState->onChange(onStateChangeDelegate(&LightSystemClass::toggleAllOff, this));
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
		_binHttpButtons.add(httpButton);
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
		}
	}

	if (input)
	{
		_inputs.add(input);
		input->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, _allOffState));
	}
	if (httpButton)
	{
		httpButton->addOutputState(_allOffState);
//		_turnAllState.onChange(onStateChangeDelegate(&BinHttpButtonClass::wsSendButton, httpButton));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, _allOffState));
		_binHttpButtons.add(httpButton);
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
	float interval = max - min;
	long rand = os_random();
	return min + lround((labs(rand) * interval) / LONG_MAX);
//	Serial.printf("Random long: %d, Random idx: %d\n", rand, idx);
//	Serial.println((labs(rand) * 7.0 ) / LONG_MAX);
}

void LightSystemClass::_randomTurnOn()
{
	randomLightGroupIdx = getRandom(0, _outputs.count());
	_outputs[randomLightGroupIdx]->state.set(true);
	uint8_t onTime = getRandom(minOn, maxOn);
	Serial.printf("Random ON, %d: for %d seconds\n", randomLightGroupIdx, onTime);
	_randomTimer.initializeMs(onTime * 1000, TimerDelegate(&LightSystemClass::_randomTurnOff, this)).start(true);
}

void LightSystemClass::_randomTurnOff()
{
	_outputs[randomLightGroupIdx]->state.set(false);
	uint8_t offTime = getRandom(minOff, maxOff);
	Serial.printf("Random OFF, %d: wait for %d seconds\n", randomLightGroupIdx, offTime);
	_randomTimer.initializeMs(offTime * 1000, TimerDelegate(&LightSystemClass::_randomTurnOn, this)).start(true);
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
		_outputs[randomLightGroupIdx]->state.set(false);
		_randomTimer.stop();

	}
}

void LightSystemClass::addRandomButton(BinHttpButtonClass* httpButton)
{
	if (httpButton)
	{
		_randomState = new BinStateClass();
		_randomState->onChange(onStateChangeDelegate(&LightSystemClass::randomLight, this));

		httpButton->addOutputState(_randomState);
//		_turnAllState.onChange(onStateChangeDelegate(&BinHttpButtonClass::wsSendButton, httpButton));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::toggle, _randomState));
		_binHttpButtons.add(httpButton);
	}
}
