/* Startup-only tiled rasterizer: reuse the base font at integer scales.
 * Tiles 64..663 are video memory only; the manual editor's font is retained.
 * No heap, image file, permanent RAM, timer or IRQ is needed. */
#include "banner_font.h"
#define BANNER_TILE 64
static void banner_pixel(u32 x,u32 y,u32 color) {
    u32 tile=BANNER_TILE+(y>>3)*30+(x>>3);
    volatile u32 *word=(volatile u32*)0x06000000+tile*8+(y&7);
    *word|=color<<((x&7)*4);
}
static const u8 *banner_glyph(u8 ch) {
    if(ch>='a'&&ch<='z')return banner_lower[ch-'a'];
    if(ch==0xE9)return banner_e_acute;
    return font[ch>=32&&ch<=90?ch-32:0];
}
static void banner_text(u32 x,u32 y,const char *s,u32 scale,u32 color) {
    for(;*s;s++,x+=6*scale) {
        const u8 *g=banner_glyph((u8)*s);
        for(u32 col=0;col<5;col++)for(u32 row=0;row<7;row++)if(g[col]&(1U<<row))
            for(u32 dy=0;dy<scale;dy++)for(u32 dx=0;dx<scale;dx++)
                banner_pixel(x+col*scale+dx,y+row*scale+dy,color);
    }
}
static void banner_center(u32 y,const char *s,u32 scale,u32 color) {
    u32 n=0;while(s[n])n++;
    banner_text((240-(n*6-1)*scale)/2,y,s,scale,color);
}
static void banner_digits(char *out,u32 value,u32 width) {
    while(width){u32 rem;value=divmod(value,10,&rem);out[--width]=(char)('0'+rem);}
}
static void banner_footer(void) {
    const char *s=NYBY_FOOTER;u32 n=sizeof(NYBY_FOOTER)-1;
    u32 x=(240-(n*4-1))/2;
    for(;*s;s++,x+=4) {
        u32 g=banner_tiny[(u8)*s-32];
        for(u32 row=0;row<5;row++)for(u32 col=0;col<3;col++)
            if(g&(1U<<(row*3+col)))banner_pixel(x+col,153+row,3);
    }
}
static void panel_begin(void) {
    DISPCNT=0x80;
    for(u32 i=0;i<600*8;i++)((volatile u32*)0x06000000)[BANNER_TILE*8+i]=0;
    for(u32 y=0;y<20;y++)for(u32 x=0;x<30;x++)MAP[y*32+x]=BANNER_TILE+y*30+x;
}
static void boot_banner(const Fields *f,u32 available) {
    panel_begin();
    banner_center(8,"RTC UNBOUND",2,2);
    banner_center(28,"Powered by Codex and NyBy",1,3);
    banner_center(43,"DATE",1,3);
    banner_center(76,MSG("HEURE","TIME"),1,3);
    if(available) {
        char date[]="00/00/0000",time[]="00:00";
        banner_digits(date,f->day,2);banner_digits(date+3,f->month,2);
        banner_digits(date+6,f->year,4);
        banner_digits(time,f->hour,2);banner_digits(time+3,f->minute,2);
        banner_center(52,date,2,1);banner_center(88,time,3,1);
        banner_center(120,MSG("L - r\xE9gler l'horloge","L - set the clock"),1,3);
        banner_center(134,MSG("start - lancer le jeu","start - launch game"),1,4);
    }else {
        banner_center(52,"--/--/----",2,1);banner_center(88,"--:--",3,1);
        banner_center(120,MSG("RTC SuperFW absente","No SuperFW RTC data"),1,1);
        banner_center(134,MSG("start - r\xE9gler l'heure","start - set the clock"),1,4);
    }
    banner_footer();
    DISPCNT=0x0100;
}

