"""baseline_store — persist and load per-run nightly baselines for regression triage.

Purpose
-------
The nightly build produces per-chunk artifacts (`results/fact.json`,
`results/benchmark.json`, `results/hotupdate.json`, `results/fact-results.json`,
`results/provenance.json`).  Those are overwritten on every run, so there is no
way to answer "did this get worse than last time?".

This module snapshots the *aggregate* metrics from one run into an append-only
baseline directory, tagged with provenance (git commit / device / platform /
nativeConfig) so that only genuinely comparable runs are ever diffed.

Layout (under the nightly report dir)::

    baseline/
        index.json                  # run list + reference pointer
        run-<run_id>.json           # one snapshot per run (aggregate metrics)
        reference.json              # optional "golden" run for long-horizon trend

Design notes
------------
* Only **aggregate** metrics are stored per chunk (not the full per-method
  table) — that keeps one snapshot around ~100 KB for 82 chunks.  The per-method
  detail stays in `results/` and is read directly by the triage tool.
* `is_comparable()` is the gate that prevents false regressions when runs come
  from different devices, platforms, or native configs.
* Everything is best-effort: a missing or corrupt chunk artifact must never
  abort the nightly.  Callers get an empty/partial snapshot instead.
"""

from __future__ import annotations

import json
import time
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Iterable

SCHEMA_VERSION = 1

# Keep at most this many non-reference snapshots (rolling window growth guard).
MAX_RETAINED_RUNS = 30


# ── data model ────────────────────────────────────────────────────────────────

@dataclass
class ChunkBaseline:
    """Aggregate metrics for one (assembly, chunk) in one run."""

    key: str                      # "<assembly>__<slug>"
    fact: dict[str, Any] = field(default_factory=dict)
    benchmark: dict[str, Any] = field(default_factory=dict)
    hotupdate: dict[str, Any] = field(default_factory=dict)
    crosstech: dict[str, Any] = field(default_factory=dict)

    def to_dict(self) -> dict:
        return {
            "fact": self.fact,
            "benchmark": self.benchmark,
            "hotupdate": self.hotupdate,
            "crosstech": self.crosstech,
        }

    @classmethod
    def from_dict(cls, key: str, d: dict) -> "ChunkBaseline":
        return cls(
            key=key,
            fact=d.get("fact") or {},
            benchmark=d.get("benchmark") or {},
            hotupdate=d.get("hotupdate") or {},
            crosstech=d.get("crosstech") or {},
        )


@dataclass
class Baseline:
    """One nightly run's snapshot."""

    run_id: str
    timestamp: str
    provenance: dict[str, Any] = field(default_factory=dict)
    chunks: dict[str, ChunkBaseline] = field(default_factory=dict)
    reference: bool = False

    def to_dict(self) -> dict:
        return {
            "schemaVersion": SCHEMA_VERSION,
            "runId": self.run_id,
            "timestamp": self.timestamp,
            "provenance": self.provenance,
            "chunks": {k: v.to_dict() for k, v in self.chunks.items()},
        }

    @classmethod
    def from_dict(cls, d: dict) -> "Baseline":
        return cls(
            run_id=d.get("runId", ""),
            timestamp=d.get("timestamp", ""),
            provenance=d.get("provenance") or {},
            chunks={
                k: ChunkBaseline.from_dict(k, v)
                for k, v in (d.get("chunks") or {}).items()
            },
            reference=bool(d.get("reference", False)),
        )


# ── paths ─────────────────────────────────────────────────────────────────────

def baseline_dir(report_dir: Path) -> Path:
    return Path(report_dir) / "baseline"


def _index_path(report_dir: Path) -> Path:
    return baseline_dir(report_dir) / "index.json"


def _run_path(report_dir: Path, run_id: str) -> Path:
    return baseline_dir(report_dir) / f"run-{run_id}.json"


