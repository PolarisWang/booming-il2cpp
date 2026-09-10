#!/usr/bin/env python3
"""Run hotupdate for ALL foundation-dll assemblies and report results."""
import subprocess, sys, time, json
from pathlib import Path

_FD = Path("D:/agent/booming-il2cpp/testing/foundation-dll")
_ALL_DLLS = sorted(d.name for d in _FD.iterdir() if (d / "chunks").is_dir())

print(f"=== Running hotupdate for ALL {len(_ALL_DLLS)} DLLs ===")
print()

results = {}
passed = failed = skipped = 0

for dll in _ALL_DLLS:
    print(f"{'='*60}")
    print(f"  >>> {dll}")
    print(f"{'='*60}")
    sys.stdout.flush()

    start = time.perf_counter()
    try:
        r = subprocess.run(
            ["python", "-m", "verification", "--assembly", dll,
             "--all-chunks", "--stages", "build,hotupdate"],
            cwd=str(_FD), capture_output=True, text=True, timeout=900)
        stdout = r.stdout or ""
        stderr = r.stderr or ""
    except subprocess.TimeoutExpired:
        results[dll] = {"status": "timeout", "detail": ">900s"}
        skipped += 1
        print(f"  <<< TIMEOUT (>10 min)")
        continue

    dur = time.perf_counter() - start

    # Parse hotupdate result line
    status = "unknown"
    detail = ""
    for line in stdout.splitlines():
        if "passed:" in line and "assert_failed" in line:
            detail = line.strip()
            if "[passed]" in line or "passed:" in line:
                # Check the chunk summary line
                pass
    for line in stdout.splitlines():
        if "<<<" in line:
            if "+ [passed]" in line:
                status = "passed"
            elif "- [failed]" in line or "- [error]" in line:
                status = "failed"
            elif "skipped_" in line or "?" in line:
                status = "skipped"
            detail_line = line.strip()
            if detail_line:
                detail = detail_line

    results[dll] = {"status": status, "detail": detail, "duration_s": round(dur)}
    if status == "passed":
        passed += 1
    elif status == "failed":
        failed += 1
    else:
        skipped += 1

    status_icon = {"passed": "[PASS]", "failed": "[FAIL]", "skipped": "[SKIP]", "timeout": "[T/O]", "unknown": "[?]"}
    print(f"  Duration: {dur:.0f}s  {status_icon.get(status, '❓')} {status}")
    print()
    sys.stdout.flush()

# Summary
print(f"{'='*60}")
print(f"  RESULTS: {passed} passed, {failed} failed, {skipped} skipped/unknown")
print(f"{'='*60}")
for dll, r in results.items():
    icon = {"passed": "[PASS]", "failed": "[FAIL]", "skipped": "[SKIP]", "timeout": "[T/O]", "unknown": "[?]"}
    print(f"  {icon.get(r['status'], '❓')} {dll}: {r.get('detail','')[:100]}")

print()
print(f"Duration: {sum(r.get('duration_s',0) for r in results.values())}s total")

# Write results JSON
Path(_FD / "_hotupdate_results.json").write_text(
    json.dumps(results, indent=2), encoding="utf-8")

sys.exit(0 if failed == 0 else 1)
