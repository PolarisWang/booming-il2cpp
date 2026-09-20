import json, io
from collections import Counter

def load(p):
    d = json.load(io.open(p, encoding='utf-8'))
    return {r['methodSubjectId']: r for r in d['aot']}

before = load('docs/dev/in-progress/ctor-handle-abi-fix/notes/fact-baseline.json')
after  = load('artifacts/foundation-dll/System.Private.Xml/chunks/xml/results/fact-results.json')

print(f"baseline={len(before)}  after={len(after)}")
ob, oa = set(before) - set(after), set(after) - set(before)
print(f"only-baseline={len(ob)}  only-after={len(oa)}")
for s in list(ob)[:5]: print("   -", s)
for s in list(oa)[:5]: print("   +", s)

imp, reg = [], []
for sid in sorted(set(before) & set(after)):
    b, a = before[sid], after[sid]
    if b['resultKind'] != a['resultKind'] or b['passed'] != a['passed']:
        (imp if (a['passed'] and not b['passed']) else reg).append((sid, b, a))

def cls(r): return f"{r['resultKind']}(pass={r['passed']})"

print(f"\n=== 改善 {len(imp)} ===")
for k, v in Counter(f"{cls(b)} -> {cls(a)}" for _, b, a in imp).most_common():
    print(f"  {v:4d}  {k}")

print(f"\n=== 回退 {len(reg)} ===")
if reg:
    for k, v in Counter(f"{cls(b)} -> {cls(a)}" for _, b, a in reg).most_common():
        print(f"  {v:4d}  {k}")
    print("  --- 明细 ---")
    for sid, b, a in reg[:30]:
        print(f"    {sid.split('::')[-1]:60s} {cls(b)} -> {cls(a)}")
else:
    print("  （无）")