def _reference_path(report_dir: Path) -> Path:
    return baseline_dir(report_dir) / "reference.json"


# ── extraction helpers (best-effort, never raise) ─────────────────────────────

def _read_json(path: Path) -> dict | None:
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError):
        return None


def _extract_fact(results_dir: Path) -> dict[str, Any]:
    """Aggregate fact metrics for one chunk (runtime-derived, see fact_chunk.py)."""
    d = _read_json(results_dir / "fact.json")
    if not d:
        return {}
    total = d.get("total") or 0
    real = d.get("realVerified")
    if real is None:
        # Legacy producer: fall back to the pre-runtime fields.
        real = d.get("realPassed") or 0
    return {
        "total": total,
        "realVerified": real,
        "unassertable": d.get("unassertable", 0),
        "smokeUnknown": d.get("smokeUnknown", 0),
        "failed": d.get("failed", 0),
        "passed": d.get("passed", 0),
    }


def _extract_benchmark(results_dir: Path) -> dict[str, Any]:
    """Aggregate benchmark metrics for one chunk."""
    d = _read_json(results_dir / "benchmark.json")
    if not d:
        return {}
    return {
        "methodCount": d.get("methodCount", 0),
        "stubCount": d.get("stubCount", 0),
        "nonStubCount": d.get("nonStubCount", 0),
        "exitCode": d.get("exitCode", 0),
    }


def _extract_hotupdate(results_dir: Path) -> dict[str, Any]:
    """Aggregate hotupdate metrics for one chunk."""
    d = _read_json(results_dir / "hotupdate.json")
    if not d:
        return {}
    return {
        "passed": d.get("passed", 0),
        "failed": d.get("failed", 0),
        "assertFailed": d.get("assertFailed", 0),
        "semanticChangedCount": d.get("semanticChangedCount", 0),
        "revertPassed": d.get("revertPassed", 0),
        "patchFailed": bool(d.get("patchFailed", False)),
        "crash": bool(d.get("crash", False)),
    }


def _extract_crosstech(results_dir: Path) -> dict[str, Any]:
    """AOT-vs-JIT agreement counts for one chunk."""
    d = _read_json(results_dir / "fact-results.json")
    if not d:
        return {}
    aot = d.get("aot") or []
    jit = d.get("jit") or []
    if not aot:
        return {}
    aot_pass = sum(1 for r in aot if r.get("passed"))
    if not jit:
        return {"aotPassed": aot_pass, "jitPassed": None, "diffCount": None}
    jit_pass = sum(1 for r in jit if r.get("passed"))
    # Count records whose pass flag disagrees, matched by method index.
    aot_by_si = {r.get("si"): bool(r.get("passed")) for r in aot}
    jit_by_si = {r.get("si"): bool(r.get("passed")) for r in jit}
    diff = sum(
        1 for si in set(aot_by_si) & set(jit_by_si)
        if aot_by_si[si] != jit_by_si[si]
    )
    return {"aotPassed": aot_pass, "jitPassed": jit_pass, "diffCount": diff}


def _read_provenance(results_dir: Path) -> dict[str, Any]:
    return _read_json(results_dir / "provenance.json") or {}


# ── record ────────────────────────────────────────────────────────────────────

def _iter_chunk_dirs(results_base: Path) -> Iterable[tuple[str, Path]]:
    """Yield (chunk_key, results_dir) for every chunk under a foundation root."""
    if not results_base.is_dir():
        return
    for asm_dir in sorted(results_base.iterdir()):
        chunks_dir = asm_dir / "chunks"
        if not chunks_dir.is_dir():
            continue
        for chunk_dir in sorted(chunks_dir.iterdir()):
            results_dir = chunk_dir / "results"
            if results_dir.is_dir():
                yield f"{asm_dir.name}__{chunk_dir.name}", results_dir


