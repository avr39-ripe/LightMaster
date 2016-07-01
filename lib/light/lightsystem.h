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
	LightSystemClass();
//	virtual ~LightSystemClass();

	void addLightGroup(BinOutClass* output, BinInClass* input, BinHttpButtonClass* httpButton);
	void addLightGroup(BinOutClass* output, BinInClass* input) { addLightGroup(output, input, nullptr); };
	void addLightGroup(BinOutClass* output, BinHttpButtonClass* httpButton) { addLightGroup(output,nullptr,httpButton); };
	void addAllOffGroup(BinOutClass* output, BinInClass* input, BinHttpButtonClass* httpButton);
	void addAllOffGroup(BinInClass* input) { addAllOffGroup(nullptr,input, nullptr); };
	void addAllOffGroup(BinHttpButtonClass* httpButton) {addAllOffGroup(nullptr, nullptr, httpButton); };
	void addRandomButton(BinHttpButtonClass* httpButton);
	void onWSReceiveButton(JsonObject& jsonRoot) { _binHttpButtons.onWSReceiveButton(jsonRoot); };
	void onWSGetButtons(WebSocket& socket) { _binHttpButtons.onWSGetButtons(socket); };
	void onWSReceiveRandom(JsonObject& jsonRoot);
	void onWSGetRandom(WebSocket& socket);
	void toggleAllOff(uint8_t state);
	uint16_t getRandom(uint16_t min, uint16_t max);
	void randomLight(uint8_t state);
private:
	void _saveBinConfig();
	void _loadBinConfig();
	void _randomTurnOn();
	void _randomTurnOff();
	void _randomEnabler(uint8_t state);
	void _randomEnablerCheck();
	BinStateClass* _allOffState;
	BinOutClass* _allOffOutput = nullptr;
	Vector<BinInClass*> _inputs;
	Vector<BinOutClass*> _outputs;
	BinHttpButtonsClass _binHttpButtons;
	BinStateClass* _randomState = nullptr;
	uint8_t _randomLightGroupIdx = 0;
	Timer _randomEnablerTimer;
	Timer _randomTimer;
	uint16_t _randomStartTime = 0;
	uint16_t _randomStopTime = 1439;
	uint8_t _minOn = 15; //minimum random On time in seconds
	uint8_t _maxOn = 45; //max random On time in seconds
	uint8_t _minOff = 20; //min random Off time
	uint8_t _maxOff = 60; //max random Off time
};


#endif /* LIB_LIGHT_LIGHTSYSTEM_H_ */
