#include "rtc.h"
#include "font.h"
#include "build_info.h"
#define MSG(fr,en) (NYBY_EN ? (en) : (fr))
#define REG16(a) (*(volatile u16*)(a))
#define IME REG16(0x04000208)
#define IE REG16(0x04000200)
#define DISPCNT REG16(0x04000000)
#define BG0CNT REG16(0x04000008)
#define KEYS REG16(0x04000130)
#define VCOUNT REG16(0x04000006)
#define MAP ((volatile u16*)0x0600F800)
#define PAL ((volatile u16*)0x05000000)
#define A 1
#define B 2
#define START 8
#define RIGHT 16
#define LEFT 32
#define UP 64
#define DOWN 128

static u32 field_value(const Fields *f,u32 field) {
    switch(field){case 0:return f->day;case 1:return f->month;case 2:return f->year;case 3:return f->hour;default:return f->minute;}
}
static void field_set(Fields *f,u32 field,u32 value) {
    switch(field){case 0:f->day=value;break;case 1:f->month=value;break;case 2:f->year=value;break;case 3:f->hour=value;break;default:f->minute=value;}
    u32 last=month_days(f->year,f->month);if(f->day>last)f->day=last;
}
static u32 moved_value(const Fields *f,u32 field,int delta) {
    u32 low=0,high=59;
    if(field<3)low=1;
    if(field==0)high=month_days(f->year,f->month);
    if(field==1)high=12;
    if(field==2){low=2000;high=2099;}
    if(field==3)high=23;
    int value=(int)field_value(f,field)+delta;
    while(value<(int)low)value+=high-low+1;
    while(value>(int)high)value-=high-low+1;
    return value;
}
static void wait_frame(void) {while(VCOUNT>=160){}while(VCOUNT<160){}}
#include "banner.h"
#include "manual.h"

int read_flash_checked(u32 offset,void *buffer,u32 count) {
    u32 result=NATIVE_READ_STATUS(31,offset,buffer,count);
    // DirectSaving returns an SD status; the original SDK ReadFlash is void.
    return *(const volatile u16*)0x081DEA98!=0xB580 || result==0;
}
__attribute__((section(".text.rtc_boot"))) void rtc_boot(void) {
    extern void original_init(void);
    original_init();
    u16 ime=IME,ie=IE,display=DISPCNT,bg=BG0CNT;
    u16 blend=REG16(0x04000050),color0=PAL[0];
    IME=0;IE=1<<5;DISPCNT=0x80;
    Fields f;u32 previous=0,previous_record=0,remembered=0,staged=0;
    if(*(volatile u32*)0x03005004) {
        if(read_flash_checked(0,BOOT_BUFFER,0x1000)) {
            previous_record=*(u32*)(BOOT_BUFFER+0xFF0);
            remembered=record_decode(previous_record,&previous);
        }
    }
    if(!remembered)previous=845812800U; // 20/10/2026 12:00; user sets the real time.
    seconds_fields(previous,&f);
    PAL[0]=0x1C82;PAL[1]=0x7FFF;PAL[2]=0x7F58;PAL[3]=0x5294;PAL[4]=0x03FF;
    // Native RegisterRamReset has already zeroed BG0 scroll. These registers
    // are write-only: do not read/restore open-bus data on real hardware.
    BG0CNT=31<<8;REG16(0x04000050)=0;
    for(u32 y=0;y<20;y++)for(u32 x=0;x<30;x++)MAP[y*32+x]=BANNER_TILE+y*30+x;
    u32 field=3,repeat=0,oldkeys;
    screen(&f,field);
    oldkeys=(~KEYS)&0x3ff;
    for(;;) {
        wait_frame();
        u32 keys=(~KEYS)&0x3ff,pressed=keys&~oldkeys;
        if(keys && keys==oldkeys) {
            repeat++;
            if(repeat>=30){pressed|=keys&(UP|DOWN|LEFT|RIGHT);repeat=24;}
        }else repeat=0;
        oldkeys=keys;
        if(!pressed)continue;
        if(pressed&START) {
            u32 chosen=fields_seconds(&f),record=record_encode(chosen),ok=1;
            if(staged || !remembered || record!=previous_record) {
                screen_notice(0);
                if(*(volatile u32*)0x03005004) {
                    if(!staged)staged=read_flash_checked(0,BOOT_BUFFER,0x1000);
                    ok=staged;
                    if(ok) {
                        *(u32*)(BOOT_BUFFER+0xFF0)=record;
                        IME=1;ok=NATIVE_PROGRAM_VERIFY(31,BOOT_BUFFER)==0;IME=0;
                        ok=ok&&verify_flash_sector(BOOT_BUFFER);
                    }
                }else ok=0;
            }
            if(ok) {
                while((~KEYS)&0x3ff)wait_frame();
                anchor_set(chosen,FRAMES);break;
            }
            screen_notice(1);
            u32 released=0;
            for(;;) {
                wait_frame();u32 k=(~KEYS)&0x3ff;
                if(!k)released=1;
                if(released&&(k&A)){oldkeys=k;break;}
                if(released&&(k&B)) {
                    while((~KEYS)&0x3ff)wait_frame();
                    anchor_set(chosen,FRAMES);goto done;
                }
            }
            screen(&f,field);continue;
        }
        if(pressed&LEFT)field=field?field-1:4;
        else if(pressed&RIGHT)field=field==4?0:field+1;
        else if(pressed&UP)field_set(&f,field,moved_value(&f,field,-1));
        else if(pressed&DOWN)field_set(&f,field,moved_value(&f,field,1));
        else continue;
        screen_update(&f,field);
    }
done:
    DISPCNT=0x80;
    video_fast_set(&video_zero,(volatile void*)0x06000000,0x01004000U);
    video_fast_set(&video_zero,PAL,0x01000100U);
    PAL[0]=color0;BG0CNT=bg;REG16(0x04000050)=blend;
    DISPCNT=display;IE=ie;IME=ime;
}
