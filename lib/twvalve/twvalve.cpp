/*
 * twvalve.cpp
 *
 *  Created on: 14 марта 2016
 *      Author: shurik
 */

#include <twvalve.h>

//TWValve

TWValveClass::TWValveClass(TempSensors &tempSensors, uint8_t sensorId, BinStateClass& warmState, BinStateClass& coldState)
: _tempSensors(&tempSensors), _sensorId(sensorId), _warmState(warmState), _coldState(coldState)
{
	state.onChange(onStateChangeDelegate(&TWValveClass::_enable, this));
// turn of both switches for sure
	_valveTurn(TWValveDirection::STOP, 0);
}

void TWValveClass::_enable(uint8_t enableState)
{
	if (enableState)
	{
		start();
	}
	else
	{
		stop();
	}
}

void TWValveClass::setTermostatUpdateTime(uint16_t refresh)
{
	_refresh = refresh;

	if(_refreshTimer.isStarted())
	{
		_thermostat();
		_refreshTimer.initializeMs(_refresh*1000, TimerDelegate(&TWValveClass::_thermostat, this)).start(true);
	}
};

void TWValveClass::start()
{
	if (_consumers == 0)
	{
		_thermostat();
		_refreshTimer.initializeMs(_refresh*1000, TimerDelegate(&TWValveClass::_thermostat, this)).start(true);
	}
	_consumers++;
}

void TWValveClass::stop()
{
	if (_consumers > 0)
		_consumers--;
	if (_consumers == 0)
	{
		_refreshTimer.stop();
		_valveTurn(TWValveDirection::COLD, _valveEdgeTime); //Default to full cold position
	}
}

void TWValveClass::_valveTurn(uint8_t direction, uint16_t moveTime)
{
	_valveDirection = direction;

	switch (_valveDirection)
	{
	case TWValveDirection::WARM:
		_coldState.set(false);
		_warmState.set(true);
//		setState(out_reg, _coldPin, false); //OFF
//		setState(out_reg, _warmPin, true); //ON
		Serial.printf("TurnON WARM\n");
		break;
	case TWValveDirection::COLD:
		_warmState.set(false);
		_coldState.set(true);
//		setState(out_reg, _warmPin, false); //OFF
//		setState(out_reg, _coldPin, true); //ON
		Serial.printf("TurnON COLD\n");
		break;
	case TWValveDirection::STOP:
		_warmState.set(false);
		_coldState.set(false);
//		setState(out_reg, _warmPin, false); //OFF
//		setState(out_reg, _coldPin, false); //OFF
		Serial.printf("TurnOFF ALL\n");
		return; //DO NOT ARM TIMER!
		break;
	};
	_stopperTimer.initializeMs(moveTime*1000, TimerDelegate(&TWValveClass::_valveStop, this)).start(false);
}

void TWValveClass::_valveStop()
{
	switch (_valveDirection)
	{
	case TWValveDirection::WARM:
		_warmState.set(false);
//		setState(out_reg, _warmPin, false); //OFF
		Serial.printf("TurnOFF WARM\n");
		break;
	case TWValveDirection::COLD:
		_coldState.set(false);
//		setState(out_reg, _coldPin, false); //OFF
		Serial.printf("TurnOFF COLD\n");
		break;
	};
}

void TWValveClass::_thermostat()
{
	float currentTemp = _tempSensors->getTemp(_sensorId);
	//TODO: do something if temperature is invalid!!
	Serial.printf("3-Way Valve temp: "); Serial.println(currentTemp);

	//above range
	if ( currentTemp > (_targetTemp + _targetTempDelta) )
	{
		Serial.printf("Above tT+tTD = "); Serial.println(_targetTemp + _targetTempDelta);
		_valveTurn(TWValveDirection::COLD, _valveStepTime);
		return;
	}
	//below range
	if ( currentTemp < (_targetTemp - _targetTempDelta) )
	{
		Serial.printf("Below tT+tTD = "); Serial.println(_targetTemp - _targetTempDelta);
		_valveTurn(TWValveDirection::WARM, _valveStepTime);
		return;
	}
	//in range
	if ( (_targetTemp - _targetTempDelta) <= currentTemp <= (_targetTemp + _targetTempDelta) )
	{
		Serial.printf("In range!\n");
		_valveTurn(TWValveDirection::STOP, 0);
		return;
	}
}
