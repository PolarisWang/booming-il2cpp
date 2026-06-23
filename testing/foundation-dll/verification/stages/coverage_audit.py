"""Coverage audit stage — verifies all chunk methods are covered by subjects.

Reads dll-method-manifest.json and namespace-partition.json, then compares
against the subjects metadata to detect gaps.
"""

from __future__ import annotations

import json
import time
from pathlib import Path

from verification.orchestration.context import ChunkContext, StageResult


def run_coverage_audit(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Coverage audit: verify chunk subjects cover the declared method list."""
    start = time.perf_counter()

    manifest_path = ctx.dll_manifest_dir / "dll-method-manifest.json"
    partition_path = ctx.namespace_partition_path
    metadata_path = ctx.subjects_metadata_path

    errors: list[str] = []

    # If build was skipped (0 subjects), skip coverage audit too
    build_result = stages.get("build")
    if build_result and (isinstance(build_result, dict) and build_result.get("status") == "skipped"
                         or getattr(build_result, 'status', None) == "skipped"):
        print(f"  [coverage-audit] Build was skipped (0 subjects), skipping coverage audit")
        return StageResult(
            stage="coverage-audit", status="skipped",
            summary="skipped (build produced 0 subjects)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── 1. Load dll-method-manifest.json ──
    if not manifest_path.exists():
        return StageResult(
            stage="coverage-audit", status="error",
            summary=f"dll-method-manifest.json not found: {manifest_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    manifest_methods = manifest.get("methods", [])
    manifest_total = manifest.get("totalMethods", len(manifest_methods))
    print(f"  [coverage-audit] Manifest: {manifest_total} methods in {manifest.get('assemblyName', '?')}")

    # ── 2. Load namespace-partition.json ──
    if not partition_path.exists():
        return StageResult(
            stage="coverage-audit", status="error",
            summary=f"namespace-partition.json not found: {partition_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    partition = json.loads(partition_path.read_text(encoding="utf-8"))
    chunks = partition.get("chunks", [])
    chunk_def = next((c for c in chunks if c["slug"] == ctx.slug), None)
    if chunk_def is None:
        return StageResult(
            stage="coverage-audit", status="error",
            summary=f"Chunk '{ctx.slug}' not found in partition",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    chunk_methods = chunk_def.get("methods", [])
    chunk_method_ids = {m["methodSubjectId"] for m in chunk_methods}
    print(f"  [coverage-audit] Chunk '{ctx.slug}': {len(chunk_method_ids)} declared methods")

    # ── 3. Load subjects metadata ──
    if not metadata_path.exists():
        errors.append(f"subjects.metadata.json not found: {metadata_path}")
    else:
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
        subject_methods = metadata.get("methods", [])
        subject_method_ids = {m["methodSubjectId"] for m in subject_methods}
        print(f"  [coverage-audit] Subjects metadata: {len(subject_method_ids)} subjects")

        # ── 4. Compare ──
        missing = chunk_method_ids - subject_method_ids
        extra = subject_method_ids - chunk_method_ids

        coverage_pct = round(len(subject_method_ids) / len(chunk_method_ids) * 100, 1) if chunk_method_ids else 100.0

        if missing:
            missing_pct = round(len(missing) / len(chunk_method_ids) * 100, 1)
            msg = f"{len(missing)} chunk methods not in subjects metadata ({missing_pct}% of chunk)"
            errors.append(msg)
            for mid in sorted(missing)[:10]:
                print(f"  [coverage-audit]   MISSING: {mid}")
            if len(missing) > 10:
                print(f"  [coverage-audit]   ... and {len(missing) - 10} more")

            # Strict mode: fail if >5% methods are missing
            if ctx.mode == "strict" and missing_pct > 5.0:
                errors.append(
                    f"STRICT MODE: {missing_pct}% methods missing exceeds 5% threshold"
                )

        if extra:
            # Extra subjects are harmless — they represent additional coverage
            # beyond the declared chunk methods (e.g. void methods producing
            # "no crash" assertions via A3). Report as info, not error.
            print(f"  [coverage-audit]   {len(extra)} extra subjects not in chunk definition (OK)")
            for mid in sorted(extra)[:5]:
                print(f"  [coverage-audit]     EXTRA: {mid}")

        # ── Namespace-level coverage analysis ──
        # Compute per-namespace coverage to detect gaps that chunk-level aggregate
        # misses.  Exclude known AOT-intrinsic types (Reflection.Emit, COM interop)
        # which the ATG cannot probe.
        _INTRINSIC_PREFIXES = (
            "System.Reflection.Emit",
            "System.Runtime.InteropServices.ComWrappers",
            "System.Runtime.CompilerServices.Unsafe",
            "System.Resources.Resource",
        )

        def _ns(mid: str) -> str:
            parts = mid.split("/", 1)
            return parts[0] if len(parts) < 2 else (parts[1].rsplit(".", 1)[0]
                                                     if "." in parts[1] else parts[1])

        chunk_by_ns: dict[str, set[str]] = {}
        for mid in chunk_method_ids:
            ns = _ns(mid)
            if not ns.startswith(_INTRINSIC_PREFIXES):
                chunk_by_ns.setdefault(ns, set()).add(mid)
        covered_by_ns: dict[str, set[str]] = {}
        for mid in subject_method_ids:
            covered_by_ns.setdefault(_ns(mid), set()).add(mid)

        namespace_gaps: list[dict] = []
        for ns, methods in sorted(chunk_by_ns.items()):
            covered = covered_by_ns.get(ns, set())
            missing_in_ns = methods - covered
            if missing_in_ns:
                ns_cov = round((len(covered) / len(methods)) * 100, 1) if methods else 100
                namespace_gaps.append({
                    "namespace": ns, "declared": len(methods),
                    "covered": len(covered), "coveragePct": ns_cov,
                    "gaps": sorted(missing_in_ns)[:20],
                })
                print(f"  [coverage-audit]   NS {ns}: {len(covered)}/{len(methods)} ({ns_cov}%)"
                      f" — {len(missing_in_ns)} gaps")

        if namespace_gaps:
            max_gap = max(g["coveragePct"] for g in namespace_gaps)
            min_gap = min(g["coveragePct"] for g in namespace_gaps)
            print(f"  [coverage-audit] Namespace range: {min_gap}%–{max_gap}%"
                  f" ({len(namespace_gaps)} NS with gaps)")

        coverage_pct = round(len(subject_method_ids) / len(chunk_method_ids) * 100, 1) \
            if chunk_method_ids else 100.0
        print(f"  [coverage-audit] Coverage: {len(subject_method_ids)}/{len(chunk_method_ids)}"
              f" ({coverage_pct}%)")

    # ── 5. Final result ──
    duration_ms = int((time.perf_counter() - start) * 1000)
    status = "passed" if not errors else "failed"
    for err in errors:
        print(f"  [coverage-audit] ERROR: {err}")

    return StageResult(
        stage="coverage-audit", status=status,
        summary=f"{status}: chunk={ctx.slug}, declared={len(chunk_method_ids)},"
                f" covered={len(subject_method_ids) if metadata_path.exists() else 0}",
        details={
            "chunkSlug": ctx.slug,
            "declaredMethods": len(chunk_method_ids),
            "coveredMethods": len(subject_method_ids) if metadata_path.exists() else 0,
            "missingMethods": len(chunk_method_ids - subject_method_ids)
                if metadata_path.exists() else 0,
            "coveragePct": coverage_pct if metadata_path.exists() else None,
            "namespaceGaps": namespace_gaps if metadata_path.exists() else [],
            "errors": errors,
        },
        duration_ms=duration_ms,
    )
