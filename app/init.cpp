#include <app.h>
#include <user_config.h>
#include <lightmaster.h>

NtpClient* ntpClient;
#ifdef MCP23S17 //use MCP23S17
MCP* mcp000;
#endif

#ifdef GPIO_MCP23017 //use MCP23S17
MCP23017* mcp000;
#endif

BinOutClass* outputs[7];

BinInPollerClass binInPoller(100);
AntiTheftClass* antiTheft;

AppClass App;

void init()
{
		Serial.printf("INITIAL Free Heap: %d\n", system_get_free_heap_size());
        App.init();
        App.start();
}
