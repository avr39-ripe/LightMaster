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

class BinHttpButtonClass : public BinInClass
{
public:
	BinHttpButtonClass(uint8_t unitNumber, String name = "Button") : BinInClass(unitNumber, 1), _name(name) {};
	virtual ~BinHttpButtonClass() {};
	void onHttpGetState(HttpRequest &request, HttpResponse &response);
	void onHttpSetState(HttpRequest &request, HttpResponse &response);
	String getName() { return _name; };
	void setName(String name) { _name = name; };
	uint8_t _readUnit() {}; //Do not used here
private:
	String _name; //Name on button to display on the web
};

class BinHttpButtonsClass
{
public:
	void onHttp(HttpRequest &request, HttpResponse &response);
	void add(BinHttpButtonClass &button);
private:
	Vector<BinHttpButtonClass*> _buttons;
};
#endif /* LIB_BINIO_BINHTTPBUTTON_H_ */
