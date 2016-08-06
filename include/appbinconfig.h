/*
 * appbinconfig.h
 *
 *  Created on: 6 авг. 2016 г.
 *      Author: shurik
 */

#ifndef INCLUDE_APPBINCONFIG_H_
#define INCLUDE_APPBINCONFIG_H_

class AppBinConfigClass
{
public:
	void load();
	void save();
	void wsBinSetter(WebSocket& socket, uint8_t* data, size_t size);
	void wsBinGetter(WebSocket& socket, uint8_t* data, size_t size);
	static const uint8_t sysId = 4;

//Application specific global configuration variables
	uint16_t	ventCycleDuration; //in seconds
	uint16_t	ventCycleInterval; // in seconds
	uint16_t	caldronOnDelay;
	uint16_t	caldronOffDelay;
};



#endif /* INCLUDE_APPBINCONFIG_H_ */
