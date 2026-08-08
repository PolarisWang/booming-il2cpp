"""AI-ready data export — reads pipeline-runs.jsonl and generates per-domain JSONL.

Output files:
  - results/ai/fact-failures.jsonl       — fact failures with routing hints
  - results/ai/benchmark-regressions.jsonl — perf regressions with bottleneck
  - results/ai/hu-targets.jsonl          — hotupdate candidates with overhead
  - results/ai/memory-targets.jsonl       — high-allocation methods
  - results/ai/known-failures.jsonl       — per-platform known-failure patterns
"""

from __future__ import annotations

import json
import sys
from pathlib import Path


def export_all(pipeline_runs_path: Path, ai_dir: Path) -> dict[str, int]:
    """Read pipeline-runs.jsonl and write all AI-ready exports.

    Args:
        pipeline_runs_path: Path to pipeline-runs.jsonl.
        ai_dir: Output directory for AI exports.

    Returns:
        Dict mapping export name to record count.
    """
    if not pipeline_runs_path.exists():
        print(f"[ai-export] Source not found: {pipeline_runs_path}", file=sys.stderr)
        return {"error": -1}

    ai_dir.mkdir(parents=True, exist_ok=True)
    counts: dict[str, int] = {}

    fact_failures: list[dict] = []
    benchmark_regressions: list[dict] = []
    hu_targets: list[dict] = []
    memory_targets: list[dict] = []

    with open(pipeline_runs_path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                run = json.loads(line)
            except json.JSONDecodeError:
                continue

            platform = run.get("platform", "unknown")
            git_commit = run.get("gitCommit", "")
            assemblies = run.get("assemblies", {})

            for asm_name, asm_data in assemblies.items():
                for chunk in asm_data.get("chunks", []):
                    slug = chunk.get("slug", "")
                    fact = chunk.get("fact", {})
                    _collect_fact_failures(fact_failures, asm_name, slug, platform, git_commit, fact)
                    _collect_benchmark_regressions(benchmark_regressions, asm_name, slug, platform, git_commit,
                                                   chunk.get("benchmark") or {})
                    _collect_hu_targets(hu_targets, asm_name, slug, platform, git_commit,
                                        chunk.get("hotupdate") or {})
                    _collect_memory_targets(memory_targets, asm_name, slug, platform, git_commit,
                                            chunk.get("benchmark") or {})

    # Write fact-failures.jsonl
    _write_jsonl(ai_dir / "fact-failures.jsonl", fact_failures)
    counts["fact-failures"] = len(fact_failures)

    # Write benchmark-regressions / hu-targets / memory-targets (were declared but
    # never populated — the docstring promised them but the files were never written).
    _write_jsonl(ai_dir / "benchmark-regressions.jsonl", benchmark_regressions)
    counts["benchmark-regressions"] = len(benchmark_regressions)
    _write_jsonl(ai_dir / "hu-targets.jsonl", hu_targets)
    counts["hu-targets"] = len(hu_targets)
    _write_jsonl(ai_dir / "memory-targets.jsonl", memory_targets)
    counts["memory-targets"] = len(memory_targets)

    # Write known-failures.jsonl (per-platform aggregated pattern view)
    known = _build_known_failures(fact_failures)
    _write_jsonl(ai_dir / "known-failures.jsonl", known)
    counts["known-failures"] = len(known)

    print(f"[ai-export] Wrote {counts}")
    return counts


def _collect_fact_failures(
    failures: list[dict], asm: str, slug: str,
    platform: str, git_commit: str, fact: dict,
) -> None:
    """Extract fact failures into flat JSONL entries."""
    for f_entry in fact.get("failures", []):
        failures.append({
            "assembly": asm,
            "slug": slug,
            "platform": platform,
            "gitCommit": git_commit,
            "methodIndex": f_entry.get("methodIndex"),
            "value": f_entry.get("value"),
            "message": f_entry.get("message"),
            "errorPattern": f_entry.get("errorPattern", "unknown"),
            "routeHint": f_entry.get("routeHint", "dev-il2cpp-debug-expert"),
        })

    # If fact had a metadata mismatch, add a synthetic entry
    if fact.get("metadataMismatch"):
        failures.append({
            "assembly": asm,
            "slug": slug,
            "platform": platform,
            "gitCommit": git_commit,
            "methodIndex": -1,
            "value": None,
            "message": f"Metadata mismatch: passed={fact.get('passed')}, total={fact.get('total')}",
            "errorPattern": "metadata_mismatch",
            "routeHint": "dev-il2cpp-codegen-expert",
        })


def _collect_benchmark_regressions(
    out: list[dict], asm: str, slug: str,
    platform: str, git_commit: str, benchmark: dict,
) -> None:
    """Extract benchmark methods whose regressionDelta indicates a regression."""
    for m in benchmark.get("perMethodStats") or []:
        delta = m.get("regressionDelta")
        if delta is not None and float(delta) > 0:
            out.append({
                "assembly": asm,
                "slug": slug,
                "platform": platform,
                "gitCommit": git_commit,
                "methodSubjectId": m.get("methodSubjectId"),
                "regressionDelta": delta,
                "bottleneck": m.get("bottleneck", ""),
                "routeHint": route_for_bottleneck_str(m.get("bottleneck", "")),
            })


def _collect_hu_targets(
    out: list[dict], asm: str, slug: str,
    platform: str, git_commit: str, hotupdate: dict,
) -> None:
    """Extract hotupdate methods worth keeping native (low overhead, patchable)."""
    for m in hotupdate.get("perMethodStats") or []:
        if m.get("keepNative") or (m.get("postPatchNsPerOp") is not None
                                   and m.get("postPatchNsPerOp") > 0):
            out.append({
                "assembly": asm,
                "slug": slug,
                "platform": platform,
                "gitCommit": git_commit,
                "methodSubjectId": m.get("methodSubjectId"),
                "postPatchNsPerOp": m.get("postPatchNsPerOp"),
                "keepNative": m.get("keepNative", False),
                "routeHint": route_for_bottleneck_str(m.get("bottleneck", "")),
            })


def _collect_memory_targets(
    out: list[dict], asm: str, slug: str,
    platform: str, git_commit: str, benchmark: dict,
) -> None:
    """Extract high-allocation benchmark methods as memory-optimization targets."""
    for m in benchmark.get("perMethodStats") or []:
        alloc = m.get("allocPerOp")
        if alloc is not None:
            out.append({
                "assembly": asm,
                "slug": slug,
                "platform": platform,
                "gitCommit": git_commit,
                "methodSubjectId": m.get("methodSubjectId"),
                "allocPerOp": alloc,
            })


def route_for_bottleneck_str(bottleneck: str) -> str:
    """Map a bottleneck label to the routing hint (default fallback)."""
    if not bottleneck:
        return "dev-il2cpp-runtime-expert"
    return bottleneck  # models.route_for_bottleneck would map it if imported


def _build_known_failures(fact_failures: list[dict]) -> list[dict]:
    """Aggregate fact failures by (assembly, slug, errorPattern) per platform."""
    seen: dict[str, dict] = {}
    for f in fact_failures:
        key = f"{f['assembly']}/{f['slug']}/{f['errorPattern']}/{f['platform']}"
        if key not in seen:
            seen[key] = {
                "assembly": f["assembly"],
                "slug": f["slug"],
                "errorPattern": f["errorPattern"],
                "platform": f["platform"],
                "routeHint": f["routeHint"],
                "firstSeen": f.get("gitCommit", ""),
                "occurrences": 0,
            }
        seen[key]["occurrences"] += 1

    result = list(seen.values())
    result.sort(key=lambda x: x["occurrences"], reverse=True)
    return result


def _write_jsonl(path: Path, records: list[dict]) -> None:
    """Write a list of dicts as newline-delimited JSON."""
    with open(path, "w", encoding="utf-8") as f:
        for r in records:
            f.write(json.dumps(r, ensure_ascii=False) + "\n")


def main() -> int:
    """CLI entry point: python -m verification.tools.ai_export <pipeline-runs.jsonl> [ai-dir]"""
    args = sys.argv[1:]
    if not args:
        print("Usage: python -m verification.tools.ai_export <pipeline-runs.jsonl> [ai-dir]",
              file=sys.stderr)
        return 1

    src = Path(args[0])
    dst = Path(args[1]) if len(args) > 1 else src.parent / "ai"
    result = export_all(src, dst)
    # export_all returns {"error": -1} when the source is missing — propagate as
    # a nonzero exit (was discarded, so a missing source silently "succeeded").
    if "error" in result:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
