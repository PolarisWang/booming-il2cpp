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


def _return_type_of(subject_id: str | None) -> str | None:
    """Extract the declared return type from a methodSubjectId.

    methodSubjectId format: ``<Assembly>/<Type>::<Method>:<ReturnType>(<params>)``
    e.g. ``System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:System.IO.Compression.ZipArchive(System.String)``
    → ``System.IO.Compression.ZipArchive``

    Returns None when the subject-id is missing/malformed.
    """
    if not subject_id or "::" not in subject_id:
        return None
    after = subject_id.split("::", 1)[1]
    if ":" not in after:
        return None
    ret = after.split(":", 1)[1]
    return ret.split("(", 1)[0].strip() or None


# Return types that carry NO assertable return value: the subject is a smoke
# execution ("did it run without crashing") rather than a semantic check.
#   - System.Void            — nothing to compare
#   - Task / ValueTask       — async void; unwrapped to void by the emitter
_UNASSERTABLE_RETURN_TYPES = frozenset({
    "System.Void",
    "System.Threading.Tasks.Task",
    "System.Threading.Tasks.ValueTask",
})


def classify_fact_record(rec: dict, return_type: str | None) -> str:
    """Classify one runtime fact record into exactly one bucket.

    This is THE single source of truth for real-vs-smoke.  Both the chunk-level
    ``fact.json`` aggregate and the standalone honest_report consume this, so the
    two can never disagree.

    Buckets:
      * ``real``        — value != 42: a genuine assertion produced a real value.
      * ``unassertable``— value == 42 on a method with no return value (void /
                          async-void).  The 42 is structural, not a coverage gap.
      * ``smoke``       — value == 42 on a method that DOES return a value: it
                          should have produced a real assertion but did not.
                          This is the honest coverage gap.
      * ``failed``      — passed == False: a genuine assertion failure.
    """
    if not rec.get("passed"):
        return "failed"
    if rec.get("value") != 42:
        return "real"
    if return_type in _UNASSERTABLE_RETURN_TYPES:
        return "unassertable"
    return "smoke"


def _count_unverified_markers(ctx: ChunkContext) -> int:
    """Scan CombinedSubjects.cs for [UNVERIFIED] markers.

    When an external AOT assembly method has a managed exception that the AOT
    stub cannot replicate, TestEmitter emits a `// [UNVERIFIED] ...` comment
    instead of a real assertion.  These subjects execute the call (smoke test)
    but produce no Assert.* call, so the native runner always counts them as
    "passed" — a false positive.  This function detects them so the fact stage
    can exclude them from the authoritative passed count.

    Returns the count of [UNVERIFIED] markers found.
    """
    combined_cs = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.cs"
    if not combined_cs.exists():
        return 0
    try:
        text = combined_cs.read_text(encoding="utf-8", errors="replace")
        count = text.count("[UNVERIFIED]")
        return count
    except OSError:
        return 0


