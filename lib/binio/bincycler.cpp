/*
 * bincycler.cpp
 *
 *  Created on: 15 июля 2016 г.
 *      Author: shurik
 */

#include <bincycler.h>

BinCyclerClass::BinCyclerClass(BinStateClass& cycleState, uint16_t duration, uint16_t interval)
: _cycleState(cycleState), _duration(duration), _interval(interval)
{
	state.onChange(onStateChangeDelegate(&BinCyclerClass::_enable, this));
}

void BinCyclerClass::_enable(uint8_t enableState)
{
	if (enableState)
	{
		_setTrue();
	}
	else
	{
		_cycleState.set(false);
		_timer.stop();
	}
}

void BinCyclerClass::_setTrue()
{
	_cycleState.set(true);
	_timer.initializeMs(_duration * 60000, TimerDelegate(&BinCyclerClass::_setFalse, this)).start(false);
}

void BinCyclerClass::_setFalse()
{
	_cycleState.set(false);
	_timer.initializeMs(_interval * 60000, TimerDelegate(&BinCyclerClass::_setTrue, this)).start(false);
}
