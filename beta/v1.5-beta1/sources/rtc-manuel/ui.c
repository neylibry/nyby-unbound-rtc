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
#define L 512

extern const u32 nyby_superfw_gate;
extern u32 nyby_import_superfw_clock(const u32 *gate,u32 frames);

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
#include "banner.h"
#include "manual.h"
static u32 boot_seconds(u32 initial,u32 waited) {
    u32 now=initial+frame_seconds(waited);
    return now>MAX_SECONDS?MAX_SECONDS:now;
}
static void wait_frame(void) {while(VCOUNT>=160){}while(VCOUNT<160){}}

int read_flash_checked(u32 offset,void *buffer,u32 count) {
    u32 result=NATIVE_READ_STATUS(31,offset,buffer,count);
    // SDK ReadFlash is void. SuperFW 0.21's DirectSaving replacement starts
    // with push{r7,lr} and propagates a real SD read error in r0. Honor it.
    return *(const volatile u16*)0x081DEA98!=0xB580 || result==0;
}

__attribute__((section(".text.rtc_boot"))) void rtc_boot(void) {
    // Hooked immediately after CheckForFlashMemory, in place of this call.
    extern void original_init(void);
    u32 received=nyby_import_superfw_clock(&nyby_superfw_gate,FRAMES);
    u32 incoming=received?(u32)anchor_get():0;
    original_init();
    u16 ime=IME,ie=IE,display=DISPCNT,bg=BG0CNT;
    u16 ho=REG16(0x04000010),vo=REG16(0x04000012),blend=REG16(0x04000050),color0=PAL[0];
    IME=0;IE=1<<5;DISPCNT=0x80;
    Fields f;u32 previous=0,previous_record=0,remembered=0;
    if(*(volatile u32*)0x03005004) {
        if(read_flash_checked(0,BOOT_BUFFER,0x1000)) {
            previous_record=*(u32*)(BOOT_BUFFER+0xFF0);
            remembered=record_decode(previous_record,&previous);
        }
    }
    if(!remembered)previous=845640000U; // 18/10/2026 12:00; user sets the real time.
    seconds_fields(received?incoming:previous,&f);
    PAL[0]=0x1C82;PAL[1]=0x7FFF;PAL[2]=0x7F58;PAL[3]=0x5294;PAL[4]=0x03FF;
    BG0CNT=31<<8;REG16(0x04000010)=0;REG16(0x04000012)=0;REG16(0x04000050)=0;
    u32 field=3,repeat=0,oldkeys=(~KEYS)&0x3ff,confirm=1,waited=0;
    boot_banner(&f,received);
    oldkeys=(~KEYS)&0x3ff;
    for(;;) {
        wait_frame();if(confirm&&received)waited++;
        u32 keys=(~KEYS)&0x3ff;
        u32 pressed=keys&~oldkeys;
        if(keys && keys==oldkeys) {
            repeat++;
            if(repeat>=30){pressed|=keys&(UP|DOWN|LEFT|RIGHT);repeat=24;}
        }else repeat=0;
        oldkeys=keys;
        if(!pressed)continue;
        if(confirm) {
            if((pressed&L)||(!received&&(pressed&START))) {
                if(received)seconds_fields(boot_seconds(incoming,waited),&f);
                received=0;confirm=0;
                screen(&f,field);continue;
            }
            if(!(pressed&START))continue;
        }
        if(pressed&START) {
            u32 chosen=confirm?boot_seconds(incoming,waited):fields_seconds(&f);
            u32 record=record_encode(chosen),ok=1;
            if(!remembered || record!=previous_record) {
                if(!confirm)screen_notice(0);
                if(*(volatile u32*)0x03005004) {
                    ok=read_flash_checked(0,BOOT_BUFFER,0x1000);
                    if(ok) {
                        *(u32*)(BOOT_BUFFER+0xFF0)=record;
                        // Native, verified sector write; no filesystem/ROM access.
                        IME=1;ok=NATIVE_PROGRAM_VERIFY(31,BOOT_BUFFER)==0;IME=0;
                        u32 readback=0;
                        ok=ok&&read_flash_checked(0xFF0,&readback,4)&&readback==record;
                    }
                }else ok=0;
            }
            if(ok) {
                while((~KEYS)&0x3ff){wait_frame();if(confirm&&received)waited++;}
                if(confirm)chosen=boot_seconds(incoming,waited);
                anchor_set(chosen,FRAMES);
                break;
            }
            screen_notice(1);
            u32 released=0;
            for(;;){wait_frame();if(confirm&&received)waited++;u32 k=(~KEYS)&0x3ff;if(!k)released=1;if(released&&(k&A)){oldkeys=k;break;}if(released&&(k&B)){while((~KEYS)&0x3ff){wait_frame();if(confirm&&received)waited++;}if(confirm)chosen=boot_seconds(incoming,waited);anchor_set(chosen,FRAMES);goto done;}}
            if(confirm)boot_banner(&f,received);else screen(&f,field);continue;
        }
        if(pressed&LEFT)field=field?field-1:4;
        else if(pressed&RIGHT)field=field==4?0:field+1;
        else if(pressed&UP)field_set(&f,field,moved_value(&f,field,-1));
        else if(pressed&DOWN)field_set(&f,field,moved_value(&f,field,1));
        else continue;
        screen(&f,field);
    }
done:
    DISPCNT=0x80;
    // This hook runs before any game screen: clear the small UI's video data.
    for(u32 i=0;i<0x10000/4;i++)((volatile u32*)0x06000000)[i]=0;
    for(u32 i=0;i<512;i++)PAL[i]=0;
    PAL[0]=color0;BG0CNT=bg;REG16(0x04000010)=ho;REG16(0x04000012)=vo;REG16(0x04000050)=blend;
    DISPCNT=display;IE=ie;IME=ime;
}
