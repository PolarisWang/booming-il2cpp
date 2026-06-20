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

        coverage_pct = round(len(subject_method_ids) / len(chunk_method_ids) * 100, 1) if chunk_method_ids else 100.0
        coverage_pct = min(100.0, coverage_pct)  # cap at 100% (extra subjects are bonus)

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
            print(f"  [coverage-audit]   {len(extra)} extra subjects not in chunk definition (OK)")
            for mid in sorted(extra)[:5]:
                print(f"  [coverage-audit]     EXTRA: {mid}")

        coverage_pct = round(len(subject_method_ids) / len(chunk_method_ids) * 100, 1) if chunk_method_ids else 100.0
        coverage_pct = min(100.0, coverage_pct)  # cap at 100% (extra subjects are bonus)
        print(f"  [coverage-audit] Coverage: {len(subject_method_ids)}/{len(chunk_method_ids)} ({coverage_pct}%)")

        # ── 4b. Namespace-level breakdown ──
        # MethodSubjectId format: "Assembly.Name/Namespace.Type::Method"
        # Extract namespace = Assembly/Namespace.Type (before ::)
        def _extract_ns(mid: str) -> str:
            if "::" in mid:
                return mid.split("::")[0]
            return mid

        ns_declared: dict[str, int] = {}
        ns_covered: dict[str, int] = {}
        for mid in chunk_method_ids:
            ns = _extract_ns(mid)
            ns_declared[ns] = ns_declared.get(ns, 0) + 1
        for mid in subject_method_ids:
            ns = _extract_ns(mid)
            ns_covered[ns] = ns_covered.get(ns, 0) + 1

        namespace_gaps: dict[str, dict] = {}
        for ns in sorted(ns_declared):
            declared = ns_declared[ns]
            covered = ns_covered.get(ns, 0)
            gap = max(0, declared - covered)
            gap_pct = round(gap / declared * 100, 1) if declared > 0 else 0
            if gap > 0:
                namespace_gaps[ns] = {
                    "declared": declared,
                    "covered": covered,
                    "gap": gap,
                    "gapPct": gap_pct,
                }

        if namespace_gaps:
            print(f"  [coverage-audit] Namespace gaps: {len(namespace_gaps)} namespace(s) with missing coverage")
            for ns, info in list(namespace_gaps.items())[:10]:
                print(f"    {ns}: gap={info['gap']}/{info['declared']} ({info['gapPct']}%)")
            if len(namespace_gaps) > 10:
                print(f"    ... and {len(namespace_gaps) - 10} more")

        # ── 4c. Categorize missing methods by probeability ──
        # Hardware intrinsics (Vector2/4, Matrix4x4, Plane, BitOperations, etc.)
        # can never be probed by ATG because they require CPU SIMD support.
        # Separate them from fixable gaps for a more accurate coverage picture.
        _INTRINSIC_TYPE_NAMES = frozenset({
            "Vector2", "Vector3", "Vector4", "Vector128", "Vector256", "Vector64",
            "Matrix4x4", "Matrix3x2", "Plane", "Quaternion",
            "BitOperations", "Vector", "VectorT",
            # Static classes that can't be instantiated by ATG
            "WebUtility",
        })
        unprobeable_count = 0
        fixable_missing = []
        for mid in missing:
            type_part = mid.split("::")[0] if "::" in mid else mid.split("/")[-1]
            base_name = type_part.rsplit(".", 1)[-1] if "." in type_part else type_part
            base_name = base_name.split("`")[0]
            if base_name in _INTRINSIC_TYPE_NAMES:
                unprobeable_count += 1
            else:
                fixable_missing.append(mid)

        if unprobeable_count > 0:
            print(f"  [coverage-audit] Of {len(missing)} missing methods, "
                  f"{unprobeable_count} are hardware intrinsics (cannot probe) and "
                  f"{len(fixable_missing)} are fixable gaps")
        if fixable_missing:
            for mid in sorted(fixable_missing)[:10]:
                print(f"  [coverage-audit]   FIXABLE GAP: {mid}")

    # ── 5. Write results to chunk results dir ──
    chunk_results_dir = ctx.chunk_dir / "results"
    chunk_results_dir.mkdir(parents=True, exist_ok=True)
    audit_path = chunk_results_dir / "coverage-audit.json"
    audit_data = {
        "chunkSlug": ctx.slug,
        "declaredMethods": len(chunk_method_ids),
        "coveredMethods": len(subject_method_ids) if metadata_path.exists() else 0,
        "missingMethods": len(chunk_method_ids - subject_method_ids) if metadata_path.exists() else 0,
        "coveragePct": coverage_pct,
        "namespaceGaps": namespace_gaps if metadata_path.exists() else {},
        "unprobeableMethods": unprobeable_count,
        "fixableGapCount": len(fixable_missing),
    }
    try:
        audit_path.write_text(json.dumps(audit_data, indent=2), encoding="utf-8")
    except OSError:
        pass

    # ── 6. Final result ──
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
            "coveragePct": coverage_pct,
            "namespaceGaps": namespace_gaps if metadata_path.exists() else {},
            "errors": errors,
        },
        duration_ms=duration_ms,
    )
