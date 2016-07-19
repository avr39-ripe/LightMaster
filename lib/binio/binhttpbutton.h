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
	BinHttpButtonClass(HttpServer& webServer, BinStatesHttpClass& binStatesHttp, uint8_t unitNumber, String name = "Button", BinStateClass *outputState = nullptr);
	virtual ~BinHttpButtonClass() {};
	String getName() { return _name; };
	void setName(String name) { _name = name; };
	void addOutputState(BinStateClass *outputState);
	uint8_t _readUnit() {}; //Do not used here
private:
	BinStateHttpClass* _binStateHttp;
	String _name; //Name on button to display on the web
	BinStateClass* _outputState = nullptr; //optional BinStateClass reference to get state from
	BinStatesHttpClass& _binStatesHttp;
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
