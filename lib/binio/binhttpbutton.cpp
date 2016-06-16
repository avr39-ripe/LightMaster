/*
 * binhttpbutton.cpp
 *
 *  Created on: 16 июня 2016 г.
 *      Author: shurik
 */

#include <binhttpbutton.h>

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
			Serial.print(request.getBody());
			DynamicJsonBuffer jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(request.getBody());
//Uncomment next line for extra debuginfo
			root.prettyPrintTo(Serial);
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
				data["state"] = _buttons[id]->state.get();
			}
		}
		else
		{
			uint8_t id = request.getQueryParameter("button").toInt();
			if (id >= 0 && id < _buttons.count())
			{
				root["name"] = _buttons[id]->getName();
				root["state"] = _buttons[id]->state.get();
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
