#include "Gdt.h"


uint64 MngGdt::gdt[MngGdt::size];

DescSeg32::DescSeg32(uint32 base, uint32 limit, uint8 access, uint8 flags)
{
    this->baseLow = base & 0x0000FFFF;
    this->baseMiddle = (base & 0x00FF0000) >> 16;
    this->baseHight = (base & 0xFF000000) >> 24;

    this->limitLow = limit & 0x0000FFFF;
    this->limitMiddle = (limit & 0x000F0000) >> 16;

    this->access = access;
    this->flags = flags & 0x000E;
};

uint64 DescSeg32::getDescriptor()
{
    uint64 descriptor = 0;

    descriptor |= (uint64)limitLow;
    descriptor |= (uint64)baseLow << 16;
    descriptor |= (uint64)baseMiddle << 32;
    descriptor |= (uint64)access << 40;
    descriptor |= (uint64)limitMiddle << 48;
    descriptor |= (uint64)flags << 52;
    descriptor |= (uint64)baseHight << 56;

    return descriptor;
};

uint64 DescSeg32::flatCodeKernel()
{
    return DescSeg32(0, 0xFFFFF, CODE_ER | PRESENT | NSYSTEMSEG
                                    , PAGEGRAN | HIGHSIZE).getDescriptor();    
};

uint64 DescSeg32::flatDataKernel()
{
    return DescSeg32(0, 0xFFFFF, DATA_RW | PRESENT | NSYSTEMSEG
                                    , PAGEGRAN | HIGHSIZE).getDescriptor();    
}

uint64 DescSeg32::flatCodeUser()
{

    return DescSeg32(0, 0xFFFFF, CODE_ER | PRESENT | NSYSTEMSEG | USER
                                    , PAGEGRAN | HIGHSIZE).getDescriptor();    
};

uint64 DescSeg32::flatDataUser()
{

    return DescSeg32(0, 0xFFFFF, DATA_RW | PRESENT | NSYSTEMSEG | USER
                                    , PAGEGRAN | HIGHSIZE).getDescriptor();    
};

MngGdt::MngGdt()
{
    gdt[0] = 0;
    currIdx = 1;
    isGdtLoaded = 0;
};

bool MngGdt::addDescriptor(uint64 desc)
{
    if(currIdx >= size || currIdx == 0)
    {
        return false;
    }

    this->gdt[currIdx++] = desc;
    return true;
};

bool MngGdt::loadGdt()
{
    if(isGdtLoaded)
    {
        return false;
    }

    uint64 pointer = 0;
    uint16 size = sizeof(uint64) * this->size - 1;
    uint32 offset = (uint32)((void *)(&gdt));

    pointer |= offset;
    pointer <<= 16;
    pointer |= size;

    SWITCHTOGDT(pointer);

    isGdtLoaded = 1;
    return true;
};

bool MngGdt::getIsGdtLoaded()
{
    return isGdtLoaded;
};

void MngGdt::operator=(const MngGdt& src)
{
    if(this->isGdtLoaded == 1 || src.isGdtLoaded == 1)
    {
        return;
    }

    // Fix it. Write method to copy from ... to ...
    for(int i = 0; i < this->size; i++) this->gdt[i] = src.gdt[i];
    this->isGdtLoaded = src.isGdtLoaded;
    this->currIdx = src.currIdx;
};
