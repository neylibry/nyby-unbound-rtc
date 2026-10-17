#ifndef RTC_MANUEL_H
#define RTC_MANUEL_H
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef struct {u8 y,mo,d,w,h,mi,s,status,ah,am;} RtcInfo;
typedef struct {u16 year,month,day,hour,minute;} Fields;
u32 divmod(u32 value,u32 divisor,u32 *remainder);
u32 fields_seconds(const Fields *f);
void seconds_fields(u32 seconds,Fields *f);
u32 record_encode(u32 seconds);
int record_decode(u32 record,u32 *seconds);
u32 month_days(u32 year,u32 month);
u64 anchor_get(void);
void anchor_set(u32 seconds,u32 frames);
u32 frame_seconds(u32 frames);
void rtc_boot(void);
u32 rtc_datetime(RtcInfo *out);
u32 rtc_time(RtcInfo *out);
u32 rtc_status(RtcInfo *out);
u32 rtc_success(void);
u32 rtc_write_sector(u32 sector,u8 *buffer);
#define FRAMES (*(volatile u32*)0x03003114)
#define NATIVE_READ ((void(*)(u16,u32,void*,u32))0x081DEA99)
#define NATIVE_READ_STATUS ((u32(*)(u16,u32,void*,u32))0x081DEA99)
int read_flash_checked(u32 offset,void *buffer,u32 count);
#define NATIVE_WRITE ((u32(*)(u32,u8*))0x080D99D9)
#define NATIVE_PROGRAM_VERIFY ((u32(*)(u16,u8*))0x081DEC95)
#define BOOT_BUFFER ((u8*)0x02039A38)
#define MAX_SECONDS 3155759999U
#endif
