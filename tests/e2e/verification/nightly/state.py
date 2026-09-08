"""state — per-chunk state files (heartbeat + result separated) and error_class.

Risk mitigations baked in:
  - P1-3: heartbeat (.heartbeat) and result (.result) written to SEPARATE files,
          so the observer thread (heartbeat writer) never races the orchestrator
          main thread (result writer) on the same file.
  - P2-5 / P1-4: classify_exit uses strict precedence narrow→broad on observable
          root cause text (not just exit code), with real-failure-text mock tests.
  - P3-1: all reads use .get() with tolerant defaults (no fragile schema).

Layout under <report_dir>/run-state/<run_id>/:
    <asm>__<slug>.heartbeat   JSON, touched by observer every few seconds
    <asm>__<slug>.result      JSON, written once by orchestrator at completion
"""

from __future__ import annotations

import json
import time
from pathlib import Path
from typing import Any


def run_state_dir(config) -> Path:
    return Path(config.report_dir) / "run-state" / config.run_id


def heartbeat_path(config, key: str) -> Path:
    return run_state_dir(config) / f"{key}.heartbeat"


def result_path(config, key: str) -> Path:
    return run_state_dir(config) / f"{key}.result"


# ── Heartbeat (observer thread writes; no result data here) ─────────────
def touch_heartbeat(config, key: str, pid: int | None = None) -> None:
    p = heartbeat_path(config, key)
    p.parent.mkdir(parents=True, exist_ok=True)
    try:
        p.write_text(
            json.dumps({"pid": pid, "updated": time.time()}),
            encoding="utf-8",
        )
    except OSError:
        pass  # best-effort; a lost heartbeat is caught by the watchdog


def read_heartbeat(config, key: str) -> dict[str, Any]:
    p = heartbeat_path(config, key)
    if not p.is_file():
        return {}
    try:
        return json.loads(p.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return {}


# ── Result (orchestrator writes once at completion) ─────────────────────
def write_result(config, key: str, *, status: str, phase: str | None = None,
                 error_class: str | None = None, exit_code: int | None = None,
                 stage: str | None = None) -> None:
    p = result_path(config, key)
    p.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "key": key,
        "status": status,          # running|passed|failed|stalled|retrying|skipped
        "phase": phase,
        "error_class": error_class or "none",
        "exit_code": exit_code,
        "stage": stage,
        "updated": time.time(),
    }
    # atomic-ish write: write tmp then rename to avoid partial reads
    tmp = p.with_suffix(".tmp")
    try:
        tmp.write_text(json.dumps(payload, indent=2), encoding="utf-8")
        tmp.replace(p)
    except OSError:
        pass


def read_result(config, key: str) -> dict[str, Any]:
    p = result_path(config, key)
    if not p.is_file():
        return {"key": key, "status": "unknown"}
    try:
        return json.loads(p.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return {"key": key, "status": "unknown"}


def read_all_results(config, *, for_run_id: str | None = None):
    """Scan run-state/<run_id>/ for all .result files. Returns dict key→payload.

    If `for_run_id` is given, scan THAT run's state dir instead of
    config.run_id — used by resume (which must read the PRIOR run's results,
    not the current one's).
    """
    run_id = for_run_id if for_run_id else config.run_id
    d = Path(config.report_dir) / "run-state" / run_id
    out: dict[str, dict] = {}
    if not d.is_dir():
        return out
    for f in sorted(d.glob("*.result")):
        key = f.name[: -len(".result")]
        p = f
        if not p.is_file():
            continue
        try:
            out[key] = json.loads(p.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            out[key] = {"key": key, "status": "unknown"}
    return out


# ── error_class classifier ────────────────────────────────────────────────
# Strict precedence narrow→broad, keyed on observable text (P1-4 / P2-5).
def classify_exit(exit_code: int | None, tail_text: str) -> str:
    t = tail_text or ""
    # 1. native-codegen-missing-symbol: codegen emitted a reference to a type it
    #    didn't declare (e.g. chaos_type_... undeclared + C2065/C2440).
    if "chaos_type_" in t and ("undeclared identifier" in t or "C2065" in t):
        return "native-codegen-missing-symbol"
    # 2. native-codegen-syntax: generated native-aot C++ has syntax error
    #    (C2061/C2143/C3536/C2440), distinct from missing symbol.
    if "native-aot.generated.cpp" in t and ("error C2061" in t or "error C2143" in t
                                            or "error C3536" in t or "error C2440" in t):
        return "native-codegen-syntax"
    # 3. atg-combined-cs: ATG-generated CombinedSubjects.cs has a C# compile error
    #    (e.g. CS1503 wrong-arg-type → the ATG argument-ordering defect).
    if "CombinedSubjects.cs" in t and "error CS" in t:
        return "atg-combined-cs"
    # 4. platform-diagnostic: compiler diagnostic that must be suppressed
    #    (SYSLIBxxxx — e.g. AsyncHelpers "evaluation purposes only").
    if "error SYSLIB" in t or ("warning SYSLIB" in t and "Suppress" in t):
        return "platform-diagnostic"
    # 5. native-crash: process crashed (0xC0000005 AV, SEH 0xC0000409, CLR internal).
    if exit_code in (0xC0000005, 0xC0000409) or "Internal CLR error" in t \
        or "0xc0000005" in t.lower() or "0xc0000409" in t.lower():
        return "native-crash"
    # 6. oom: killed by OOM killer.
    if exit_code == 137:
        return "oom"
    # 6b. hotupdate-patch-arm: hotupdate incremental-rebuild failure —
    #    "stale executable cannot apply patch" is a known infra/how-to
    #    (not a codegen defect); don't blind-retry.
    if "incremental rebuild failed" in t and ("stale executable" in t or "patch" in t):
        return "hotupdate-patch-arm"
    # 7. csharp-error: some other C# compile error not CombinedSubjects/ATG.
    if "error CS" in t:
        return "csharp-error"
    # 8. success
    if exit_code == 0:
        return "pass"
    # 9. killed
    if exit_code in (-9, -15):
        return "killed"
    # 10. native-linker-error: C/C++ linker/build error NOT in codegen output
    #     (e.g. LNK2019, LNK2001, or C errors in non-codegen source files).
    #     Rule #2 already catches native-aot.generated.cpp C errors; the "error C"
    #     here is for the remaining C/C++ errors from other TU compilation.
    #     The "error CS" sub-branch is dead (rule #3 and #7 catch it earlier)
    #     but harmless; keep for documentation.
    if "error C" in t:
        return "native-linker-error"
    return "unknown"
