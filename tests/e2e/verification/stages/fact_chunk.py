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
import re
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


def _parse_fact_stdout(stdout: str) -> tuple[list[dict], bool]:
    """Parse ``--fact-json`` stdout into (results, truncated).

    ``entry.exe`` streams the document — it prints ``{"factResults":[`` then one
    object per subject with an fflush after each — so a process killed mid-run
    (deadlock, abort, watchdog) leaves a *prefix* of a valid array.  A plain
    ``json.loads`` on that prefix fails, which is how a chunk with 448 healthy
    subjects ends up reported as ``0/0 passed``.

    Recover by closing the open array after the last complete object instead of
    discarding everything.  Returns ``([], False)`` when no header is present,
    so genuinely empty/garbage output is never dressed up as partial success.
    """
    header = '{"factResults":['
    start = stdout.find(header)
    if start < 0:
        return [], False

    # Fast path: the document is complete (process exited normally).
    json_end = stdout.rfind("}") + 1
    if json_end > start:
        try:
            parsed = json.loads(stdout[start:json_end])
        except json.JSONDecodeError:
            pass
        else:
            return parsed.get("factResults", []), False

    # Truncated stream: keep every fully-emitted object after the header.
    body = stdout[start + len(header):]
    results: list[dict] = []
    depth = 0
    obj_start = -1
    in_string = False
    escaped = False
    for i, ch in enumerate(body):
        if in_string:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == '"':
                in_string = False
            continue
        if ch == '"':
            in_string = True
        elif ch == "{":
            if depth == 0:
                obj_start = i
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0 and obj_start >= 0:
                try:
                    results.append(json.loads(body[obj_start:i + 1]))
                except json.JSONDecodeError:
                    # A malformed object means the stream is not trustworthy
                    # past this point — stop rather than skip and continue.
                    break
                obj_start = -1
        elif ch == "]" and depth == 0:
            break
    return results, True


def _run_single_fact(exe_path: Path, tech: str) -> dict:
    """Run --fact-json for a single binary, return parsed results dict."""
    print(f"  [fact] [{tech}] Running {exe_path} --fact-json...")
    try:
        r = subprocess.run(
            [str(exe_path), "--fact-json"],
            capture_output=True, timeout=600,
        )
    except subprocess.TimeoutExpired as exc:
        # A deadlocking subject parks the process forever. The streamed output
        # produced before the park is still on stdout and still useful — keep
        # it so one bad subject does not void the whole chunk (see
        # _parse_fact_stdout). ``timeout`` kills the child before we get a
        # CompletedProcess, so read the partial buffers off the exception.
        partial_out = (exc.stdout or b"").decode("utf-8", errors="replace")
        partial_err = (exc.stderr or b"").decode("utf-8", errors="replace")
        fact_results, _ = _parse_fact_stdout(partial_out)
        print(f"  [fact] [{tech}] TIMED OUT after 600s — recovered "
              f"{len(fact_results)} subject result(s) emitted before the hang")
        return {
            "error": "timed_out", "returncode": -1,
            "stdout": partial_out, "stderr": partial_err,
            "passed": sum(1 for fr in fact_results if fr.get("passed")),
            "total": len(fact_results),
            "results": fact_results, "truncated": True,
        }

    stdout = r.stdout.decode("utf-8", errors="replace") if r.stdout else ""
    stderr = r.stderr.decode("utf-8", errors="replace") if r.stderr else ""

    fact_results, json_truncated = _parse_fact_stdout(stdout)

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


# Callees whose contract LEGITIMATELY produces a negative return value, so a
# negative reading is not a data anomaly.  Verified against .NET 8:
#
#   Math/MathF.ILogB    — ECMA-335 specifies FP_ILOGB0 == FP_ILOGBNAN ==
#                         int.MinValue for zero/NaN input (measured: ILogB(0.0)
#                         = -2147483648, ILogB(0.5) = -1).
#   Math/MathF.BitDecrement
#                       — returns the largest value BELOW its input, so any
#                         input <= 0 yields a negative (measured:
#                         BitDecrement(0.0) = -5E-324, a negative subnormal).
#
# Keyed on the callee rather than the value: the probe supplies input-dependent
# results (ILogB(1.0) is 0, ILogB(0.5) is -1), so what is anomalous depends on
# WHICH function ran, not on what it returned.  A blanket "ignore negatives"
# would also swallow the real defect this check was written for — the tagged
# pointer that leaks through an INT64 return slot as a large negative
# (see the reflection line's 0x80007ff68290f0f0 case).
#
# Matched against the generated method id, which is "<MethodName>_<index>_..._<set>"
# (e.g. "ILogB_38_double_0", "BitDecrement_20_float_0"); a leading "_" anchors the
# name so a hypothetical "XILogB" would not match.
_NEGATIVE_RETURN_METHOD_PREFIXES = (
    "ILogB_",
    "BitDecrement_",
)


def _is_legitimately_negative(subject_id: str) -> bool:
    """True when a negative return value is expected for this callee.

    The subject id ends with "<DeclaringType>::<GeneratedMethodId>:<ReturnType>",
    so the generated method id is the segment after the last "::" up to the
    first ":".
    """
    tail = subject_id.rsplit("::", 1)[-1]
    gen_id = tail.split(":", 1)[0]
    return gen_id.startswith(_NEGATIVE_RETURN_METHOD_PREFIXES)


def _generated_method_id_from_subject_id(sid: str) -> str | None:
    """Extract the generatedMethodId from a CombinedSubjects SubjectId.

    The fact runner emits SubjectIds of the form::

        CombinedSubjects/AutoGenerated.Ns.Class::GeneratedMethodId:ReturnType

    while the metadata stores the *original* SubjectId
    (``Assembly/Namespace.Type::Method``) and separately stores
    ``generatedMethodId``.  Extracting the component after ``::``
    and before the next ``:`` gives the id that matches metadata.

    Module-level: classify_fact_record (line ~199) calls this for nullArg
    classification; it was previously nested inside a loader function and
    raised NameError for every fact run.
    """
    if not sid or "::" not in sid:
        return sid
    after = sid.split("::", 1)[1]
    after = after.split(":", 1)[0] if ":" in after else after
    return after.strip() or None


