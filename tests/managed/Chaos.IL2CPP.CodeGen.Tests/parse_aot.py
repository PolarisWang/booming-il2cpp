import xml.etree.ElementTree as ET
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.CodeGen"]')

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if 'AotCoreIrLowering' not in fn or '.cs' not in fn:
        continue
    print('File:', fn)
    print('Line rate:', cls.get('line-rate'))
    # Group methods by coverage
    zero = []
    partial = []
    full = []
    for m in cls.findall('.//method'):
        mname = m.get('name', '?')
        lines = m.findall('.//line')
        covered = sum(1 for l in lines if int(l.get('hits', '0')) > 0)
        t = len(lines)
        if t > 0:
            pct = covered / t * 100
            entry = (pct, covered, t, mname)
            if pct == 0:
                zero.append(entry)
            elif pct == 100:
                full.append(entry)
            else:
                partial.append(entry)

    print(f'\n  FULLY COVERED ({len(full)}):')
    for pct, c, t, n in sorted(full, key=lambda x: x[3]):
        print(f'    100.0%  {n}')

    print(f'\n  PARTIAL ({len(partial)}):')
    for pct, c, t, n in sorted(partial, key=lambda x: x[0]):
        print(f'    {pct:5.1f}%  {n} ({c}/{t})')

    print(f'\n  ZERO COVERAGE ({len(zero)}):')
    for pct, c, t, n in sorted(zero, key=lambda x: x[3]):
        print(f'    0.0%  {n}')
PYEOF