def record_from_run(
    run_id: str,
    report_dir: Path,
    results_base: Path,
    native_config: str = "check",
) -> Path | None:
    """Snapshot one run's aggregate metrics into the baseline store.

    Args:
        run_id: the nightly run identifier.
        report_dir: nightly report root (baseline/ lives under it).
        results_base: foundation artifact root containing <asm>/chunks/<slug>/results.
        native_config: the run's native config tier (for provenance gating).

    Returns the written snapshot path, or None if nothing was recorded.
    """
    chunks: dict[str, ChunkBaseline] = {}
    provenance: dict[str, Any] = {
        "nativeConfig": native_config,
        "platform": "",
        "deviceId": "",
        "gitCommit": "",
        "gitBranch": "",
    }

    for key, results_dir in _iter_chunk_dirs(Path(results_base)):
        fact = _extract_fact(results_dir)
        bench = _extract_benchmark(results_dir)
        hu = _extract_hotupdate(results_dir)
        xt = _extract_crosstech(results_dir)
        if not (fact or bench or hu or xt):
            continue
        chunks[key] = ChunkBaseline(key=key, fact=fact, benchmark=bench,
                                    hotupdate=hu, crosstech=xt)
        # Provenance is per-chunk but identical within a run; capture once.
        if not provenance["gitCommit"]:
            p = _read_provenance(results_dir)
            if p:
                provenance.update({
                    "platform": p.get("platform", ""),
                    "deviceId": p.get("deviceId", ""),
                    "gitCommit": p.get("gitCommit", ""),
                    "gitBranch": p.get("gitBranch", ""),
                    "nativeConfig": p.get("nativeConfig", native_config),
                })

    if not chunks:
        return None

    baseline = Baseline(
        run_id=run_id,
        timestamp=datetime.now(timezone.utc).isoformat(),
        provenance=provenance,
        chunks=chunks,
    )

    out_dir = baseline_dir(Path(report_dir))
    out_dir.mkdir(parents=True, exist_ok=True)
    out_path = _run_path(Path(report_dir), run_id)
    out_path.write_text(
        json.dumps(baseline.to_dict(), ensure_ascii=False, indent=2),
        encoding="utf-8",
    )

    _update_index(Path(report_dir), baseline)
    _prune(Path(report_dir))
    return out_path


def _update_index(report_dir: Path, baseline: Baseline) -> None:
    idx = _read_json(_index_path(report_dir)) or {
        "schemaVersion": SCHEMA_VERSION, "runs": [], "reference": None,
    }
    runs = [r for r in idx.get("runs", []) if r.get("runId") != baseline.run_id]
    runs.append({
        "runId": baseline.run_id,
        "timestamp": baseline.timestamp,
        "gitCommit": baseline.provenance.get("gitCommit", ""),
        "deviceId": baseline.provenance.get("deviceId", ""),
        "nativeConfig": baseline.provenance.get("nativeConfig", ""),
    })
    idx["runs"] = runs
    _index_path(report_dir).write_text(
        json.dumps(idx, ensure_ascii=False, indent=2), encoding="utf-8")


def _prune(report_dir: Path, keep: int = MAX_RETAINED_RUNS) -> None:
    """Drop the oldest non-reference snapshots beyond `keep`."""
    idx = _read_json(_index_path(report_dir))
    if not idx:
        return
    runs = idx.get("runs", [])
    if len(runs) <= keep:
        return
    reference = idx.get("reference")
    runs_sorted = sorted(runs, key=lambda r: r.get("timestamp", ""))
    keep_runs: list[dict] = []
    drop: list[dict] = []
    # Always retain the reference; then the newest (keep-1) others.
    protected = [r for r in runs_sorted if r.get("runId") == reference]
    others = [r for r in runs_sorted if r.get("runId") != reference]
    keep_runs = others[-(keep - len(protected)):] if keep > len(protected) else []
    drop = [r for r in others if r not in keep_runs]
    for r in drop:
        p = _run_path(report_dir, r.get("runId", ""))
        try:
            p.unlink(missing_ok=True)
        except OSError:
            pass
    idx["runs"] = sorted(protected + keep_runs, key=lambda r: r.get("timestamp", ""))
    _index_path(report_dir).write_text(
        json.dumps(idx, ensure_ascii=False, indent=2), encoding="utf-8")


