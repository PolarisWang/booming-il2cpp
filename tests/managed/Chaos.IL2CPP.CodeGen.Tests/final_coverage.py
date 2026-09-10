"""Calculate final coverage from coverlet output with ReferenceProof/PatchData excluded."""
import xml.etree.ElementTree as ET
from collections import defaultdict
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')
target = 'CodeGen'

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

# Categories
cats = defaultdict(lambda: {"c": 0, "t": 0})
for fn, d in file_data.items():
    fn_norm = fn.replace('\\', '/')
    if '/obj/' in fn_norm:
        continue
    if '/ReferenceProof/' in fn_norm or 'PatchData' in fn_norm:
        continue
    categorized = False
    for prefix, name in [('/Emission/', 'Emission'), ('/Planning/', 'Planning'),
                          ('/RuntimeSupport/', 'RuntimeSupport'), ('/Pluginization/', 'Pluginization'),
                          ('/BuildSystem/', 'BuildSystem'), ('/Validation/', 'Validation'),
                          ('/Lowering/', 'Lowering'), ('/Templating/', 'Templating')]:
        if prefix in fn_norm:
            cats[name]["c"] += d["c"]
            cats[name]["t"] += d["t"]
            categorized = True
            break
    if not categorized:
        cats["Root"]["c"] += d["c"]
        cats["Root"]["t"] += d["t"]

total_c = sum(v["c"] for v in cats.values())
total_t = sum(v["t"] for v in cats.values())

print(f'Final CodeGen coverage: {total_c}/{total_t} = {total_c/total_t*100:.1f}%')
print()
for name in sorted(cats.keys()):
    v = cats[name]
    pct = v["c"] / v["t"] * 100 if v["t"] > 0 else 0
    print(f'  {name:20s}: {pct:5.1f}% ({v["c"]:5d}/{v["t"]:5d})')
