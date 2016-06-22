/*
 * lightsystem.cpp
 *
 *  Created on: 22 июня 2016 г.
 *      Author: shurik
 */

#include <lightsystem.h>

//LightSystemClass::LightSystemClass() {
//	// TODO Auto-generated constructor stub
//
//}


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
		_binHttpButtons.add(*httpButton);
	}
}
