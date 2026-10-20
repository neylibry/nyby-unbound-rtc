/* ROM-free tests against the production JS engine. Node.js 22+. */
'use strict';
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
globalThis.NYBY_PAYLOAD=JSON.parse(fs.readFileSync(path.join(__dirname,'../sources/installer/payload.json'),'utf8'));
require('../sources/installer/installer-core.js');
const engine=globalThis.NyByRTC,passed=[];
function varint(n){const a=[];for(;;){const b=n%128;n=Math.floor(n/128);if(!n){a.push(b|128);return Buffer.from(a);}a.push(b);n--;}}
function bps(src,target,actions){const h=Buffer.concat([Buffer.from('BPS1'),varint(src.length),varint(target.length),varint(0),...actions]);const f=Buffer.alloc(12);f.writeUInt32LE(engine.crc32(src));f.writeUInt32LE(engine.crc32(target),4);f.writeUInt32LE(engine.crc32(Buffer.concat([h,f.subarray(0,8)])),8);return Buffer.concat([h,f]);}
async function rejects(name,fn){await assert.rejects(async()=>fn());passed.push(name);}
(async()=>{
  assert.equal(engine.crc32(Buffer.from('123456789')),0xcbf43926);passed.push('CRC32 reference');
  assert.equal(await engine.sha256(Buffer.from('abc')),'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad');passed.push('SHA256 reference');
  const src=Buffer.from('ABCDEFGH'),target=Buffer.from('ABxyCDEFGHDEFGHxyxyxyABx');
  const fixture=bps(src,target,[varint(4),varint(5),Buffer.from('xy'),varint(22),varint(4),varint(18),varint(11),varint(5),Buffer.from('xy'),varint(15),varint(30),varint(11),varint(39)]);
  assert.deepEqual(Buffer.from(engine.applyBps(src,fixture)),target);passed.push('four BPS modes / overlapping copy');
  const corrupt=Buffer.from(fixture);corrupt[8]^=1;
  await rejects('bad patch CRC',()=>engine.applyBps(src,corrupt));
  await rejects('wrong source CRC',()=>engine.applyBps(Buffer.from('ABCDEFGX'),fixture));
  await rejects('truncated patch',()=>engine.applyBps(src,fixture.subarray(0,10)));
  await rejects('forward target copy',()=>engine.applyBps(src,bps(src,Buffer.from('A'),[varint(3),varint(0)])));
  await rejects('source overread',()=>engine.applyBps(src,bps(src,Buffer.from('ABCDE'),[varint(18),varint(14)])));
  await rejects('output overwrite',()=>engine.applyBps(src,bps(src,Buffer.from('A'),[varint(4)])));
  await rejects('unprepared 16-MiB input',()=>engine.prepareRom(Buffer.alloc(16777216)));
  const synthetic=Buffer.alloc(131072,42);
  assert.deepEqual(Buffer.from(engine.validateSave(synthetic,'fixture.sav')),synthetic);passed.push('native save copied unchanged');
  await rejects('blank FF save',()=>engine.validateSave(Buffer.alloc(131072,255),'fixture.sav'));
  await rejects('blank zero save',()=>engine.validateSave(Buffer.alloc(131072),'fixture.sav'));
  await rejects('wrong save size',()=>engine.validateSave(Buffer.alloc(65536),'fixture.sav'));
  await rejects('save-state extension',()=>engine.validateSave(synthetic,'fixture.ss0'));
  await rejects('ZIP traversal',()=>engine.makeZip([{name:'../evil',data:synthetic}]));
  await rejects('duplicate ZIP names',()=>engine.makeZip([{name:'same',data:synthetic},{name:'same',data:synthetic}]));
  console.log(JSON.stringify({passed:true,checks:passed.length,tests:passed,rom_used:false,personal_save_used:false},null,2));
})().catch(e=>{console.error(e.stack);process.exitCode=1;});
