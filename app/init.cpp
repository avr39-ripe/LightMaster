#include <app.h>
#include <user_config.h>
#include <lightmaster.h>

NtpClient* ntpClient;
#ifdef MCP23S17 //use MCP23S17
MCP* mcp000;
MCP* mcp001;
MCP* mcp002;
#endif

#ifdef GPIO_MCP23017 //use MCP23S17
MCP23017* mcp000;
MCP23017* mcp001;
MCP23017* mcp002;
#endif
BinInPollerClass binInPoller(100);
AntiTheftClass* antiTheft;

AppClass App;

void init()
{
		Serial.printf("INITIAL Free Heap: %d\n", system_get_free_heap_size());
        App.init();
        App.start();
}
