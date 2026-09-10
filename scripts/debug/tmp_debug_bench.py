import json

# Read report (latest run)
with open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/unified-verification-report.json', encoding='utf-8') as f:
    r = json.load(f)
b = r.get('stages', {}).get('benchmark', {}).get('details', {})

print("=== AOT results ===")
for res in b.get('native-aot', {}).get('results', []):
    mi = res.get('methodIndex')
    ops = res.get('opsPerSecond', 0)
    err = res.get('error', '')
    ns = (1e9 / ops) if ops and ops > 0 else 0
    print(f"  methodIndex={mi}, ops={ops}, ns={ns:.1f}, err={err}")

print("\n=== JIT results ===")
for res in b.get('native-jit', {}).get('results', []):
    mi = res.get('methodIndex')
    ops = res.get('opsPerSecond', 0)
    err = res.get('error', '')
    ns = (1e9 / ops) if ops and ops > 0 else 0
    print(f"  methodIndex={mi}, ops={ops}, ns={ns:.1f}, err={err}")

# Read contract
contract = json.load(open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/capability-family-contract.json', encoding='utf-8'))
mids = contract.get('methodSubjectIds', [])
bmis = contract.get('benchmarkMethodIndices', [])
print(f"\n=== Contract ===")
print(f"methodSubjectIds count: {len(mids)}")
print(f"benchmarkMethodIndices: {bmis}")

# Read old multi-run for net8 data
multi = json.load(open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/multi-run/multi-run-report.json', encoding='utf-8'))
print(f"\n=== Multi-run methods ===")
for m in multi.get('methods', []):
    label = m.get('label', '')
    net8 = m.get('net8_ns_per_op')
    sid = m.get('subject_id', '')
    aot_ns = m.get('aot_ns_per_op')
    jit_ns = m.get('jit_ns_per_op')
    print(f"  label={label:<60} sid={sid[:50]:<50} aot={aot_ns} jit={jit_ns} net8={net8}")
