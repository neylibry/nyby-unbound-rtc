/* Native arithmetic models of rtc_fast.S, not an ARM/GBA emulator.
 * Independent division/calendar oracles; actual clock.c linked unchanged.
 */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtc.h"
static uint64_t anchor;
u64 anchor_get(void){return anchor;}
u32 original_write_sector(u32 sector,u8 *buffer){(void)sector;(void)buffer;abort();}
static uint64_t checks;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"failed line %d: %s\n",__LINE__,#x);exit(2);}}while(0)
static u32 bcd(u32 n){return (n/10)*16+n%10;}
static u32 model_bcd(u32 n){u32 q=(n*103U)>>10;return n+6*q;}
static u32 model_elapsed(u32 frames){
    uint64_t product=(uint64_t)frames*0x44940U;
    u32 lo=(u32)product,hi=(u32)(product>>32);
    return (lo>>24)|(hi<<8);
}
static u32 reduced_elapsed(u32 frames){
    uint64_t product=(uint64_t)frames*0x1125U;
    u32 lo=(u32)product,hi=(u32)(product>>32);
    return (lo>>18)|(hi<<14);
}
static u32 model_now(u32 initial_seconds,u32 initial_frame,u32 current_frame){
    u32 delta=current_frame-initial_frame;
    u32 value=initial_seconds>MAX_SECONDS?MAX_SECONDS:initial_seconds;
    value+=model_elapsed(delta);
    return value>MAX_SECONDS?MAX_SECONDS:value;
}
static void model_split(u32 value,u32 *days,u32 *rem){
    u32 q=(u32)(((uint64_t)value*0xc22e4507U)>>48);
    *days=q;*rem=value-86400U*q;
}
static void model_hms(u32 seconds,u8 *out){
    u32 h=(seconds*37283U)>>27;
    u32 remainder=seconds-3600U*h;
    u32 m=(remainder*2185U)>>17;
    out[0]=(u8)model_bcd(h);out[1]=(u8)model_bcd(m);out[2]=(u8)model_bcd(remainder-60U*m);
}
static void model_datetime(u32 value,u8 out[8]){
    u32 days,seconds;model_split(value,&days,&seconds);
    u32 cycle=(days*22967U)>>25;
    u32 y=cycle*4,day=days-cycle*1461U;
    u32 n=day?day-1:0;
    u32 within=(n*1437U)>>19;
    y+=within;day-=365U*within;if(within)day--;
    out[0]=(u8)model_bcd(y);
    u32 jan_feb=59+((y&3)==0);
    day=day>=jan_feb?day-jan_feb:day+306;
    u32 month=((day*5U+2U)*857U)>>17;
    u32 day_of_month=day-(((month*153U+2U)*1639U)>>13)+1;
    out[2]=(u8)model_bcd(day_of_month);
    month=month<10?month+3:month-9;
    out[1]=(u8)model_bcd(month);
    u32 weekday=days+6,quotient=(weekday*18725U)>>17;
    out[3]=(u8)(weekday-7U*quotient);
    model_hms(seconds,out+4);out[7]=0x40;
}
static uint64_t test_vectors(const char *path){
    FILE *f=fopen(path,"rb");CHECK(f!=NULL);uint64_t count=0;
    struct {u32 seconds;u8 expected[8];} vector;
    while(fread(&vector,1,sizeof(vector),f)==sizeof(vector)){
        u8 actual[8];model_datetime(vector.seconds,actual);
        CHECK(memcmp(actual,vector.expected,8)==0);
        Fields fields;seconds_fields(vector.seconds,&fields);
        CHECK(fields.year==2000+10*(actual[0]>>4)+(actual[0]&15));
        CHECK(bcd(fields.month)==actual[1]&&bcd(fields.day)==actual[2]);
        CHECK(bcd(fields.hour)==actual[4]&&bcd(fields.minute)==actual[5]);
        CHECK(fields_seconds(&fields)==vector.seconds/60*60);
        u32 encoded=record_encode(vector.seconds),decoded=0;
        CHECK(record_decode(encoded,&decoded)&&decoded==vector.seconds/60*60);
        count++;
    }
    CHECK(feof(f)&&!ferror(f));fclose(f);return count;
}
int main(int argc,char **argv){
    CHECK(argc==2);CHECK(sizeof(u32)==4&&sizeof(uint64_t)==8);
    uint64_t calendar=test_vectors(argv[1]);
    // Each quotient interval: monotonicity makes its endpoints a proof of
    // reciprocal correctness for every uint32 input, not just samples.
    uint64_t split_endpoints=0;
    for(uint64_t q=0;q<=UINT32_MAX/86400U;q++){
        uint64_t a=q*86400U,b=a+86399U;if(b>UINT32_MAX)b=UINT32_MAX;
        u32 d,r;model_split((u32)a,&d,&r);CHECK(d==q&&r==0);
        model_split((u32)b,&d,&r);CHECK(d==q&&r==b-a);split_endpoints+=2;
    }
    for(u32 s=0;s<86400;s++){
        u8 h[3];model_hms(s,h);
        CHECK(h[0]==bcd(s/3600)&&h[1]==bcd(s/60%60)&&h[2]==bcd(s%60));
    }
    for(u32 n=0;n<=99;n++)CHECK(model_bcd(n)==bcd(n));
    for(u32 control=0;control<256;control++){
        const u32 high[]={0,0xff000000U,0x55555500U,0xaaaaaa00U,0xffffff00U};
        for(unsigned i=0;i<sizeof(high)/sizeof(*high);i++){
            u32 cpsr=high[i]|control;
            u32 before=((cpsr|0x80U)&~0x1fU)|0x1bU;
            u32 after=(cpsr&~0x1fU)|0x9bU;
            CHECK(before==after);CHECK((after&0x1f)==0x1b&&(after&0x80));
            CHECK((after&~0x9fU)==(cpsr&~0x9fU));
        }
    }
    // 280896 / 2^24 = 4389 / 2^18 exactly. Verify all fractional phases
    // in both first and last full 2^18-frame blocks.
    for(u32 phase=0;phase<(1U<<18);phase++){
        for(u32 top=0;top<2;top++){
            u32 frames=phase+(top?0xfffc0000U:0);
            CHECK(model_elapsed(frames)==(uint64_t)frames*280896U/16777216U);
            CHECK(model_elapsed(frames)==(uint64_t)frames*4389U/262144U);
            CHECK(model_elapsed(frames)==reduced_elapsed(frames));
        }
    }
    // Every frame over nominal 72 hours. Initial count near rollover.
    // Read many times without moving the anchor, ensuring rounding cannot
    // accumulate. Adding a full fractional period adds exactly 4389 s.
    const u32 initial_frame=0xffff0000U,initial_seconds=845832600U;
    u32 frames=(u32)((uint64_t)72*3600U*16777216U/280896U);
    u32 prev=initial_seconds;uint64_t session=0;
    for(u32 n=0;n<=frames;n++){
        u32 elapsed=model_elapsed(n),value=model_now(initial_seconds,initial_frame,initial_frame+n);
        CHECK(value==initial_seconds+(uint64_t)n*280896U/16777216U);
        CHECK(value>=prev&&value-prev<=1);prev=value;
        CHECK(reduced_elapsed(n)==elapsed);
        if(n<=UINT32_MAX-(1U<<18))CHECK(model_elapsed(n+(1U<<18))-elapsed==4389U);
        session++;
    }
    // Saturation and modulo subtraction; no uint32 overflow of seconds.
    const u32 origins[]={0,1,845832600U,MAX_SECONDS-1,MAX_SECONDS,UINT32_MAX};
    const u32 deltas[]={0,1,59,60,0xffff,0xffffff,0x7fffffff,0xfffffffe,0xffffffff};
    uint64_t boundaries=0;
    for(unsigned i=0;i<sizeof(origins)/sizeof(*origins);i++)
      for(unsigned j=0;j<sizeof(deltas)/sizeof(*deltas);j++){
        uint64_t base=origins[i]>MAX_SECONDS?MAX_SECONDS:origins[i];
        uint64_t expected=base+(uint64_t)deltas[j]*280896U/16777216U;
        if(expected>MAX_SECONDS)expected=MAX_SECONDS;
        CHECK(model_now(origins[i],initial_frame,initial_frame+deltas[j])==expected);boundaries++;
    }
    CHECK((uint64_t)MAX_SECONDS+model_elapsed(UINT32_MAX)<=UINT32_MAX);
    CHECK(model_now(845832600U,0,0)==845832600U);
    // This existing representation stores the last entered time, at minute
    // precision; it does not automatically save the current computed time.
    anchor=((u64)0xabcd1234U<<32)|845832600U;
    u32 decoded;CHECK(record_decode(record_encode((u32)anchor_get()),&decoded));
    CHECK(decoded==845832600U);
    printf("{\"passed\":true,\"checks\":%llu,\"calendar_cases\":%llu,\"hms_cases\":86400,\"split_interval_endpoints\":%llu,\"fraction_phase_cases\":524288,\"session_hours\":72,\"session_frames\":%llu,\"clamp_and_wrap_cases\":%llu,\"last_session_elapsed_seconds\":%u,\"frame_fraction_denominator\":262144}\n",
      (unsigned long long)checks,(unsigned long long)calendar,(unsigned long long)split_endpoints,
      (unsigned long long)session,(unsigned long long)boundaries,model_elapsed(frames));
    return 0;
}
