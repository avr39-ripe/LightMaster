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

namespace BinState
{
//Bitmask of _state field
	const uint8_t stateBit = 1u;
	const uint8_t prevStateBit = 2u;
	const uint8_t polarityBit = 4u;
	const uint8_t toggleActiveBit = 8u;
	const uint8_t persistentBit = 16u;
}

class BinStateClass
{
public:
	BinStateClass(uint8_t polarity = true, uint8_t toggleActive = true);
	uint8_t get() { return _state & BinState::stateBit ? getPolarity() : !getPolarity(); };
	uint8_t getRawState() { return (_state & BinState::stateBit) != 0; };
	uint8_t getPrev() { return (_state & BinState::prevStateBit) != 0; };
	uint8_t getPolarity() { return (_state & BinState::polarityBit) != 0; }
	uint8_t getToggleActive() { return (_state & BinState::toggleActiveBit) != 0; };
	void persistent(uint8_t uid); //Call this after constructor to save state in file, uid = UNIC on device id
	void set(uint8_t state, uint8_t forceDelegatesCall);
	void set(uint8_t state) { set(state, false); };
	void setPolarity(uint8_t polarity) { polarity ? _state |= BinState::polarityBit : _state &= ~(BinState::polarityBit); };
	void setToggleActive(uint8_t toggleActive) { toggleActive ? _state |= BinState::toggleActiveBit : _state &= ~(BinState::toggleActiveBit); };
	void toggle(uint8_t state = true); //there is argument for use this method in tru/false delegates, can use it without arguments too
	void onSet(onStateChangeDelegate delegateFunction);
	void onChange(onStateChangeDelegate delegateFunction, uint8_t directState = true);
protected:
private:
	void _saveBinConfig();
	void _loadBinConfig();
	void _setState(uint8_t state) { state ? _state |= BinState::stateBit : _state &= ~(BinState::stateBit);};
	void _setPrev(uint8_t state) { state ? _state |= BinState::prevStateBit : _state &= ~(BinState::prevStateBit);};
	uint8_t _state = 0; //BITMASK FIELD!!!
	uint8_t _uid = 0; // unic id used if persistent enabled as file name uid, must be unic on device
	void _callOnChangeDelegates();
	onStateChangeDelegate _onSet = nullptr; // call this with _state as argument
	Vector<onStateChangeDelegate> _onChange; // call them with _state as argument
	Vector<onStateChangeDelegate> _onChangeInverse; // call them with !_state as argument

};

class BinStateHttpClass
{
public:
	BinStateHttpClass(HttpServer& webServer, BinStateClass& state, String name, uint8_t uid);
//	: _webServer(webServer), _state(state), _name(name), _uid(uid) { _updateLength(); };
	void wsBinGetter(WebSocket& socket, uint8_t* data, size_t size);
	void wsBinSetter(WebSocket& socket, uint8_t* data, size_t size);
	void wsSendState(uint8_t state);
	static const uint8_t sysId = 2;
private:
	void _updateLength();
	BinStateClass& _state;
	String _name;
	uint16_t _nameLength = 0;
	uint8_t _uid = 0;
	HttpServer& _webServer;
};
#endif /* LIB_BINIO_BINSTATE_H_ */
