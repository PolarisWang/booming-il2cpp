import json

# Read multi-run report (has net8 baseline)
multi = json.load(open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/multi-run/multi-run-report.json', encoding='utf-8'))

# Read perf history
history = []
with open('D:/agent/booming-il2cpp/testing/foundation-dll/results/foundation-dll/System.Private.CoreLib/convert-char/perf/benchmark-history.jsonl', encoding='utf-8') as f:
    for line in f:
        if line.strip():
            history.append(json.loads(line))

# Get only the latest run (2026-06-01T08:50)
latest_ts = '2026-06-01T08:50'
latest = [h for h in history if h['timestamp'].startswith(latest_ts)]

# Index by methodSubjectId + technology
by_key = {}
for h in latest:
    tech = h.get('technology', '')
    mid = h.get('methodSubjectId', '')
    if tech in ('chaos-aot', 'chaos-jit') and h.get('status') == 'completed':
        by_key[(mid, tech)] = h

# Match by label
label_to_sid = {}
for m in multi.get('methods', []):
    label_to_sid[m.get('label', '')] = m.get('subject_id', '')

print(f"{'label':<55} {'aot_ns':>10} {'jit_ns':>10} {'net8_ns':>10} {'aot_vs%':>9} {'jit_vs%':>9}")
print('-' * 103)

failures = []
for m in multi.get('methods', []):
    label = m.get('label', '?')
    sid = m.get('subject_id', '')
    net8 = m.get('net8_ns_per_op')

    aot_h = by_key.get((sid, 'chaos-aot'))
    jit_h = by_key.get((sid, 'chaos-jit'))

    aot_ns = (aot_h['metrics']['elapsedMilliseconds'] * 1e6 / aot_h['iterations']) if aot_h else None
    jit_ns = (jit_h['metrics']['elapsedMilliseconds'] * 1e6 / jit_h['iterations']) if jit_h else None

    aot_s = f'{aot_ns:.1f}' if aot_ns else 'N/A'
    jit_s = f'{jit_ns:.1f}' if jit_ns else 'N/A'
    net8_s = f'{net8:.1f}' if net8 else 'N/A'

    aot_vs = ((aot_ns / net8) - 1) * 100 if aot_ns and net8 else 0
    jit_vs = ((jit_ns / net8) - 1) * 100 if jit_ns and net8 else 0
    aot_vs_s = f'{aot_vs:+.0f}%' if aot_ns and net8 else 'N/A'
    jit_vs_s = f'{jit_vs:+.0f}%' if jit_ns and net8 else 'N/A'

    print(f'{label[:55]:<55} {aot_s:>10} {jit_s:>10} {net8_s:>10} {aot_vs_s:>9} {jit_vs_s:>9}')
    if aot_vs > 20:
        failures.append(f'AOT {label}: {aot_vs:.0f}%')
    if jit_vs > 20:
        failures.append(f'JIT {label}: {jit_vs:.0f}%')

print()
if failures:
    print(f'FAILURES (>20%):')
    for f in failures:
        print(f'  {f}')
else:
    print('All methods within 20% threshold')
