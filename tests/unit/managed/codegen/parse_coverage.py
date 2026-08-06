import xml.etree.ElementTree as ET
from collections import defaultdict
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')
target = 'Chaos.IL2CPP.Generator'
all_total_l = 0
all_total_c = 0
dirs = defaultdict(lambda: {"c": 0, "t": 0})

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if target not in fn:
        continue
    lines = cls.findall('.//line')
    if not lines:
        continue
    covered = sum(1 for l in lines if int(l.get('hits', '0')) > 0)
    t = len(lines)
    all_total_l += t
    all_total_c += covered
    # Get subdirectory
    sub = fn
    for sep in ['/']:
        idx = fn.find('Chaos.IL2CPP.Generator' + sep)
        if idx >= 0:
            sub = fn[idx + len('Chaos.IL2CPP.Generator' + sep):]
            break
    if not sub or sub == fn:
        for sep in ['\\']:
            idx = fn.find('Chaos.IL2CPP.Generator' + sep)
            if idx >= 0:
                sub = fn[idx + len('Chaos.IL2CPP.Generator' + sep):]
                break
    parts = sub.replace('\\', '/').split('/')
    d = parts[0] if len(parts) > 1 and parts[0] else '(root)'
    if d in ('ReferenceProof', 'RuntimeSupport', 'Pluginization', 'BuildSystem', 'Planning', 'Emission', 'Lowering', 'Validation'):
        pass  # keep as is
    elif d == '(root)':
        pass
    else:
        d = '(other)'
    dirs[d]["c"] += covered
    dirs[d]["t"] += t

print(f'CodeGen overall: {all_total_c}/{all_total_l} = {all_total_c/all_total_l*100:.1f}%')
for d, v in sorted(dirs.items()):
    pct = v["c"] / v["t"] * 100 if v["t"] > 0 else 0
    print(f'  {d}: {pct:.1f}% ({v["c"]}/{v["t"]})')

# Also print per-file breakdown for files with non-zero coverage
print()
for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if target not in fn:
        continue
    lines = cls.findall('.//line')
    if not lines:
        continue
    covered = sum(1 for l in lines if int(l.get('hits', '0')) > 0)
    t = len(lines)
    if covered > 0:
        pct = covered / t * 100
        # Get short filename
        for sep in ['/']:
            idx = fn.find('CodeGen' + sep)
            if idx >= 0:
                fn = fn[idx + len('CodeGen' + sep):]
                break
        if fn == cls.get('filename', ''):
            for sep in ['\\']:
                idx = cls.get('filename', '').find('CodeGen' + sep)
                if idx >= 0:
                    fn = cls.get('filename', '')[idx + len('CodeGen' + sep):]
                    break
        print(f'  {pct:5.1f}%  {fn}  ({covered}/{t})')
