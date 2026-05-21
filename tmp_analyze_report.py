"""Analyze batch report and show per-family status."""
import json

with open('testing/results/batch-report.json', 'rb') as f:
    r = json.loads(f.read().decode('utf-8'))

results = r.get('results', [])
total = r.get('total_families', len(results))
passed = r.get('passed', 0)
failed = r.get('failed', 0)
print(f'Total: {total}, Passed: {passed}, Failed: {failed}')
print()

for p in results:
    slug = p.get('slug','?')
    status = p.get('status','?')
    stages = p.get('stages', {})
    failed_stages = [(k, v) for k, v in stages.items() if isinstance(v, dict) and v.get('status') == 'failed']
    passed_count = sum(1 for v in stages.values() if isinstance(v, dict) and v.get('status') == 'passed')

    print(f'{slug}: {status}  (stages: {passed_count}/{len(stages)})', end='')
    if failed_stages:
        details = []
        for fs, s in failed_stages:
            summary = s.get('summary', '?')[:100]
            details.append(f'{fs}={summary}')
        print(f'  FAILED: {" | ".join(details)}', end='')
    print()
