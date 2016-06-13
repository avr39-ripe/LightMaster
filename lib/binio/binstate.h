/*
 * binstate.h
 *
 *  Created on: 18 апр. 2016 г.
 *      Author: shurik
 */

#ifndef LIB_BINIO_BINSTATE_H_
#define LIB_BINIO_BINSTATE_H_
#include <SmingCore/SmingCore.h>

#ifndef ONSTATECHANGEDELEGATE_TYPE_DEFINED
#define ONSTATECHANGEDELEGATE
typedef Delegate<void(uint8_t state)> onStateChangeDelegate;
#endif

class BinStateClass
{
public:
	BinStateClass(){};
	uint8_t get() { return _state; };
	void set(uint8_t state, uint8_t forceDelegatesCall = false);
	void onChange(onStateChangeDelegate delegateFunction, uint8_t directState = true);
protected:
private:
	uint8_t _state = false;
	void _callOnChangeDelegates();
	Vector<onStateChangeDelegate> _onChange; // call them with _state as argument
	Vector<onStateChangeDelegate> _onChangeInverse; // call them with !_state as argument
};

#endif /* LIB_BINIO_BINSTATE_H_ */
