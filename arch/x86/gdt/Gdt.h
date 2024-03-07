#ifndef GDT_H
#define GDT_H

#include <types.h>


// Define access options
#define PRESENT (1 << 7)        // Present bit
#define USER (3 << 5)           // Descriptor privelege level
#define NSYSTEMSEG (1 << 4)     // Descriptor type
#define EXEC (1 << 3)           // Executable bit
#define GROWDOWN (1 << 2)       // Direction bit
#define WRITABLE (1 << 1)       // Readable/Writable bit
#define ACCESSED 1              // Accessed bit

// Define flags byte
#define PAGEGRAN (1 << 3)       // Granularity flag
#define HIGHSIZE (1 << 2)       // Size flag
#define LONGMODEON (1 << 1)     // Long-mode flag

// Set of segment descriptors assign acesses bits
#define DATA_RO         0x00        // data seg, grows up, read only, not accessed
#define DATA_ROA        0x01        // data seg, grows up, read only, accessed
#define DATA_RW         0x02        // data seg, grows up, read/write, not accessed
#define DATA_RWA        0x03        // data seg, grows up, read/write, accessed
#define DATA_ROD        0x04        // data seg, grows down, read only, not accessed
#define DATA_RODGA      0x05        // data seg, grows down, read only, accessed
#define DATA_RWD        0x06        // data seg, grows down, read/write, not accessed
#define DATA_RWDA       0x07        // data seg, grows down, read/write, accessed
#define CODE_EO         0x08        // code seg, conforming, execute only, not accessed
#define CODE_EOA        0x09        // code seg, conforming, execute only, accessed
#define CODE_ER         0x0A        // code seg, conforming, execute/read, not accessed
#define CODE_ERA        0x0B        // code seg, conforming, execute/read, accessed
#define CODE_EONC       0x0C        // code seg, not conforming, execute only, not accessed
#define CODE_EONCA      0x0D        // code seg, not conforming, execute only, accessed
#define CODE_ERNC       0x0E        // code seg, not conforming, execute/read, not accessed
#define CODE_ERNCA      0x0F        // code seg, not conforming, execute/read, accessed

class DescSeg32                     // Segment descriptor
{
    private:
        uint16 limitLow;
        uint16 baseLow;
        uint8 baseMiddle;
        uint8 access;
        uint8 limitMiddle; // one word limit value 19:16
        uint8 flags;       // one word located 55:52 bits in descriptor
        uint8 baseHight;
    public:
        DescSeg32(uint32 base, uint32 limit, uint8 access, uint8 flags);
        void setDescriptor(uint32 base, uint32 limit, uint8 access, uint8 flags);
        uint64 getDescriptor();
        static uint64 flatCodeKernel();
        static uint64 flatDataKernel();
        static uint64 flatCodeUser();
        static uint64 flatDataUser();
};

#ifdef __cplusplus
extern "C" {
#endif
extern void SWITCHTOGDT(uint64 pointer);
#ifdef __cplusplus
};
#endif

class MngGdt
{
    private:
       const static uint8 size = 10;
       static uint64 gdt[size]; 
       uint8 currIdx;
       bool isGdtLoaded;

    public:
       MngGdt();
       void operator=(const MngGdt& src);
       bool addDescriptor(uint64 desc);  // 0 - not set descriptor, 1 - set
       bool loadGdt();      // 0 - if GDT is not loaded, 1 - loaded
       bool getIsGdtLoaded();
};

#endif
