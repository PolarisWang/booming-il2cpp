#!/usr/bin/env python3
"""publish-smoke.py — matrix e2e test for the chaos-il2cpp publish paths.

Each case is independent (own output dir, --clean), so a failure in one does not
taint the others. Asserts product integrity (entry.exe / C++ sources / manifest),
not just exit code.

Cases:
  app          — publish App.csproj --mode app; assert entry.exe + manifest + run exit 0
  app-jit      — publish App.csproj --mode app --jit; assert entry.exe (JIT dispatch)
  source-only  — publish App.csproj --source-only; assert C++ sources + manifest, NO exe

Runs the repo-built Driver (dev mode) so it works even in environments where the
standalone embedded-SDK build has known issues (e.g. MSVC 14.44 terminate — the
JIT case uses the same repo codegen path that foundation-dll exercises).

Usage:
  python scripts/publish-smoke.py                # run all cases
  python scripts/publish-smoke.py --case app     # run one case
  python scripts/publish-smoke.py --case app,jit # run subset
  python scripts/publish-smoke.py --json out.json
  python scripts/publish-smoke.py --driver <path-to-Driver.dll>   # override driver

Exit: 0 all passed, 1 any failed. Writes publish-smoke-report.json on request.
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import tempfile
from dataclasses import dataclass, asdict, field
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
FIXTURE = REPO_ROOT / "tests" / "fixtures" / "publish-smoke" / "App" / "App.csproj"


@dataclass
class CaseResult:
    case: str
    status: str                    # passed | failed | error | skipped
    summary: str = ""
    details: dict = field(default_factory=dict)
    errors: list = field(default_factory=list)
    duration_ms: int = 0


def resolve_driver() -> Path:
    """Locate the repo-built Chaos.IL2CPP.Driver.dll (Release)."""
    candidates = [
        REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll",
        REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Debug" / "net8.0" / "Chaos.IL2CPP.Driver.dll",
    ]
    for c in candidates:
        if c.exists():
            return c
    return candidates[0]


def run_publish(driver: Path, extra_args: list[str], out_dir: Path) -> tuple[int, str]:
    base = ["dotnet", str(driver), "publish", str(FIXTURE), "--config-tier", "check", "--clean", "--output", str(out_dir)]
    proc = subprocess.run(base + extra_args, capture_output=True, text=True,
                          encoding="utf-8", errors="replace", cwd=REPO_ROOT, timeout=1200)
    tail = (proc.stdout or "")[-600:] + (proc.stderr or "")[-400:]
    return proc.returncode, tail


def find_entry_exe(out_dir: Path) -> Path | None:
    for rel in ["build/RelWithDebInfo/chaos_entry.exe", "build/Release/chaos_entry.exe",
                "build/Debug/chaos_entry.exe", "chaos_entry.exe"]:
        p = out_dir / rel
        if p.exists():
            return p
    return None


def run_entry(exe: Path) -> tuple[int, str]:
    # The generated entry.exe may emit trace/debug output to stderr along with
    # stdout.  Merge both so the assertion can find the expected stdout marker.
    proc = subprocess.run([str(exe)], capture_output=True, text=True,
                          encoding="utf-8", errors="replace", cwd=exe.parent, timeout=120)
    merged = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode, merged


def case_app(driver: Path, out_dir: Path) -> CaseResult:
    import time
    t0 = time.time()
    rc, output = run_publish(driver, ["--mode", "app"], out_dir)
    r = CaseResult(case="app", status="failed", duration_ms=int((time.time()-t0)*1000))
    if rc != 0:
        r.errors.append(f"publish rc={rc}: {output[-300:]}")
        r.summary = "publish failed"
        return r
    exe = find_entry_exe(out_dir)
    if exe is None:
        r.errors.append("entry.exe not produced")
        r.summary = "missing entry.exe"
        return r
    r.details["entryExe"] = str(exe)
    manifest = out_dir / "publish.manifest.json"
    if not manifest.exists():
        r.errors.append("publish.manifest.json missing")
        r.summary = "missing manifest"
        return r
    try:
        m = json.loads(manifest.read_text(encoding="utf-8"))
        r.details["manifest"] = {"status": m.get("status"), "toolVersion": m.get("toolVersion"), "assemblyName": m.get("assemblyName")}
    except Exception as e:
        r.errors.append(f"manifest parse failed: {e}")
        r.summary = "bad manifest"
        return r
    # run the produced exe
    erc, eout = run_entry(exe)
    if erc != 0:
        r.errors.append(f"entry.exe exit={erc}: {eout[-200:]}")
        r.summary = "entry.exe did not exit 0"
        return r
    if "ChaosPublishSmoke" not in eout:
        r.errors.append(f"expected stdout marker missing: {eout[-200:]}")
        r.summary = "stdout marker missing"
        return r
    r.details["entryOutput"] = eout.strip()[:200]
    r.status = "passed"
    r.summary = "entry.exe produced, manifest valid, exit 0"
    return r


def case_app_jit(driver: Path, out_dir: Path) -> CaseResult:
    import time
    t0 = time.time()
    rc, output = run_publish(driver, ["--mode", "app", "--jit"], out_dir)
    r = CaseResult(case="app-jit", status="failed", duration_ms=int((time.time()-t0)*1000))
    if rc != 0:
        r.errors.append(f"publish(jit) rc={rc}: {output[-300:]}")
        r.summary = "publish(jit) failed"
        return r
    exe = find_entry_exe(out_dir)
    if exe is None:
        r.errors.append("entry.exe not produced in JIT mode")
        r.summary = "missing entry.exe (jit)"
        return r
    r.details["entryExe"] = str(exe)
    manifest = out_dir / "publish.manifest.json"
    if not manifest.exists():
        r.errors.append("publish.manifest.json missing")
        r.summary = "missing manifest (jit)"
        return r
    erc, eout = run_entry(exe)
    if erc != 0:
        r.errors.append(f"entry.exe(jit) exit={erc}: {eout[-200:]}")
        r.summary = "jit entry.exe did not exit 0"
        return r
    r.details["entryOutput"] = eout.strip()[:200]
    r.status = "passed"
    r.summary = "JIT entry.exe produced + ran exit 0"
    return r


def case_source_only(driver: Path, out_dir: Path) -> CaseResult:
    import time
    t0 = time.time()
    rc, output = run_publish(driver, ["--source-only"], out_dir)
    r = CaseResult(case="source-only", status="failed", duration_ms=int((time.time()-t0)*1000))
    if rc != 0:
        r.errors.append(f"publish(source-only) rc={rc}: {output[-300:]}")
        r.summary = "publish(source-only) failed"
        return r
    # source-only: no exe, but C++ sources + manifest must exist
    if find_entry_exe(out_dir) is not None:
        r.errors.append("source-only unexpectedly produced entry.exe")
        r.summary = "source-only produced exe (unexpected)"
        return r
    cpp_sources = list((out_dir / "codegen" / "generated").glob("*.cpp"))
    if not cpp_sources:
        r.errors.append("no generated .cpp sources under codegen/generated")
        r.summary = "no C++ sources"
        return r
    r.details["generatedCppCount"] = len(cpp_sources)
    manifest = out_dir / "publish.manifest.json"
    if not manifest.exists():
        r.errors.append("publish.manifest.json missing")
        r.summary = "missing manifest (source-only)"
        return r
    r.status = "passed"
    r.summary = f"{len(cpp_sources)} C++ sources + manifest, no exe"
    return r


CASES = {
    "app": case_app,
    "jit": case_app_jit,
    "source-only": case_source_only,
}


def main() -> int:
    ap = argparse.ArgumentParser(description="publish-smoke matrix runner")
    ap.add_argument("--case", default="app,jit,source-only", help="comma-separated cases (default: all)")
    ap.add_argument("--json", default=None, help="write report to this JSON file")
    ap.add_argument("--driver", default=None, help="path to Driver.dll (default: repo Release build)")
    ar = ap.parse_args()

    driver = Path(ar.driver) if ar.driver else resolve_driver()
    if not driver.exists():
        print(f"[publish-smoke] ERROR: driver not found at {driver}")
        print("  Build it first: dotnet build src/managed/Chaos.IL2CPP.Driver -c Release")
        return 1
    print(f"[publish-smoke] driver: {driver}")

    chosen = [c.strip() for c in ar.case.split(",") if c.strip()]
    unknown = [c for c in chosen if c not in CASES]
    if unknown:
        print(f"[publish-smoke] ERROR: unknown case(s): {unknown}; valid: {list(CASES)}")
        return 1

    results = []
    failed = 0
    for name in chosen:
        # Each case gets an isolated temp output dir (matrix isolation).
        with tempfile.TemporaryDirectory(prefix=f"pubsmoke-{name}-") as tmp:
            out_dir = Path(tmp) / "out"
            fn = CASES[name]
            try:
                r = fn(driver, out_dir)
            except subprocess.TimeoutExpired:
                r = CaseResult(case=name, status="error", summary="timeout")
            except Exception as e:
                r = CaseResult(case=name, status="error", summary=f"exception: {e}")
            results.append(r)
            print(f"[publish-smoke] {name}: {r.status} — {r.summary}")
            for e in r.errors:
                print(f"    ERROR: {e}")
            if r.status != "passed":
                failed += 1

    if ar.json:
        report = {"cases": [asdict(r) for r in results], "failed": failed, "total": len(results)}
        Path(ar.json).write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"[publish-smoke] report: {ar.json}")

    print(f"\n[publish-smoke] OVERALL: {'PASS' if failed == 0 else f'{failed}/{len(results)} FAILED'}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
