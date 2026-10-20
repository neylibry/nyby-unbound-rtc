"""Inspect actual installer ZIPs independently. Run after tests/browser.cjs."""
from pathlib import Path
import hashlib, importlib.util, json, os, sys, zipfile
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[1]
OUT=Path(os.environ.get('NYBY_TEST_OUTPUT',str(ROOT/'build-tests/installer'))).resolve()
INPUTS=Path(os.environ['NYBY_TEST_INPUTS']).resolve()
sha=lambda b:hashlib.sha256(b).hexdigest()
release=json.loads((ROOT/'release.json').read_text(encoding='utf-8'))
payload=json.loads((ROOT/'sources/installer/payload.json').read_text(encoding='utf-8'))
checks=[]
def require(ok,message):
    if not ok:raise ValueError(message)
for origin in ['engine','browser']:
    for v in release['variants']:
        lang=v['id'];cases=[('with-save',True),('without-save',False)]
        if origin=='browser':cases.append(('already-patched',False))
        assets={p.relative_to(ROOT/v['sd_folder']).as_posix():p.read_bytes() for p in (ROOT/v['sd_folder']).rglob('*') if p.is_file()}
        for suffix,with_save in cases:
            name=v['target']['filename'];save='SAVEGAME/'+name[:-4]+'.sav';file=OUT/(origin+'-'+lang+'-'+suffix+'.zip')
            with zipfile.ZipFile(file) as z:
                require(z.testzip() is None,'ZIP CRC')
                expected={name,*assets,'INSTALLATION-FR-EN.txt','INSTALLATION-REPORT.json'}
                if with_save:expected.add(save)
                require(set(z.namelist())==expected and len(z.namelist())==len(expected),'ZIP inventory')
                rom=z.read(name);require(len(rom)==v['target']['bytes'] and sha(rom)==v['target']['sha256'],'ROM fingerprint')
                report=json.loads(z.read('INSTALLATION-REPORT.json'))
                require(report['patch_version']==release['version'] and report['game_language']==lang,'report version/language')
                require(report['already_patched']==(suffix=='already-patched'),'double patch report')
                require(report['output_rom_sha256']==sha(rom),'report output SHA')
                require(z.read('INSTALLATION-FR-EN.txt')==payload['copyHelp'].encode('utf-8'),'help content')
                for path,data in assets.items():require(z.read(path)==data,'SD asset '+path)
                if with_save:
                    data=z.read(save);require(data==(OUT/'inputs'/(lang.upper()+'.sav')).read_bytes(),'save changed')
                    require(report['save_sha256']==sha(data) and report['save_copied_without_conversion'],'save report')
                else:require(report['save_sha256'] is None and not report['save_copied_without_conversion'],'unexpected save')
            checks.append(file.name)
sys.path.insert(0,str(ROOT/'outils-python'))
spec=importlib.util.spec_from_file_location('nyby_patch',ROOT/'outils-python/appliquer_patch.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
for v in release['variants']:
    lang=v['id'];source=INPUTS/(lang.upper()+'.gba');before=sha(source.read_bytes())
    destination=OUT/('python-'+lang+'.gba')
    if not destination.exists():module.apply_release(source,destination)
    require(sha(destination.read_bytes())==v['target']['sha256'],'Python output')
    try:module.apply_release(source,destination)
    except FileExistsError:pass
    else:raise ValueError('Python fallback overwrote an existing output')
    require(sha(source.read_bytes())==before,'private source changed')
report={'passed':True,'zips_verified':len(checks),'cases':checks,'python_fallback_exact_and_refuses_overwrite':True,'personal_save_used':False,'sd_accessed':False}
(OUT/'download-tests.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8');print(json.dumps(report,indent=2))
