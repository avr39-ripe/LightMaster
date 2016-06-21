/*
 * binhttpbutton.cpp
 *
 *  Created on: 16 июня 2016 г.
 *      Author: shurik
 */

#include <binhttpbutton.h>

//BinHttpButtonClass

BinHttpButtonClass::BinHttpButtonClass(HttpServer& webServer, uint8_t unitNumber, String name, BinOutClass *output)
:BinInClass(unitNumber, 1), _webServer(webServer), _name(name), _output(output)
{
	if (output)
	{
		output->state.onChange(onStateChangeDelegate(&BinHttpButtonClass::wsSendButton, this));
	}
}
void BinHttpButtonClass::onHttpSetState(HttpRequest &request, HttpResponse &response)
{
	if (request.getRequestMethod() == RequestMethod::POST)
	{
		if (request.getBody() == NULL)
		{
			debugf("Empty Request Body!\n");
			return;
		}
		else // Request Body Not Empty
		{
//Uncomment next line for extra debuginfo
//			Serial.print(request.getBody());
			DynamicJsonBuffer jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(request.getBody());
//Uncomment next line for extra debuginfo
//			root.prettyPrintTo(Serial);
			if (root["state"].success()) // There is loopInterval parameter in json
			{
				state.set(root["state"]);
			}
		} // Request Body Not Empty
	} // Request method is POST
//	else
//	{
//		response.setCache(86400, true); // It's important to use cache for better performance.
//		response.sendFile("config.html");
//	}
}

void BinHttpButtonClass::wsSendButton(uint8_t state)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["response"] = "getButton";
	root["button"] = getUnitNumber();
	root["state"] = state;
//	root.prettyPrintTo(Serial);
	String buf;
	root.printTo(buf);
	WebSocketsList &clients = _webServer.getActiveWebSockets();
	for (int i = 0; i < clients.count(); i++)
	{
		clients[i].sendString(buf);
	}

}
//BinHttpButtonsClass

void BinHttpButtonsClass::add(BinHttpButtonClass &button)
{
	_buttons.add(&button);
}

void BinHttpButtonsClass::onHttp(HttpRequest &request, HttpResponse &response)
{
	if (request.getRequestMethod() == RequestMethod::GET)
	{
		DynamicJsonBuffer jsonBuffer;
		String buf;
		JsonObject& root = jsonBuffer.createObject();
		String queryParam = request.getQueryParameter("button", "-1");
		if (queryParam == "-1")
		{
			for (uint8_t id=0; id < _buttons.count(); id++)
			{
				JsonObject& data = root.createNestedObject((String)id);
				data["name"] = _buttons[id]->getName();
				data["state"] = _buttons[id]->getOutputState();
			}
		}
		else
		{
			uint8_t id = request.getQueryParameter("button").toInt();
			if (id >= 0 && id < _buttons.count())
			{
				root["name"] = _buttons[id]->getName();
				root["state"] = _buttons[id]->getOutputState();
			}
		}

		root.printTo(buf);

		response.setHeader("Access-Control-Allow-Origin", "*");
		response.setContentType(ContentType::JSON);
		response.sendString(buf);
	}
	else //request method POST
	{
		String queryParam = request.getQueryParameter("button", "-1");
		if (queryParam == "-1")
			return;
		else
		{
			uint8_t id = request.getQueryParameter("button").toInt();
			if (id >= 0 && id < _buttons.count())
			{
				_buttons[id]->onHttpSetState(request, response);
			}
		}
	}
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
