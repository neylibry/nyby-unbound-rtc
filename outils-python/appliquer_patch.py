"""NyBy manual RTC: offline BPS installer, Python 3.10+ standard library.

Only the SHA-256 verified inputs in release.json are accepted. No downloads,
no in-place modification, no save access, and no overwrite of an existing file.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import sys

from patch_formats import apply_bps

ROOT = Path(__file__).resolve().parent


def digest(data):
    return hashlib.sha256(data).hexdigest()


def apply_release(source, output=None):
    manifest = json.loads((ROOT / 'release.json').read_text(encoding='utf-8'))
    source = Path(source).expanduser().resolve(strict=True)
    if source.suffix.lower() != '.gba':
        raise ValueError('Choisir un fichier .gba / Select a .gba file')
    original = source.read_bytes()
    source_sha = digest(original)
    endpoint = next((item for item in manifest['inputs'] if item['sha256'] == source_sha), None)
    if endpoint is None:
        if any(source_sha == variant['target']['sha256'] for variant in manifest['variants']):
            raise ValueError('ROM deja a jour / ROM already up to date: ' + manifest['version'])
        raise ValueError('ROM source non reconnue / Wrong source ROM. SHA-256: ' + source_sha)
    target = next(v['target'] for v in manifest['variants'] if v['id'] == endpoint['variant'])
    patch_path = ROOT / endpoint['patch']
    patch = patch_path.read_bytes()
    if digest(patch) != endpoint['patch_sha256']:
        raise ValueError('Patch endommage / Damaged patch file')
    result = apply_bps(original, patch)
    if len(result) != target['bytes'] or digest(result) != target['sha256']:
        raise ValueError('Resultat incorrect / Output verification failed')
    output = (Path(output).expanduser() if output else ROOT / 'ROM-patchee' / target['filename']).resolve()
    if output.suffix.lower() != '.gba':
        raise ValueError('La sortie doit etre un nouveau .gba / Output must be a new .gba file')
    if source == output:
        raise ValueError('La source doit etre conservee / Cannot replace the source ROM')
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open('xb') as handle:
        handle.write(result)
        handle.flush()
        os.fsync(handle.fileno())
    if digest(output.read_bytes()) != target['sha256']:
        raise OSError('Verification du fichier ecrit impossible / Written file verification failed')
    if digest(source.read_bytes()) != source_sha:
        raise OSError('La source a change / Source changed during installation')
    return output


def main():
    parser = argparse.ArgumentParser(description='NyBy RTC - application locale / offline patching')
    parser.add_argument('source', nargs='?', help='ROM VF FR179 ou beta1 / French FR179 or beta1 ROM')
    parser.add_argument('--output', type=Path, help='Nouveau fichier .gba / New .gba file (must not exist)')
    args = parser.parse_args()
    source = args.source
    if not source:
        try:
            import tkinter as tk
            from tkinter import filedialog
            window = tk.Tk()
            window.withdraw()
            source = filedialog.askopenfilename(title='Choisir la ROM source / Choose the source ROM',
                                               filetypes=[('GBA ROM', '*.gba')])
            window.destroy()
        except Exception as error:
            parser.error('Indiquer le chemin de la ROM / Supply the ROM path: ' + str(error))
        if not source:
            return 0
    try:
        output = apply_release(source, args.output)
    except (OSError, ValueError, KeyError) as error:
        print('ERREUR / ERROR: ' + str(error), file=sys.stderr)
        return 1
    print('OK - ROM creee et verifiee / ROM created and verified: ' + str(output))
    print('Copier ensuite le dossier SD et la ROM sur la carte. Voir README_FR.md.')
    print('Next, copy the SD folder contents and the ROM to the card. See README_EN.md.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
