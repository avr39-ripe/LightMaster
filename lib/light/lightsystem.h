/*
 * lightsystem.h
 *
 *  Created on: 22 июня 2016 г.
 *      Author: shurik
 */

#ifndef LIB_LIGHT_LIGHTSYSTEM_H_
#define LIB_LIGHT_LIGHTSYSTEM_H_
#include <SmingCore/SmingCore.h>
#include <binin.h>
#include <binout.h>
#include <binhttpbutton.h>

class LightSystemClass {
public:
//	LightSystemClass();
//	virtual ~LightSystemClass();

	void addLightGroup(BinOutClass* output, BinInClass* input, BinHttpButtonClass* httpButton);
	void addLightGroup(BinOutClass* output, BinInClass* input) { addLightGroup(output, input, nullptr); };
	void addLightGroup(BinOutClass* output, BinHttpButtonClass* httpButton) { addLightGroup(output,nullptr,httpButton); };
private:
	Vector<BinInClass*> _inputs;
	Vector<BinOutClass*> _outputs;
	BinHttpButtonsClass _binHttpButtons;
};

#endif /* LIB_LIGHT_LIGHTSYSTEM_H_ */
