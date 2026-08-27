#!/usr/bin/env python3
"""Chaos repo-hygiene orchestrator — a single central entrypoint for every
project wrap-up / cleanliness / drift / disk check.

Instead of N scripts wired in M places, everything is:
  - declared in hygiene-registry.json (add a check = add one entry + module)
  - run through this one entrypoint with uniform PASS/WARN/FAIL aggregation
  - reported to artifacts/hygiene-report.json (machine) + stdout (human)

Usage:
  chaos_hygiene.py --gate        # commit gate: all hard-mode checks; any FAIL -> exit 1
  chaos_hygiene.py --soft        # advisory: all checks, warn-only, exit 0 (Claude hook)
  chaos_hygiene.py --report      # run all + write artifacts/hygiene-report.json + refresh STATUS.md
  chaos_hygiene.py --check <name># run a single registered check
  chaos_hygiene.py --ci          # = --gate + --report + exit on any FAIL
  chaos_hygiene.py --disk        # shorthand for --check disk-health
  chaos_hygiene.py --help

Each registered check is itself an importable, side-effect-light module exposing
  run(mode: str) -> dict {status: PASS|WARN|FAIL, detail: str}
The orchestrator subprocess-calls the check module (list of extra argv) so every
check stays independently runnable.
"""

import importlib.util
import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
REGISTRY_PATH = REPO_ROOT / "scripts" / "cleanliness" / "hygiene-registry.json"
REPORT_PATH = REPO_ROOT / "artifacts" / "hygiene-report.json"
STATUS_PATH = REPO_ROOT / "docs" / "dev" / "in-progress" / "repo-cleanliness" / "STATUS.md"
_SOFT_STATE = REPO_ROOT / ".claude" / ".hygiene-soft-state.json"

_STATUS_FIELDS = [
    "root-clean",
    "disk-health",
    "generated-drift",
]


def _load_registry() -> dict:
    if not REGISTRY_PATH.exists():
        raise SystemExit(f"registry missing: {REGISTRY_PATH}")
    return json.loads(REGISTRY_PATH.read_text(encoding="utf-8"))


def _run_check(check: dict, extra_args: list) -> dict:
    """Invoke a check module as a subprocess; return {status, detail}."""
    module = check.get("module")
    check_args = check.get("args", []) + extra_args
    script = REPO_ROOT / "scripts" / "cleanliness" / f"{module}.py"
    if not script.exists():
        return {"status": "FAIL", "detail": f"missing module {module}.py"}
    try:
        proc = subprocess.run(
            [sys.executable, str(script)] + check_args,
            cwd=REPO_ROOT, capture_output=True, text=True,
            encoding="utf-8", errors="replace", timeout=check.get("timeout", 180),
        )
    except (subprocess.SubprocessError, OSError) as e:
        return {"status": "FAIL", "detail": f"spawn failed: {e}"}
    out = (proc.stdout or "") + (proc.stderr or "")
    # A check exits non-zero => FAIL, unless the check itself self-declares WARN
    # (module prints '[WARN]' line and exits 0).
    if proc.returncode != 0:
        status = "FAIL"
    elif "[WARN]" in out:
        status = "WARN"
    elif "[OK]" in out:
        status = "PASS"
    else:
        status = "PASS"
    # Advisory self-reports that exit 0 but contain an issue keyword are WARN,
    # not PASS — keeps the dashboard honest (e.g. generated-drift stale output).
    if status == "PASS" and any(
        kw in out for kw in ("STALE", "drift", "warn-only", "[warn", "ALERT", "WARNING")
    ):
        status = "WARN"
    detail = (out.strip() or "no output")
    # trim to first N lines for the report
    detail = "\n".join(detail.splitlines()[:8])
    if len(detail.splitlines()) >= 8:
        detail += "\n  ..."
    return {"status": status, "detail": detail}


def _resolve_mode(opts: set) -> str:
    if "--ci" in opts or "--gate" in opts:
        return "gate"
    if "--soft" in opts:
        return "soft"
    return "soft"


