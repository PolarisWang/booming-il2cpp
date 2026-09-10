import xml.etree.ElementTree as ET
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if 'NativeAotEmitter' not in fn or '.cs' not in fn:
        continue
    print('File:', fn)
    print('Line rate:', cls.get('line-rate'))
    for m in cls.findall('.//method'):
        mname = m.get('name', '?')
        lines = m.findall('.//line')
        covered = sum(1 for l in lines if int(l.get('hits', '0')) > 0)
        t = len(lines)
        if t > 0:
            pct = covered / t * 100
            print(f'  {pct:5.1f}%  {mname} ({covered}/{t})')
        else:
            print(f'  ----  {mname}')
