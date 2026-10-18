"""Build the manual-clock patch on PC only, against one exact personal ROM."""
from pathlib import Path
import argparse,hashlib,json,struct,subprocess,sys,zlib
ROOT=Path(__file__).resolve().parent
CHECKUP=ROOT.parent
SOURCE=ROOT/'input.gba'
SOURCE_SHA='55bd66a6cd6a54b36b1c90b750541dfd0d621466bfb1ddee1e222f6207ad4164'
NAME='Unbound FR 179 RTC NyBy v1.5 BETA1'
VERSION='1.5-beta1'
RELEASE_DATE='2026-10-18'
BASE=0x0964c000
OFFSET=BASE-0x08000000
LIMIT=0x4000
ZIG=ROOT/'outils'/'python'/'ziglang'/'zig.exe'
from patch_formats import apply_bps

def variable(value):
    result=bytearray()
    while True:
        byte=value&127;value>>=7
        if not value:result.append(byte|128);return result
        result.append(byte);value-=1
def make_bps(source,target,changes):
    metadata=('Unbound FR 179 manual RTC '+VERSION+' by NyBy, '+RELEASE_DATE+': SuperFW date import, minimal START confirmation, ARM7TDMI runtime.').encode('ascii')
    patch=bytearray(b'BPS1')+variable(len(source))+variable(len(target))+variable(len(metadata))+metadata
    position=0
    for offset,length in sorted(changes):
        assert offset>=position
        if offset>position:patch+=variable((offset-position-1)<<2)
        patch+=variable(((length-1)<<2)|1)+target[offset:offset+length]
        position=offset+length
    if position<len(source):patch+=variable((len(source)-position-1)<<2)
    patch+=struct.pack('<II',zlib.crc32(source),zlib.crc32(target))
    patch+=struct.pack('<I',zlib.crc32(patch))
    return bytes(patch)

def sha(b):return hashlib.sha256(b).hexdigest()
def elf_read(path):
    data=path.read_bytes()
    assert data[:7]==b'\x7fELF\x01\x01\x01'
    shoff=struct.unpack_from('<I',data,32)[0]
    entsize,count,names_index=struct.unpack_from('<HHH',data,46)
    headers=[struct.unpack_from('<10I',data,shoff+i*entsize) for i in range(count)]
    sym={};alloc=[]
    for h in headers:
        name,typ,flags,addr,off,size,link,info,align,ent=h
        if flags&2 and size:
            assert typ!=8,'Unexpected persistent RAM allocation'
            assert BASE<=addr<BASE+LIMIT
            alloc.append((addr,data[off:off+size]))
        if typ==2:
            st=headers[link];strings=data[st[4]:st[4]+st[5]]
            for at in range(off,off+size,ent):
                n,v,s,inf,other,idx=struct.unpack_from('<IIIBBH',data,at)
                label=strings[n:].split(b'\0',1)[0].decode()
                if idx and label:sym[label]={'address':v,'size':s,'info':inf}
    end=max(a+len(b) for a,b in alloc)
    binary=bytearray(b'\xff'*(end-BASE))
    for addr,b in alloc:binary[addr-BASE:addr-BASE+len(b)]=b
    return bytes(binary),sym

