"""Allure report stage — converts per-method fact results to Allure JSON format."""

from __future__ import annotations

import json
import shutil
import time
import uuid
from datetime import datetime, timezone
from pathlib import Path

from verification.orchestration.context import ChunkContext, StageResult


def run_allure_report(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Generate Allure-format result files from per-method fact results.

    Reads fact-results.json from each chunk under *all* assemblies in
    ``ctx.foundation_dir`` and writes Allure-compatible ``<uuid>-result.json``
    files to ``<foundation-dll>/_allure-results/`` (the path the Jenkins
    Allure Report stage expects).

    This is intentionally non-fatal: exceptions are caught and logged so a
    failed Allure generation does not break the nightly pipeline.
    """
    start = time.perf_counter()

    foundation_dll_root = ctx.foundation_dir.resolve()
    chunks_glob = list(foundation_dll_root.rglob("chunks/*/results/fact-results.json"))

    if not chunks_glob:
        return StageResult(
            stage="allure", status="skipped",
            summary="No fact-results.json files found",
            duration_ms=__duration_ms(start),
        )

    # Write into the parent foundation-dll dir (shared across all assemblies)
    # so the Jenkins Allure stage finds it at the expected path:
    #   ${BOOMING_DIR}/testing/foundation-dll/_allure-results
    allure_dir = foundation_dll_root.parent / "_allure-results"
    if allure_dir.exists():
        shutil.rmtree(allure_dir)
    allure_dir.mkdir(parents=True, exist_ok=True)

    total_cases = 0
    total_passed = 0
    total_failed = 0

    for results_path in sorted(chunks_glob):
        try:
            data = json.loads(results_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            continue

        # Derive suite label from path:  .../<assembly>/chunks/<slug>/results/fact-results.json
        rel = results_path.relative_to(foundation_dll_root)
        slug = rel.parent.parent.name       # chunks/<slug>
        assembly = rel.parent.parent.parent.name  # <assembly>

        passed, failed = __write_allure_results(
            allure_dir, data.get("aot", []),
            assembly, slug, "AOT",
        )
        total_cases += passed + failed
        total_passed += passed
        total_failed += failed

        if "jit" in data:
            p, f = __write_allure_results(
                allure_dir, data["jit"],
                assembly, slug, "JIT",
            )
            total_cases += p + f
            total_passed += p
            total_failed += f

    # Write environment info
    try:
        (allure_dir / "environment.properties").write_text(
            f"Assembly={foundation_dll_root.name}\n"
            f"Platform={ctx.platform}\n"
            f"NativeConfig={ctx.native_config}\n"
            f"RunId={ctx.run_id}\n",
            encoding="utf-8",
        )
    except OSError:
        pass

    status = "passed" if total_failed == 0 else "partial"
    duration = __duration_ms(start)
    return StageResult(
        stage="allure", status=status,
        summary=f"Allure: {total_cases} cases ({total_passed} passed, {total_failed} failed) "
                f"→ {len(list(allure_dir.glob('*.json')))} result files",
        duration_ms=duration,
    )


# ── helpers ──────────────────────────────────────────────────────────────

def __write_allure_results(
    allure_dir: Path,
    results: list[dict],
    assembly: str,
    slug: str,
    tech: str,
) -> tuple[int, int]:
    """Write Allure result files for one technology (AOT or JIT).

    Returns ``(passed, failed)`` counts.
    """
    passed = 0
    failed = 0
    now_ms = int(datetime.now(timezone.utc).timestamp() * 1000)

    for fr in results:
        mid = fr.get("methodSubjectId", f"unknown-{uuid.uuid4()}")
        is_passed = fr.get("passed", False)
        message = fr.get("message", "")

        allure_result = {
            "name": f"{tech}: {mid}",
            "status": "passed" if is_passed else "failed",
            "stage": "finished",
            "start": now_ms,
            "stop": now_ms,
            "parameters": [],
            "labels": [
                {"name": "suite", "value": f"{assembly}/{slug}"},
                {"name": "testClass", "value": f"FactTest[{tech}]"},
                {"name": "package", "value": assembly},
                {"name": "parentSuite", "value": assembly},
                {"name": "subSuite", "value": slug},
            ],
            "links": [],
            "statusDetails": {
                "message": message if not is_passed and message else "",
                "trace": "",
            },
        }

        result_file = allure_dir / f"{uuid.uuid4()}-result.json"
        try:
            result_file.write_text(
                json.dumps(allure_result, ensure_ascii=False), encoding="utf-8",
            )
        except OSError:
            continue

        if is_passed:
            passed += 1
        else:
            failed += 1

    return passed, failed


def __duration_ms(start: float) -> int:
    return int((time.perf_counter() - start) * 1000)
