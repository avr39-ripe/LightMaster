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
	const uint8_t deferredSetBit = 32u;
}

struct OnStateChange
{
	onStateChangeDelegate delegateFunction;
	uint8_t polarity;
};

class BinStateClass
{
public:
	BinStateClass(uint8_t polarity = true, uint8_t toggleActive = true);
	virtual ~BinStateClass() {};
	uint8_t get() { return _state & BinState::stateBit ? getPolarity() : !getPolarity(); };
	uint8_t getRawState() { return (_state & BinState::stateBit) != 0; };
	uint8_t getPrev() { return (_state & BinState::prevStateBit) != 0; };
	uint8_t getPolarity() { return (_state & BinState::polarityBit) != 0; }
	uint8_t getToggleActive() { return (_state & BinState::toggleActiveBit) != 0; };
	void persistent(uint8_t uid); //Call this after constructor to save state in file, uid = UNIC on device id
	virtual void set(uint8_t state, uint8_t forceDelegatesCall);
	virtual void set(uint8_t state) { set(state, false); };
	virtual void setTrue(uint8_t state) { if (state) set(true); };
	virtual void setTrue() { set(true); };
	virtual void setFalse(uint8_t state) { if (state) set(false); };
	virtual void setFalse() { set(false); };
	void setPolarity(uint8_t polarity) { polarity ? _state |= BinState::polarityBit : _state &= ~(BinState::polarityBit); };
	void setToggleActive(uint8_t toggleActive) { toggleActive ? _state |= BinState::toggleActiveBit : _state &= ~(BinState::toggleActiveBit); };
	void toggle(uint8_t state = true); //there is argument for use this method in tru/false delegates, can use it without arguments too
	void onSet(onStateChangeDelegate delegateFunction);
	void onChange(onStateChangeDelegate delegateFunction, uint8_t polarity = true);
protected:
	void _saveBinConfig();
	void _loadBinConfig();
	void _setState(uint8_t state) { state ? _state |= BinState::stateBit : _state &= ~(BinState::stateBit);};
	void _setPrev(uint8_t state) { state ? _state |= BinState::prevStateBit : _state &= ~(BinState::prevStateBit);};
	uint8_t _state = 0; //BITMASK FIELD!!!
	uint8_t _uid = 0; // unic id used if persistent enabled as file name uid, must be unic on device
	void _callOnChangeDelegates();
	onStateChangeDelegate _onSet = nullptr; // call this with _state as argument
	Vector<OnStateChange> _onChange = Vector<OnStateChange>(0,1); // call them with _state as argument
};

class BinStateHttpClass
{
public:
	BinStateHttpClass(HttpServer& webServer, BinStateClass& state, String name, uint8_t uid);
//	: _webServer(webServer), _state(state), _name(name), _uid(uid) { _updateLength(); };
	void wsBinGetter(WebSocket& socket, uint8_t* data, size_t size);
	void wsBinSetter(WebSocket& socket, uint8_t* data, size_t size);
	void wsSendStateAll(uint8_t state);
	void wsSendState(WebSocket& socket);
	void wsSendName(WebSocket& socket);
	void setState(uint8_t state) { _state.set(state); };
	uint8_t getState() { return _state.get(); };
	uint8_t getUid() { return _uid; };
	static const uint8_t sysId = 2;
private:
	void _updateLength();
	void _fillNameBuffer(uint8_t* buffer);
	void _fillStateBuffer(uint8_t* buffer);
	BinStateClass& _state;
	String _name;
	uint16_t _nameLength = 0;
	uint8_t _uid = 0;
	HttpServer& _webServer;
};

class BinStatesHttpClass
{
public:
	void wsBinGetter(WebSocket& socket, uint8_t* data, size_t size);
	void wsBinSetter(WebSocket& socket, uint8_t* data, size_t size);
	void add(BinStateHttpClass* binStateHttp) { _binStatesHttp[binStateHttp->getUid()] = binStateHttp; };
	static const uint8_t sysId = 3;
private:
	HashMap<uint8_t,BinStateHttpClass*> _binStatesHttp;
};

class BinStateSharedDeferredClass : public BinStateClass
{
public:
	virtual void set(uint8_t state);
	void setTrueDelay(uint16_t trueDelay) { _trueDelay = trueDelay; };
	void setFalseDelay(uint16_t falseDelay) { _falseDelay = falseDelay; };
private:
	void _setDeferredState(uint8_t state) { state ? _state |= BinState::deferredSetBit : _state &= ~(BinState::deferredSetBit);};
	uint8_t _getDefferedState() { return (_state & BinState::deferredSetBit) != 0; };
	void _deferredSet();
	uint8_t _consumers = 0;
	uint16_t _trueDelay = 0;
	uint16_t _falseDelay = 0;
	Timer _delayTimer;
};
#endif /* LIB_BINIO_BINSTATE_H_ */
