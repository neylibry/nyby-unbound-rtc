/* Exercise the shipped HTML and real downloads in a fresh offline Chromium. */
const fs=require('node:fs'),path=require('node:path'),vm=require('node:vm');
const assert=require('node:assert/strict');
const {createHash}=require('node:crypto');
const {pathToFileURL}=require('node:url');
const {chromium}=require('playwright');
const root=path.resolve(__dirname,'..'),stage=root;
const tests=path.resolve(process.env.NYBY_TEST_OUTPUT||path.join(root,'build-tests/installer'));
assert.ok(process.env.NYBY_TEST_INPUTS,'Set NYBY_TEST_INPUTS to a private directory containing FR.gba and EN.gba');
fs.mkdirSync(tests,{recursive:true});
const privateInputs=path.resolve(process.env.NYBY_TEST_INPUTS);
const inputs=path.join(tests,'inputs');fs.mkdirSync(inputs,{recursive:true});
for(const lang of ['FR','EN']){
  const synthetic=Buffer.alloc(131072);for(let i=0;i<synthetic.length;i++)synthetic[i]=(i*17+(lang==='FR'?3:5))&255;
  fs.writeFileSync(path.join(inputs,lang+'.sav'),synthetic);
}
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const payload=JSON.parse(fs.readFileSync(path.join(stage,'sources/installer/payload.json'),'utf8'));
globalThis.NYBY_PAYLOAD=payload;
require(path.join(stage,'sources/installer/installer-core.js'));
const engine=globalThis.NyByRTC,passed=[],variants=Object.fromEntries(payload.release.variants.map(v=>[v.id,v]));
const originals=new Map();
const read=p=>{const b=fs.readFileSync(p);originals.set(p,hash(b));return b;};
const source={fr:read(path.join(privateInputs,'FR.gba')),en:read(path.join(privateInputs,'EN.gba'))};
const saves={fr:read(path.join(inputs,'FR.sav')),en:read(path.join(inputs,'EN.sav'))};
async function rejects(name,fn){await assert.rejects(async()=>fn());passed.push(name);}
function varint(n){const a=[];for(;;){const b=n%128;n=Math.floor(n/128);if(!n){a.push(b|128);return Buffer.from(a);}a.push(b);n--;}}
function bps(src,target,actions){const h=Buffer.concat([Buffer.from('BPS1'),varint(src.length),varint(target.length),varint(0),...actions]);const f=Buffer.alloc(12);f.writeUInt32LE(engine.crc32(src));f.writeUInt32LE(engine.crc32(target),4);f.writeUInt32LE(engine.crc32(Buffer.concat([h,f.subarray(0,8)])),8);return Buffer.concat([h,f]);}
(async()=>{
  assert.equal(engine.crc32(Buffer.from('123456789')),0xcbf43926);passed.push('CRC32 reference vector');
  assert.equal(await engine.sha256(Buffer.from('abc')),'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad');passed.push('SHA256 reference vector');
  const src=Buffer.from('ABCDEFGH'),target=Buffer.from('ABxyCDEFGHDEFGHxyxyxyABx');
  const fixture=bps(src,target,[varint(4),varint(5),Buffer.from('xy'),varint(22),varint(4),varint(18),varint(11),varint(5),Buffer.from('xy'),varint(15),varint(30),varint(11),varint(39)]);
  assert.deepEqual(Buffer.from(engine.applyBps(src,fixture)),target);passed.push('All four BPS actions and overlapping target copies');
  const corrupt=Buffer.from(fixture);corrupt[8]^=1;
  await rejects('Damaged BPS rejected',()=>engine.applyBps(src,corrupt));
  await rejects('Wrong BPS source CRC rejected',()=>engine.applyBps(Buffer.from('ABCDEFGX'),fixture));
  await rejects('Truncated BPS rejected',()=>engine.applyBps(src,fixture.subarray(0,10)));
  await rejects('Forward target copy rejected',()=>engine.applyBps(src,bps(src,Buffer.from('A'),[varint(3),varint(0)])));
  await rejects('Source overread rejected',()=>engine.applyBps(src,bps(src,Buffer.from('ABCDE'),[varint(18),varint(14)])));
  await rejects('Output overwrite rejected',()=>engine.applyBps(src,bps(src,Buffer.from('A'),[varint(4)])));
  for(const lang of ['fr','en']){
    const rom=await engine.prepareRom(source[lang]);
    assert.equal(rom.variant,lang);assert.equal(rom.alreadyPatched,false);
    assert.equal(hash(rom.data),variants[lang].target.sha256);passed.push(lang+' clean Unbound produces exact compiled stable output');
    const ready=await engine.prepareRom(rom.data);assert.equal(ready.alreadyPatched,true);assert.equal(hash(ready.data),hash(rom.data));passed.push(lang+' stable output reused without double patch');
    const readyPath=path.join(tests,lang+'-already-patched.gba');fs.writeFileSync(readyPath,rom.data);
    assert.equal(hash(engine.validateSave(saves[lang],lang+'.sav')),hash(saves[lang]));
    for(const withSave of [false,true]){
      const bundle=await engine.buildBundle(source[lang],withSave?saves[lang]:null,lang+'.sav');
      assert.equal(bundle.report.patch_version,'1.1');
      fs.writeFileSync(path.join(tests,`engine-${lang}-${withSave?'with':'without'}-save.zip`),Buffer.from(await bundle.blob.arrayBuffer()));
      passed.push(lang+' generated bundle '+(withSave?'with byte-identical save':'without save'));
    }
  }
  await rejects('Raw FireRed size rejected by RTC engine',()=>engine.prepareRom(Buffer.alloc(16777216)));
  const rawInput=path.join(tests,'raw-size-fixture.gba');fs.writeFileSync(rawInput,Buffer.alloc(16777216));
  const unsupported=path.join(tests,'unsupported.gba');fs.writeFileSync(unsupported,Buffer.alloc(33554432,42));
  await rejects('Unsupported full-size ROM rejected',()=>engine.prepareRom(fs.readFileSync(unsupported)));
  const damaged=Buffer.from(source.fr);damaged[0x1000]^=1;
  await rejects('Single-byte modified source rejected',()=>engine.prepareRom(damaged));
  const blank=path.join(tests,'blank.sav');fs.writeFileSync(blank,Buffer.alloc(131072,255));
  await rejects('Blank FF save rejected',()=>engine.validateSave(fs.readFileSync(blank),'blank.sav'));
  await rejects('Blank zero save rejected',()=>engine.validateSave(Buffer.alloc(131072),'blank.sav'));
  await rejects('Wrong save size rejected',()=>engine.validateSave(Buffer.alloc(65536),'small.sav'));
  await rejects('Save state rejected',()=>engine.validateSave(saves.fr,'state.ss0'));
  await rejects('ZIP traversal rejected',()=>engine.makeZip([{name:'../evil',data:Buffer.from('x')}]));
  await rejects('ZIP duplicate names rejected',()=>engine.makeZip([{name:'same',data:saves.fr},{name:'same',data:saves.fr}]));
  const first=payload.sd.fr[0],savedHash=first.sha256;first.sha256='0'.repeat(64);
  await rejects('Damaged embedded cache rejected',()=>engine.buildBundle(source.fr));first.sha256=savedHash;
  const patch=payload.release.inputs[0],savedPatch=patch.patch_sha256;patch.patch_sha256='0'.repeat(64);
  await rejects('Damaged embedded patch rejected',()=>engine.prepareRom(source.fr));patch.patch_sha256=savedPatch;
  fs.writeFileSync(path.join(tests,'engine-tests.json'),JSON.stringify({success:true,checks:passed.length,passed,source_roms_preserved:true,personal_save_used:false},null,2));
  const browser=await chromium.launch({headless:true,...(process.env.NYBY_BROWSER?{executablePath:process.env.NYBY_BROWSER}:{channel:'chrome'}),args:['--disable-crash-reporter','--no-first-run','--no-default-browser-check']});
  const proof={success:false,browser:await browser.version(),offline:true,file_url:true,checks:[]},errors=[],network=[];
  try{
    const context=await browser.newContext({viewport:{width:1100,height:960},offline:true,acceptDownloads:true,serviceWorkers:'block'});
    const page=await context.newPage();page.on('pageerror',e=>errors.push(e.message));
    page.on('request',r=>{if(/^https?:/.test(r.url()))network.push(r.url());});
    await page.goto(pathToFileURL(path.join(stage,'INSTALLER-FR-EN.html')).href);
    await page.waitForFunction(()=>!!globalThis.NyByRTC);
    assert.equal(await page.locator('#build').isDisabled(),true);
    assert.match(await page.locator('[data-i18n="notice"]').innerText(),/STABLE 1\.1/);
    assert.match(await page.locator('[data-i18n="intro"]').innerText(),/Règle la date/);
    assert.match(await page.locator('[data-i18n="rtc"]').innerText(),/désactivée/);
    assert.match(await page.locator('[data-i18n="launchHelp"]').innerText(),/gauche\/droite/);
    assert.equal(payload.release.required_superfw.rtc,false);
    for(const [filename,language] of [['INSTALLER-EN.html','en'],['INSTALLER-FR.html','fr']]){
      const alias=await context.newPage();
      await alias.goto(pathToFileURL(path.join(stage,filename)).href);
      await alias.waitForFunction(()=>!!globalThis.NyByRTC);
      assert.equal(await alias.locator('html').getAttribute('lang'),language);
      assert.equal(await alias.locator('#lang').inputValue(),language);
      assert.equal(await alias.locator('#build').isDisabled(),true);
      await alias.close();proof.checks.push(filename+' default language and offline startup');
    }

    await page.screenshot({path:path.join(tests,'installer-FR.png'),fullPage:true});
    proof.checks.push('French stable screen; empty submission disabled');
    for(const lang of ['fr','en'])for(const withSave of [false,true]){
      // Deliberately choose the opposite UI language: it must not change the game.
      await page.locator('#lang').selectOption(lang==='fr'?'en':'fr');
      await page.locator('#rom').setInputFiles(path.join(privateInputs,lang.toUpperCase()+'.gba'));
      if(withSave)await page.locator('#save').setInputFiles(path.join(inputs,lang.toUpperCase()+'.sav'));
      else await page.locator('#clearSave').click();
      await page.locator('#build').click();
      await page.waitForFunction(()=>!document.getElementById('download').hidden,undefined,{timeout:30000});
      assert.match(await page.locator('#status').innerText(),new RegExp('^'+lang.toUpperCase()));
      assert.match(await page.locator('[data-i18n="rtc"]').innerText(),/désactivée|disabled/);
      const promise=page.waitForEvent('download');await page.locator('#download').click();const dl=await promise;
      assert.equal(dl.suggestedFilename(),`NyBy-Unbound-RTC-v1.1-${lang.toUpperCase()}-SD-PERSONAL.zip`);
      await dl.saveAs(path.join(tests,`browser-${lang}-${withSave?'with':'without'}-save.zip`));
      proof.checks.push(`${lang} clean Unbound -> actual HTML ZIP ${withSave?'with native save':'without save'}; interface language independent`);
    }
    for(const lang of ['fr','en']){
      await page.locator('#rom').setInputFiles(path.join(tests,lang+'-already-patched.gba'));
      await page.locator('#clearSave').click();await page.locator('#build').click();
      await page.waitForFunction(()=>!document.getElementById('download').hidden);
      const promise=page.waitForEvent('download');await page.locator('#download').click();
      await (await promise).saveAs(path.join(tests,`browser-${lang}-already-patched.zip`));
      proof.checks.push(lang+' already-patched stable output accepted');
    }
    for(const [name,file] of [['unrecognized ROM',unsupported],['raw FireRed',rawInput]]){
      await page.locator('#rom').setInputFiles(file);assert.equal(await page.locator('#download').isHidden(),true);
      await page.locator('#build').click();await page.waitForFunction(()=>document.getElementById('status').classList.contains('error'));
      assert.equal(await page.locator('#download').isHidden(),true);proof.checks.push(name+' rejected; stale download cleared');
    }
    await page.locator('#rom').setInputFiles(path.join(privateInputs,'FR.gba'));
    await page.locator('#save').setInputFiles(blank);await page.locator('#build').click();
    await page.waitForFunction(()=>document.getElementById('status').classList.contains('error'));
    assert.equal(await page.locator('#download').isHidden(),true);proof.checks.push('Blank save rejected in HTML');
    await page.locator('#clearSave').click();await page.locator('#build').click();
    await page.waitForFunction(()=>!document.getElementById('download').hidden);
    await page.setViewportSize({width:360,height:900});await page.locator('#lang').selectOption('en');
    assert.equal(await page.evaluate(()=>document.documentElement.scrollWidth>innerWidth),false);
    assert.match(await page.locator('#status').innerText(),/no save included/);
    await page.screenshot({path:path.join(tests,'installer-EN-mobile.png'),fullPage:true});
    proof.checks.push('360px viewport and language switching after completion');

    assert.deepEqual(errors,[]);assert.deepEqual(network,[]);
    Object.assign(proof,{success:true,javascript_errors:0,http_requests:0,personal_save_used:false});
    fs.writeFileSync(path.join(tests,'browser-tests.json'),JSON.stringify(proof,null,2));
    await context.close();
  }finally{await browser.close();}
  for(const [p,before] of originals)assert.equal(hash(fs.readFileSync(p)),before);
  console.log(JSON.stringify({success:true,engine_checks:passed.length,browser_checks:proof.checks.length,http_requests:network.length}));
})().catch(error=>{console.error(error.stack);process.exitCode=1;});
