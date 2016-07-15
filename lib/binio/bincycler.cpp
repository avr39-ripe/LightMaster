/*
 * bincycler.cpp
 *
 *  Created on: 15 июля 2016 г.
 *      Author: shurik
 */

#include "bincycler.h"

BinCyclerClass::BinCyclerClass(BinStateClass& cycleState, uint16_t duration, uint16_t interval)
: _cycleState(cycleState), _duration(duration), _interval(interval)
{
	state.onChange(onStateChangeDelegate(&BinCyclerClass::_enable, this));
}

void BinCyclerClass::_enable(uint8_t state)
{
	if (state)
	{
		_setTrue();
	}
	else
	{
		state.set(false);
		_timer.stop();
	}
}

void BinCyclerClass::_setTrue()
{
	state.set(true);
	_timer.initializeMs(_duration * 1000, TimerDelegate(&BinCyclerClass::_setFalse, this)).start(false);
}

void BinCyclerClass::_setFalse()
{
	state.set(false);
	_timer.initializeMs(_interval * 1000, TimerDelegate(&BinCyclerClass::_setTrue, this)).start(false);
}
