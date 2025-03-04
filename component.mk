ARDUINO_LIBRARIES := OneWire MCP23017 ArduinoJson5

HWCONFIG := ota

CONFIG_VARS := RBOOT_TWO_ROMS 
RBOOT_TWO_ROMS := 1

COMPONENT_SRCDIRS = app lib/application lib/binio lib/light lib/wsbinconst
COMPONENT_INCDIRS = include lib/application lib/binio lib/light lib/wsbinconst

ENABLE_CMD_EXECUTOR = 0

WEBPACK_OUT = files/index.*.js* files/thermostat.*.js*
SPIFF_OUT = files
INDEX = index.html.gz

.PHONY: webpacker
webpacker: 
	$(vecho) "Bundling html+css+js with Webpack"
	$(Q) rm -rf $(WEBPACK_OUT)/*
	$(Q) webpack
	$(Q) mv $(WEBPACK_OUT)/$(INDEX) $(SPIFF_OUT)/$(INDEX)
