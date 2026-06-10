import json

# Read contract
contract = json.load(open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/capability-family-contract.json', encoding='utf-8'))
mids = contract.get('methodSubjectIds', [])
bmis = contract.get('benchmarkMethodIndices', list(range(len(mids))))

# Read report (latest run)
with open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/unified-verification-report.json', encoding='utf-8') as f:
    r = json.load(f)
b = r.get('stages', {}).get('benchmark', {}).get('details', {})

# Read old multi-run for .NET 8 baseline
multi = json.load(open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/multi-run/multi-run-report.json', encoding='utf-8'))
net8_by_label = {}
for m in multi.get('methods', []):
    net8 = m.get('net8_ns_per_op')
    if net8 is not None:
        net8_by_label[m.get('label', '')] = net8

# Build subject_id -> label mapping from multi-run
sid_to_label = {}
for m in multi.get('methods', []):
    sid = m.get('subject_id', '')
    label = m.get('label', '')
    if sid:
        sid_to_label[sid] = label

# Parse benchmark results
aot_by_idx = {}
for res in b.get('native-aot', {}).get('results', []):
    mi = res.get('methodIndex')
    if mi is not None:
        aot_by_idx[mi] = res

jit_by_idx = {}
for res in b.get('native-jit', {}).get('results', []):
    mi = res.get('methodIndex')
    if mi is not None:
        jit_by_idx[mi] = res

print(f"{'si':>4} {'sid':<65} {'aot_ns':>10} {'jit_ns':>10} {'net8_ns':>10} {'aot_vs%':>9} {'jit_vs%':>9}")
print("-" * 117)

failures = []
for bi in bmis:
    sid = mids[bi] if bi < len(mids) else f'index_{bi}'

    aot_res = aot_by_idx.get(bi, {})
    jit_res = jit_by_idx.get(bi, {})

    aot_ops = aot_res.get('opsPerSecond', 0) or 0
    jit_ops = jit_res.get('opsPerSecond', 0) or 0
    aot_ns = (1e9 / aot_ops) if aot_ops > 0 else 0
    jit_ns = (1e9 / jit_ops) if jit_ops > 0 else 0

    # Find net8 baseline for this subject
    label = sid_to_label.get(sid, '')
    net8_ns = net8_by_label.get(label, 0)

    aot_vs = (aot_ns / net8_ns * 100) if net8_ns > 0 else 0
    jit_vs = (jit_ns / net8_ns * 100) if net8_ns > 0 else 0

    aot_str = f"{aot_ns:.1f}" if aot_ns > 0 else "ERR"
    jit_str = f"{jit_ns:.1f}" if jit_ns > 0 else "ERR"
    net8_str = f"{net8_ns:.1f}" if net8_ns > 0 else "N/A"
    aot_vs_str = f"{aot_vs-100:.0f}%" if aot_vs > 0 else "N/A"
    jit_vs_str = f"{jit_vs-100:.0f}%" if jit_vs > 0 else "N/A"

    if aot_vs > 120 and net8_ns > 0:
        failures.append(f"  AOT [{bi}] {sid[:60]}: {aot_ns:.1f}ns vs {net8_ns:.1f}ns ({aot_vs-100:.0f}%)")
    if jit_vs > 120 and net8_ns > 0:
        failures.append(f"  JIT [{bi}] {sid[:60]}: {jit_ns:.1f}ns vs {net8_ns:.1f}ns ({jit_vs-100:.0f}%)")

    print(f"{bi:>4} {sid[:65]:<65} {aot_str:>10} {jit_str:>10} {net8_str:>10} {aot_vs_str:>9} {jit_vs_str:>9}")

print()
if failures:
    print(f"FAILURES (>20% slowdown):")
    for f in failures:
        print(f)
else:
    print("All methods within 20% threshold")
