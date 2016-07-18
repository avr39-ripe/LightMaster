/*
 * binhttpbutton.h
 *
 *  Created on: 16 июня 2016 г.
 *      Author: shurik
 */

#ifndef LIB_BINIO_BINHTTPBUTTON_H_
#define LIB_BINIO_BINHTTPBUTTON_H_
#include <SmingCore/SmingCore.h>
#include <binin.h>
#include <binout.h>

class BinHttpButtonClass : public BinInClass
{
public:
	BinHttpButtonClass(HttpServer& webServer, uint8_t unitNumber, String name = "Button", BinStateClass *outputState = nullptr);
	virtual ~BinHttpButtonClass() {};
	String getName() { return _name; };
	void setName(String name) { _name = name; };
	uint8_t getOutputState() { return (_outputState == nullptr) ? this->state.get() : _outputState->get(); };
	void addOutputState(BinStateClass *outputState);
	void wsSendButton(uint8_t state);
	uint8_t _readUnit() {}; //Do not used here
private:
	String _name; //Name on button to display on the web
	BinStateClass* _outputState = nullptr; //optional BinStateClass reference to get state from
	HttpServer& _webServer;
};

class BinHttpButtonsClass
{
public:
	void onWSReceiveButton(JsonObject& jsonRoot);
	void add(BinHttpButtonClass* button);
	void onWSGetButtons(WebSocket& socket);
private:
	Vector<BinHttpButtonClass*> _buttons;
};
#endif /* LIB_BINIO_BINHTTPBUTTON_H_ */
