"""Pipeline reporting stage — runs after aggregate.

Produces unified cross-assembly reports and AI-ready data exports.
Does NOT modify any generated files — reads only existing results.
Non-fatal: if this stage fails, the pipeline still produces all existing output.
"""

from __future__ import annotations

import json
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult


def run_reporting(ctx: ChunkContext, previous_stages: dict[str, StageResult]) -> StageResult:
    """Generate unified pipeline report and AI-ready data exports.

    Reads per-chunk results from the aggregate stage output and writes:
      - results/pipeline-runs.jsonl        (one JSON line per run)
      - results/ai/fact-failures.jsonl     (AI-consumable fact failures)
      - results/summary/cross-dll-dashboard.json  (cross-assembly summary)

    Args:
        ctx: Chunk context (used for assembly/foundation dir).
        previous_stages: Results from all preceding stages.

    Returns:
        StageResult with status "passed" or "failed".
    """
    start = time.perf_counter()
    foundation_dir = ctx.foundation_dir
    if not foundation_dir:
        return StageResult(
            stage="reporting", status="skipped",
            summary="No foundation_dir in context",
            duration_ms=0)

    results_base = foundation_dir.parent / "results"
    ai_dir = results_base / "ai"
    summary_dir = results_base / "summary"

    try:
        # ── Collect per-assembly reports ──
        assemblies = _collect_assembly_reports(foundation_dir)
        if not assemblies:
            return StageResult(
                stage="reporting", status="passed",
                summary="No assembly reports found",
                duration_ms=int((time.perf_counter() - start) * 1000))

        # ── Load per-chunk fact data ──
        for asm_name, asm_data in assemblies.items():
            _enrich_with_chunk_facts(asm_data, foundation_dir / asm_name / "chunks")

        # ── Write pipeline-runs.jsonl ──
        results_base.mkdir(parents=True, exist_ok=True)
        run_entry = {
            "runId": ctx.run_id,
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "platform": ctx.platform,
            "device": ctx.device,
            "gitCommit": ctx.git_commit,
            "gitBranch": ctx.git_branch,
            "nativeConfig": ctx.native_config,
            "mode": ctx.mode,
            "assemblies": assemblies,
        }
        pipeline_runs_path = results_base / "pipeline-runs.jsonl"
        with open(pipeline_runs_path, "a", encoding="utf-8") as f:
            f.write(json.dumps(run_entry, ensure_ascii=False) + "\n")

        # ── Write ai/fact-failures.jsonl ──
        ai_dir.mkdir(parents=True, exist_ok=True)
        fact_failures = _extract_fact_failures(assemblies)
        if fact_failures:
            with open(ai_dir / "fact-failures.jsonl", "w", encoding="utf-8") as f:
                for entry in fact_failures:
                    f.write(json.dumps(entry, ensure_ascii=False) + "\n")

        # ── Write summary/cross-dll-dashboard.json ──
        summary_dir.mkdir(parents=True, exist_ok=True)
        cross_dll = _build_cross_dll_summary(assemblies)
        with open(summary_dir / "cross-dll-dashboard.json", "w", encoding="utf-8") as f:
            json.dump(cross_dll, f, indent=2, ensure_ascii=False)

        duration_ms = int((time.perf_counter() - start) * 1000)
        return StageResult(
            stage="reporting", status="passed",
            summary=f"Reported {len(assemblies)} assemblies, "
                    f"{len(fact_failures)} fact failures, "
                    f"{len(cross_dll.get('assemblies', []))} cross-DLL entries",
            duration_ms=duration_ms)

    except Exception as ex:
        return StageResult(
            stage="reporting", status="error",
            summary=f"Reporting stage failed: {ex}",
            errors=[str(ex)],
            duration_ms=int((time.perf_counter() - start) * 1000))


def _collect_assembly_reports(foundation_dir: Path) -> dict[str, dict]:
    """Collect latest aggregate reports for all assemblies under foundation_dir."""
    assemblies = {}
    # Check if foundation_dir itself contains _dll/reports
    _try_add_assembly(assemblies, foundation_dir)
    return assemblies