# ── load ──────────────────────────────────────────────────────────────────────

def load_recent(report_dir: Path, n: int = 5) -> list[Baseline]:
    """Load the most recent `n` snapshots, oldest-first."""
    idx = _read_json(_index_path(report_dir))
    if not idx:
        return []
    runs = sorted(idx.get("runs", []), key=lambda r: r.get("timestamp", ""))
    out: list[Baseline] = []
    for r in runs[-n:]:
        d = _read_json(_run_path(report_dir, r.get("runId", "")))
        if d:
            out.append(Baseline.from_dict(d))
    return out


def load_reference(report_dir: Path) -> Baseline | None:
    """Load the run marked as the golden reference, if any."""
    d = _read_json(_reference_path(report_dir))
    return Baseline.from_dict(d) if d else None


def mark_reference(report_dir: Path, run_id: str) -> bool:
    """Mark a recorded run as the golden reference."""
    d = _read_json(_run_path(report_dir, run_id))
    if not d:
        return False
    baseline = Baseline.from_dict(d)
    baseline.reference = True
    d["reference"] = True
    _run_path(report_dir, run_id).write_text(
        json.dumps(d, ensure_ascii=False, indent=2), encoding="utf-8")
    _reference_path(report_dir).write_text(
        json.dumps(d, ensure_ascii=False, indent=2), encoding="utf-8")
    idx = _read_json(_index_path(report_dir)) or {"schemaVersion": SCHEMA_VERSION,
                                                  "runs": []}
    idx["reference"] = run_id
    _index_path(report_dir).write_text(
        json.dumps(idx, ensure_ascii=False, indent=2), encoding="utf-8")
    return True


# ── comparability ─────────────────────────────────────────────────────────────

# Provenance fields that MUST match for two runs to be diffed.  A mismatch means
# the numbers are not comparable (different machine / platform / build tier) and
# any "regression" would be an artifact, not a real change.
_COMPARABLE_FIELDS = ("platform", "deviceId", "nativeConfig")


def is_comparable(cur: Baseline, base: Baseline) -> tuple[bool, str]:
    """Return (comparable, reason) for two baselines.

    `gitCommit` is deliberately NOT compared — every run has a new commit, and
    that is the whole point of the regression check.  The commit distance is
    surfaced separately in the report for human judgement.
    """
    for f in _COMPARABLE_FIELDS:
        cv = (cur.provenance or {}).get(f, "")
        bv = (base.provenance or {}).get(f, "")
        # Empty provenance on either side -> treat as unknown, not a mismatch
        # (older snapshots may predate the field).
        if cv and bv and cv != bv:
            return False, f"{f}-mismatch({bv}->{cv})"
    return True, "ok"


def user_summary(baseline: Baseline) -> str:
    """One-line human summary of a snapshot."""
    return (f"{baseline.run_id} @ {baseline.provenance.get('gitCommit', '?')[:8]} "
            f"({len(baseline.chunks)} chunks)")


if __name__ == "__main__":  # pragma: no cover - tiny CLI for manual inspection
    import argparse

    ap = argparse.ArgumentParser(description="baseline store inspector")
    ap.add_argument("--report-dir", default="nightly-build-report")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--mark-reference", metavar="RUN_ID")
    args = ap.parse_args()

    rd = Path(args.report_dir)
    if args.mark_reference:
        ok = mark_reference(rd, args.mark_reference)
        print(f"mark_reference({args.mark_reference}) -> {ok}")
    if args.list:
        for b in load_recent(rd, n=100):
            print(" ", user_summary(b), "[reference]" if b.reference else "")