def build(outname,rom=SOURCE,zig=ZIG):
    rom=Path(rom).resolve(strict=True)
    source=rom.read_bytes()
    assert sha(source)==SOURCE_SHA
    out=(ROOT/outname).resolve()
    assert out.is_relative_to(ROOT) and out!=ROOT,'Build directory must stay inside this PC project'
    out.mkdir(exist_ok=True)
    flags=[str(zig),'cc','-target','thumb-freestanding-eabi','-mcpu=arm7tdmi',
             '-Oz','-ffreestanding','-fno-builtin','-fno-stack-protector','-fno-unwind-tables',
             '-fno-asynchronous-unwind-tables','-fomit-frame-pointer','-fno-pic','-fno-pie',
             '-ffunction-sections','-fdata-sections','-nostdlib','-c']
    objects=[]
    for source_file in ['clock.c','ui.c','lowlevel.S','rtc_fast.S','superfw_import.S']:
        obj=out/(Path(source_file).stem+'.o');objects.append(str(obj))
        subprocess.run(flags+['-o',str(obj),source_file],cwd=ROOT,check=True)
    command=[str(zig),'build-exe','-target','arm-freestanding-eabi','-mcpu','arm7tdmi',
             '-O','ReleaseSmall','-fno-entry','-fno-strip','-T',str(ROOT/'link.ld'),
             '-femit-bin='+str(out/'clock.elf')]+objects
    subprocess.run(command,cwd=ROOT,check=True)
    code,symbols=elf_read(out/'clock.elf')
    assert len(code)<=LIMIT
    assert source[OFFSET:OFFSET+LIMIT]==b'\xff'*LIMIT
    entries=[
        (0x4c4,'10b50b4800240462','rtc_boot'),
        (0xd99d8,'10b50006040e201c','rtc_write_sector'),
        (0xa0b1dc,'f7b5164f3b780190','rtc_status'),
        (0xa0b2c4,'1fb5402301a8c371','rtc_success'),
        (0xa0b2d4,'f7b5144e33780500','rtc_datetime'),
        (0xa0b334,'f7b5154b01931b78','rtc_time'),
        (0xa0b398,'30b585b001ad2800','rtc_success')]
    target=bytearray(source);changes=[]
    for offset,before,label in entries:
        assert source[offset:offset+8]==bytes.fromhex(before),(label,source[offset:offset+8].hex())
        assert offset%4==0
        assert symbols[label]['address']&1,(label,'Expected Thumb entry')
        target[offset:offset+8]=b'\x00\x4b\x18\x47'+struct.pack('<I',symbols[label]['address'])
        changes.append((offset,8))
    target[OFFSET:OFFSET+len(code)]=code;changes.append((OFFSET,len(code)))
    target=bytes(target);patch=make_bps(source,target,changes)
    assert apply_bps(source,patch)==target
    name=NAME
    (out/(name+'.gba')).write_bytes(target)
    (out/(name+'.bps')).write_bytes(patch)
    (out/'payload.bin').write_bytes(code)
    report={'rom_name':NAME,'version':VERSION,'author':'NyBy','release_date':RELEASE_DATE,
            'source_files':{name:sha((ROOT/name).read_bytes()) for name in ['clock.c','ui.c','lowlevel.S','rtc_fast.S','font.h','rtc.h','link.ld','superfw_import.S','build_info.h','banner.h','banner_font.h','manual.h']},
            'source_sha256':sha(source),'target_sha256':sha(target),
            'target_crc32':f'{zlib.crc32(target):08X}','payload_offset':hex(OFFSET),
            'payload_bytes':len(code),'bps_bytes':len(patch),'bps_sha256':sha(patch),'symbols':symbols,
            'patched_ranges':[{'offset':hex(o),'size':n,'before_sha256':sha(source[o:o+n]),
                               'after_sha256':sha(target[o:o+n])} for o,n in changes],
            'toolchain':'Zig 0.15.2 cc/Clang, ARM7TDMI Thumb/ARM, -Oz, no libc',
            'firmware_modified':False,'source_unchanged':sha(rom.read_bytes())==SOURCE_SHA,
            'persistence':'4-byte CRC6/minute record at sector31 offset0xFF0',
            'clock':'entered seconds plus VBlank frames*280896/16777216'}
    (out/'build.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    print(json.dumps({k:report[k] for k in ('payload_bytes','bps_bytes','target_crc32','target_sha256')}))
    return out
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--out',default='optim-v1.1-rc1');p.add_argument('--rom',type=Path,default=SOURCE);p.add_argument('--zig',type=Path,default=ZIG)
    a=p.parse_args();build(a.out,a.rom,a.zig)
