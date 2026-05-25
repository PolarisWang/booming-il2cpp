"""Performance data store — append-only JSONL persistence for benchmark results.

Three stores:
  - benchmark-history.jsonl : per-method runtime perf (technology × method)
  - microbench-history.jsonl: interpreter-wide metrics (one line per run)
  - asmcompare-history.jsonl: per-method instruction-level analysis

All stored under {results_dir}/perf/ per family.
"""

from __future__ import annotations

import json
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from orchestration.context import FamilyContext

# ── Environment / device detection ──────────────────────────────────────

_GIT_COMMIT_CACHE: str | None = None
_DEVICE_CACHE: dict[str, str] | None = None


def _get_git_commit(repo_root: Path | None = None) -> str:
    global _GIT_COMMIT_CACHE
    if _GIT_COMMIT_CACHE is not None:
        return _GIT_COMMIT_CACHE
    try:
        root = repo_root or Path(__file__).resolve().parents[4]
        r = subprocess.run(
            ["git", "rev-parse", "--short", "HEAD"],
            capture_output=True, text=True, timeout=5,
            cwd=str(root),
        )
        _GIT_COMMIT_CACHE = r.stdout.strip()
    except Exception:
        _GIT_COMMIT_CACHE = "unknown"
    return _GIT_COMMIT_CACHE


def _detect_device() -> dict[str, str]:
    global _DEVICE_CACHE
    if _DEVICE_CACHE is not None:
        return _DEVICE_CACHE

    import platform
    system = platform.system().lower()
    machine = platform.machine().lower()

    cpu_map = {
        "amd64": "x64",
        "x86_64": "x64",
        "i386": "x86",
        "i686": "x86",
        "aarch64": "arm64",
        "arm64": "arm64",
    }
    cpu = cpu_map.get(machine, machine)

    cpu_model = cpu
    if system == "windows":
        try:
            r = subprocess.run(
                ["wmic", "cpu", "get", "name"],
                capture_output=True, text=True, timeout=5,
            )
            for line in r.stdout.splitlines():
                line = line.strip()
                if line and "Name" not in line and line != cpu_model:
                    name = line.lower().replace("(r)", "").replace("(tm)", "")
                    name = name.replace("cpu", "").replace("@", "").strip()
                    parts = name.replace(" ", "-").split("-")
                    parts = [p for p in parts if p]
                    cpu_model = "-".join(parts[:6])
                    break
        except Exception:
            pass

    device_id = f"{system}-{cpu}"
    _DEVICE_CACHE = {
        "id": device_id,
        "os": system,
        "cpu": cpu_model,
        "arch": machine,
    }
    return _DEVICE_CACHE


# ── Common record fields ────────────────────────────────────────────────

def _common_fields(ctx: FamilyContext) -> dict[str, Any]:
    """Return fields common to all record types."""
    return {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "slug": ctx.slug,
        "assembly": ctx.assembly,
        "gitCommit": _get_git_commit(),
        "device": _detect_device(),
        "compiler": "msvc",
        "configuration": ctx.mode,
    }


def _store_dir(ctx: FamilyContext) -> Path:
    d = ctx.results_dir / "perf"
    d.mkdir(parents=True, exist_ok=True)
    return d


