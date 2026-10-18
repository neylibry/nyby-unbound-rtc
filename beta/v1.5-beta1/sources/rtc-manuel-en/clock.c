#include "rtc.h"

// Private arithmetic: no libc, heap, division helper or 64-bit runtime library.
u32 divmod(u32 value,u32 divisor,u32 *remainder) {
    u32 quotient=0,bit=1;
    while(divisor<=value/2U && !(divisor&0x80000000U)) {divisor<<=1;bit<<=1;}
    while(bit) {
        if(value>=divisor) {value-=divisor;quotient|=bit;}
        divisor>>=1;bit>>=1;
    }
    if(remainder)*remainder=value;
    return quotient;
}

u32 month_days(u32 year,u32 month) {
    static const u8 lengths[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    return lengths[month-1]+(month==2 && (year&3)==0);
}

u32 fields_seconds(const Fields *f) {
    u32 days=0;
    for(u32 y=2000;y<f->year;y++)days+=365+((y&3)==0);
    for(u32 m=1;m<f->month;m++)days+=month_days(f->year,m);
    days+=f->day-1;
    return days*86400U+f->hour*3600U+f->minute*60U;
}

void seconds_fields(u32 seconds,Fields *f) {
    u32 rem,days=divmod(seconds,86400,&rem);
    f->hour=divmod(rem,3600,&rem);
    f->minute=divmod(rem,60,0);
    u32 year=2000;
    while(days>=365+((year&3)==0))days-=365+((year++&3)==0);
    f->year=year;
    u32 month=1;
    while(days>=month_days(year,month))days-=month_days(year,month++);
    f->month=month;f->day=days+1;
}

// 26-bit minutes since 2000 plus a 6-bit error-detection code. Four bytes only.
static u32 crc6(u32 value) {
    u32 crc=0x2D;
    for(u32 i=0;i<26;i++) {
        u32 mix=((crc>>5)^(value>>(25-i)))&1;
        crc=(crc<<1)&63;
        if(mix)crc^=0x27;
    }
    return crc^0x15;
}
u32 record_encode(u32 seconds) {
    u32 minutes=divmod(seconds,60,0);
    return minutes|(crc6(minutes)<<26);
}
int record_decode(u32 record,u32 *seconds) {
    u32 minutes=record&0x03ffffff;
    if(record==0 || record==0xffffffff || minutes>52595999U || (record>>26)!=crc6(minutes))return 0;
    *seconds=minutes*60U;
    return 1;
}

// Runtime reads are implemented in rtc_fast.S. Generic arithmetic above is
// retained for the startup editor and save-record validation only.
u32 rtc_status(RtcInfo *out) {out->status=0x40;return 1;}
u32 rtc_success(void) {return 1;}

u32 rtc_write_sector(u32 sector,u8 *buffer) {
    // Only sector 31's proven 4-byte gap is ours. Other sectors are untouched.
    if(sector==31) *(u32*)(buffer+0xFF0)=record_encode((u32)anchor_get());
    // Relocated original TryWriteSector prologue, then unmodified native body.
    extern u32 original_write_sector(u32,u8*);
    return original_write_sector(sector,buffer);
}
