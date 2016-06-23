/*
 * lightsystem.cpp
 *
 *  Created on: 22 июня 2016 г.
 *      Author: shurik
 */

#include <lightsystem.h>

LightSystemClass::LightSystemClass()
{
	_turnAllState.onChange(onStateChangeDelegate(&LightSystemClass::turnAll, this));
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

void LightSystemClass::addTurnAllInput(BinInClass* input, BinHttpButtonClass* httpButton)
{
	if (input)
	{
		_inputs.add(input);
		input->state.onChange(onStateChangeDelegate(&BinStateClass::set, &_turnAllState));
	}
	if (httpButton)
	{
		httpButton->addOutputState(&_turnAllState);
//		_turnAllState.onChange(onStateChangeDelegate(&BinHttpButtonClass::wsSendButton, httpButton));
		httpButton->state.onChange(onStateChangeDelegate(&BinStateClass::set, &_turnAllState));
		_binHttpButtons.add(httpButton);
	}
}
void LightSystemClass::turnAll(uint8_t state)
{
	if (!state)
	{
//		Serial.println("TurnPREVAll");
//		for (uint8_t i = 0; i < _outputs.count(); i++)
//		{
//			_outputs[i]->state.set(_outputs[i]->state.getPrev());
//		}
//		Serial.printf("TurnAllState: %s\n", _turnAllState.get() ? "true" : "false");
	}
	else
	{
		Serial.println("TurnOFFAll");
		for (uint8_t i = 0; i < _outputs.count(); i++)
		{
			_outputs[i]->state.set(false);
		}
		Serial.printf("TurnAllState: %s\n", _turnAllState.get() ? "true" : "false");
	}
}
