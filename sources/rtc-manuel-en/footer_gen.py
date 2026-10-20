"""Build-time conversion of our 3x5 footer; not executed on GBA."""
import ast,re
GLYPHS={16: 31599, 17: 29850, 18: 29347, 19: 14499, 20: 18925, 21: 14543, 22: 31694, 23: 9383, 24: 31727, 25: 14831, 33: 23530, 34: 15083, 35: 25166, 36: 15211, 37: 29391, 38: 4815, 39: 27470, 40: 23533, 41: 29847, 42: 11044, 43: 23277, 44: 29257, 45: 23549, 46: 24573, 47: 11114, 48: 4843, 49: 28522, 50: 23275, 51: 14478, 52: 9367, 53: 31597, 54: 11117, 55: 24557, 56: 23213, 57: 9389, 58: 29351, 14: 8192, 15: 4772, 13: 448}
def generate(root):
    info=(root/'build_info.h').read_text()
    text=ast.literal_eval(re.search(r'^#define NYBY_FOOTER (".*")$',info,re.M)[1])
    values=[]
    for ch in text:
        assert 32<=ord(ch)<=90, 'Unsupported footer character'
        if ch==' ':values.append(0)
        else:
            assert ord(ch)-32 in GLYPHS, 'Missing footer glyph: '+ch
            values.append(GLYPHS[ord(ch)-32])
    assert 1<=len(values)<=60 and len(values)*4-1<=239
    header='/* Generated from NYBY_FOOTER: '+text+' */\n'
    header+='static const u16 footer_glyphs[]={'+','.join(hex(v) for v in values)+'};\n'
    (root/'footer.h').write_text(header,encoding='utf-8')
