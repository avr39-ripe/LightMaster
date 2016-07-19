/*
 * binhttpbutton.cpp
 *
 *  Created on: 16 июня 2016 г.
 *      Author: shurik
 */

#include <binhttpbutton.h>

//BinHttpButtonClass

BinHttpButtonClass::BinHttpButtonClass(HttpServer& webServer, BinStatesHttpClass& binStatesHttp, uint8_t unitNumber, String name, BinStateClass *outputState)
:BinInClass(unitNumber, 1), _binStatesHttp(binStatesHttp)
{
	if (outputState)
	{
		_binStateHttp = new BinStateHttpClass(webServer, outputState, name, unitNumber, &state);
	}
	else
	{
		_binStateHttp = new BinStateHttpClass(webServer, &state, name, unitNumber, &state);
	}
	_binStatesHttp.add(_binStateHttp);
}
