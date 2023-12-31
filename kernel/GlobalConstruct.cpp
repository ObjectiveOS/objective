#include "GlobalConstruct.h"

void GlobalConstruct::terminalInit()
{
    display = VGADisplay();
};

void GlobalConstruct::gdtInit()
{
    mngGdtInit();
    mngGdt.addDescriptor(DescSeg32::flatCodeKernel());
    mngGdt.addDescriptor(DescSeg32::flatDataKernel()); 
    mngGdt.loadGdt();
};
