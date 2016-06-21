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
	BinHttpButtonClass(HttpServer& webServer, uint8_t unitNumber, String name = "Button", BinOutClass *output = nullptr);
//	: BinInClass(unitNumber, 1), _webServer(webServer), _name(name), _output(output) {};
	virtual ~BinHttpButtonClass() {};
	void onHttpGetState(HttpRequest &request, HttpResponse &response);
	void onHttpSetState(HttpRequest &request, HttpResponse &response);
	String getName() { return _name; };
	void setName(String name) { _name = name; };
	uint8_t getOutputState() { return (_output == nullptr) ? this->state.get() : _output->state.get(); };
	void setOutput(BinOutClass *output) { _output = output; };
	void wsSendButton(uint8_t state);
	uint8_t _readUnit() {}; //Do not used here
private:
	String _name; //Name on button to display on the web
	BinOutClass* _output = nullptr; //optional BinOut reference to get state from
	HttpServer& _webServer;
};

class BinHttpButtonsClass
{
public:
	void onHttp(HttpRequest &request, HttpResponse &response);
	void onWSReceiveButton(JsonObject& jsonRoot);
	void add(BinHttpButtonClass &button);
	void setButton(uint8_t button, uint8_t state) { _buttons[button]->state.set(state); };
	uint8_t getButton(uint8_t button) { return _buttons[button]->getOutputState(); };
//	void wsSendButton(uint8_t button, uint8_t state);
private:
	Vector<BinHttpButtonClass*> _buttons;
};
#endif /* LIB_BINIO_BINHTTPBUTTON_H_ */
