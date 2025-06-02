
#include "../arch/SecondaryInit.h"
#include "../arch/x86/ic/Pic.h"


void empty() {};

extern "C" void kmain()
{
    SecondaryInitialization init;

    init.processorInitialize();
    init.devicesInitialize();

    out::display << "Hello, World!\n";

    asm("sti");
    while(true);
};
