/*
 * lightgroup.h
 *
 *  Created on: 10 июня 2016
 *      Author: shurik
 */

#ifndef LIB_LIGHT_LIGHTGROUP_H_
#define LIB_LIGHT_LIGHTGROUP_H_
#include <SmingCore/SmingCore.h>
#include <binin.h>
#include <binout.h>
#include <binstate.h>

#ifndef ONSTATECHANGEDELEGATE_TYPE_DEFINED
#define ONSTATECHANGEDELEGATE
typedef Delegate<void(uint8_t state)> onStateChangeDelegate;
#endif

class LightGroupClass
{
public:
	LightGroupClass(BinOutClass &output, uint8_t toggleActive = true);
//	virtual ~LightGroupClass() {};
	void addInput(BinInClass &input);
	void toggle(uint8_t state);
	BinStateClass state;
private:
	uint8_t _toggleActive = true;
//	Vector<BinInClass*> _input;
	BinOutClass* _output;
};

#endif /* LIB_LIGHT_LIGHTGROUP_H_ */
