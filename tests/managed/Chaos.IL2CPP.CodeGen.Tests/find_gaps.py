import xml.etree.ElementTree as ET
from collections import defaultdict
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')
target = 'CodeGen'

# Group by file (deduplicate class entries)
file_data = defaultdict(lambda: {"c": 0, "t": 0})

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if target not in fn:
        continue
    fn_norm = fn.replace('\\', '/')
    lines = cls.findall('.//line')
    if not lines:
        continue
    covered = sum(1 for l in lines if int(l.get('hits', '0')) > 0)
    t = len(lines)
    file_data[fn_norm]["c"] += covered
    file_data[fn_norm]["t"] += t

# Filter to src/ only (not obj/)
src_files = [(fn, d) for fn, d in file_data.items() if '/obj/' not in fn]

# Sort by uncovered lines descending
sorted_files = sorted(src_files, key=lambda x: x[1]["t"] - x[1]["c"], reverse=True)

print(f"{'Coverage':>8} {'Uncovered':>10} {'Total':>8}  File")
print("-" * 80)
for fn, d in sorted_files[:50]:
    pct = d["c"] / d["t"] * 100 if d["t"] > 0 else 0
    uncovered = d["t"] - d["c"]
    short_fn = fn.split('Chaos.IL2CPP.Generator/')[-1] if 'Chaos.IL2CPP.Generator/' in fn else fn
    print(f'{pct:>7.1f}% {uncovered:>10} {d["t"]:>8}  {short_fn}')