def classify_fact_record(rec: dict, return_type: str | None,
                         is_factory_subject: bool = False,
                         stub_gap_ids: frozenset[str] | None = None,
                         throwing_assert_ids: frozenset[str] | None = None,
                         manifest_prefixes: frozenset[tuple[str, str]] | None = None,
                         subject_id: str | None = None,
                         null_arg_ids: frozenset[str] | None = None,
                         env_sensitive_ids: frozenset[str] | None = None) -> str:
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
      * ``stubGap``     — ATG emitted ``AOT-STUB-GAP`` (no AOT body at all, so
                          the call was never executed).  An implementation
                          backlog item, not a verification gap in the method
                          under test — excluded from the gate denominator the
                          same way factoryGap is.
      * ``factoryGap``  — the dispatch threw before reaching the method: the
                          test's ``SubjectInstanceFactory.Create<T>()`` returned
                          null in AOT (its generic instantiation has no native
                          stub), so the emitted null-guard raised NRE.  The
                          method under test never ran.  This is an
                          infrastructure gap, reported separately so it is
                          neither hidden inside ``failed`` nor confused with a
                          genuine assertion failure.
      * ``realDefect``  — passed == False on a method the *capability manifest*
                          marks as ``real`` (the codegen has a native shape /
                          inline body for it).  A genuine AOT implementation
                          defect: the runtime produced a wrong result for a
                          method it *should* handle.  This is the actionable
                          bug backlog.
      * ``notSupported``— passed == False on a method the *capability manifest*
                          marks as ``not-supported`` (the method has no native
                          shape and no interpreter fallback).  An expected
                          boundary: the AOT runtime correctly cannot implement
                          this API (e.g. Activator.CreateInstanceFrom needs
                          assembly loading).  No action needed unless the
                          status is later upgraded to ``real``.
      * ``failed``      — passed == False with no capability manifest entry.
                          Falls into the same bucket as today, reported as
                          an unclassified failure.

    **Missing metadata**: some fact records come from supplemental-coverage
    methods whose ``si`` does NOT align with any metadata ``index``.  In that
    case ``return_type`` is None and we cannot determine void-ness.  Because
    these are supplementary methods (delegate Invoke, compiler-generated, etc.)
    that ATG could never probe, classifying them as ``smoke`` would create a
    false coverage gap.  They are therefore treated as ``unassertable`` (the
    call executed without crashing, which is as much verification as a subject
    can give for a method ATG cannot reach).
    """
    if not rec.get("passed"):
        # assertFailed is stamped by the runner when the subject's own Assert.*
        # executed and left a non-zero exit code.  A bare `caught` with no
        # assertion failure, on a subject whose body routes through the
        # instance factory, is the factory returning null — not a defect in the
        # method under test.
        if is_factory_subject and not rec.get("assertFailed"):
            return "factoryGap"

        # Capability-manifest-aware classification: if the codegen has a shape
        # for this method, the failure is a genuine implementation defect rather
        # than an expected architectural boundary.
        if _has_codegen_shape(manifest_prefixes, subject_id):
            # nullArg construction (ATG) — see _get_null_arg_subject_ids.
            # A subject that passes only `default(T)` reference args throws
            # ArgumentNullException in the AOT body before any assertion runs.
            # That is correct BCL behavior, not an implementation defect.
            #
            # Match on the RUNNER-FORM methodSubjectId (rec["methodSubjectId"]
            # from entry.exe, "CombinedSubjects/...::<GeneratedId>:...") because
            # null_arg_ids is keyed by generatedMethodId like
            # "CreateInstance_3_System_Type_int_0".  The passed-in `subject_id`
            # is the metadata/BCL form ("...::CreateInstance:...") whose id is
            # only the bare method name — it would never match.
            _rec_sid = rec.get("methodSubjectId") or subject_id
            _rec_gid = _generated_method_id_from_subject_id(_rec_sid)
            if (rec.get("caught") and not rec.get("assertFailed")
                    and null_arg_ids
                    and _rec_gid in null_arg_ids):
                return "nullArg"
            # envSensitive (ATG) — see _get_env_sensitive_subject_ids.
            # GC counters and PRNG stream positions describe the probe's own
            # process; entry.exe cannot reproduce them.  The AOT body is
            # correct, the expectation is not portable.  Applies to records
            # whose assertion ran (assertFailed) as well as pre-assertion
            # raises, so no caught/assertFailed guard here.
            if env_sensitive_ids and _rec_gid in env_sensitive_ids:
                return "envSensitive"
            return "realDefect"

        # ── unimplemented (C2, 2026-09-22) ────────────────────────────────
        # The method has NO codegen shape, so it is not a realDefect (there is
        # no implementation to be defective).  It previously fell through to
        # the bare `failed` bucket, whose stated meaning is "unclassified
        # failure" — which reads as "should work but doesn't" and inflates the
        # defect backlog with methods that were simply never implemented.
        #
        # Measured on the text chunk: 29 records, e.g. System.Text.Ascii::IsValid,
        # Encoding::GetMaxByteCount, DecoderFallbackBuffer::MovePrevious.  All of
        # them (a) have zero entries in aot-capability-manifest.json and (b) hit
        # the runtime catch-all at execution time, whose own warning says
        # "no native body; delegates to Phase 1/2, else returns 0" — the 0 is
        # then compared against the probe's expected value and the assertion
        # fails.  They are an implementation backlog, not verification failures.
        #
        # Guarded on BOTH conditions on purpose: manifest-absence alone would
        # relabel genuine defects on methods whose shape lives in a path the
        # manifest does not enumerate, and that would be exactly the
        # "turn red into green by relabelling" failure this project forbids.
        # `assertFailed` must be True for the same reason — a caught-without-
        # assertion raise is a different story (factoryGap / nullArg /
        # preAssertionRaise), handled above.
        if rec.get("assertFailed") and rec.get("value") == 0:
            # The assertion ran and compared the catch-all's 0 against the
            # expected value.  `value == 0` is the independent corroboration
            # that an unimplemented fallback produced the result: the runner
            # only records 0 here when the subject returned the fallback's
            # default, and a genuinely-defective native implementation would
            # have to coincidentally return exactly 0 as well.  Combined with
            # shape-absence this is unambiguous enough to bucket honestly,
            # while a non-zero wrong value still falls through to `failed`
            # (a real implementation that answers incorrectly is a defect).
            return "unimplemented"

        return "failed"
    if rec.get("value") != 42:
        return "real"
    # The runner now stamps assertFailed=true when the subject's own Assert.*
    # executed and left a non-zero exit code (the AOT body returned a different
    # value than the managed probe expected).  Such a record is a genuine
    # verification failure, not a smoke gap — report it as "failed".
    if rec.get("assertFailed"):
        return "failed"

    # P0-B (json-xml-production-readiness): if this record's generatedMethodId
    # carries an AOT-STUB-GAP marker from the ATG, it is a known stub gap
    # rather than a smoke gap.  Bucket as stubGap so the gate does not penalise
    # methods that the toolchain positively knows have no AOT body.
    if stub_gap_ids is not None and not stub_gap_ids.isdisjoint(
            _gen_method_ids(rec)):
        return "stubGap"

    if return_type is None:
        # No metadata = supplemental-coverage method ATG never probed.
        # We have no way to decide void vs non-void — treat conservatively
        # as unassertable so the gate doesn't report a phantom gap.
        return "unassertable"
    if return_type in _UNASSERTABLE_RETURN_TYPES:
        # A void-returning method has no value to compare, so the structural 42
        # normally means "ran without crashing".  But when the subject wrapped
        # the call in a typed try/catch, it DID verify a contract — the call
        # raises exactly the exception type the managed probe observed, and
        # raises nothing else.  That verification is invisible in the record
        # (nothing is returned, nothing is asserted against a value).
        #
        # The ATG marks those sites with AOT-THROWS-ASSERT so they can be
        # counted as `real`.  Measured on the xml chunk: 121 records moved from
        # unassertable to real, i.e. verified coverage had been under-reported
        # by ~2x.
        #
        # `passed` is required, not implied.  Reaching this branch means
        # assertFailed was already False, and a subject that never caught
        # anything returns normally with the structural 42 — but being explicit
        # keeps this rule from silently depending on the order of the checks
        # above it.  A subject that DID fail cannot be counted as verified even
        # if it carries the marker (the marker says an assertion was emitted,
        # not that it held).
        if (rec.get("passed")
                and throwing_assert_ids is not None
                and not throwing_assert_ids.isdisjoint(_gen_method_ids(rec))):
            return "real"
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


_CODEGEN_FAILURE_RE = re.compile(
    r'extern\s+"C"\s+const\s+int\s+kCodegenFailureCount\s*=\s*(\d+)\s*;')


def _read_codegen_failure_count(ctx: ChunkContext) -> int:
    """Read `kCodegenFailureCount` from the chunk's generated AOT source.

    The generator emits ``extern "C" const int kCodegenFailureCount = N;``
    (NativeAotLoweringPlanner.Methods.cs:1326) whenever it had to replace a
    method body with a stub.  The symbol is a compile-time constant in the
    generated TU, so the value is readable from source without building — and
    reading source avoids racing a binary built from a different revision.

    The declaration is emitted ONLY when the count is > 0, so an absent symbol
    means zero failures (not "unknown").  We still return 0 rather than None in
    that case so the caller's comparison is a plain int compare.

    Returns the count, or 0 if the symbol/file is absent.
    """
    subjects_dir = ctx.chunk_dir / "native" / "subjects"
    if not subjects_dir.is_dir():
        return 0
    total = 0
    try:
        # Match both native-aot.generated.cpp and generated.page2/3/...cpp
        files = sorted(subjects_dir.glob("native-aot.generated*.cpp"))
    except OSError:
        return 0
    for path in files:
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        match = _CODEGEN_FAILURE_RE.search(text)
        if match:
            total += int(match.group(1))
    return total


def _codegen_failure_tolerance(ctx: ChunkContext) -> int:
    """Per-chunk tolerance for kCodegenFailureCount; default 0 (block on any).

    Default 0 is deliberate: a silently stubbed method is a correctness hazard
    (it returns a default where real behavior was expected), so the safe default
    is to block.  A family that legitimately has unsupported methods raises this
    in its chunk config explicitly — making the allowance a visible decision
    rather than a weakened global threshold.
    """
    config = _load_chunk_config(ctx.chunk_dir)
    value = config.get("codegenFailureTolerance")
    if isinstance(value, bool):   # bool is an int subclass; reject explicitly
        return 0
    if isinstance(value, int) and value >= 0:
        return value
    return 0
def _gen_method_ids(rec: dict) -> list[str]:
    """Extract the generatedMethodId(s) from a fact record.

    The record key varies between run-epochs — prefer the runtime-stamped
    ``generatedMethodId``, then fall back to ``methodSubjectId``.
    """
    raw = rec.get("generatedMethodId") or rec.get("methodSubjectId") or ""
    return [raw] if raw else []


def _stub_gap_method_ids(ctx: ChunkContext) -> frozenset[str]:
    """Scan CombinedSubjects.cs for the machine-readable ``// AOT-STUB-GAP`` marker.

    P0-B (json-xml-production-readiness): TestEmitter now emits ``// AOT-STUB-GAP``
    immediately before every ``[UNVERIFIED]`` external-assembly stub comment.
    This function scans CombinedSubjects.cs for the marker and records the
    **methodSubjectId** of the owning method from the proximity context.

    The generated code is shaped like::

        [Fact]
        [HotUpdate]
        public long SomeMethod_0()
        {
            // AOT-STUB-GAP
            // [UNVERIFIED] AOT stub: ...
            return 42L;
        }

    We walk backward from the marker line to find the nearest ``public long``
    method signature, then reconstruct the canonical CombinedSubjects subject ID
    from the class context and method name.

    Returns a frozenset of methodSubjectId strings carrying the marker.
    """
    combined_cs = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.cs"
    if not combined_cs.exists():
        return frozenset()
    try:
        text = combined_cs.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return frozenset()

    if "AOT-STUB-GAP" not in text:
        return frozenset()

    lines = text.split("\n")
    gap_ids: set[str] = set()

    # Walk each marker line, backward-search to the containing method.
    for i, line in enumerate(lines):
        if "AOT-STUB-GAP" not in line:
            continue
        # Scan backward from line i to find the method signature.
        for j in range(i - 1, -1, -1):
            m = re.search(r'public long (\w+)\(\)', lines[j])
            if m:
                method_name = m.group(1)
                # Scan further back for the enclosing class and namespace.
                # Take the FIRST (nearest) class hit, then the namespace.
                ns_name = ""
                class_name = ""
                for k in range(j - 1, -1, -1):
                    if not class_name:
                        cm = re.search(r'\bclass\s+(\w+)', lines[k])
                        if cm:
                            class_name = cm.group(1)
                            continue
                    nm = re.search(r'\bnamespace\s+(\S+)', lines[k])
                    if nm:
                        ns_name = nm.group(1)
                        break
                subject_id = (
                    "CombinedSubjects/"
                    f"{ns_name}.{class_name}::{method_name}:System.Int64()"
                )
                gap_ids.add(subject_id)
                break

    return frozenset(gap_ids)


def _throwing_assert_method_ids(ctx: ChunkContext) -> frozenset[str]:
    """Scan CombinedSubjects.cs for the machine-readable ``// AOT-THROWS-ASSERT`` marker.

    TestEmitter emits this immediately before every ``Assert.Throws`` /
    ``Assert.Throws<T>`` it generates.  Those subjects verify a real contract —
    "calling this with these arguments raises exactly this exception type" —
    but leave no trace in the fact record: the subject's own return value is
    the structural 42, and the method UNDER TEST returns void, so the recorded
    ``returnType`` is ``System.Void``.

    Without this marker such a subject is indistinguishable from one that
    asserted nothing, and ``classify_fact_record`` dumps it into
    ``unassertable``.  Measured on the xml chunk: 119 of 171 ``unassertable``
    records carried a try/catch and all passed — the pipeline was
    under-reporting verified coverage by ~90%.

    Resolution mirrors ``_stub_gap_method_ids``: walk backward from the marker
    to the containing method signature, then further back for class/namespace.

    Returns a frozenset of methodSubjectId strings carrying the marker.
    """
    combined_cs = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.cs"
    if not combined_cs.exists():
        return frozenset()
    try:
        text = combined_cs.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return frozenset()

    if "AOT-THROWS-ASSERT" not in text:
        return frozenset()

    lines = text.split("\n")
    ids: set[str] = set()

    for i, line in enumerate(lines):
        if "AOT-THROWS-ASSERT" not in line:
            continue
        for j in range(i - 1, -1, -1):
            m = re.search(r'public long (\w+)\(\)', lines[j])
            if not m:
                continue
            method_name = m.group(1)
            ns_name = ""
            class_name = ""
            for k in range(j - 1, -1, -1):
                if not class_name:
                    cm = re.search(r'\bclass\s+(\w+)', lines[k])
                    if cm:
                        class_name = cm.group(1)
                        continue
                nm = re.search(r'\bnamespace\s+(\S+)', lines[k])
                if nm:
                    ns_name = nm.group(1)
                    break
            ids.add(
                "CombinedSubjects/"
                f"{ns_name}.{class_name}::{method_name}:System.Int64()"
            )
            break

    return frozenset(ids)


def _load_capability_manifest(ctx) -> frozenset[tuple[str, str]] | None:
    """Load the AOT capability manifest emitted by the codegen layer.

    The manifest records which managed methods the codegen can dispatch
    natively (SimpleForward / InlineBody shape) versus those that have no
    shape at all.  It lets a failing fact record be reported as a genuine
    implementation defect (``realDefect``) instead of a generic ``failed``.

    Returns a set of ``("Namespace.Type::Method", "(Param1,Param2)")``
    tuples, or None when no manifest exists.

    Why a tuple and not a string prefix: a prefix ending at the method name
    over-claims (``Activator::CreateInstance`` would match its 5-arg
    overload, which has no shape), while a prefix that also embeds the
    signature does not line up with the metadata's
    ``…::Method:ReturnType(Params)`` form.  Comparing the method's identity
    and its parameter list as separate fields is exact for both.
    """
    cap_path = (ctx.chunk_dir / "native" / "codegen" / "generated"
                / "aot-capability-manifest.json")
    if not cap_path.exists():
        return None

    try:
        manifest = json.loads(cap_path.read_text(encoding="utf-8-sig"))
    except (json.JSONDecodeError, OSError):
        return None

    keys: set[tuple[str, str]] = set()
    for entry in manifest.get("entries", []) or []:
        if entry.get("kind") != "exact":
            continue
        type_name = entry.get("typeDisplayName")
        method_name = entry.get("methodName")
        params = entry.get("paramTypes")
        if not type_name or not method_name or params is None:
            continue
        keys.add((f"{type_name}::{method_name}", ",".join(params)))
    return frozenset(keys) if keys else None


def _has_codegen_shape(manifest_keys: frozenset[tuple[str, str]] | None,
                       subject_id: str | None) -> bool:
    """True when *subject_id* names a method the codegen has a shape for.

    ``subject_id`` has the metadata shape
    ``Assembly/Namespace.Type::Method:ReturnType(Param1,Param2)``; the
    manifest keys are ``("Namespace.Type::Method", "Param1,Param2")``.  Both
    halves must match — the method identity *and* the exact overload.
    """
    if not manifest_keys or not subject_id:
        return False

    after_slash = subject_id.split("/", 1)[1] if "/" in subject_id else subject_id
    if "::" not in after_slash:
        return False
    type_and_method, _, rest = after_slash.partition("::")
    if ":" not in rest:
        return False
    method_name, _, signature = rest.partition(":")
    # signature == "ReturnType(Param1,Param2)"
    if "(" not in signature or not signature.endswith(")"):
        return False
    params = signature[signature.index("(") + 1:-1]

    return (f"{type_and_method}::{method_name}", params) in manifest_keys


def _get_factory_subject_ids(ctx: ChunkContext) -> frozenset[str]:
    """Scan CombinedSubjects.cs for methods using SubjectInstanceFactory.Create<T>().

    Tests of the form ``SubjectInstanceFactory.Create<T>().Method()`` dispatch
    through a factory that, in AOT mode, may return null when the generic
    instantiation of ``Create<T>`` has no native stub.  When the factory
    returns null, the subsequent null-guard raises NullReferenceException
    and the dispatch records ``caught=true`` — a failure of the *infrastructure*
    rather than of the method under test.

    Returns a frozenset of ``generatedMethodId`` values whose test body
    calls ``SubjectInstanceFactory.Create<``.
    """
    combined_cs = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.cs"
    if not combined_cs.exists():
        return frozenset()
    try:
        text = combined_cs.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return frozenset()

    factory_ids: set[str] = set()
    for m in re.finditer(r'public long (\w+)\(\)\s*\n\s*\{', text, re.MULTILINE):
        gid = m.group(1)
        body_start = m.end()
        # Body runs to the next method declaration (or end of file).
        next_method = re.search(r'public (?:long|static)\s', text[body_start:])
        body_end = body_start + (next_method.start() if next_method else len(text) - body_start)
        if 'SubjectInstanceFactory.Create<' in text[body_start:body_end]:
            factory_ids.add(gid)
    return frozenset(factory_ids)


def _get_null_arg_subject_ids(ctx: ChunkContext) -> frozenset[str]:
    """Scan CombinedSubjects.cs for [Fact] bodies that only pass defaults.

    ATG synthesises argument values per overload, and for reference-typed
    parameters the first value set is ``default(T)`` — i.e. ``null``.  A test
    body such as::

        var result = global::System.Array.CreateInstance(default(System.Type)!, 0);

    passes ``null`` where the real BCL throws ``ArgumentNullException``.  The
    AOT body faithfully performs the null check and raises, so the record is
    ``caught=true, assertFailed=false`` — indistinguishable at the fact level
    from a genuine runtime defect.

    It is NOT a defect in the method under test: .NET Core throws
    ``ArgumentNullException`` for exactly these calls (verified against
    net8.0 for Array.CreateInstance / Enum.Format / Activator.CreateInstance).
    Bucketing them as ``realDefect`` sends every such subject to the bug
    backlog as a false positive.

    Returns a frozenset of ``generatedMethodId`` values whose [Fact] body
    passes a null default for a *reference-typed* parameter.

    Only reference types count.  ``default(double)`` / ``default(int)`` are
    legal values that genuinely exercise the method (``Math.Cos(0.0)`` must
    return 1.0), so treating those as null-arg would suppress real defects.
    ATG marks the null-suppressed reference defaults with the null-forgiving
    ``!`` operator — ``default(System.Type)!``, ``default(string)!`` — which
    is what this matches.
    """
    combined_cs = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.cs"
    if not combined_cs.exists():
        return frozenset()
    try:
        text = combined_cs.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return frozenset()

    # `default(<RefType>)!` — the `!` is emitted only for reference-typed
    # defaults that are deliberately null (see ATG null-forgiving emission).
    null_default = re.compile(r'default\(\s*(?:global::)?[A-Za-z_][\w.]*\s*\)\s*!')
    # Enum.Parse/Enum.TryParse with an empty name string.  ATG also emits "" for
    # a string parameter's value set, and for Enum.Parse an empty string is as
    # invalid as null: .NET throws ArgumentException("Must specify valid
    # information for parsing in the string", verified on net8.0), which the
    # AOT body reproduces via RaiseArgumentException.  The record is
    # caught-before-assert with no assertion failure — the null-arg shape.
    #
    # Scoped to Enum.Parse/TryParse on purpose.  A bare `""` appears in hundreds
    # of unrelated subjects (Activator.CreateInstance("",""), CreateInstanceFrom,
    # …) that legitimately succeed or fail for other reasons; treating an empty
    # string as broadly invalid would sweep real defects into this bucket.
    enum_empty_name = re.compile(
        r'global::System\.Enum\.(?:Parse|TryParse)\s*\(([^;]*?)\)\s*;', re.DOTALL)

    ids: set[str] = set()
    for m in re.finditer(r'public long (\w+)\(\)\s*\n\s*\{', text, re.MULTILINE):
        gid = m.group(1)
        body_start = m.end()
        next_method = re.search(r'public (?:long|static)\s', text[body_start:])
        body_end = body_start + (next_method.start() if next_method else len(text) - body_start)
        body = text[body_start:body_end]

        has_null_default = bool(null_default.search(body))
        enum_empty = any('""' in call.group(1)
                         for call in enum_empty_name.finditer(body))
        if not has_null_default and not enum_empty:
            continue
        # Require the call to use ONLY null/empty literals for its reference
        # args — a NON-empty quoted string or a `new` expression means ATG
        # injected a real value that may genuinely exercise the defect.
        call_args = re.search(r'= [^\n]*?\((.*)\);', body, re.DOTALL)
        args = call_args.group(1) if call_args else body
        has_real_ref = bool(re.search(r'"[^"]+"', args)) or "new " in args
        if not has_real_ref:
            ids.add(gid)
    return frozenset(ids)


def _get_env_sensitive_subject_ids(ctx: ChunkContext) -> frozenset[str]:
    """Scan CombinedSubjects.cs for assertions on process-environment snapshots.

    ATG derives each expected value by RUNNING the probe in its own process and
    recording what came back.  That is sound for pure functions, but wrong for
    methods whose result is a property of the running process:

      * ``GC.CollectionCount`` / ``GC.GetTotalMemory`` /
        ``GC.GetAllocatedBytesForCurrentThread`` / ``GC.GetTotalAllocatedBytes``
        return counters that describe *this* process's heap.  The probe baked
        in its own numbers (14 collections, 178064 bytes, …); entry.exe has a
        different GC history, so the values cannot match by construction.
      * ``Random.NextDouble`` / ``NextSingle`` / ``Next*`` return the next value
        of a PRNG stream.  The AOT runtime seeds its own xorshift generator, so
        the probe's sequence (0.490344…, 0.6086793…) is unreproducible.
      * ``Thread.GetCurrentProcessorId`` / ``Environment.ProcessorCount`` /
        ``Environment.TickCount*`` report a property of the *host* the probe
        ran on.  The probe baked in its own machine's numbers (e.g. 6 logical
        CPUs); a 24-core host cannot reproduce them, and neither can a CI
        runner with yet another shape.

    None is a defect in the method under test: the AOT implementation is
    correct, the *expectation* is not portable.  Bucketing them as
    ``realDefect`` sends them to the bug backlog forever.

    Detection is by return-type-independent body shape: the call is to a
    System.GC snapshot accessor, a System.Random sampling accessor, or a
    host-environment accessor, and the Assert compares against a literal that
    ATG captured.  Deliberately narrow — GC.CollectionCount documented as
    ">= 0" style properties, and Random methods that take an explicit seed,
    are NOT matched here.

    Returns a frozenset of ``generatedMethodId`` values.
    """
    combined_cs = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.cs"
    if not combined_cs.exists():
        return frozenset()
    try:
        text = combined_cs.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return frozenset()

    # Process-environment accessors whose value is not a function of the inputs.
    env_call = re.compile(
        r'global::System\.GC\.(?:CollectionCount|GetTotalMemory|'
        r'GetAllocatedBytesForCurrentThread|GetTotalAllocatedBytes)\s*\('
        r'|'
        r'\.Next(?:Double|Single|Int64|Bytes)\s*\('
        r'|'
        # Host property, not a function of the inputs: processor count, the
        # calling thread's processor id, and the tick counters all describe the
        # machine/process the probe ran on.  Measured case: a
        # `Thread.GetCurrentProcessorId_60__0` subject asserting AreEqual(6, …)
        # because the probe host had 6 logical CPUs — unreproducible on any
        # other host, and equally unreproducible by the AOT runtime which has no
        # native body for the accessor at all.
        r'\.GetCurrentProcessorId\s*\('
        r'|'
        r'(?:global::System\.)?Environment\.ProcessorCount\b'
        r'|'
        r'(?:global::System\.)?Environment\.TickCount(?:64)?\b'
    )

    ids: set[str] = set()
    for m in re.finditer(r'public long (\w+)\(\)\s*\n\s*\{', text, re.MULTILINE):
        gid = m.group(1)
        body_start = m.end()
        next_method = re.search(r'public (?:long|static)\s', text[body_start:])
        body_end = body_start + (next_method.start() if next_method else len(text) - body_start)
        body = text[body_start:body_end]
        if not env_call.search(body):
            continue
        # Must actually assert against a captured literal — a body that only
        # calls the accessor without comparing is not evidence of a bad
        # expectation.
        if "Assert.AreEqual(" in body:
            ids.add(gid)
    return frozenset(ids)


# Kind pairs whose PASS/FAIL verdicts are NOT comparable across technologies.
#
#   unassertable  ↔  factoryGap
#
# `unassertable` means the method RAN but produced no assertable value
# (value==42 is structural — void/async-void, or a smoke body that just returns
# 42).  `factoryGap` means the method NEVER RAN: the test's
# SubjectInstanceFactory.Create<T>() could not construct the subject, so the
# emitted null-guard raised before the call.  Neither verdict is a conclusion
# about the method's behaviour, so two of them cannot contradict each other —
# reporting the mismatch as a cross-tech inconsistency is noise.
#
# Why the two technologies legitimately differ here: AOT marks subject methods
# `kHotpatchKeepNative` so the native body runs, while JIT leaves the entry flags
# at 0 and routes the method through the interpreter — that IS JIT mode's
# purpose (see hotpatch_dispatch.h, "Dispatch priority").  For a body reaching
# an unimplemented external-runtime helper, the interpreter raises where the
# native path returns the structural 42.  Measured on the threading-tasks chunk:
# AOT carries 605 `kHotpatchKeepNative` entries, JIT 0 across all 1061 entries,
# yet only 3 subjects diverge — those whose bodies reach such a helper.
#
# Every other kind combination (real / failed / realDefect / stubGap / smoke /
# nullArg / envSensitive / notSupported) is compared strictly: a real assertion
# disagreeing across technologies is exactly the signal this diff exists for.
_NON_COMPARABLE_KIND_PAIRS = frozenset({
    frozenset({"unassertable", "factoryGap"}),
})


# Result kinds that represent an INFRASTRUCTURE gap rather than a measurement
# of the method under test:
#
#   factoryGap    — the test's SubjectInstanceFactory.Create<T>() returned null
#                   in AOT, so the emitted null-guard raised before the call.
#   stubGap       — ATG marked the subject AOT-STUB-GAP: there is no AOT body,
#                   so the call never executed.
#   nullArg       — the subject passes default(T) reference args and the AOT
#                   body correctly throws ArgumentNullException before asserting.
#   envSensitive  — the expectation describes the probe's own process (GC
#                   counters, PRNG stream position) and cannot be reproduced.
#
# None of these say anything about whether the method is implemented correctly,
# so they are excluded from BOTH the gate numerator and the gate denominator.
_EXCLUDED_FROM_GATE = frozenset({
    "factoryGap",
    "stubGap",
    # C2 (2026-09-22): no codegen shape AND the runtime catch-all produced the
    # value.  An implementation backlog item, exactly like stubGap — including
    # it in the denominator would penalise the gate for work that was never
    # done, which is the same reasoning stubGap already follows.
    "unimplemented",
    "nullArg",
    "envSensitive",
})


def compute_gate_counts(annotated: list[dict]) -> tuple[int, int]:
    """Return (gate_total, gate_passed) for a set of annotated fact records.

    The numerator and the denominator MUST describe the same population.
    ``gate_total`` removes the infrastructure-gap kinds above; ``gate_passed``
    must therefore count only PASSING records drawn from that same remainder.

    The previous implementation used the raw run-wide pass count as the
    numerator while subtracting those kinds from the denominator, so the two
    described different populations and the ratio could exceed 100%.  Measured
    on the reflection chunk: gateTotal 50, gatePassed 440 (880%) — 390 of those
    passes were stubGap records the denominator had already excluded, i.e.
    methods that never executed were being counted as verified.
    """
    gate_total = sum(1 for r in annotated if r.get("resultKind") not in _EXCLUDED_FROM_GATE)
    gate_passed = sum(
        1 for r in annotated
        if r.get("passed") and r.get("resultKind") not in _EXCLUDED_FROM_GATE)
    return gate_total, gate_passed


def split_cross_tech_diffs(
    aot_results: list[dict],
    jit_results: list[dict],
) -> tuple[list[dict], list[dict]]:
    """Split AOT/JIT verdict mismatches into (real_diffs, excluded).

    A mismatch is a *real* cross-tech diff when the two technologies disagree
    on something that is actually a conclusion about the method.  It is
    *excluded* when the two verdicts come from a non-comparable kind pair (see
    ``_NON_COMPARABLE_KIND_PAIRS``) — i.e. neither side asserted anything, so
    there is nothing to contradict.

    Records without a ``methodSubjectId`` fall back to a positional key, which
    is why the enumerations are built with an index default.
    """
    aot_by_id = {r.get("methodSubjectId", f"idx_{i}"): r
                 for i, r in enumerate(aot_results)}
    jit_by_id = {r.get("methodSubjectId", f"idx_{i}"): r
                 for i, r in enumerate(jit_results)}
    diffs: list[dict] = []
    excluded: list[dict] = []
    for mid in sorted(set(aot_by_id) | set(jit_by_id)):
        aot_rec = aot_by_id.get(mid, {})
        jit_rec = jit_by_id.get(mid, {})
        aot_pass = aot_rec.get("passed", False)
        jit_pass = jit_rec.get("passed", False)
        if aot_pass == jit_pass:
            continue
        aot_kind = aot_rec.get("resultKind")
        jit_kind = jit_rec.get("resultKind")
        entry = {
            "methodSubjectId": mid,
            "aotPassed": aot_pass,
            "jitPassed": jit_pass,
            "aotKind": aot_kind,
            "jitKind": jit_kind,
        }
        if frozenset({aot_kind, jit_kind}) in _NON_COMPARABLE_KIND_PAIRS:
            entry["reason"] = ("neither verdict is an assertion conclusion "
                               "(method did not run, or ran without asserting)")
            excluded.append(entry)
        else:
            diffs.append(entry)
    return diffs, excluded


def _tech_status(tech_result: dict, meta_total: int | None) -> str:
    """Determine status for a single technology result.

    Returns "error" when total==0 so silent failures (e.g. missing
    kHotpatchKeepNative flag on all subjects) are exposed rather than
    silently skipped.

    ``meta_total`` is the subject count the run was supposed to produce.  When
    a result carries far fewer than that, the run was cut short (deadlock
    killed mid-stream — see _parse_fact_stdout) and the missing subjects were
    never evaluated.  That must NOT surface as "partial": the pipeline promotes
    ``partial`` to ``passed`` whenever JIT succeeds, so a chunk that evaluated
    8 of 258 subjects would go green.  Report "error" instead — a truncated run
    is a hard stop, not a soft degradation.
    """
    passed = tech_result["passed"]
    total = tech_result["total"]
    rc = tech_result["returncode"]
    if total == 0:
        return "error"
    # A run that stopped early (timed out / killed) yields a prefix of the real
    # subject set. Guard on the truncation flag rather than on a count ratio:
    # only the timeout path sets it, so a legitimately smaller-but-complete run
    # is unaffected.
    if tech_result.get("truncated") and meta_total is not None and total < meta_total:
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


def select_reference_counts(aot_result: dict,
                            jit_result: dict | None) -> tuple[int, int, int, int]:
    """Pick the (passed, total, jit_passed, jit_total) figure set for fact.json.

    AOT is ALWAYS the reference population.  Every bucket in the aggregate
    (real / unassertable / smoke / factoryGap / failed) is computed from the AOT
    annotated records, so ``passed``/``total`` must come from the same run or the
    numerator and denominator describe two different executions.

    This deliberately does NOT swap in the JIT counts when the JIT pass-rate is
    higher.  Doing so mixed a JIT numerator with an AOT-derived factory-gap
    subtraction and produced the impossible ``gatePassed > gateTotal``
    (observed 520 > 519 on the threading chunk).  It was also optimistic in
    precisely the case that carries the most signal: AOT failing where JIT
    passes, which is a real AOT lowering defect and the thing this stage exists
    to surface.

    JIT numbers are returned alongside rather than discarded, so callers can
    report them separately (the cross-tech diff is the actionable form).
    """
    passed = aot_result.get("passed", 0) or 0
    total = aot_result.get("total", 0) or 0
    jit_passed = (jit_result.get("passed", 0) or 0) if jit_result else 0
    jit_total = (jit_result.get("total", 0) or 0) if jit_result else 0
    return passed, total, jit_passed, jit_total


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
    # ── Reference population for every aggregate below is AOT ──────────────
    # See select_reference_counts() for why the JIT counts are NOT substituted
    # in.  They are carried through for separate reporting only.
    passed, total, jit_passed, jit_total = select_reference_counts(aot_result, jit_result)

    # ── Build the per-method annotated records FIRST (source of truth) ──
    # Each record is stamped with bodyAvailability + returnType + resultKind by
    # _annotate(), so fact.json can be derived from these rather than from the
    # compile-time [UNVERIFIED] marker count (which only sees what ATG chose to
    # label and therefore diverges from the runtime truth).
    avail_by_index: dict[int, str] = {}
    sid_by_index: dict[int, str] = {}
    meta_by_genid: dict[str, dict] = {}
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
            # generatedMethodId-keyed lookup — stable under any reordering.
            gid = mm.get("generatedMethodId")
            if gid:
                meta_by_genid[gid] = mm
    except (json.JSONDecodeError, OSError):
        avail_by_index = {}
        sid_by_index = {}
        meta_by_genid = {}

    # Factory-gap detection: pre-scan CombinedSubjects.cs for methods whose
    # test body calls SubjectInstanceFactory.Create<T>(), then feed that
    # signal to classify_fact_record so caught-from-factory entries are
    # reported in their own bucket rather than hidden inside "failed".
    factory_subjects = _get_factory_subject_ids(ctx)

    # Stub-gap detection (P0-B): pre-scan CombinedSubjects.cs for the
    # `// AOT-STUB-GAP` marker TestEmitter writes above every [UNVERIFIED]
    # external-assembly stub.  Those methods have no AOT body by construction,
    # so they are bucketed separately from `smoke` — see classify_fact_record.
    stub_gap_ids = _stub_gap_method_ids(ctx)
    throwing_assert_ids = _throwing_assert_method_ids(ctx)

    # Capability manifest (AOT shape registry) — the set of subject-id prefixes
    # the codegen can dispatch natively.  A failing record whose subject id is
    # covered is a realDefect (actionable bug); one that is not is an expected
    # architectural boundary (notSupported).
    manifest_prefixes = _load_capability_manifest(ctx)

    # Null-argument subjects: ATG's first value set for a reference parameter
    # is default(T) (= null).  Those calls throw ArgumentNullException in the
    # real BCL, so a caught-before-assert record on such a subject is correct
    # behavior, not an AOT defect.  See _get_null_arg_subject_ids.
    null_arg_ids = _get_null_arg_subject_ids(ctx)

    # Environment-snapshot subjects: ATG captured GC counters / PRNG stream
    # positions from its own probe process; entry.exe cannot reproduce them.
    # See _get_env_sensitive_subject_ids.
    env_sensitive_ids = _get_env_sensitive_subject_ids(ctx)

    def _annotate(records: list) -> list:
        if not records:
            return records
        for rec in records:
            # Preferred path: match by generatedMethodId extracted from the
            # CombinedSubjects SubjectId (see meta_by_genid note above).
            rec_sid = rec.get("methodSubjectId")
            gen_id = _generated_method_id_from_subject_id(rec_sid) if rec_sid else None
            mm = meta_by_genid.get(gen_id) if gen_id else None
            if mm is not None:
                # Rewrite methodIndex from the kMethodTable index space to the
                # metadata index space.  The runtime reports the index it
                # dispatched through (a kMethodTable slot); downstream tools all
                # treat "methodIndex" as an index into subjects.metadata.json.
                # Those two orderings differ, so the raw value silently resolved
                # a *different* method (502/586 records on System.Net.Sockets).
                # Once the record is identified by generatedMethodId we know the
                # true metadata row, and its index is the value consumers expect.
                # Nothing reads methodIndex back out to address the native
                # dispatch table — that stays inside entry.exe via
                # kSubjectSlotMap — so rewriting it here is lossless.
                meta_idx = mm.get("index")
                if meta_idx is not None:
                    rec["methodIndex"] = meta_idx
                ba = mm.get("bodyAvailability")
                if ba:
                    rec["bodyAvailability"] = ba
                rt = _return_type_of(mm.get("methodSubjectId", ""))
                rec["returnType"] = rt
                rec["resultKind"] = classify_fact_record(
                    rec, rt,
                    is_factory_subject=(gen_id in factory_subjects) if gen_id else False,
                    stub_gap_ids=stub_gap_ids,
                    throwing_assert_ids=throwing_assert_ids,
                    manifest_prefixes=manifest_prefixes,
                    subject_id=mm.get("methodSubjectId"),
                    null_arg_ids=null_arg_ids,
                    env_sensitive_ids=env_sensitive_ids)
                continue

            # Fallback for records without methodSubjectId (pre-rebuild).
            idx = rec.get("si", rec.get("methodIndex"))
            if idx is not None and int(idx) in avail_by_index:
                rec["bodyAvailability"] = avail_by_index[int(idx)]
            if idx is not None and int(idx) in sid_by_index:
                sid = sid_by_index[int(idx)]
                rt = _return_type_of(sid)
                rec["returnType"] = rt
                rec["resultKind"] = classify_fact_record(
                    rec, rt, stub_gap_ids=stub_gap_ids, throwing_assert_ids=throwing_assert_ids,
                    manifest_prefixes=manifest_prefixes,
                    subject_id=sid)
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
    factory_gap_ct = sum(1 for r in annotated if r.get("resultKind") == "factoryGap")
    stub_gap_ct = sum(1 for r in annotated if r.get("resultKind") == "stubGap")
    unimplemented_ct = sum(1 for r in annotated if r.get("resultKind") == "unimplemented")
    null_arg_ct = sum(1 for r in annotated if r.get("resultKind") == "nullArg")
    env_sensitive_ct = sum(1 for r in annotated if r.get("resultKind") == "envSensitive")
    failed_ct = sum(1 for r in annotated if r.get("resultKind") == "failed")
    # b03893810 removed this definition but left the "realTotal" summary field
    # reading it — restore the legacy real-signal total (real + failed records).
    real_signal = real_ct + failed_ct

    # ── Failure attribution (needs the runner's assertFailed/caught stamps) ──
    #
    # Before the runner started stamping these, `caught=true` was a mixed
    # signal: it meant "assertion failed", "an unregistered external symbol
    # raised via the catch-all fallback", or "a genuine runtime bug",
    # indistinguishably.  Triaging a chunk then required reading generated C++
    # by hand.  These two counts separate the buckets so the split is visible
    # in fact.json without any manual work:
    #
    #   assertFailed=true               → the subject's own Assert.* ran and
    #                                     recorded a failure (a real signal; the
    #                                     assertion executed and disagreed)
    #   caught=true, assertFailed=false → it raised BEFORE any assertion ran
    #                                     (pre-assertion crash: bad inputs,
    #                                     unregistered API, or a runtime defect)
    #
    # Both are subsets of `failed` / shaped failures, not a new denominator —
    # they exist for attribution, not for gating.  Older records (from runs
    # before the runner stamped these fields) simply contribute 0 here; that is
    # why these are reported alongside `attributionAvailable` rather than being
    # trusted blindly.
    attr_records = [r for r in annotated if "assertFailed" in r or "caught" in r]
    assertion_failed_ct = sum(1 for r in annotated if r.get("assertFailed"))
    pre_assertion_raise_ct = sum(
        1 for r in annotated if r.get("caught") and not r.get("assertFailed"))

    # Real-signal numerator: records that produced/or would produce a genuine
    # semantic check (a real value, or a genuine failure).  unassertable records
    # stay in the denominator so an all-void chunk cannot claim a free 100%.
    # factoryGap and stubGap records are infrastructure gaps (factory returned
    # null / method has no AOT body), not defects in the method under test —
    # they are excluded from numerator AND denominator so they cannot block the
    # gate while remaining fully visible.
    # ── Gate numerator/denominator ──
    gate_total, gate_passed = compute_gate_counts(annotated)

    fact_data = {
        "passed": passed,
        "total": total,
        # ── 口径说明（机器可读；防止 `passed` 被误读为"验证通过"）──────
        # `passed` 是**原生 runner 的运行结果**：只要没有 Assert.* 抛出就记 True。
        # 因此 stubGap（ATG 写死 42L、从未调用 native）与 smoke（无断言）也会
        # 计入 passed。**`passed` 不是品质指标。**
        # 真正表达"执行了且断言通过"的字段是 `realVerified`。
        #
        # 实测虚高幅度（2026-09-20，JSON/XML 线）：
        #   text-json          passed 209 vs realVerified   2  (104.5x)
        #   system-xml-xsl     passed 251 vs realVerified   4  ( 62.8x)
        #   json-converters    passed  94 vs realVerified   8  ( 11.8x)
        #   xml                passed 590 vs realVerified 136  (  4.3x)
        # 消费方若只需一个"可信通过数"，请用 `verifiedPassed` 或 `realVerified`。
        "verifiedPassed": real_ct,
        "passedIsNotVerification": True,
        # ── Runtime-derived real-vs-smoke (single source of truth) ──
        "realVerified": real_ct,        # value != 42: genuine assertion value
        "unassertable": unassertable_ct,  # void/async-void: 42 is structural
        "smokeUnknown": smoke_ct,       # has a return type but returned 42 → GAP
        "stubGap": stub_gap_ct,         # ATG marked AOT-STUB-GAP: no AOT body at all
        # C2: no codegen shape + the catch-all produced the value.  Distinct
        # from stubGap because the driver is a runtime observation rather than
        # an ATG compile-time marker, and distinct from `failed` because
        # nothing was implemented to be defective.
        "unimplemented": unimplemented_ct,
        "factoryGap": factory_gap_ct,   # factory returned null → caught before method ran
        "nullArg": null_arg_ct,         # ATG passes only default(T) ref args → ArgumentNullException is correct BCL behavior
        "envSensitive": env_sensitive_ct,  # GC counters / PRNG stream positions captured from probe's own process, not reproducible
        "failed": failed_ct,            # passed == False
        # ── Failure attribution (runner stamps; attribution-only, no gating) ──
        #
        # NOTE: these do NOT partition `failed`.  They count by RUNTIME STAMP
        # across all records, while `failed` counts a resultKind bucket — the
        # same subject can be assertFailed AND classified realDefect/failed,
        # and a stubGap subject can be caught without being a "failure".  Use
        # these to attribute, not to reconcile against `failed`.
        #
        # assertionFailed: the subject's Assert.* executed and disagreed — a
        #   real signal, and the only bucket that justifies "the implementation
        #   is wrong" for a subject whose assertions are themselves sound.
        # preAssertionRaise: raised before any assertion ran — bad inputs,
        #   an unregistered API hitting the catch-all fallback, or a genuine
        #   runtime defect.  Must be triaged, not assumed to be a defect.
        "assertionFailed": assertion_failed_ct,
        "preAssertionRaise": pre_assertion_raise_ct,
        # Records that actually carry the attribution stamps.  Zero means the
        # chunk's records predate the runner change, so the two counts above
        # are 0-by-absence rather than 0-by-measurement.
        "attributionAvailable": len(attr_records),
        # ── Gate numerator/denominator (factoryGap + stubGap excluded) ──
        "gateTotal": gate_total,
        "gatePassed": gate_passed,
        # ── JIT run (SEPARATE population — never mixed into the fields above) ──
        # Reported so a JIT-better-than-AOT gap is visible instead of being
        # silently folded into the AOT headline.  The cross-tech diff below is
        # the actionable form of this signal.
        "jitPassed": jit_passed,
        "jitTotal": jit_total,
        # ── Legacy fields (backward compat; now runtime-based, not marker-based) ──
        "realTotal": real_signal,
        "realPassed": real_ct,
        "unverifiedSmoke": unassertable_ct + smoke_ct + stub_gap_ct,
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
    # Fallback denominator — must be bound even when the metadata file is
    # missing entirely (e.g. after a build-stage invalidation), otherwise the
    # `expected = ... else meta_unique_fact` lookup below raises UnboundLocalError.
    meta_unique_fact: int | None = None
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
    #
    # Compares the PASS/FAIL verdict of each method across the two executables.
    # Not every mismatch is a defect — see split_cross_tech_diffs for the one
    # pair (unassertable ↔ factoryGap) that is reported separately rather than
    # as an inconsistency.
    cross_tech_diffs: list[dict] = []
    cross_tech_excluded: list[dict] = []
    status = aot_status  # initial: AOT status; cross-tech + JIT upgrade may modify below
    if jit_result and aot_result.get("results") and jit_result.get("results"):
        cross_tech_diffs, cross_tech_excluded = split_cross_tech_diffs(
            aot_result["results"], jit_result["results"])
        if cross_tech_excluded:
            print(f"  [fact] Cross-tech diff: {len(cross_tech_excluded)} non-comparable "
                  f"(unassertable<->factoryGap) pair(s) excluded from the diff")
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
    # NOTE: this is advisory (informative), NOT a gate.  Codegen session changes
    # (sentinel IR, hotupdate patches, etc.) can legitimately alter how many
    # methods the runtime dispatches vs the metadata baseline — raising an
    # alert here is noisy and the severity lives in the fact-passing, not in
    # a metadata count delta that is inherently imprecise.
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
            errors.append(f"aot: {aot_dropped} methods dropped vs metadata ({expected}) — advisory ({drop_ratio:.0%})")
        else:
            errors.append(f"aot: {aot_dropped} methods dropped vs metadata ({expected})")
    jit_dropped = max(0, (expected or 0) - (jit_result['total'] if jit_result else 0)) if expected else 0
    if jit_dropped > 0:
        jit_drop_ratio = jit_dropped / expected if expected else 1
        if jit_drop_ratio > 0.1:
            errors.append(f"jit: {jit_dropped} methods dropped vs metadata ({expected}) — advisory ({jit_drop_ratio:.0%})")
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

    # ── T4.1: kCodegenFailureCount gate ──────────────────────────────────
    #
    # The generator emits `extern "C" const int kCodegenFailureCount = N;` into
    # the chunk's generated TU whenever it silently replaced a method body with a
    # stub (NativeAotLoweringPlanner.Methods.cs:1322-1327).  Until now NOTHING
    # read it: a chunk could degrade N methods to stubs and still report a clean
    # build+fact.  That is a silent-correctness通路 — the stub returns a default
    # and the surrounding test may still "pass".
    #
    # Read it from the GENERATED SOURCE (not from the binary): the symbol is a
    # compile-time constant in the emitted C++, so its value is available before
    # the build succeeds, and reading the source avoids depending on a binary
    # that may have been built from a different revision.
    #
    # Threshold semantics: a nonzero count is REPORTED always; it BLOCKS only
    # above a configured tolerance, because some families legitimately have a
    # small number of unsupported methods.  The tolerance is per-chunk so a
    # family can raise it deliberately rather than by weakening the check
    # globally.
    codegen_failures = _read_codegen_failure_count(ctx)
    codegen_tolerance = _codegen_failure_tolerance(ctx)
    if codegen_failures > 0:
        msg = (f"codegen: kCodegenFailureCount={codegen_failures} "
               f"(tolerance {codegen_tolerance}) — {codegen_failures} method(s) "
               f"silently replaced with stubs")
        print(f"  [fact] WARNING: {msg}")
        if codegen_failures > codegen_tolerance:
            errors.append(msg)
            print(f"  [fact] ERROR: codegen failure count exceeds tolerance — blocking")
            if status == "passed":
                status = "failed"

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
    #
    # -1 is excluded because "not found" / "less than" are its documented
    # meanings (BinarySearch, string.Compare, WaitForFullGC*).  Callees whose
    # contract legitimately yields other negatives (Math.ILogB's FP_ILOGB0,
    # Math.BitDecrement below zero) are excluded by name — see
    # _is_legitimately_negative.
    value_warnings = sum(
        1 for r in aot_result["results"]
        if r.get("passed") and r.get("value", 0) < 0 and r.get("value", 0) != -1
        and not _is_legitimately_negative(r.get("methodSubjectId", ""))
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
            **({"crossTechExcluded": cross_tech_excluded} if cross_tech_excluded else {}),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
        value_suspicious=value_suspicious,
    )
