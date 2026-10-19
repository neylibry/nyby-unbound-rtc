"""Rebuild a matching SuperFW bridge cache after compiling the RTC module.

Use an exact supported clean Unbound input and the supplied cache as a template.
The 19 compatibility operations are preserved; only the dedicated RTC probe moves.
"""
from pathlib import Path
import argparse,hashlib,json,struct

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--build',required=True,type=Path,help='Directory containing build.json and the new .gba')
    p.add_argument('--rom',required=True,type=Path,help='Exact clean Unbound source used for compilation')
    p.add_argument('--template',required=True,type=Path,help='Supplied beta2 .patch for the same language')
    p.add_argument('--output',required=True,type=Path,help='New .patch path; existing files are never overwritten')
    a=p.parse_args()
    report=json.loads((a.build/'build.json').read_text(encoding='utf-8'))
    source=a.rom.read_bytes();rom=(a.build/(report['rom_name']+'.gba')).read_bytes()
    assert sha(source)==report['source_sha256'] and sha(rom)==report['target_sha256']
    template=bytearray(a.template.read_bytes())
    assert sha(template) in {'f394eb3ef42b84c461985bcfacd7c746b8211823fa59dff57fd8a46a78650501',
                             '4a8dff9ed73b65fa1eb22c39d205b1e2b9649d038c64b4bcfa9605f4e6a7e8e7'}
    assert len(template)==800 and template[:16]==b'SUPERFWPATCHV01\0'
    assert [template[i] for i in [16,17,19,20]]==[7,8,4,1]
    used=template[16]+template[17]+template[19]
    for word in struct.unpack_from('<'+'I'*used,template,288):
        opcode,arg,offset=word>>28,(word>>25)&7,word&0x1ffffff
        if opcode==0:
            assert arg<4;size=struct.unpack_from('<I',template,32+arg*64)[0]
        elif opcode in [1,2]:size=2 if opcode==1 else 4
        elif opcode==5:size=4 if arg<2 else 8
        elif opcode==9:size=2048
        else:raise AssertionError(('Unsupported compatibility operation',hex(word)))
        assert rom[offset:offset+size]==source[offset:offset+size],hex(word)
    hinfo=struct.unpack_from('<I',template,22)[0]
    start,size=(hinfo>>16)*1024,(hinfo&0xffff)*1024
    payload_start=int(report['payload_offset'],16)
    assert size>=7*1024 and rom[start:start+size]==b'\xff'*size
    assert max(start,payload_start)>=min(start+size,payload_start+report['payload_bytes'])
    gate=report['symbols']['nyby_superfw_gate']['address']-0x08000000
    assert gate%4==0 and payload_start<=gate<payload_start+report['payload_bytes']
    assert rom[gate:gate+4]==struct.pack('<I',0x5942594e)
    struct.pack_into('<I',template,288+used*4,0x70000000|gate)
    if a.output.exists():
        assert a.output.read_bytes()==template,'Existing cache differs; use a new output path'
    else:
        a.output.parent.mkdir(parents=True,exist_ok=True)
        with a.output.open('xb') as f:f.write(template)
    print(json.dumps({'success':True,'bytes':len(template),'sha256':sha(template),'gate':hex(gate)}))

if __name__=='__main__':main()
