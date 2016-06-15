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
	BinStateClass(uint8_t polarity = true, uint8_t toggleActive = true);
	uint8_t get() { return (_state & stateBit ? getPolarity() : !getPolarity()); };
	uint8_t getPrev() { return _state & prevStateBit ? true : false; };
	uint8_t getPolarity() { return _state & polarityBit ? true : false; }
	uint8_t getToggleActive() { return _state & toggleActiveBit ? true : false; };
	void set(uint8_t state, uint8_t forceDelegatesCall);
	void set(uint8_t state) { set(state, false); };
	void setPolarity(uint8_t polarity) { polarity ? _state |= polarityBit : _state &= ~(polarityBit); };
	void setToggleActive(uint8_t toggleActive) { toggleActive ? _state |= toggleActiveBit : _state &= ~(toggleActiveBit); };
	void toggle(uint8_t state = true); //there is argument for use this method in tru/false delegates, can use it without arguments too
	void onSet(onStateChangeDelegate delegateFunction);
	void onChange(onStateChangeDelegate delegateFunction, uint8_t directState = true);
protected:
private:
	void _setState(uint8_t state) { state ? _state |= stateBit : _state &= ~(stateBit);};
	void _setPrev(uint8_t state) { state ? _state |= prevStateBit : _state &= ~(prevStateBit);};
	uint8_t _state = 0; //BITMASK FIELD!!!
	void _callOnChangeDelegates();
	onStateChangeDelegate _onSet; // call this with _state as argument
	Vector<onStateChangeDelegate> _onChange; // call them with _state as argument
	Vector<onStateChangeDelegate> _onChangeInverse; // call them with !_state as argument
//Bitmask of _state field
	const uint8_t stateBit = 1u;
	const uint8_t prevStateBit = 2u;
	const uint8_t polarityBit = 4u;
	const uint8_t toggleActiveBit = 8u;
};

#endif /* LIB_BINIO_BINSTATE_H_ */
