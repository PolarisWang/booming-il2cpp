import xml.etree.ElementTree as ET
from collections import defaultdict
import sys

def get_subdir(fn, target):
    """Extract subdirectory from filename."""
    # Try forward slash
    idx = fn.find(target + '/')
    if idx >= 0:
        sub = fn[idx:]
    else:
        # Try backward slash
        idx = fn.find(target + '\\')
        if idx >= 0:
            sub = fn[idx:]
        else:
            return None

    parts = sub.replace('\\', '/').split('/')
    if len(parts) > 1 and parts[1]:
        d = parts[1]
    else:
        d = '(root)'

    known = {'ReferenceProof', 'Emission', 'Planning', 'RuntimeSupport',
             'Pluginization', 'BuildSystem', 'Validation', 'Lowering',
             'Templating', '(root)'}
    return d if d in known else '(other)'

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')
target = 'Chaos.IL2CPP.Generator'

print(f'Root: lines-covered={root.get("lines-covered")}, lines-valid={root.get("lines-valid")}')

total = 0
covered = 0
dirs = defaultdict(lambda: {"c": 0, "t": 0})

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if target not in fn:
        continue
    sd = get_subdir(fn, target)
    if sd is None:
        continue

    for line in cls.findall('.//line'):
        total += 1
        dirs[sd]["t"] += 1
        hits = int(line.get('hits', '0'))
        if hits > 0:
            covered += 1
            dirs[sd]["c"] += 1

print(f'\nCodeGen total: {covered}/{total} = {covered/total*100:.1f}%')
for d in sorted(dirs.keys()):
    v = dirs[d]
    pct = v["c"] / v["t"] * 100 if v["t"] > 0 else 0
    print(f'  {d:20s}: {pct:5.1f}% ({v["c"]:5d}/{v["t"]:5d})')