def _append_jsonl(path: Path, record: dict[str, Any]) -> None:
    with open(path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")


_TECHNOLOGY_LABEL_MAP: dict[str, str] = {
    "native-aot": "chaos-aot",
    "native-jit": "chaos-jit",
}


# ── Benchmark records (per-method) ──────────────────────────────────────


def save_managed_benchmark_records(
    ctx: FamilyContext,
    technology: str,
    method_results: list[dict[str, Any]],
) -> None:
    """Save managed runner (.NET JIT / MONO) results to JSONL."""
    sdir = _store_dir(ctx)
    path = sdir / "benchmark-history.jsonl"
    common = _common_fields(ctx)
    method_subject_ids = _load_method_subject_ids(ctx)

    for i, res in enumerate(method_results):
        record = {
            **common,
            "technology": technology,
            "methodSubjectId": method_subject_ids[i] if i < len(method_subject_ids) else f"method_{i}",
            "methodIndex": res.get("methodIndex", i),
            "metrics": {
                "elapsedMilliseconds": res.get("elapsedMilliseconds", 0),
                "opsPerSecond": res.get("opsPerSecond", 0),
            },
            "iterations": res.get("iterations", 100000),
            "status": res.get("status", "completed"),
        }
        _append_jsonl(path, record)


@dataclass
class BenchmarkQuery:
    """Filter parameters for querying benchmark history."""
    slug: str = ""
    assembly: str = ""
    technology: str = ""
    method_index: int | None = None
    limit: int = 0
    device_id: str = ""


def _load_jsonl(path: Path) -> list[dict[str, Any]]:
    if not path.exists():
        return []
    records = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                try:
                    records.append(json.loads(line))
                except json.JSONDecodeError:
                    pass
    return records


def _filter_records(
    records: list[dict[str, Any]],
    query: BenchmarkQuery,
) -> list[dict[str, Any]]:
    result = records
    if query.slug:
        result = [r for r in result if r.get("slug") == query.slug]
    if query.assembly:
        result = [r for r in result if r.get("assembly") == query.assembly]
    if query.technology:
        result = [r for r in result if r.get("technology") == query.technology]
    if query.method_index is not None:
        result = [r for r in result if r.get("methodIndex") == query.method_index]
    if query.device_id:
        result = [r for r in result if r.get("device", {}).get("id") == query.device_id]
    if query.limit > 0:
        result = result[-query.limit:]
    return result


def query_benchmarks(
    ctx: FamilyContext, query: BenchmarkQuery | None = None,
) -> list[dict[str, Any]]:
    path = _store_dir(ctx) / "benchmark-history.jsonl"
    records = _load_jsonl(path)
    if query:
        records = _filter_records(records, query)
    return records


def query_microbench(
    ctx: FamilyContext, limit: int = 0,
) -> list[dict[str, Any]]:
    path = _store_dir(ctx) / "microbench-history.jsonl"
    records = _load_jsonl(path)
    if limit > 0:
        records = records[-limit:]
    return records


def query_asmcompare(
    ctx: FamilyContext, query: BenchmarkQuery | None = None,
) -> list[dict[str, Any]]:
    path = _store_dir(ctx) / "asmcompare-history.jsonl"
    records = _load_jsonl(path)
    if query:
        records = _filter_records(records, query)
    return records


def query_latest_benchmarks(
    ctx: FamilyContext, technology: str,
) -> list[dict[str, Any]]:
    path = _store_dir(ctx) / "benchmark-history.jsonl"
    records = _load_jsonl(path)
    records = [r for r in records if r.get("technology") == technology]
    if not records:
        return []

    timestamps = set(r.get("timestamp", "") for r in records)
    latest_ts = max(timestamps)
    latest = [r for r in records if r.get("timestamp") == latest_ts]
    latest.sort(key=lambda r: r.get("methodIndex", 0))
    return latest


# ── Auto-save hook ──────────────────────────────────────────────────────

def auto_save_perf_data(
    ctx: FamilyContext,
    report_stages: dict[str, dict[str, Any]],
) -> None:
    """Automatically persist performance data from relevant stages."""
    sd = report_stages.get("asm_compare", {})
    if sd.get("status") == "passed" and sd.get("details"):
        _save_asmcompare_from_dict(ctx, sd)

    sd = report_stages.get("microbench", {})
    if sd.get("status") == "passed" and sd.get("details"):
        _save_microbench_from_dict(ctx, sd)

    sd = report_stages.get("benchmark", {})
    if sd.get("status") == "passed" and sd.get("details"):
        _save_benchmark_from_dict(ctx, sd)

    sd = report_stages.get("hotupdate_aot_benchmark", {})
    if sd.get("status") == "passed" and sd.get("details"):
        _save_benchmark_from_dict(ctx, sd)

    sd = report_stages.get("hotupdate_jit_benchmark", {})
    if sd.get("status") == "passed" and sd.get("details"):
        _save_benchmark_from_dict(ctx, sd)

    print(f"  [perf_store] saved to {_store_dir(ctx)}")


def _save_asmcompare_from_dict(ctx: FamilyContext, sd: dict[str, Any]) -> None:
    details = sd.get("details", {})
    report_path = details.get("reportPath")
    if not report_path:
        return
    report_file = Path(report_path)
    if not report_file.exists():
        return
    try:
        report = json.loads(report_file.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return

    sdir = _store_dir(ctx)
    path = sdir / "asmcompare-history.jsonl"
    common = _common_fields(ctx)

    for entry in report.get("methodResults", []):
        rec = {
            **common,
            "methodSubjectId": entry.get("methodSubjectId", ""),
            "methodIndex": entry.get("methodIndex", 0),
            "jitInstructionCount": entry.get("metrics", {}).get("jitInstructionCount", 0),
            "aotInstructionCount": entry.get("metrics", {}).get("aotInstructionCount", 0),
            "irExpansionRatio": entry.get("metrics", {}).get("ratio", 0),
            "jitCodeSize": entry.get("metrics", {}).get("jitCodeSize", 0),
            "analysis": dict(entry.get("instructionCategories", {})),
        }
        _append_jsonl(path, rec)


def _save_microbench_from_dict(ctx: FamilyContext, sd: dict[str, Any]) -> None:
    details = sd.get("details", {})
    metrics_raw = details.get("metrics", {})
    metrics = {
        k: v for k, v in metrics_raw.items()
        if isinstance(v, dict) and "value" in v and "unit" in v
    }
    if not metrics:
        return
    sdir = _store_dir(ctx)
    path = sdir / "microbench-history.jsonl"
    record = {
        **_common_fields(ctx),
        "metrics": {
            k: {"value": v["value"], "unit": v["unit"]}
            for k, v in metrics.items()
        },
    }
    _append_jsonl(path, record)


def _save_benchmark_from_dict(ctx: FamilyContext, sd: dict[str, Any]) -> None:
    details = sd.get("details", {})
    stage_name = sd.get("stage", "")
    sdir = _store_dir(ctx)
    path = sdir / "benchmark-history.jsonl"
    common = _common_fields(ctx)
    mids = _load_method_subject_ids(ctx)

    if stage_name == "benchmark":
        for key, bench_data in details.items():
            if not isinstance(bench_data, dict) or "results" not in bench_data:
                continue
            technology = _TECHNOLOGY_LABEL_MAP.get(bench_data.get("label", ""))
            if not technology:
                continue
            results = bench_data.get("results") or []
            for i, res in enumerate(results):
                if not res:
                    continue
                record = {
                    **common,
                    "technology": technology,
                    "methodSubjectId": mids[i] if i < len(mids) else f"method_{i}",
                    "methodIndex": res.get("methodIndex", i),
                    "metrics": {
                        "elapsedMilliseconds": res.get("elapsedMilliseconds", 0),
                        "opsPerSecond": res.get("opsPerSecond", 0),
                        "calibratedMs": res.get("calibratedMs", 0),
                    },
                    "iterations": res.get("iterations", 100000),
                    "status": "completed" if "error" not in (res or {}) else "error",
                }
                _append_jsonl(path, record)
    else:
        technology_map = {
            "hotupdate_aot_benchmark": "chaos-hu-aot",
            "hotupdate_jit_benchmark": "chaos-hu-jit",
        }
        technology = technology_map.get(stage_name, stage_name)
        results = details.get("results") or []
        for i, res in enumerate(results):
            if not res:
                continue
            metrics = {
                "elapsedMilliseconds": res.get("elapsedMilliseconds", 0),
            }
            if "postPatchNsPerOp" in res:
                metrics["postPatchNsPerOp"] = res["postPatchNsPerOp"]
            record = {
                **common,
                "technology": technology,
                "methodSubjectId": mids[i] if i < len(mids) else f"method_{i}",
                "methodIndex": res.get("methodIndex", i),
                "metrics": metrics,
                "iterations": res.get("iterations", 100000),
                "status": "completed" if "error" not in (res or {}) else "error",
            }
            _append_jsonl(path, record)


def _load_method_subject_ids(ctx: FamilyContext) -> list[str]:
    contract_path = ctx.contract_path
    if not contract_path.exists():
        return []
    try:
        c = json.loads(contract_path.read_text(encoding="utf-8"))
        mids = c.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in c.get("methodContracts", []) if m.get("methodSubjectId")]
        return mids
    except Exception:
        return []
