#include <app.h>
#include <user_config.h>
#include <lightmaster.h>

NtpClient* ntpClient;
#ifdef MCP23S17 //use MCP23S17
MCP* mcp000;
#endif
//BinInPollerClass binInPoller(100);
TWValveClass* TWValve;
OneWire ds(onewire_pin);
TempSensors* tempSensor;

BinStateSharedDeferredClass* caldron;
BinCyclerClass* binCycler;

AppClass App;

void init()
{
		Serial.printf("INITIAL Free Heap: %d\n", system_get_free_heap_size());
        App.init();
        App.start();
}
