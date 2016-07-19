/*
 * binhttpbutton.cpp
 *
 *  Created on: 16 июня 2016 г.
 *      Author: shurik
 */

#include <binhttpbutton.h>

//BinHttpButtonClass

BinHttpButtonClass::BinHttpButtonClass(HttpServer& webServer, BinStatesHttpClass& binStatesHttp, uint8_t unitNumber, String name, BinStateClass *outputState)
:BinInClass(unitNumber, 1), _binStatesHttp(binStatesHttp), _name(name), _outputState(outputState)
{
	if (outputState)
	{
		_binStateHttp = new BinStateHttpClass(webServer, *outputState, name, unitNumber, &state);
	}
	else
	{
		_binStateHttp = new BinStateHttpClass(webServer, state, name, unitNumber, &state);
	}

}

void BinHttpButtonClass::addOutputState(BinStateClass *outputState)
{
	if (outputState)
	{
		_outputState = outputState;
		outputState->onChange(onStateChangeDelegate(&BinHttpButtonClass::wsSendButton, this));
	}
}

//BinHttpButtonsClass

void BinHttpButtonsClass::add(BinHttpButtonClass* button)
{
	_buttons.add(button);
}

void BinHttpButtonsClass::onWSReceiveButton(JsonObject& jsonRoot)
{
	if (jsonRoot["button"].success())
	{
		if (jsonRoot["state"].success())
		{
			uint8_t button = jsonRoot["button"];
			uint8_t state = jsonRoot["state"];
			_buttons[button]->state.set(state);
		}
	}
}

void BinHttpButtonsClass::onWSGetButtons(WebSocket& socket)
{
	DynamicJsonBuffer jsonBuffer;
	String buf;
	JsonObject& root = jsonBuffer.createObject();
	root["response"] = "getButtons";
	for (uint8_t id=0; id < _buttons.count(); id++)
	{
		JsonObject& data = root.createNestedObject((String)id);
		data["name"] = _buttons[id]->getName();
		data["state"] = _buttons[id]->getOutputState();
	}
	root.printTo(buf);
	socket.sendString(buf);

}
