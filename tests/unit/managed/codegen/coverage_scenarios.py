"""Calculate what coverage is achievable by analyzing per-file gaps."""
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

# Separate into categories
cat_totals = defaultdict(lambda: {"c": 0, "t": 0})
categories = {
    "ReferenceProof": ["ReferenceProof/"],
    "RuntimeSupport": ["RuntimeSupport/"],
    "Emission": ["Emission/"],
    "Planning": ["Planning/"],
    "Pluginization": ["Pluginization/"],
    "BuildSystem": ["BuildSystem/"],
    "Validation": ["Validation/"],
    "Lowering": ["Lowering/"],
    "Templating": ["Templating/"],
    "Root": [],  # catch-all
}
root_files = []
refproof_files = []

for fn, d in file_data.items():
    if '/obj/' in fn:
        continue
    categorized = False
    for cat, prefixes in categories.items():
        if any(p in fn for p in prefixes):
            cat_totals[cat]["c"] += d["c"]
            cat_totals[cat]["t"] += d["t"]
            categorized = True
            if cat == "ReferenceProof":
                refproof_files.append((fn, d))
            break
    if not categorized:
        cat_totals["Root"]["c"] += d["c"]
        cat_totals["Root"]["t"] += d["t"]
        root_files.append((fn, d))

# Remove categories with zero lines
cat_totals = {k: v for k, v in cat_totals.items() if v["t"] > 0}

total_c = sum(v["c"] for v in cat_totals.values())
total_t = sum(v["t"] for v in cat_totals.values())

print("=" * 70)
print(f"CURRENT: {total_c}/{total_t} = {total_c/total_t*100:.1f}%")
print()

# Scenario 1: boost each non-ReferenceProof category by 10pp
print("SCENARIO 1: +10pp in each non-ReferenceProof category")
new_c = 0
for cat, v in sorted(cat_totals.items()):
    current_pct = v["c"] / v["t"] * 100
    target_pct = min(current_pct + 10, 95)
    target_covered = int(v["t"] * target_pct / 100)
    added = target_covered - v["c"]
    new_c += target_covered
    print(f"  {cat:20s}: {current_pct:5.1f}% -> {target_pct:5.1f}% (+{added:5d})")
new_total = new_c + sum(v["c"] for cat, v in cat_totals.items() if cat == "ReferenceProof")
print(f"  Result: {new_total}/{total_t} = {new_total/total_t*100:.1f}%")
print()

# Scenario 2: double ReferenceProof coverage to 5%
print("SCENARIO 2: ReferenceProof 0% -> 5%")
if "ReferenceProof" in cat_totals:
    rp = cat_totals["ReferenceProof"]
    rp_current = rp["c"] / rp["t"] * 100
    rp_target = 5.0
    rp_gain = int(rp["t"] * rp_target / 100) - rp["c"]
    if rp_gain < 0:
        rp_gain = 0
    new_c2 = total_c + rp_gain
    print(f"  ReferenceProof: {rp_current:.1f}% -> {rp_target:.1f}% (+{rp_gain})")
    print(f"  Result: {new_c2}/{total_t} = {new_c2/total_t*100:.1f}%")
print()

# Scenario 3: combined
print("SCENARIO 3: +10pp non-ReferenceProof + ReferenceProof to 5%")
if "ReferenceProof" in cat_totals:
    sc3_c = new_c
    rp_gain = max(0, int(rp["t"] * 0.05) - rp["c"])
    sc3_c += rp["c"] + rp_gain
    print(f"  Result: {sc3_c}/{total_t} = {sc3_c/total_t*100:.1f}%")
print()

# What would it take to reach 30%?
needed = int(total_t * 0.3) - total_c
print(f"TO REACH 30%: Need {needed} more covered lines")
print(f"That's {needed/total_t*100:.1f}pp more")

# Show biggest non-ReferenceProof gaps
print()
print("BIGGEST NON-REFERENCEPROOF GAPS (sorted by uncovered lines):")
non_rp = [(fn, d) for fn, d in root_files]
non_rp_sorted = sorted(non_rp, key=lambda x: x[1]["t"] - x[1]["c"], reverse=True)
for fn, d in non_rp_sorted[:15]:
    uncovered = d["t"] - d["c"]
    pct = d["c"] / d["t"] * 100 if d["t"] > 0 else 0
    short_fn = fn.split('Chaos.IL2CPP.Generator/')[-1] if 'Chaos.IL2CPP.Generator/' in fn else fn
    print(f"  {uncovered:5d} uncovered ({pct:.1f}%)  {short_fn}")