def _dispatch(registry: dict, names: list | None, opts: set):
    results = []
    for check in registry.get("checks", []):
        name = check["name"]
        if names and name not in names:
            continue
        # mode: override per invocation where meaningful
        extra = []
        if "--disk" in opts and name == "disk-health":
            extra = ["--disk"]
        results.append({"name": name, **( _run_check(check, extra) )})
    return results


def _write_report(results: list, overall: str) -> None:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    report = {
        "timestamp": datetime.now(timezone.utc).isoformat() + "Z",
        "overall": overall,
        "checks": results,
        "issues": [r["detail"] for r in results if r["status"] != "PASS"],
    }
    REPORT_PATH.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")
    _refresh_status(results, overall)


def _refresh_status(results: list, overall: str) -> None:
    """Rewrite the hygiene dashboard STATUS.md from the last run."""
    lines = [
        "# Repo-Hygiene Dashboard",
        "",
        f"> Last run: {datetime.now(timezone.utc).isoformat()[:19]}Z · overall: **{overall}**",
        "",
        "| check | status | note |",
        "|-------|--------|------|",
    ]
    for r in results:
        note = r["detail"].replace("|", "\\|").splitlines()[0][:90]
        lines.append(f"| {r['name']} | {r['status']} | {note} |")
    lines += [
        "",
        "Run `python scripts/cleanliness/chaos_hygiene.py --report` to refresh.",
        "",
    ]
    STATUS_PATH.parent.mkdir(parents=True, exist_ok=True)
    STATUS_PATH.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    # Windows console may default to GBK; force UTF-8 so the orchestrator's own
    # prints (which can contain —— emdash / CJK from check output) never crash.
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    except (AttributeError, ValueError):  # not a real stdout (CI/pipe) — fine
        pass

    opts = set(sys.argv[1:])
    if "--help" in opts or "-h" in opts:
        print(__doc__)
        return 0
    registry = _load_registry()
    names = None
    if "--check" in opts:
        try:
            names = [sys.argv[sys.argv.index("--check") + 1]]
        except (ValueError, IndexError):
            print("usage: --check <name>", file=sys.stderr)
            return 2

    results = _dispatch(registry, names, opts)
    if names:
        results = [r for r in results if r["name"] in names]

    # Gate: FAIL in any hard-mode check fails the commit/CI.
    mode = _resolve_mode(opts)
    has_fail = any(r["status"] == "FAIL" for r in results)

    if "--report" in opts or "--ci" in opts:
        _write_report(results, "FAIL" if has_fail else "PASS")

    # --soft (realtime hook): dedup — only nudge when the issue signature CHANGED
    # vs the last soft run, so the agent isn't told about the same dirt every tool
    # call. Keep quiet when there's nothing new to say.
    if mode == "soft":
        import hashlib
        sig = hashlib.sha256(
            "\n".join(f"{r['name']}:{r['status']}:{r['detail']}" for r in results).encode()
        ).hexdigest()
        last_sig = None
        try:
            last_sig = json.loads(_SOFT_STATE.read_text(encoding="utf-8")).get("signature")
        except (OSError, ValueError):
            last_sig = None
        try:
            _SOFT_STATE.parent.mkdir(parents=True, exist_ok=True)
            _SOFT_STATE.write_text(json.dumps({"signature": sig}, ensure_ascii=False), encoding="utf-8")
        except OSError:
            pass
        changed = sig != last_sig
        if not changed:
            return 0  # nothing new — stay silent

    # human output
    for r in results:
        nudge = (mode == "soft" and r["status"] != "PASS")
        if mode == "soft" and r["status"] == "PASS":
            continue  # keep the realtime hook quiet about passing checks
        print(f"  [{r['status']:>4}] {r['name']}")
        for line in r["detail"].splitlines()[:4]:
            print(f"          {line}")
    print(f"=== hygiene: overall={ 'FAIL' if has_fail else 'PASS' } (mode={mode}) ===")
    if "--report" in opts:
        print(f"report: {REPORT_PATH}")
        print(f"dashboard: {STATUS_PATH}")

    if has_fail and mode == "gate":
        return 1
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        sys.exit(130)
