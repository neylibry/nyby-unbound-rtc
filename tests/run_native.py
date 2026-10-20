"""Host C mathematics/clock.c checks; no ARM or GBA emulation."""
from pathlib import Path
import argparse, datetime as dt, hashlib, json, os, struct, subprocess
ROOT=Path(__file__).resolve().parents[1]
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--zig',required=True,type=Path);p.add_argument('--out',type=Path,default=ROOT/'build-tests/native')
    a=p.parse_args();zig=a.zig.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True);env=os.environ.copy()
    for key,folder in [('TEMP','tmp'),('TMP','tmp'),('ZIG_GLOBAL_CACHE_DIR','zig-global'),('ZIG_LOCAL_CACHE_DIR','zig-local')]:
        target=out/folder;target.mkdir(exist_ok=True);env[key]=str(target)
    epoch=dt.datetime(2000,1,1);date=epoch;vector=bytearray();cases=0
    bcd=lambda n:n//10*16+n%10
    while date.year<2100:
        for sec in [0,1,59,60,3599,3600,43199,43200,86399]:
            v=date+dt.timedelta(seconds=sec)
            vector+=struct.pack('<I8B',int((v-epoch).total_seconds()),bcd(v.year-2000),bcd(v.month),bcd(v.day),(v.weekday()+1)%7,bcd(v.hour),bcd(v.minute),bcd(v.second),0x40);cases+=1
        date+=dt.timedelta(days=1)
    vectors=out/'calendar-vectors.bin';vectors.write_bytes(vector)
    src=ROOT/'sources/rtc-manuel';exe=out/('rtc_math.exe' if os.name=='nt' else 'rtc_math')
    subprocess.run([str(zig),'cc','-O2','-fno-builtin','-Wall','-Wextra','-I',str(src),str(ROOT/'tests/rtc_math.c'),str(src/'clock.c'),'-o',str(exe)],cwd=out,env=env,check=True)
    done=subprocess.run([str(exe),str(vectors)],cwd=out,env=env,check=True,capture_output=True,text=True)
    report=json.loads(done.stdout)
    if report['calendar_cases']!=cases:raise ValueError('incomplete calendar test')
    sha=lambda b:hashlib.sha256(b).hexdigest()
    report.update({'calendar_vectors_sha256':sha(vector),'production_clock_source_sha256':sha((src/'clock.c').read_bytes()),'test_source_sha256':sha((ROOT/'tests/rtc_math.c').read_bytes()),'scope':'host C model plus actual clock.c; no ARM, BIOS, game or flashcart execution'})
    (out/'native-tests.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(report,indent=2))
if __name__=='__main__':main()
