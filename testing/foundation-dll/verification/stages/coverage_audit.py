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

        if missing:
            msg = f"{len(missing)} chunk methods not in subjects metadata"
            errors.append(msg)
            for mid in sorted(missing)[:10]:
                print(f"  [coverage-audit]   MISSING: {mid}")
            if len(missing) > 10:
                print(f"  [coverage-audit]   ... and {len(missing) - 10} more")

        if extra:
            msg = f"{len(extra)} extra subjects not in chunk definition"
            errors.append(msg)
            for mid in sorted(extra)[:5]:
                print(f"  [coverage-audit]   EXTRA: {mid}")

        coverage_pct = round(len(subject_method_ids) / len(chunk_method_ids) * 100, 1) if chunk_method_ids else 100.0
        print(f"  [coverage-audit] Coverage: {len(subject_method_ids)}/{len(chunk_method_ids)} ({coverage_pct}%)")

    # ── 5. Final result ──
    duration_ms = int((time.perf_counter() - start) * 1000)
    status = "passed" if not errors else "failed"
    for err in errors:
        print(f"  [coverage-audit] ERROR: {err}")

    return StageResult(
        stage="coverage-audit", status=status,
        summary=f"{status}: chunk={ctx.slug}, declared={len(chunk_method_ids)}, covered={len(subject_method_ids) if metadata_path.exists() else 0}",
        details={
            "chunkSlug": ctx.slug,
            "declaredMethods": len(chunk_method_ids),
            "coveredMethods": len(subject_method_ids) if metadata_path.exists() else 0,
            "missingMethods": len(chunk_method_ids - subject_method_ids) if metadata_path.exists() else 0,
            "errors": errors,
        },
        duration_ms=duration_ms,
    )
