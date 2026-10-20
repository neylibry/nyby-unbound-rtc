/* Startup-only tiled rasterizer: reuse the base font at integer scales.
 * Tiles 64..663 are video memory only; the manual editor's font is retained.
 * No heap, image file, permanent RAM, timer or IRQ is needed. */
#include "banner_font.h"
#define BANNER_TILE 64
static const u32 video_zero=0;
static volatile u32 *banner_page(void) {
    return (volatile u32*)(0x06000000+((BG0CNT^8)&8)*4096);
}
static __attribute__((always_inline)) inline void banner_plot(volatile u32 *page,u32 x,u32 y,u32 color) {
    u32 tile=BANNER_TILE+(y>>3)*30+(x>>3);
    volatile u32 *word=page+tile*8+(y&7);
    *word|=color<<((x&7)*4);
}
static void banner_pixel(u32 x,u32 y,u32 color) {banner_plot(banner_page(),x,y,color);}
static const u8 *banner_glyph(u8 ch) {
    if(ch>='a'&&ch<='z')return banner_lower[ch-'a'];
    if(ch==0xE9)return banner_e_acute;
    return font[ch>=32&&ch<=90?ch-32:0];
}
static void banner_text(u32 x,u32 y,const char *s,u32 scale,u32 color) {
    volatile u32 *page=banner_page();
    for(;*s;s++,x+=6*scale) {
        const u8 *g=banner_glyph((u8)*s);
        for(u32 col=0;col<5;col++)for(u32 row=0;row<7;row++)if(g[col]&(1U<<row))
            for(u32 dy=0;dy<scale;dy++)for(u32 dx=0;dx<scale;dx++)
                banner_plot(page,x+col*scale+dx,y+row*scale+dy,color);
    }
}
static void banner_center(u32 y,const char *s,u32 scale,u32 color) {
    u32 n=0;while(s[n])n++;
    banner_text((240-(n*6-1)*scale)/2,y,s,scale,color);
}
static void banner_digits(char *out,u32 value,u32 width) {
    while(width){u32 rem;value=divmod(value,10,&rem);out[--width]=(char)('0'+rem);}
}
#include "footer.h"
static void banner_footer(void) {
    volatile u32 *page=banner_page();
    u32 n=sizeof(footer_glyphs)/sizeof(footer_glyphs[0]);
    u32 x=(240-(n*4-1))/2;
    for(u32 i=0;i<n;i++,x+=4) {
        u32 g=footer_glyphs[i];
        for(u32 row=0;row<5;row++)for(u32 col=0;col<3;col++)
            if(g&(1U<<(row*3+col)))banner_plot(page,x+col,153+row,3);
    }
}
// The inactive tile page is 32 KiB away; both fit in startup BG VRAM.
// BG0CNT selects the visible page, so no writable persistent state is needed.
static volatile u32 *panel_back(void) {
    return (volatile u32*)(0x06000000+((BG0CNT^8)&8)*4096+BANNER_TILE*32);
}
static void panel_clear(u32 first_row,u32 rows) {
    video_fast_set(&video_zero,panel_back()+first_row*30*8,0x01000000U|rows*30*8);
}
static void panel_begin(void) {panel_clear(0,20);}
static void panel_present(void) {
    wait_frame();
    BG0CNT^=8;DISPCNT=0x0100;
}
static void panel_clone(void) {
    volatile u32 *back=panel_back();
    const volatile u32 *front=(const volatile u32*)((u32)back^0x8000);
    video_fast_set(front,back,600*8);
}
