#include <app.h>
#include <lightmaster.h>

NtpClient* ntpClient;

BinOutClass* outputs[outputsCount];

BinInPollerClass binInPoller(100);
AntiTheftClass* antiTheft;

AppClass App;

void init()
{
		Serial.printf(_F("INITIAL Free Heap: %d\n"), system_get_free_heap_size());
        App.init();
        App.start();
}
