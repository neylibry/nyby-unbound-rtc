"""Release integrity checks, Python 3.10+; never acquire a ROM."""
from pathlib import Path
import argparse, base64, hashlib, json, sys
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[1]
def sha(data): return hashlib.sha256(data).hexdigest()
def load(name): return json.loads((ROOT/name).read_text(encoding='utf-8'))
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--skip-manifest',action='store_true')
    p.add_argument('--rom-fr',type=Path); p.add_argument('--rom-en',type=Path)
    a=p.parse_args(); checks=[]
    def check(ok,label):
        if not ok: raise ValueError(label)
        checks.append(label)
    release=load('release.json'); payload=load('sources/installer/payload.json')
    check(release==load('outils-python/release.json')==payload['release'],'metadata copies')
    variants={v['id']:v for v in release['variants']}
    for item in release['inputs']:
        lang=item['variant']; v=variants[lang]; patch=(ROOT/item['patch']).read_bytes()
        check(len(patch)==item['patch_bytes'] and sha(patch)==item['patch_sha256'],lang+' BPS fingerprint')
        check(base64.b64decode(payload['patches'][item['patch']],validate=True)==patch,lang+' embedded BPS')
        report=load('docs/build-'+lang.upper()+'.json')
        check(report['target_sha256']==v['target']['sha256'] and report['source_sha256']==item['sha256'],lang+' build inputs/outputs')
        check(report['payload_bytes']==v['payload_bytes'] and report['bps_sha256']==item['patch_sha256'],lang+' build/BPS metadata')
        source=ROOT/'sources'/('rtc-manuel' if lang=='fr' else 'rtc-manuel-en')
        for name,digest in report['source_files'].items():
            check(sha((source/name).read_bytes())==digest,lang+' source '+name)
        sd=ROOT/v['sd_folder']; bundled={}
        for asset in payload['sd'][lang]:
            path=Path(asset['path'])
            check(not path.is_absolute() and '..' not in path.parts,'safe SD path '+asset['path'])
            data=(sd/path).read_bytes()
            check(sha(data)==asset['sha256'] and base64.b64decode(asset['base64'],validate=True)==data,lang+' SD '+asset['path'])
            bundled[asset['path']]=data
        check(set(bundled)=={p.relative_to(sd).as_posix() for p in sd.rglob('*') if p.is_file()},lang+' complete SD inventory')
        cache=bundled[v['target']['filename'][:-4]+'.patch']
        check(len(cache)==v['superfw_cache']['bytes']==800 and sha(cache)==v['superfw_cache']['sha256'],lang+' pinned cache')
        check(all(data==cache for name,data in bundled.items() if name.endswith('.patch')),lang+' cache copies')
        configs=[data.decode('ascii').splitlines() for name,data in bundled.items() if name.endswith('.config')]
        check(len(configs)==1 and set(configs[0])=={'patchmode=1','igm=0','rtc=0','directsaving=1','cheats=0'},lang+' profile')
        rom=getattr(a,'rom_'+lang)
        if rom:
            data=rom.read_bytes()
            check(len(data)==item['bytes'] and sha(data)==item['sha256'],lang+' private input')
            sys.path.insert(0,str(source)); import patch_formats
            out=patch_formats.apply_patch(data,patch)
            check(len(out)==v['target']['bytes'] and sha(out)==v['target']['sha256'],lang+' independent Python BPS')
            check(rom.read_bytes()==data,lang+' input preserved')
    fr=ROOT/'sources/rtc-manuel'; en=ROOT/'sources/rtc-manuel-en'
    for file in fr.iterdir():
        if file.is_file() and file.name not in {'build.py','build_info.h','footer.h'}:
            check(file.read_bytes()==(en/file.name).read_bytes(),'shared FR/EN '+file.name)
    folder=ROOT/'sources/installer'
    html=(folder/'installer.template.html').read_text(encoding='utf-8').replace('__PAYLOAD__',json.dumps(payload,ensure_ascii=False).replace('<','\\u003c')).replace('__CORE__',(folder/'installer-core.js').read_text(encoding='utf-8'))
    for name in ['INSTALLER-FR-EN.html','INSTALLER-FR.html','INSTALLER-EN.html']:
        expected=html.replace('<option value="en">English</option>','<option value="en" selected>English</option>') if name=='INSTALLER-EN.html' else html
        check((ROOT/name).read_text(encoding='utf-8')==expected,'HTML content reproduction '+name)
    if not a.skip_manifest:
        manifest=load('MANIFEST.json'); listed={}
        for item in manifest['files']:
            name=item['path']; path=Path(name)
            check(not path.is_absolute() and '..' not in path.parts and name not in listed,'safe unique manifest path '+name)
            data=(ROOT/path).read_bytes()
            check(len(data)==item['bytes'] and sha(data)==item['sha256'],'manifest '+name); listed[name]=True
        excluded={'.git','__pycache__','build','build-tests','node_modules','.zig-cache'}
        actual={f.relative_to(ROOT).as_posix() for f in ROOT.rglob('*') if f.is_file() and not (set(f.relative_to(ROOT).parts)&excluded)}
        check(actual==set(listed)|{'MANIFEST.json'},'complete public inventory')
        forbidden={'.gba','.sav','.srm','.ss0','.elf','.o','.exe','.dll','.bin','.zip'}
        check(all(Path(name).suffix.lower() not in forbidden for name in actual),'no ROM/save/executable distributed')
    print(json.dumps({'passed':True,'checks':len(checks),'manifest_checked':not a.skip_manifest,'private_inputs_checked':[lang for lang in ['fr','en'] if getattr(a,'rom_'+lang)]},indent=2))
if __name__=='__main__': main()
