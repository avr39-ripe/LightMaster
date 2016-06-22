#include <app.h>
#include <user_config.h>
#include <lightmaster.h>

NtpClient* ntpClient;
#ifdef MCP23S17 //use MCP23S17
MCP* mcp000;
#endif
BinInClass* input[8];
BinInPollerClass binInPoller(100);
BinOutClass* output[8];
LightGroupClass* lightGroup[8];
BinHttpButtonsClass* httpButtons;
LightSystemClass* lightSystem;

AppClass App;

void init()
{
        App.init();
        App.start();
}
