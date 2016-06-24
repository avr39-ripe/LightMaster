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
	void toggleAllOff(uint8_t state);
	uint16_t getRandom(uint16_t min, uint16_t max);
	void randomLight(uint8_t state);
private:
	void _randomTurnOn();
	void _randomTurnOff();
	BinStateClass* _allOffState;
	BinOutClass* _allOffOutput = nullptr;
	Vector<BinInClass*> _inputs;
	Vector<BinOutClass*> _outputs;
	BinHttpButtonsClass _binHttpButtons;
	BinStateClass* _randomState = nullptr;
	uint8_t randomLightGroupIdx = 0;
	Timer _randomTimer;
	uint8_t minOn = 15; //minimum random On time in seconds
	uint8_t maxOn = 45; //max random On time in seconds
	uint8_t minOff = 20; //min random Off time
	uint8_t maxOff = 60; //max random Off time
};

#endif /* LIB_LIGHT_LIGHTSYSTEM_H_ */