def _try_add_assembly(assemblies: dict, asm_dir: Path) -> None:
    """Try to read aggregate reports for a single assembly directory."""
    reports_dir = asm_dir / "_dll" / "reports" / "latest"
    if not reports_dir.is_dir():
        return

    asm_name = asm_dir.name
    entry: dict[str, Any] = {"chunks": []}

    for name in ("dashboard", "fact-summary", "benchmark-summary",
                  "comparison-summary", "coverage-audit"):
        path = reports_dir / f"{name}.json"
        if path.exists():
            try:
                entry[name] = json.loads(path.read_text(encoding="utf-8"))
            except (json.JSONDecodeError, OSError):
                entry[name] = None

    assemblies[asm_name] = entry


def _enrich_with_chunk_facts(asm_data: dict, chunks_dir: Path) -> None:
    """Read per-chunk fact.json and add per-method failure data."""
    if not chunks_dir.is_dir():
        return
    for chunk_dir in sorted(chunks_dir.iterdir()):
        if not chunk_dir.is_dir():
            continue
        slug = chunk_dir.name
        fact_path = chunk_dir / "results" / "fact.json"
        if not fact_path.exists():
            continue
        try:
            fact_data = json.loads(fact_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            continue

        results = fact_data.get("results", [])
        failures = []
        for r in results:
            if not r.get("passed", True):
                from verification.reporting.models import classify_failure, route_for_error
                pattern = classify_failure(r.get("message", ""), r.get("value"))
                failures.append({
                    "methodIndex": r.get("methodIndex", 0),
                    "value": r.get("value"),
                    "message": r.get("message", ""),
                    "errorPattern": pattern,
                    "routeHint": route_for_error(pattern),
                })

        chunk_entry = {
            "slug": slug,
            "fact": {
                "passed": fact_data.get("passed", 0),
                "total": fact_data.get("total", 0),
                "truncated": fact_data.get("truncated", False),
                "failures": failures,
            },
        }
        asm_data.setdefault("chunks", []).append(chunk_entry)


def _extract_fact_failures(assemblies: dict[str, dict]) -> list[dict]:
    """Extract all fact failures across assemblies as flat JSONL entries."""
    failures = []
    for asm_name, asm_data in assemblies.items():
        for chunk in asm_data.get("chunks", []):
            fact = chunk.get("fact", {})
            for f in fact.get("failures", []):
                failures.append({
                    "assembly": asm_name,
                    "slug": chunk.get("slug", ""),
                    "methodIndex": f.get("methodIndex"),
                    "value": f.get("value"),
                    "message": f.get("message"),
                    "errorPattern": f.get("errorPattern"),
                    "routeHint": f.get("routeHint"),
                })
    return failures


def _build_cross_dll_summary(assemblies: dict[str, dict]) -> dict:
    """Build cross-DLL aggregate summary."""
    entries = []
    total_passed = 0
    total_fact_methods = 0
    for asm_name, asm_data in sorted(assemblies.items()):
        fs = asm_data.get("fact-summary") or {}
        ds = asm_data.get("dashboard") or {}
        cs = asm_data.get("comparison-summary") or {}

        fact_passed = fs.get("totalPassed", 0)
        fact_total = fs.get("totalFactMethods", 0)
        total_passed += fact_passed
        total_fact_methods += fact_total

        entries.append({
            "assembly": asm_name,
            "factPassRate": round(fact_passed / fact_total, 4) if fact_total else 0,
            "totalBenchmarkedMethods": ds.get("summary", {}).get("totalBenchmarkedMethods", 0),
            "chaosAotVsNet8Pct": (cs.get("aggregate") or {}).get("chaosAotVsNet8Pct", {}),
        })

    return {
        "generatedAt": datetime.now(timezone.utc).isoformat(),
        "totalAssemblies": len(entries),
        "overallFactPassRate": round(total_passed / total_fact_methods, 4) if total_fact_methods else 0,
        "assemblies": entries,
    }
