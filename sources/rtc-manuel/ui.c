#include "rtc.h"
#include "font.h"
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

static void text(u32 x,u32 y,const char *s,u32 palette) {
    while(*s && x<30) MAP[y*32+x++]=((u8)*s++-32)|(palette<<12);
}
static void number(u32 x,u32 y,u32 value,u32 width,u32 palette) {
    for(u32 i=width;i>0;i--){u32 rem;value=divmod(value,10,&rem);MAP[y*32+x+i-1]=(16+rem)|(palette<<12);}
}
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
static void copy_fields(Fields *to,const Fields *from) {
    for(u32 i=0;i<5;i++)((u16*)to)[i]=((const u16*)from)[i];
}
static void screen(const Fields *f,u32 field,u32 open,u32 remembered) {
    static const u8 xpos[5]={2,7,12,20,25};
    static const char *const labels[5]={"JOUR","MOIS","ANNEE","HEURE","MINUTE"};
    static const char weekdays[7][4]={"DIM","LUN","MAR","MER","JEU","VEN","SAM"};
    for(u32 i=0;i<32*20;i++)MAP[i]=0;
    text(5,1,"HORLOGE D'UNBOUND",1);
    text(2,3,"REGLER LA DATE ET L'HEURE",0);
    text(2,5,"DATE",1);text(20,5,"HEURE",1);
    text(5,6,"/",0);text(10,6,"/",0);text(23,6,":",0);
    for(u32 i=0;i<5;i++)number(xpos[i],6,field_value(f,i),i==2?4:2,i==field?2:0);
    u32 days=divmod(fields_seconds(f),86400,0),week;divmod(days+6,7,&week);
    text(2,8,weekdays[week],0);text(24,8,"24H",0);
    if(open){
        u32 x=xpos[field];
        if(x>22)x=22;
        text(x,9,labels[field],1);
        for(int n=-2;n<=2;n++) {
            number(x+1,12+n,moved_value(f,field,n),field==2?4:2,n==0?2:0);
            if(n==0)text(x,12,">",2);
        }
    }else text(2,11,remembered?"DERNIER REGLAGE RESTAURE":"PREMIER REGLAGE",0);
    text(2,16,"< > CHAMP   A LISTE / OK",0);
    text(2,17,"HAUT/BAS CHOIX  B ANNULER",0);
    text(2,18,"START LANCER LE JEU",1);
    // Displayed as: par NyBy v1.1RC1 17/10/2026; compact lowercase font slots.
    text(1,19,"[\\] N^B^ _1.1RC1 17/10/2026",3);
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
    original_init();
    u16 ime=IME,ie=IE,display=DISPCNT,bg=BG0CNT;
    u16 ho=REG16(0x04000010),vo=REG16(0x04000012),blend=REG16(0x04000050),color0=PAL[0];
    IME=0;IE=1<<5;DISPCNT=0x80;
    Fields f,backup;u32 previous=0,remembered=0;
    if(*(volatile u32*)0x03005004) {
        if(read_flash_checked(0,BOOT_BUFFER,0x1000)) {
            previous=*(u32*)(BOOT_BUFFER+0xFF0);
            remembered=record_decode(previous,&previous);
        }
    }
    if(!remembered)previous=845553600U; // 17/10/2026 12:00; user sets the real time.
    seconds_fields(previous,&f);
    for(u32 ch=0;ch<64;ch++)for(u32 row=0;row<8;row++) {
        u32 pixels=0;
        for(u32 col=0;col<5;col++)if(font[ch][col]&(1<<row))pixels|=1U<<((col+1)*4);
        ((volatile u32*)0x06000000)[ch*8+row]=pixels;
    }
    for(u32 i=0;i<32*32;i++)MAP[i]=0;
    PAL[0]=0x1C82;PAL[1]=0x7FFF;
    PAL[16]=0x1C82;PAL[17]=0x7F58;
    PAL[32]=0x2A10;PAL[33]=0x03FF;
    PAL[48]=0x1C82;PAL[49]=0x5294;
    BG0CNT=31<<8;REG16(0x04000010)=0;REG16(0x04000012)=0;REG16(0x04000050)=0;
    DISPCNT=0x0100;
    u32 field=3,open=0,repeat=0,oldkeys=(~KEYS)&0x3ff;
    screen(&f,field,open,remembered);
    for(;;) {
        wait_frame();u32 keys=(~KEYS)&0x3ff;
        u32 pressed=keys&~oldkeys;
        if(keys && keys==oldkeys) {
            repeat++;
            if(repeat>=30){pressed|=keys&(UP|DOWN|LEFT|RIGHT);repeat=24;}
        }else repeat=0;
        oldkeys=keys;
        if(!pressed)continue;
        if(pressed&START) {
            u32 chosen=fields_seconds(&f),record=record_encode(chosen),ok=1;
            if(!remembered || chosen!=previous) {
                text(2,11,"MEMORISATION...          ",1);
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
                while((~KEYS)&0x3ff)wait_frame();
                anchor_set(chosen,FRAMES);
                break;
            }
            text(2,11,"MEMOIRE NON ENREGISTREE  ",2);
            text(2,13,"A REESSAYER  B CONTINUER",0);
            u32 released=0;
            for(;;){wait_frame();u32 k=(~KEYS)&0x3ff;if(!k)released=1;if(released&&(k&A)){oldkeys=k;break;}if(released&&(k&B)){while((~KEYS)&0x3ff)wait_frame();anchor_set(chosen,FRAMES);goto done;}}
            screen(&f,field,open,remembered);continue;
        }
        if(pressed&A){if(!open)copy_fields(&backup,&f);open=!open;}
        else if(pressed&B){if(open){copy_fields(&f,&backup);open=0;}}
        else if(pressed&LEFT){field=field?field-1:4;open=0;}
        else if(pressed&RIGHT){field=field==4?0:field+1;open=0;}
        else if(pressed&UP)field_set(&f,field,moved_value(&f,field,-1));
        else if(pressed&DOWN)field_set(&f,field,moved_value(&f,field,1));
        screen(&f,field,open,remembered);
    }
done:
    DISPCNT=0x80;
    // This hook runs before any game screen: clear the small UI's video data.
    for(u32 i=0;i<0x10000/4;i++)((volatile u32*)0x06000000)[i]=0;
    for(u32 i=0;i<512;i++)PAL[i]=0;
    PAL[0]=color0;BG0CNT=bg;REG16(0x04000010)=ho;REG16(0x04000012)=vo;REG16(0x04000050)=blend;
    DISPCNT=display;IE=ie;IME=ime;
}
