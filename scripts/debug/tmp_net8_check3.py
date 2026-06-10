import json, sys

# Read perf history
history = []
with open('D:/agent/booming-il2cpp/testing/foundation-dll/results/foundation-dll/System.Private.CoreLib/convert-char/perf/benchmark-history.jsonl', encoding='utf-8') as f:
    for line in f:
        if line.strip():
            history.append(json.loads(line))

latest_ts = '2026-06-01T08:52:06'
latest = [h for h in history if h['timestamp'] == latest_ts]
aot_by_sid = {}
jit_by_sid = {}
for h in latest:
    tech = h.get('technology', '')
    sid = h.get('methodSubjectId', '')
    ops = h.get('metrics', {}).get('opsPerSecond', 0)
    if ops > 0:
        ns = 1e9 / ops
        if tech == 'chaos-aot':
            aot_by_sid[sid] = ns
        elif tech == 'chaos-jit':
            jit_by_sid[sid] = ns

# Read multi-run
multi = json.load(open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/multi-run/multi-run-report.json', encoding='utf-8'))

print(f"{'label':<55} {'aot_ns':>10} {'jit_ns':>10} {'net8_ns':>10} {'aot_vs%':>9} {'jit_vs%':>9}")
print('-' * 103)

failures = []
for m in multi.get('methods', []):
    label = m.get('label', '')
    sid = m.get('subject_id', '')

    samples = m.get('samples', {})
    net8_s = samples.get('net8-jit', {})
    net8_ns = net8_s.get('mean_ns')

    aot_ns = aot_by_sid.get(sid)
    jit_ns = jit_by_sid.get(sid)

    aot_s = f'{aot_ns:.1f}' if aot_ns else 'N/A'
    jit_s = f'{jit_ns:.1f}' if jit_ns else 'N/A'
    net8_s_str = f'{net8_ns:.1f}' if net8_ns else 'N/A'

    aot_vs = ((aot_ns / net8_ns) - 1) * 100 if aot_ns and net8_ns else 0
    jit_vs = ((jit_ns / net8_ns) - 1) * 100 if jit_ns and net8_ns else 0
    aot_vs_s = f'{aot_vs:+.0f}%' if aot_ns and net8_ns else 'N/A'
    jit_vs_s = f'{jit_vs:+.0f}%' if jit_ns and net8_ns else 'N/A'

    print(f'{label[:55]:<55} {aot_s:>10} {jit_s:>10} {net8_s_str:>10} {aot_vs_s:>9} {jit_vs_s:>9}')

    if aot_vs > 20:
        failures.append(f'AOT {label}: {aot_vs:.0f}% ({aot_ns:.1f}ns vs {net8_ns:.1f}ns)')
    if jit_vs > 20:
        failures.append(f'JIT {label}: {jit_vs:.0f}% ({jit_ns:.1f}ns vs {net8_ns:.1f}ns)')

print()
if failures:
    print(f'FAILURES (>20% slowdown):')
    for f in failures:
        print(f'  {f}')
    sys.exit(1)
else:
    print('All methods within 20% threshold ✅')
