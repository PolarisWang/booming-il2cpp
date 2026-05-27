#!/usr/bin/env python
"""Simple sequential pipeline runner - one family at a time, stdout flushed per line."""
import sys, os, subprocess, json, time
from pathlib import Path

os.chdir(r'D:\agent\booming-il2cpp')
sys.stdout.reconfigure(line_buffering=True)
sys.stderr.reconfigure(line_buffering=True)

testing_root = Path(r'D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib')
skip_dirs = {'reports', 'results', 'System.Private.CoreLib'}
families = sorted(d.name for d in testing_root.iterdir() if d.is_dir() and d.name not in skip_dirs)

results = {}
passed = failed = 0

for i, family in enumerate(families):
    print(f"\n===== [{i+1}/{len(families)}] {family} =====", flush=True)
    t0 = time.time()
    ok = False
    report = None
    proc = None
    try:
        proc = subprocess.run(
            [sys.executable, 'testing/foundation-dll/run_pipeline.py', family],
            capture_output=True, text=True, timeout=1800
        )
        t1 = time.time()
        ok = proc.returncode == 0
        # Extract JSON tail
        for line in reversed(proc.stdout.strip().split('\n')):
            if line.startswith('{'):
                try: report = json.loads(line); break
                except: pass
    except subprocess.TimeoutExpired:
        t1 = time.time()
        report = None
    except Exception as e:
        t1 = time.time()
        report = None

    if ok:
        passed += 1
    else:
        failed += 1
    cov = report.get('coverage', {}) if report else {}
    reg = report.get('regression', {}).get('hasRegression', '?') if report else '?'
    print(f"  -> {'PASS' if ok else 'FAIL'} | stages: {cov.get('stagesPassed','?')}/{cov.get('stagesFailed','?')} | regression: {reg} | {t1-t0:.0f}s", flush=True)
    if not ok and proc:
        err_lines = [l for l in proc.stderr.strip().split('\n') if l.strip()]
        for l in err_lines[-5:]:
            print(f"  stderr: {l}", flush=True)
    results[family] = {
        'status': 'PASS' if ok else 'FAIL',
        'stagesPassed': cov.get('stagesPassed'),
        'stagesFailed': cov.get('stagesFailed'),
        'hasRegression': reg,
        'elapsed_s': round(t1-t0, 0)
    }

print(f"\n===== DONE: {passed} passed, {failed} failed, {len(families)} total =====", flush=True)
Path(r'D:\agent\booming-il2cpp\build\testing\results').mkdir(parents=True, exist_ok=True)
Path(r'D:\agent\booming-il2cpp\build\testing\results\p2-baseline-summary.json').write_text(
    json.dumps(results, indent=2, ensure_ascii=False), encoding='utf-8')
print("Summary saved.", flush=True)
