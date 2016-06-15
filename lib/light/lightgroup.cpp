/*
 * lightgroup.cpp
 *
 *  Created on: 10 июня 2016
 *      Author: shurik
 */

#include <lightgroup.h>

// LightGroupClass
LightGroupClass::LightGroupClass(BinOutClass &output, uint8_t toggleActive)
{
	state.onChange(onStateChangeDelegate(&BinStateClass::set, &output.state));
	 _output = &output;
	_toggleActive = toggleActive;
}
void LightGroupClass::toggle(uint8_t extState)
{
	if ( extState == _toggleActive)
	{
		state.set(!state.get());
	}
}

void LightGroupClass::addInput(BinInClass &input)
{
	input.onStateChange(onStateChangeDelegate(&BinStateClass::toggle, &this->state));

}