def _tech_status(tech_result: dict, meta_total: int | None) -> str:
    """Determine status for a single technology result.

    Returns "error" when total==0 so silent failures (e.g. missing
    kHotpatchKeepNative flag on all subjects) are exposed rather than
    silently skipped.
    """
    passed = tech_result["passed"]
    total = tech_result["total"]
    rc = tech_result["returncode"]
    if total == 0:
        return "error"
    if rc == 0 and passed == total:
        return "passed"
    if rc != 0 and passed == total:
        return "partial"  # all parsed passed but non-zero exit: truncated JSON
    # passed < total (any rc): at least one subject genuinely failed or was
    # never dispatched. A real coverage/assertion failure must be fatal.
    return "failed"

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
                        value_warnings: int, unverified_smoke: int = 0) -> None:
    """Write fact.json + fact-results.json to chunk results dir.

    ``fact-results.json`` is the SOURCE OF TRUTH: each per-method runtime record
    is stamped with a ``resultKind`` (real / unassertable / smoke / failed) by
    ``_annotate``, which joins against metadata to get the return type.

    ``fact.json`` is an aggregate DERIVED from the same records, so down-stream
    consumers (gate, honest_report) always see the same numbers.
    """
    chunk_results_dir = ctx.chunk_dir / "results"
    chunk_results_dir.mkdir(parents=True, exist_ok=True)
    fact_path = chunk_results_dir / "fact.json"
    passed = aot_result.get("passed", 0)
    total = aot_result.get("total", 0)
    if jit_result and jit_result.get("total", 0) > 0:
        aot_rate = passed / total if total > 0 else 0.0
        jit_passed = jit_result.get("passed", 0)
        jit_total = jit_result.get("total", 0)
        jit_rate = jit_passed / jit_total if jit_total > 0 else 0.0
        if jit_rate > aot_rate:
            passed = jit_passed
            total = jit_total

    # ── Build the per-method annotated records FIRST (source of truth) ──
    # Each record is stamped with bodyAvailability + returnType + resultKind by
    # _annotate(), so fact.json can be derived from these rather than from the
    # compile-time [UNVERIFIED] marker count (which only sees what ATG chose to
    # label and therefore diverges from the runtime truth).
    avail_by_index: dict[int, str] = {}
    sid_by_index: dict[int, str] = {}
    try:
        md = json.loads(ctx.subjects_metadata_path.read_text(encoding="utf-8"))
        for mm in md.get("methods") or []:
            idx = mm.get("index")
            ba = mm.get("bodyAvailability")
            if idx is not None and ba:
                avail_by_index[int(idx)] = ba
            if idx is not None:
                sid = mm.get("methodSubjectId") or ""
                sid_by_index[int(idx)] = sid
    except (json.JSONDecodeError, OSError):
        avail_by_index = {}
        sid_by_index = {}

    def _annotate(records: list) -> list:
        if not records:
            return records
        for rec in records:
            idx = rec.get("si", rec.get("methodIndex"))
            if idx is not None and int(idx) in avail_by_index:
                rec["bodyAvailability"] = avail_by_index[int(idx)]
            if idx is not None and int(idx) in sid_by_index:
                sid = sid_by_index[int(idx)]
                rt = _return_type_of(sid)
                rec["returnType"] = rt
                rec["resultKind"] = classify_fact_record(rec, rt)
        return records

    per_method = {
        "aot": _annotate([dict(r) for r in (aot_result.get("results", []) or [])]),
    }
    if jit_result and jit_result.get("results"):
        per_method["jit"] = _annotate([dict(r) for r in jit_result["results"]])
    try:
        (chunk_results_dir / "fact-results.json").write_text(
            json.dumps(per_method, ensure_ascii=False), encoding="utf-8")
    except OSError:
        pass  # non-fatal

    # ── Aggregate per-record resultKind (records → fact.json, one truth) ──
    annotated = per_method.get("aot") or []
    real_ct = sum(1 for r in annotated if r.get("resultKind") == "real")
    unassertable_ct = sum(1 for r in annotated if r.get("resultKind") == "unassertable")
    smoke_ct = sum(1 for r in annotated if r.get("resultKind") == "smoke")
    failed_ct = sum(1 for r in annotated if r.get("resultKind") == "failed")

    # Real-signal numerator: records that produced/or would produce a genuine
    # semantic check (a real value, or a genuine failure).  unassertable records
    # stay in the denominator so an all-void chunk cannot claim a free 100%.
    real_signal = real_ct + failed_ct

    fact_data = {
        "passed": passed,
        "total": total,
        # ── Runtime-derived real-vs-smoke (single source of truth) ──
        "realVerified": real_ct,        # value != 42: genuine assertion value
        "unassertable": unassertable_ct,  # void/async-void: 42 is structural
        "smokeUnknown": smoke_ct,       # has a return type but returned 42 → GAP
        "failed": failed_ct,            # passed == False
        # ── Legacy fields (backward compat; now runtime-based, not marker-based) ──
        "realTotal": real_signal,
        "realPassed": real_ct,
        "unverifiedSmoke": unassertable_ct + smoke_ct,
        "unverifiedMarkers": unverified_smoke,  # diagnostic only (compile-time)
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

    # If build was skipped (0 subjects), skip fact too
    build_result = stages.get("build")
    if build_result and (isinstance(build_result, dict) and build_result.get("status") == "skipped"
                         or getattr(build_result, 'status', None) == "skipped"):
        print(f"  [fact] Build was skipped (0 subjects), skipping fact")
        return StageResult(
            stage="fact", status="skipped",
            summary=f"skipped (build produced 0 subjects)",
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
        print(f"  [fact] entry-jit.exe not found, skipping chaos-jit fact")

    # Build metadata reference — None if unavailable (handled by callers)
    meta_total: int | None = None
    meta_fact_count: int | None = None
    meta_path = ctx.chunk_dir / "managed" / "subjects" / "subjects.metadata.json"
    if meta_path.exists():
        try:
            md = json.loads(meta_path.read_text(encoding="utf-8"))
            meta_total = md.get("totalMethods")
            meta_fact_count = md.get("factMethodCount")
            # Distinct fact-kind generatedMethodIds — FALLBACK denominator only.
            # Not used as the authoritative expected when factMethodCount exists:
            # ATG emits one generatedMethodId PER VALUE-SET (Program.cs:433-437),
            # fanning out ~2.2x per real wrapper method, so this over-counts the
            # dispatch obligation (CoreLib system: 4855 GIDs vs 2181 fact methods).
            fact_gids: set[str] = {
                e.get("generatedMethodId") for e in md.get("methods", [])
                if e.get("kind") == "fact" and e.get("generatedMethodId")
            }
            meta_unique_fact = len(fact_gids) if fact_gids else None
        except (json.JSONDecodeError, OSError):
            meta_unique_fact = None
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

    # Cross-check: detect silent method drops from metadata.
    # Use factMethodCount (the per-wrapper-method FACT subjects the codegen/runtime
    # actually dispatches) as the authoritative denominator.  NOT meta_unique_fact:
    # that counts distinct fact-kind `generatedMethodId`s, which ATG emits ONE PER
    # VALUE-SET (Program.cs:433-437 fan-out ~2.2x per method), so it over-counts the
    # dispatch obligation and fabricates a false SEVERE (e.g. CoreLib system chunk:
    # factMethodCount=2181 but unique GIDs=4855).  The generated kSubjectEntryCount
    # equals factMethodCount (healthy numerics: 180==180), never the GID count.
    # Also avoid totalMethods (includes helper/benchmark/hotupdate-only subjects).
    expected = meta_fact_count if meta_fact_count is not None else meta_unique_fact
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
    # ── Scan for [UNVERIFIED] markers (smoke-only subjects) early, so the
    # printed summary can reveal real-vs-smoke rather than masking it.
    # Methods where the AOT stub cannot replicate a managed exception are
    # marked with `// [UNVERIFIED] ...` by TestEmitter.  The native runner
    # counts them as "passed" (no Assert.* throws) but they are NOT semantic
    # verifications.
    unverified_smoke = _count_unverified_markers(ctx)
    if unverified_smoke > 0:
        print(f"  [fact] {unverified_smoke} subject(s) marked [UNVERIFIED] "
              f"(smoke-only — AOT stub cannot replicate managed exception)")

    # Summary. The `X/Y passed` headline must not be taken as "X/Y semantic
    # verifications" when part of the tail is smoke-only: an [UNVERIFIED] stub
    # returns 42 and is counted "passed" by the native runner purely because no
    # Assert.* threw.  Reveal the real count inline so the console read is honest,
    # even though the chunk-level `status` stays nominal (high smoke is legitimate
    # for families without a native C++ body; only genuine assertion mismatches
    # hard-fail — those are already handled by _tech_status when passed < total).
    # _write_fact_results persists the same real/nominal split to fact.json.
    def _render_tech(pt: int, tt: int, tst: str) -> str:
        base = f"{pt}/{tt} passed ({tst})"
        if unverified_smoke > 0:
            real_p = max(0, pt - unverified_smoke)
            real_t = max(0, tt - unverified_smoke)
            return f"{base}; real={real_p}/{real_t} verified, {unverified_smoke} [UNVERIFIED] smoke"
        return base

    summary_parts = [_render_tech(aot_result['passed'], aot_result['total'], aot_status)]
    if has_jit and jit_result:
        summary_parts.append("jit: " + _render_tech(jit_result['passed'], jit_result['total'], jit_status))
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
    _write_fact_results(ctx, aot_result, jit_result, meta_total, meta_fact_count,
                        value_warnings, unverified_smoke)

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
