ARDUINO_LIBRARIES := MCP23017 ArduinoJson5

SPI_SIZE		?= 4M

RBOOT_ENABLED		?= 1

RBOOT_ROM0_ADDR		:= 0x008000
RBOOT_ROM1_ADDR		:= 0x108000

RBOOT_SPIFFS_0		:= 0x200000
RBOOT_SPIFFS_1		:= 0x300000

RBOOT_RTC_ENABLED	= 1
RBOOT_GPIO_SKIP_ENABLED ?= 1

SPIFF_SIZE		?= 524288

COMPONENT_SRCDIRS = app lib/application lib/binio lib/light lib/wsbinconst
COMPONENT_INCDIRS = include lib/application lib/binio lib/light lib/wsbinconst

ENABLE_CMD_EXECUTOR = 0

CXXFLAGS		+= -fno-rtti -fno-exceptions -fno-threadsafe-statics

WEBPACK_OUT = web/build
SPIFF_OUT = files
INDEX = index.html.gz

.PHONY: webpacker
webpacker: 
	$(vecho) "Bundling html+css+js with Webpack"
	$(Q) rm -rf $(WEBPACK_OUT)/*
	$(Q) webpack
	$(Q) mv $(WEBPACK_OUT)/$(INDEX) $(SPIFF_OUT)/$(INDEX)
