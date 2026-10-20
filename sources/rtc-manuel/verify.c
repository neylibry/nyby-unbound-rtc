#include "rtc.h"

// Startup-only exact 4-KiB verification. The 512-byte SD-size readback buffer
// lives only in this non-inlined function, never in the editor's stack frame.
// expected must reference 4096 readable, word-aligned bytes. It is never changed,
// so a failed write/verification can retry from the original staged sector.
// Keep the explicit noinline contract even if future builds enable LTO.
__attribute__((noinline)) int verify_flash_sector(const u8 *expected) {
    u32 readback[128];
    const u32 *source=(const u32*)expected;
    for(u32 offset=0;offset<0x1000;offset+=sizeof(readback)) {
        if(!read_flash_checked(offset,readback,sizeof(readback)))return 0;
        const u32 *word=readback,*end=readback+128;
        do {if(*word++!=*source++)return 0;} while(word!=end);
    }
    return 1;
}
