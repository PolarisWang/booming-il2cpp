"""Re-run verification for all currently failed families, then generate dashboard."""
import json
import subprocess
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parent.parent
_BATCH_REPORT = _REPO_ROOT / "testing" / "results" / "batch-report.json"

# Read current batch report
with open(_BATCH_REPORT, 'rb') as f:
    report = json.loads(f.read().decode('utf-8'))

results = report.get('results', [])
failed = [r for r in results if r.get('status') != 'passed']

print(f"Found {len(failed)} failed families to re-run:")
for r in failed:
    print(f"  {r['slug']}: {r['status']}")
print()

for i, r in enumerate(failed):
    slug = r['slug']
    print(f"\n[{i+1}/{len(failed)}] Running {slug}...")

    result = subprocess.run(
        [sys.executable, '-m', 'testing.foundation_dll._core.python.cli', slug],
        capture_output=True, text=True, timeout=7200,
        cwd=str(_REPO_ROOT),
    )

    print(result.stdout[-500:] if len(result.stdout) > 500 else result.stdout)
    if result.stderr:
        print(f"  STDERR: {result.stderr[-300:]}")
    print(f"  Exit code: {result.returncode}")

print("\n\nDONE! All families re-run.")
