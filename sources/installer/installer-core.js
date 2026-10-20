/* NyBy RTC community installer. Independent BPS/ZIP implementation; no network I/O. */
(() => {
  'use strict';
  const payload = globalThis.NYBY_PAYLOAD;
  const manifest = payload.release;
  const encoder = new TextEncoder();
  const fail = (fr, en) => { throw new Error(fr + ' / ' + en); };
  const decode = text => Uint8Array.from(atob(text), c => c.charCodeAt(0));
  const crcTable = Uint32Array.from({length: 256}, (_, n) => {
    for (let i = 0; i < 8; i++) n = (n & 1) ? (0xedb88320 ^ (n >>> 1)) : (n >>> 1);
    return n >>> 0;
  });
  function crc32(bytes) {
    let crc = 0xffffffff;
    for (let i = 0; i < bytes.length; i++) crc = crcTable[(crc ^ bytes[i]) & 255] ^ (crc >>> 8);
    return (crc ^ 0xffffffff) >>> 0;
  }
  async function sha256(bytes) {
    if (!globalThis.crypto?.subtle) fail('Ouvrir ce fichier dans un navigateur récent', 'Open this file in a modern browser');
    const hash = await crypto.subtle.digest('SHA-256', bytes);
    return Array.from(new Uint8Array(hash), b => b.toString(16).padStart(2, '0')).join('');
  }
  function applyBps(source, patch) {
    if (patch.length < 19 || new TextDecoder().decode(patch.subarray(0, 4)) !== 'BPS1') fail('Patch BPS invalide', 'Invalid BPS patch');
    const end = patch.length - 12;
    const footer = new DataView(patch.buffer, patch.byteOffset + end, 12);
    if (crc32(patch.subarray(0, patch.length - 4)) !== footer.getUint32(8, true)) fail('Patch endommagé', 'Damaged patch');
    let at = 4;
    function number() {
      let value = 0, weight = 1;
      for (let i = 0; i < 9; i++) {
        if (at >= end) fail('Patch tronqué', 'Truncated patch');
        const b = patch[at++];
        value += (b & 127) * weight;
        if (!Number.isSafeInteger(value)) fail('Entier BPS trop grand', 'BPS integer overflow');
        if (b & 128) return value;
        weight *= 128;
        value += weight;
      }
      fail('Entier BPS invalide', 'Invalid BPS integer');
    }
    const sourceSize = number(), targetSize = number(), metadataSize = number();
    if (sourceSize !== source.length || targetSize > 64 * 1024 * 1024 || at + metadataSize > end)
      fail('Taille source ou en-tête incorrect', 'Wrong source size or invalid header');
    if (crc32(source) !== footer.getUint32(0, true)) fail('La ROM ne correspond pas au patch', 'Source CRC mismatch');
    at += metadataSize;
    const out = new Uint8Array(targetSize);
    let pos = 0, sourceOffset = 0, targetOffset = 0;
    while (at < end) {
      const action = number(), mode = action % 4, length = Math.floor(action / 4) + 1;
      if (length > targetSize - pos) fail('Écriture BPS hors limites', 'BPS output out of bounds');
      if (mode === 0) {
        if (pos + length > source.length) fail('Lecture BPS hors limites', 'BPS source read out of bounds');
        out.set(source.subarray(pos, pos + length), pos);
      } else if (mode === 1) {
        if (length > end - at) fail('Données BPS tronquées', 'Truncated BPS data');
        out.set(patch.subarray(at, at + length), pos);
        at += length;
      } else {
        const encoded = number(), delta = Math.floor(encoded / 2) * (encoded % 2 ? -1 : 1);
        if (mode === 2) {
          sourceOffset += delta;
          if (sourceOffset < 0 || sourceOffset + length > source.length) fail('Copie source invalide', 'Invalid source copy');
          out.set(source.subarray(sourceOffset, sourceOffset + length), pos);
          sourceOffset += length;
        } else {
          targetOffset += delta;
          if (targetOffset < 0 || targetOffset >= pos) fail('Copie cible invalide', 'Invalid target copy');
          for (let i = 0; i < length; i++) out[pos + i] = out[targetOffset++];
        }
      }
      pos += length;
    }
    if (at !== end || pos !== targetSize || crc32(out) !== footer.getUint32(4, true)) fail('Résultat BPS non conforme', 'BPS output verification failed');
    return out;
  }
  async function prepareRom(source) {
    if (source.length !== 33554432) fail('La ROM attendue fait 32 Mio', 'The required ROM is 32 MiB');
    const inputHash = await sha256(source);
    const ready = manifest.variants.find(v => v.target.sha256 === inputHash);
    if (ready) return {data: source.slice(), alreadyPatched: true, source: ready.target.filename, variant: ready.id};
    const endpoint = manifest.inputs.find(item => item.sha256 === inputHash);
    if (!endpoint) fail('ROM non reconnue. SHA-256 : ' + inputHash, 'Unsupported ROM. See README_EN.md');
    const variant = manifest.variants.find(v => v.id === endpoint.variant);
    if (!variant) fail('Profil de jeu manquant', 'Missing game variant');
    const patch = decode(payload.patches[endpoint.patch]);
    if (await sha256(patch) !== endpoint.patch_sha256) fail('Patch intégré endommagé', 'Embedded patch is damaged');
    const result = applyBps(source, patch);
    if (result.length !== variant.target.bytes || await sha256(result) !== variant.target.sha256) fail('Résultat incorrect', 'Output SHA-256 verification failed');
    return {data: result, alreadyPatched: false, source: endpoint.name, variant: variant.id};
  }
  function validateSave(bytes, name) {
    if (!name.toLowerCase().endsWith('.sav') || bytes.length !== 131072)
      fail('Choisir une sauvegarde native .sav de 128 Kio, pas un savestate', 'Select a native 128 KiB .sav file, not a save state');
    if (bytes.every(b => b === 255) || bytes.every(b => b === 0)) fail('Sauvegarde vide', 'Blank save file');
    return bytes.slice();
  }
  function makeZip(entries) {
    const locals = [], centrals = [];
    let offset = 0;
    const names = new Set();
    for (const {name, data} of entries) {
      if (!name || name.startsWith('/') || name.includes('\\') || name.split('/').some(p => p === '..' || !p) || names.has(name))
        fail('Chemin ZIP invalide', 'Invalid ZIP path');
      names.add(name);
      const filename = encoder.encode(name), crc = crc32(data);
      const local = new Uint8Array(30 + filename.length), lv = new DataView(local.buffer);
      lv.setUint32(0, 0x04034b50, true); lv.setUint16(4, 20, true); lv.setUint16(6, 0x800, true);
      lv.setUint16(12, ((2026 - 1980) << 9) | (10 << 5) | 20, true);
      lv.setUint32(14, crc, true); lv.setUint32(18, data.length, true); lv.setUint32(22, data.length, true);
      lv.setUint16(26, filename.length, true); local.set(filename, 30);
      const central = new Uint8Array(46 + filename.length), cv = new DataView(central.buffer);
      cv.setUint32(0, 0x02014b50, true); cv.setUint16(4, 20, true); cv.setUint16(6, 20, true);
      cv.setUint16(8, 0x800, true); cv.setUint16(14, ((2026 - 1980) << 9) | (10 << 5) | 20, true);
      cv.setUint32(16, crc, true); cv.setUint32(20, data.length, true); cv.setUint32(24, data.length, true);
      cv.setUint16(28, filename.length, true); cv.setUint32(42, offset, true); central.set(filename, 46);
      locals.push(local, data); centrals.push(central); offset += local.length + data.length;
    }
    const centralSize = centrals.reduce((n, bytes) => n + bytes.length, 0);
    const end = new Uint8Array(22), ev = new DataView(end.buffer);
    ev.setUint32(0, 0x06054b50, true); ev.setUint16(8, entries.length, true); ev.setUint16(10, entries.length, true);
    ev.setUint32(12, centralSize, true); ev.setUint32(16, offset, true);
    return new Blob([...locals, ...centrals, end], {type: 'application/zip'});
  }
  async function buildBundle(sourceBytes, saveBytes = null, saveName = '') {
    const rom = await prepareRom(sourceBytes);
    const variant = manifest.variants.find(v => v.id === rom.variant);
    const entries = [{name: variant.target.filename, data: rom.data}];
    for (const file of payload.sd[variant.id]) {
      const data = decode(file.base64);
      if (await sha256(data) !== file.sha256) fail('Profil SuperFW endommagé', 'Damaged SuperFW asset');
      entries.push({name: file.path, data});
    }
    let saveHash = null;
    if (saveBytes !== null) {
      const data = validateSave(saveBytes, saveName);
      saveHash = await sha256(data);
      entries.push({name: 'SAVEGAME/' + variant.target.filename.replace(/\.gba$/, '.sav'), data});
    }
    const report = {patch_version: manifest.version, source: rom.source, game_language: variant.id,
      already_patched: rom.alreadyPatched, output_rom_sha256: variant.target.sha256,
      save_copied_without_conversion: saveBytes !== null, save_sha256: saveHash,
      save_directory: 'SAVEGAME (change if configured differently in SuperFW)',
      private_generated_bundle: true, hardware_validation: variant.validation};
    entries.push({name: 'INSTALLATION-FR-EN.txt', data: encoder.encode(payload.copyHelp)});
    entries.push({name: 'INSTALLATION-REPORT.json', data: encoder.encode(JSON.stringify(report, null, 2))});
    return {blob: makeZip(entries), report};
  }
  globalThis.NyByRTC = Object.freeze({crc32, sha256, applyBps, prepareRom, validateSave, makeZip, buildBundle});
})();
