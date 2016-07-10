#include <app.h>
#include <user_config.h>
#include <lightmaster.h>

NtpClient* ntpClient;
#ifdef MCP23S17 //use MCP23S17
MCP* mcp000;
#endif
BinInPollerClass binInPoller(100);
LightSystemClass* lightSystem;
BinStateClass* binStates[7];

AppClass App;

void init()
{
        App.init();
        App.start();
}
