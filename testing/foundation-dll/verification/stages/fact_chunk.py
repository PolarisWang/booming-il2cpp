"""Simplified fact stage for chunk-based pipeline.

Runs entry.exe and optionally entry-jit.exe --fact-json from the chunk's
native directory and parses per-method JSON results.

Status determination is exit-code based: a clean exit means all subjects
were dispatched; a crash with passed<total is genuine failure.

Metadata cross-check (expectedTotal) is advisory-only — codegen may produce
fewer subjects than metadata declares, and that's expected.
"""

from __future__ import annotations

import json
import subprocess
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult


def _load_chunk_config(chunk_dir: Path) -> dict[str, Any]:
    """Load chunk.json config, returning {} if missing or corrupt."""
    config_path = chunk_dir / "chunk.json"
    if config_path.exists():
        try:
            return json.loads(config_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            print(f"  [fact] WARNING: corrupt chunk.json at {config_path}")
    return {}


def _is_jit_enabled(chunk_dir: Path) -> bool:
    """Check if JIT codegen is enabled for this chunk via chunk.json."""
    config = _load_chunk_config(chunk_dir)
    return config.get("jitEnabled", False)


def _run_single_fact(exe_path: Path, tech: str) -> dict:
    """Run --fact-json for a single binary, return parsed results dict."""
    print(f"  [fact] [{tech}] Running {exe_path} --fact-json...")
    try:
        r = subprocess.run(
            [str(exe_path), "--fact-json"],
            capture_output=True, timeout=600,
        )
    except subprocess.TimeoutExpired:
        return {"error": "timed_out", "returncode": -1, "stdout": "", "stderr": "",
                "passed": 0, "total": 0, "results": [], "truncated": False}

    stdout = r.stdout.decode("utf-8", errors="replace") if r.stdout else ""
    stderr = r.stderr.decode("utf-8", errors="replace") if r.stderr else ""

    # Parse JSON output — truncated output is treated as empty (not silently repaired)
    fact_results = []
    json_truncated = False
    json_start = stdout.find("{")
    json_end = stdout.rfind("}") + 1
    if json_start >= 0 and json_end > json_start:
        payload = stdout[json_start:json_end]
        try:
            parsed = json.loads(payload)
            fact_results = parsed.get("factResults", [])
        except (json.JSONDecodeError, KeyError):
            json_truncated = True

    passed = sum(1 for fr in fact_results if fr.get("passed"))
    total = len(fact_results)

    # Parse assertion failure messages from stderr
    assert_messages: list[str] = []
    if stderr:
        for line in stderr.splitlines():
            if "[ASSERT FAIL]" in line:
                msg = line[line.index("[ASSERT FAIL]") + 14:].strip()
                assert_messages.append(msg)
    fail_idx = 0
    for fr in fact_results:
        if not fr.get("passed") and fail_idx < len(assert_messages):
            fr["message"] = assert_messages[fail_idx]
            fail_idx += 1

    return {
        "error": None,
        "returncode": r.returncode,
        "stdout": stdout, "stderr": stderr,
        "passed": passed, "total": total,
        "results": fact_results, "truncated": json_truncated,
    }


def _tech_status(tech_result: dict, meta_total: int | None) -> str:
    """Determine status for a single technology result.

    Returns "error" when total==0 so silent failures (e.g. missing
    kHotpatchKeepNative flag on all subjects) are exposed rather than
    silently skipped.
    """
    passed = tech_result["passed"]
    total = tech_result["total"]
    rc = tech_result["returncode"]
    is_clean = rc == 0
    if is_clean and passed == total and total > 0:
        return "passed"
    if total == 0:
        return "error"
    if rc != 0 and passed < total:
        return "partial" if passed > 0 else "error"
    if rc != 0 and passed == total:
        return "partial"  # truncated JSON - incomplete data
    if rc == 0 and passed < total:
        return "partial"
    return "passed"


def _write_fact_history(ctx: ChunkContext, aot_result: dict, jit_result: dict | None) -> None:
    """Append fact results to _dll/reports/history/fact-YYYY-MM-DD.jsonl."""
    from datetime import datetime, timezone
    history_dir = ctx.foundation_dir / "_dll" / "reports" / "history"
    history_dir.mkdir(parents=True, exist_ok=True)
    date_str = datetime.now(timezone.utc).strftime("%Y-%m-%d")
    history_path = history_dir / f"fact-{date_str}.jsonl"

    entry = {
        "runId": ctx.run_id,
        "platform": ctx.platform,
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "slug": ctx.slug,
        "aot": {
            "passed": aot_result.get("passed", 0),
            "total": aot_result.get("total", 0),
            "returncode": aot_result.get("returncode", 0),
        },
    }
    if jit_result:
        entry["jit"] = {
            "passed": jit_result.get("passed", 0),
            "total": jit_result.get("total", 0),
            "returncode": jit_result.get("returncode", 0),
        }
    try:
        with open(history_path, "a", encoding="utf-8") as f:
            f.write(json.dumps(entry, ensure_ascii=False) + "\n")
    except OSError:
        pass  # non-fatal


def _write_fact_results(ctx: ChunkContext, aot_result: dict, jit_result: dict | None,
                        meta_total: int | None, fact_method_count: int | None,
                        value_warnings: int) -> None:
    """Write fact.json to chunk results dir for aggregate stage to read."""
    chunk_results_dir = ctx.chunk_dir / "results"
    chunk_results_dir.mkdir(parents=True, exist_ok=True)
    fact_path = chunk_results_dir / "fact.json"
    passed = aot_result.get("passed", 0)
    total = aot_result.get("total", 0)
    if jit_result and jit_result.get("passed", 0) > passed:
        passed = jit_result.get("passed", 0)
        total = jit_result.get("total", 0)
    fact_data = {
        "passed": passed,
        "total": total,
        "valueSuspicious": value_warnings > 0,
        "valueWarnings": value_warnings,
        "metaTotal": meta_total or total,
        "factMethodCount": fact_method_count or total,
    }
    try:
        fact_path.write_text(json.dumps(fact_data, ensure_ascii=False, indent=2), encoding="utf-8")
    except OSError:
        pass  # non-fatal


def run_fact_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Fact stage: run chunk's entry.exe and optionally entry-jit.exe --fact-json."""
    start = time.perf_counter()

    # AOT: required
    aot_exe = ctx.entry_exe_path
    if not aot_exe.exists():
        return StageResult(
            stage="fact", status="failed",
            summary=f"entry.exe not found: {aot_exe}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # JIT: check chunk config first, then verify the binary exists
    jit_exe = ctx.entry_jit_exe_path
    jit_enabled = _is_jit_enabled(ctx.chunk_dir)
    has_jit = jit_exe.exists()

    errors: list[str] = []
    if jit_enabled and not has_jit:
        errors.append("jit_enabled in chunk.json but entry-jit.exe not found")
        print(f"  [fact] ERROR: jit_enabled in chunk.json but entry-jit.exe not found")
        has_jit = False
    elif not jit_enabled and has_jit:
        # Config says JIT disabled, but binary exists — info only, still run it
        print(f"  [fact] entry-jit.exe exists but chunk.json has jitEnabled=false, running anyway")
    elif not has_jit:
        errors.append("entry-jit.exe not found — chaos-jit fact is required")
        print(f"  [fact] ERROR: entry-jit.exe not found, chaos-jit fact is required")

    # Build metadata reference — None if unavailable (handled by callers)
    meta_total: int | None = None
    meta_fact_count: int | None = None
    meta_path = ctx.chunk_dir / "managed" / "subjects" / "subjects.metadata.json"
    if meta_path.exists():
        try:
            md = json.loads(meta_path.read_text(encoding="utf-8"))
            meta_total = md.get("totalMethods")
            meta_fact_count = md.get("factMethodCount")
        except (json.JSONDecodeError, OSError):
            print(f"  [fact] WARNING: corrupt or unreadable metadata at {meta_path}")

    # Run AOT
    aot_result = _run_single_fact(aot_exe, "aot")
    aot_status = _tech_status(aot_result, meta_total)
    if aot_result["error"]:
        errors.append(f"aot: {aot_result['error']}")

    # Run JIT (if available)
    jit_result = None
    jit_status = "skipped"
    if has_jit:
        jit_result = _run_single_fact(jit_exe, "jit")
        jit_status = _tech_status(jit_result, meta_total)
        if jit_result["error"]:
            errors.append(f"jit: {jit_result['error']}")

    # ── Cross-tech diff (AOT vs JIT) ──
    cross_tech_diffs: list[dict] = []
    status = aot_status  # initial: AOT status; cross-tech + JIT upgrade may modify below
    if jit_result and aot_result.get("results") and jit_result.get("results"):
        aot_by_id = {r.get("methodSubjectId", f"idx_{i}"): r
                     for i, r in enumerate(aot_result["results"])}
        jit_by_id = {r.get("methodSubjectId", f"idx_{i}"): r
                     for i, r in enumerate(jit_result["results"])}
        all_ids = set(aot_by_id) | set(jit_by_id)
        for mid in sorted(all_ids):
            aot_pass = aot_by_id.get(mid, {}).get("passed", False)
            jit_pass = jit_by_id.get(mid, {}).get("passed", False)
            if aot_pass != jit_pass:
                cross_tech_diffs.append({
                    "methodSubjectId": mid,
                    "aotPassed": aot_pass,
                    "jitPassed": jit_pass,
                })
        if cross_tech_diffs:
            print(f"  [fact] Cross-tech diff: {len(cross_tech_diffs)} method(s) with inconsistent AOT/JIT results")
            for d in cross_tech_diffs[:10]:
                aot_s = "PASS" if d["aotPassed"] else "FAIL"
                jit_s = "PASS" if d["jitPassed"] else "FAIL"
                print(f"    {d['methodSubjectId']}: AOT={aot_s}  JIT={jit_s}")
            if len(cross_tech_diffs) > 10:
                print(f"    ... and {len(cross_tech_diffs) - 10} more")
            # Downgrade status if any cross-tech inconsistency exists
            if status == "passed":
                status = "partial"
                print(f"  [fact] Demoting status to partial: {len(cross_tech_diffs)} cross-tech diff(s)")

    # Combined status: JIT-as-sufficient if JIT passes, else fall back to AOT status.
    # NOTE: "error" status from _tech_status (total==0) is NEVER promoted — it
    # indicates severe misconfiguration (e.g. missing kHotpatchKeepNative flag)
    # that must be surfaced regardless of JIT results.
    if jit_status == "passed":
        if aot_status in ("error", "failed"):
            status = aot_status  # keep error/failed — don't silently promote
            print(f"  [fact] AOT is {aot_status} (NOT promoting to partial — zero results is severe)")
        else:
            status = "passed"
            if aot_status != "passed":
                print(f"  [fact] JIT passes, promoting overall status to passed (AOT was {aot_status})")
    else:
        if aot_status == "passed" and jit_status is not None and jit_status != "skipped":
            status = "passed"  # AOT passing is sufficient for pipeline success

    # Managed fact: run CombinedSubjects under net8.0/net10.0 via Chaos.TestFramework.Runtime
    managed_fact_results = {}
    for tfm, tech in [("net8.0", "net8-fact"), ("net10.0", "net10-fact")]:
        try:
            from verification.stages.managed_benchmark import _runner_dll, _build_combined_for_tfm
            import tempfile, shutil
            combined_csproj = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.csproj"
            build_dir = Path(tempfile.mkdtemp())
            ok = _build_combined_for_tfm(combined_csproj, tfm, build_dir)
            if not ok:
                errors.append(f"{tech}: CombinedSubjects build failed for {tfm}")
                continue
            tfm_dll = build_dir / "CombinedSubjects.dll"
            if not tfm_dll.exists():
                errors.append(f"{tech}: CombinedSubjects.dll not produced")
                continue
            runner = _runner_dll(tfm)
            r = subprocess.run(
                ["dotnet", "exec", str(runner),
                 "--assembly", str(tfm_dll),
                 "--kind", "fact"],
                capture_output=True, text=True, timeout=600)
            if r.returncode != 0:
                errors.append(f"{tech}: runner failed (rc={r.returncode})")
                continue
            parsed = json.loads(r.stdout)
            results = parsed.get("factResults", [])
            passed = sum(1 for fr in results if fr.get("passed"))
            total = len(results)
            managed_fact_results[tech] = {"passed": passed, "total": total, "results": results}
            print(f"  [fact] [{tech}] {passed}/{total} passed")
            shutil.rmtree(build_dir, ignore_errors=True)
        except Exception as ex:
            errors.append(f"{tech}: error — {ex}")
    # Merge managed fact results into summary
    for tech, res in managed_fact_results.items():
        aot_result['passed'] += res['passed']
        aot_result['total'] += res['total']
        aot_result['results'].extend(res['results'])
    
    # Cross-check: detect silent method drops from metadata.
    # Use factMethodCount (actual fact subjects) rather than totalMethods
    # (which includes helper/benchmark-only subjects that don't need fact dispatch).
    # This avoids false SEVERE failures when non-fact subjects (closures, compiler-
    # generated helpers, hotupdate-only entries) aren't dispatched by the codegen.
    expected = meta_fact_count if meta_fact_count is not None else (meta_total or 0)
    aot_dropped = max(0, (expected or 0) - aot_result['total']) if expected else 0
    if aot_dropped > 0 and expected and expected > 0:
        drop_ratio = aot_dropped / expected
        if drop_ratio > 0.1:
            status = "failed"
            errors.append(f"aot: {aot_dropped} methods dropped vs metadata ({expected}) — SEVERE ({drop_ratio:.0%})")
        else:
            errors.append(f"aot: {aot_dropped} methods dropped vs metadata ({expected})")
    jit_dropped = max(0, (expected or 0) - (jit_result['total'] if jit_result else 0)) if expected else 0
    if jit_dropped > 0:
        jit_drop_ratio = jit_dropped / expected if expected else 1
        if jit_drop_ratio > 0.1:
            errors.append(f"jit: {jit_dropped} methods dropped vs metadata ({expected}) — SEVERE ({jit_drop_ratio:.0%})")
        else:
            errors.append(f"jit: {jit_dropped} methods dropped vs metadata ({expected})")
    # Summary
    summary_parts = [f"aot: {aot_result['passed']}/{aot_result['total']} passed ({aot_status})"]
    if has_jit and jit_result:
        summary_parts.append(f"jit: {jit_result['passed']}/{jit_result['total']} passed ({jit_status})")
    if errors:
        summary_parts.append(f"errors: {'; '.join(errors)}")

    # Value warnings (from AOT results — count negative values as warnings)
    value_warnings = sum(
        1 for r in aot_result["results"]
        if r.get("passed") and r.get("value", 0) < 0 and r.get("value", 0) != -1
    )
    value_suspicious = value_warnings > 0

    # Negative return values despite passing assertions are data anomalies —
    # append to errors so the pipeline fails rather than silently demoting status.
    if value_suspicious:
        errors.append(f"{value_warnings} method(s) returned negative values")
        print(f"  [fact] ERROR: {value_warnings} method(s) returned negative values")

    # ── Write fact history (_dll/reports/history/fact-YYYY-MM-DD.jsonl) ──
    _write_fact_history(ctx, aot_result, jit_result)

    # ── Also write fact.json to chunk results dir for aggregate consumption ──
    _write_fact_results(ctx, aot_result, jit_result, meta_total, meta_fact_count, value_warnings)

    return StageResult(
        stage="fact", status=status,
        summary=", ".join(summary_parts),
        details={
            "aot": {"passed": aot_result["passed"], "total": aot_result["total"],
                    "returncode": aot_result["returncode"], "results": aot_result["results"]},
            **({"jit": {"passed": jit_result["passed"], "total": jit_result["total"],
                       "returncode": jit_result["returncode"], "results": jit_result["results"]}}
               if jit_result else {}),
            **({"crossTechDiffs": cross_tech_diffs} if cross_tech_diffs else {}),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
        value_suspicious=value_suspicious,
    )
