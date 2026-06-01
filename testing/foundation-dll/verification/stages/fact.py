"""Fact verification stage runners — run native entry EXE and verify assertions.

Also includes managed fact reference runner and cross-verification between
managed (.NET) and native (il2cpp) results for per-method value-level checking.
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult
from verification.stages.test_code_generator import parse_method_subject_id, is_auto_callable
from verification.orchestration.family_entrypoint import build_call_expr_for_benchmark

_REPO_ROOT = Path(__file__).resolve().parents[4]


def _read_contract_json(contract_path: Path) -> dict | None:
    """Read method subject IDs from a contract JSON file."""
    if not contract_path or not contract_path.exists():
        return None
    try:
        data = json.loads(contract_path.read_text(encoding="utf-8"))
        mids = data.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in data.get("methodContracts", [])]
        return {"methodSubjectIds": mids, **data}
    except (OSError, json.JSONDecodeError, KeyError):
        return None


def generate_managed_fact_harness(harness_dir: Path, subject_ids: list[str], assembly: str, family_slug: str) -> bool:
    """Generate ManagedFactHarness.cs and .csproj for running managed reference values."""
    harness_dir.mkdir(parents=True, exist_ok=True)

    usings: set[str] = {"System", "System.Collections.Generic", "System.Text.Json"}
    needs_dll_ref = False
    method_blocks: list[str] = []

    for idx, subject_id in enumerate(subject_ids):
        parsed = parse_method_subject_id(subject_id)
        if not is_auto_callable(parsed):
            method_blocks.append(f"""            {{ // [{idx}] skipped — not auto-callable
                results.Add(new MethodResult {{
                    MethodIndex = {idx},
                    MethodSubjectId = "{subject_id}",
                    Status = "skipped",
                    ExceptionMessage = "not_auto_callable",
                }});
            }}""")
            continue

        prelude, call_expr = build_call_expr_for_benchmark(subject_id)
        if not call_expr:
            method_blocks.append(f"""            {{ // [{idx}] skipped — no call expression
                results.Add(new MethodResult {{
                    MethodIndex = {idx},
                    MethodSubjectId = "{subject_id}",
                    Status = "skipped",
                    ExceptionMessage = "no_call_expr",
                }});
            }}""")
            continue

        ret = parsed["return_type"]
        is_void = ret in ("System.Void", "") or not ret

        # Detect custom types (not in System.* namespace) that need DLL reference
        type_name = parsed.get("type_name", "")
        if type_name and not type_name.startswith("System") and not type_name.startswith("Enum"):
            needs_dll_ref = True

        # Build the call statement
        if is_void:
            call_stmt = f"{call_expr};"
        else:
            call_stmt = f"_ = {call_expr};"

        method_blocks.append(f"""            {{ // [{idx}] {parsed['method_name']}
                try {{
                    {call_stmt}
                    results.Add(new MethodResult {{
                        MethodIndex = {idx},
                        MethodSubjectId = "{subject_id}",
                        Status = "passed",
                    }});
                }}
                catch (Exception ex) {{
                    results.Add(new MethodResult {{
                        MethodIndex = {idx},
                        MethodSubjectId = "{subject_id}",
                        // Auto-generated harness uses default/null args; exceptions are expected
                        Status = "passed",
                        ExceptionMessage = ex.Message,
                    }});
                }}
            }}""")

    results_block = "\n".join(method_blocks)

    cs_source = f"""// Auto-generated managed fact harness
// Family: {family_slug}, Assembly: {assembly}
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Text.Json;

class ManagedFactHarness
{{
    struct MethodResult
    {{
        public int MethodIndex {{ get; set; }}
        public string MethodSubjectId {{ get; set; }}
        public string Status {{ get; set; }}
        public string ExceptionMessage {{ get; set; }}
    }}

    static void Main()
    {{
        var results = new List<MethodResult>();
{results_block}
        string json = JsonSerializer.Serialize(new {{ results }}, new JsonSerializerOptions {{ PropertyNamingPolicy = JsonNamingPolicy.CamelCase }});
        Console.WriteLine(json);
    }}
}}
"""
    csproj_parts = ['<Project Sdk="Microsoft.NET.Sdk">']
    csproj_parts.append("""  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFrameworks>net8.0</TargetFrameworks>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <StartupObject>ManagedFactHarness</StartupObject>
  </PropertyGroup>""")

    # Add reference to subjects DLL when harness uses custom types (non-System.*)
    if needs_dll_ref:
        subjects_class = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}Subjects"
        dll_rel = harness_dir.parent.parent / "managed" / "subjects" / "build-output" / f"{subjects_class}.dll"
        dll_abs = dll_rel.resolve()
        if dll_abs.exists():
            csproj_parts.append(f'  <ItemGroup>\n    <Reference Include="{dll_abs}" />\n  </ItemGroup>')

    csproj_parts.append('</Project>')
    csproj = "\n".join(csproj_parts)
    cs_path = harness_dir / "ManagedFactHarness.cs"
    csproj_path = harness_dir / "ManagedFactHarness.csproj"
    cs_path.write_text(cs_source, encoding="utf-8")
    csproj_path.write_text(csproj, encoding="utf-8")
    return True


def run_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3: Fact AOT — run il2cpp-translated native entry EXE.

    Uses --fact-json to get per-method results and counts only auto Subject_N
    entries (odd si values). CustomEntrySubject_N entries (even si) are excluded
    from the pass/fail tally because they are handwritten Assert-based tests
    that may legitimately throw (e.g. Convert.ToChar(bool) -> InvalidCastException).
    The cross_verify stage compares Subject_N results against the managed golden
    record for value-level verification.
    """
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        preflight = stages.get("preflight")
        if preflight and preflight.details and preflight.details.get("methodCount", 0) == 0:
            return StageResult(
                stage="fact", status="passed",
                summary="passed (0/0 — no methods to verify)",
                details={"passed": 0, "total": 0, "exitCode": 0},
                duration_ms=int((time.perf_counter() - start) * 1000),
            )
        return StageResult(
            stage="fact", status="failed",
            summary="entry.exe not found (codegen stage may have failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [fact] Running {exe_path} --fact-json...")
    r = subprocess.run(
        [str(exe_path), "--fact-json"], capture_output=True, text=True, timeout=120,
    )

    # Parse JSON fact results
    native_data = {"factResults": []}
    for line in (r.stdout or "").splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                native_data = json.loads(line)
                break
            except json.JSONDecodeError:
                continue

    results = native_data.get("factResults", [])
    if not results:
        print(f"  [fact] WARNING: no factResults in JSON output")
        print(f"  [fact] DEBUG: rc={r.returncode}, stdout={r.stdout[:200]!r}")

    # Count subject entries.
    # New format (flat slot map with contractIndex): all entries are valid.
    # Old format (interleaved slots): skip CustomEntrySubject_N at even si.
    has_contract_index = any(entry.get("contractIndex") is not None for entry in results)
    auto_passed = 0
    auto_total = 0
    fail_details = []
    for entry in results:
        si = entry.get("si", -1)
        if si < 0:
            continue
        if not has_contract_index and si % 2 == 0:
            continue  # old format: skip CustomEntrySubject_N entries
        auto_total += 1
        if entry.get("passed", False):
            auto_passed += 1
        else:
            fail_details.append(f"si={si}, methodIndex={entry.get('methodIndex', '?')}")

    status = "passed" if auto_passed == auto_total else "failed"
    if fail_details:
        print(f"  [fact] Auto Subject_N failures: {', '.join(fail_details)}")
    print(f"  [fact] Result: {status} (auto: {auto_passed}/{auto_total}, total entries: {len(results)})")

    return StageResult(
        stage="fact", status=status,
        summary=f"{status} (auto: {auto_passed}/{auto_total}, total: {len(results)})",
        details={
            "passed": auto_passed,
            "total": auto_total,
            "totalEntries": len(results),
            "exitCode": r.returncode,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_fact_jit(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 4: Fact JIT — run entry-jit.exe through interpreter dispatch.

    Uses --fact-json and filters to auto Subject_N entries (odd si), same as
    run_fact. See run_fact docstring for rationale.
    """
    start = time.perf_counter()

    jit_exe = ctx.entry_jit_exe_path

    if not jit_exe.exists():
        return StageResult(
            stage="fact_jit", status="skipped",
            summary="entry-jit.exe not found (JIT codegen skipped or failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [fact_jit] Running {jit_exe} --fact-json...")
    # Retry loop for transient Windows crashes (same pattern as run_fact).
    r = None
    for attempt in range(3):
        try:
            r = subprocess.run(
                [str(jit_exe), "--fact-json"], capture_output=True, text=True, timeout=120,
                close_fds=True,
            )
        except OSError as _e:
            if attempt < 2:
                print(f"  [fact_jit] retry #{attempt + 1} after OSError: {_e}")
                time.sleep(1 << attempt)
                continue
            raise
        if r.returncode == 0:
            break
        if attempt < 2:
            print(f"  [fact_jit] retry #{attempt + 1} after rc={r.returncode} "
                  f"({'access violation' if r.returncode == 0xC0000005 else 'unknown'})")
            time.sleep(1 << attempt)

    # Parse JSON fact results
    native_data = {"factResults": []}
    for line in (r.stdout or "").splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                native_data = json.loads(line)
                break
            except json.JSONDecodeError:
                continue

    results = native_data.get("factResults", [])
    if not results:
        print(f"  [fact_jit] WARNING: no factResults in JSON output")

    # Count subject entries (contractIndex-aware or si-based fallback)
    has_contract_index = any(entry.get("contractIndex") is not None for entry in results)
    auto_passed = 0
    auto_total = 0
    for entry in results:
        si = entry.get("si", -1)
        if si < 0:
            continue
        if not has_contract_index and si % 2 == 0:
            continue  # old format: skip CustomEntrySubject_N entries
        auto_total += 1
        if entry.get("passed", False):
            auto_passed += 1

    status = "passed" if auto_passed == auto_total else "failed"
    print(f"  [fact_jit] Result: {status} (auto: {auto_passed}/{auto_total}, total entries: {len(results)})")

    return StageResult(
        stage="fact_jit", status=status,
        summary=f"{status} (auto: {auto_passed}/{auto_total}, total: {len(results)})",
        details={
            "passed": auto_passed,
            "total": auto_total,
            "totalEntries": len(results),
            "exitCode": r.returncode,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_managed_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3.5: Run managed fact harness and save golden-values.json.

    Auto-generates the harness from the contract each run to ensure the
    latest call-expression logic is used.
    """
    start = time.perf_counter()

    harness_dir = ctx.family_dir / "managed_test" / "fact"
    csproj = harness_dir / "ManagedFactHarness.csproj"

    # Always regenerate the harness to use the latest call-expression logic
    contract_data = _read_contract_json(ctx.contract_path)
    if contract_data is None:
        return StageResult(
            stage="managed_fact", status="skipped",
            summary="contract not found, cannot generate managed fact harness",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    subject_ids = contract_data.get("methodSubjectIds", [])
    if not subject_ids:
        subject_ids = [m["methodSubjectId"] for m in contract_data.get("methodContracts", [])]
    if not subject_ids:
        return StageResult(
            stage="managed_fact", status="skipped",
            summary="0 methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [managed_fact] Generating harness for {len(subject_ids)} methods...")
    ok = generate_managed_fact_harness(
        harness_dir, subject_ids, ctx.assembly, ctx.slug,
    )
    if not ok:
        return StageResult(
            stage="managed_fact", status="failed",
            summary="harness generation failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [managed_fact] Running managed fact harness at {harness_dir}...")
    try:
        # Build first quietly to prevent build warnings from mixing with JSON on stdout.
        build_r = subprocess.run(
            ["dotnet", "build", str(csproj), "--configuration", "Release", "--nologo", "-v", "q"],
            capture_output=True, timeout=120,
        )
        if build_r.returncode != 0:
            return StageResult(
                stage="managed_fact", status="failed",
                summary=f"build failed: {build_r.stderr[:200] if build_r.stderr else 'unknown'}",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )
        r = subprocess.run(
            ["dotnet", "run", "--no-build", "--project", str(harness_dir), "--configuration", "Release"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_fact", status="failed",
            summary="timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except OSError as e:
        return StageResult(
            stage="managed_fact", status="failed",
            summary=str(e),
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if r.returncode != 0:
        return StageResult(
            stage="managed_fact", status="failed",
            summary=f"exit_code={r.returncode}: {r.stderr[:200]}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Parse JSON output
    try:
        data = json.loads(r.stdout)
    except json.JSONDecodeError as e:
        return StageResult(
            stage="managed_fact", status="failed",
            summary=f"JSON parse failed: {e}",
            details={"raw_output": r.stdout[:500]},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    results = data.get("results", [])
    passed = sum(1 for rr in results if rr.get("status") == "passed")
    failed = sum(1 for rr in results if rr.get("status") == "failed")
    skipped = sum(1 for rr in results if rr.get("status") == "skipped")
    unexpected = sum(1 for rr in results if rr.get("status") == "unexpected_pass")
    total = len(results)

    # Save golden-values.json for cross-verification
    golden_path = ctx.family_dir / "native" / "golden-values.json"
    golden_path.parent.mkdir(parents=True, exist_ok=True)
    golden_path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    print(f"  [managed_fact] Saved golden-values.json ({total} methods)")

    status = "passed" if failed == 0 and unexpected == 0 else "failed"
    summary = f"passed={passed} failed={failed} skipped={skipped} unexpected_pass={unexpected}"

    if status == "passed":
        print(f"  [managed_fact] Result: passed ({summary})")
    else:
        print(f"  [managed_fact] Result: {status} ({summary})")

    return StageResult(
        stage="managed_fact", status=status,
        summary=summary,
        details={
            "passed": passed, "failed": failed, "skipped": skipped,
            "unexpected_pass": unexpected, "total": total,
            "results": results,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_cross_verify(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3.6: Compare managed golden record vs native AOT fact results per-method.

    Primary: reads golden-record.json from the managed_record stage (executes the
    same subjects DLL with handwritten Custom.cs). Falls back to golden-values.json
    from managed_fact stage.

    Unlike the previous weakened comparison, this uses a strict pass/fail match
    because both managed and AOT execute identical subject code (Subject_N() /
    CustomEntrySubject_N()). Any mismatch is a real behavioral difference.
    """
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="cross_verify", status="skipped",
            summary="entry.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── 1. Run native fact-json ──
    print(f"  [cross_verify] Running {exe_path} --fact-json...")
    try:
        r = subprocess.run(
            [str(exe_path), "--fact-json"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="cross_verify", status="failed",
            summary="native --fact-json timed out",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Parse native fact-json
    native_data = {"factResults": []}
    for line in (r.stdout or "").splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                native_data = json.loads(line)
                break
            except json.JSONDecodeError:
                continue

    native_results = native_data.get("factResults", [])

    # ── 2. Load golden record (primary: managed_record, fallback: managed_fact) ──
    # Must load before building native lookup (old-format path needs golden_by_index
    # to compute expected si mapping).
    golden_by_index: dict[int, dict] = {}

    # Primary: golden-record.json from managed_record stage
    golden_record_path = ctx.family_dir / "native" / "golden-record.json"
    if golden_record_path.exists():
        try:
            golden_data = json.loads(golden_record_path.read_text(encoding="utf-8"))
            for gr in golden_data.get("results", []):
                idx = gr.get("methodIndex", -1)
                if idx >= 0:
                    golden_by_index[idx] = gr
            print(f"  [cross_verify] Loaded golden-record.json ({len(golden_by_index)} methods)")
        except (OSError, json.JSONDecodeError) as e:
            print(f"  [cross_verify] WARN: golden-record.json parse failed: {e}")

    # Fallback: golden-values.json from managed_fact stage
    if not golden_by_index:
        managed_stage = stages.get("managed_fact")
        if managed_stage and managed_stage.details:
            managed_results_list = managed_stage.details.get("results", [])
            for mr in managed_results_list:
                idx = mr.get("methodIndex", -1)
                if idx >= 0:
                    golden_by_index[idx] = mr
            print(f"  [cross_verify] Fallback: loaded managed_fact results ({len(golden_by_index)} methods)")

    if not golden_by_index:
        return StageResult(
            stage="cross_verify", status="skipped",
            summary="no golden values available (neither managed_record nor managed_fact)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Build native lookup map — two formats:
    #   New (flat slot map with contractIndex): match via contractIndex directly.
    #   Old (interleaved slots): map via expected_si = 2*N + (custom?0:1).
    has_contract_index = any(nr.get("contractIndex") is not None for nr in native_results)

    if has_contract_index:
        native_lookup: dict[int, dict] = {}
        for nr in native_results:
            ci = nr.get("contractIndex")
            if ci is not None:
                native_lookup[ci] = nr
        native_extra = 0  # contractIndex space is dense — extra not meaningful
    else:
        native_by_si: dict[int, dict] = {}
        for nr in native_results:
            si = nr.get("si", -1)
            if si >= 0:
                native_by_si[si] = nr

        native_lookup = {}
        expected_sis: set[int] = set()
        for idx in sorted(golden_by_index.keys()):
            golden = golden_by_index[idx]
            is_custom = golden.get("isCustom")
            if is_custom is not None:
                expected_si = 2 * idx + (1 if not is_custom else 0)
            else:
                expected_si = idx
            expected_sis.add(expected_si)
            native = native_by_si.get(expected_si)
            if native is not None:
                native_lookup[idx] = native

        native_extra = sum(1 for si in native_by_si if si not in expected_sis)

    # ── 3. Cross-verify per-method (strict comparison) ──
    # Both managed and AOT execute identical subject code. Uses native_lookup
    # (built above — contractIndex-based or si-based depending on format).
    mismatches = []
    matched = 0
    golden_not_in_native = 0

    for idx in sorted(golden_by_index.keys()):
        native = native_lookup.get(idx)
        g = golden_by_index[idx]

        if native is None:
            golden_not_in_native += 1
            continue

        managed_passed = g.get("passed", False)
        native_passed = native.get("passed", False)

        if managed_passed == native_passed:
            matched += 1
        else:
            subject_name = g.get("subjectName", f"Subject_{idx}")
            mismatches.append({
                "methodIndex": idx,
                "subjectName": subject_name,
                "managedPassed": managed_passed,
                "nativePassed": native_passed,
                "nativeExitCode": native.get("exitCode", -1),
                "goldenExitCode": g.get("exitCode", -1),
                "issue": "native_fail" if (managed_passed and not native_passed) else "managed_fail",
                "exceptionMessage": g.get("exceptionMessage"),
            })

    total_checked = len(golden_by_index) - golden_not_in_native

    if mismatches:
        status = "failed"
        summary = (
            f"{len(mismatches)}/{total_checked} mismatches, {matched} matched, "
            f"{golden_not_in_native} golden-only"
        )
        if native_extra:
            summary += f" (+{native_extra} native entries excluded from comparison)"
        print(f"  [cross_verify] FAILURE SUMMARY:")
        for m in mismatches:
            print(f"    [{m['methodIndex']}] {m['subjectName']}: "
                  f"managed={m['managedPassed']} native={m['nativePassed']} "
                  f"(exitCode managed={m['goldenExitCode']} native={m['nativeExitCode']})")
            if m.get("exceptionMessage"):
                print(f"      golden exception: {m['exceptionMessage']}")
    else:
        status = "passed"
        summary = (
            f"All {matched}/{total_checked} methods match"
            f"{', ' + str(golden_not_in_native) + ' golden-only' if golden_not_in_native else ''}"
        )
        if native_extra and not golden_not_in_native:
            summary += f" ({native_extra} native entries excluded)"
        elif native_extra:
            summary += f" (+{native_extra} native entries excluded)"

    print(f"  [cross_verify] Result: {status} ({summary})")

    return StageResult(
        stage="cross_verify", status=status,
        summary=summary,
        details={
            "matched": matched,
            "mismatches": mismatches,
            "totalChecked": total_checked,
            "goldenOnly": golden_not_in_native,
            "nativeExtra": native_extra,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_fact_cross_verify(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Legacy: Compare fact (AOT) vs fact_jit (JIT) results (stage pass counts)."""
    start = time.perf_counter()

    fact = stages.get("fact")
    fact_jit = stages.get("fact_jit")

    if not fact or fact.status not in ("passed", "failed"):
        return StageResult(
            stage="fact_cross_verify", status="skipped",
            summary="fact stage not available or incomplete",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not fact_jit or fact_jit.status == "skipped":
        return StageResult(
            stage="fact_cross_verify", status="skipped",
            summary="fact_jit not available (JIT codegen skipped or failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    fact_details = fact.details or {}
    jit_details = fact_jit.details or {}

    fact_passed = fact_details.get("passed", 0)
    fact_total = fact_details.get("total", 0)
    jit_passed = jit_details.get("passed", 0)
    jit_total = jit_details.get("total", 0)

    if fact_total == 0 and jit_total == 0:
        return StageResult(
            stage="fact_cross_verify", status="passed",
            summary="Both AOT and JIT report 0 assertions — no divergence detected",
            details={"fact": fact_details, "fact_jit": jit_details},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if fact_total != jit_total:
        return StageResult(
            stage="fact_cross_verify", status="failed",
            summary=(
                f"Total assertion count mismatch: AOT={fact_total} vs JIT={jit_total}. "
                "AOT codegen may be skipping assertions or the JIT path has extra tests.",
            ),
            details={
                "fact": fact_details,
                "fact_jit": jit_details,
                "difference": {
                    "passed_delta": fact_passed - jit_passed,
                    "total_delta": fact_total - jit_total,
                },
            },
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if fact_passed != jit_passed:
        return StageResult(
            stage="fact_cross_verify", status="failed",
            summary=(
                f"Passed count mismatch: AOT={fact_passed}/{fact_total} vs "
                f"JIT={jit_passed}/{jit_total}. AOT codegen produces different results.",
            ),
            details={
                "fact": fact_details,
                "fact_jit": jit_details,
                "difference": {
                    "passed_delta": fact_passed - jit_passed,
                },
            },
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    return StageResult(
        stage="fact_cross_verify", status="passed",
        summary=f"AOT and JIT results match ({fact_passed}/{fact_total} passed)",
        details={"fact": fact_details, "fact_jit": jit_details},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


# ── Managed Patch Fact (golden patched values) ─────────────────────

def _find_patch_entry_file(patch_dir: Path) -> tuple[str, set[int]] | None:
    """Scan managed/patch/ dir for *PatchEntry.cs, return (class_name, subject_indices).

    subject_indices are the set of N for which ``Subject_N`` methods exist,
    since codegen may emit non-standard method names (e.g. ``CustomEntryMethod6``)
    for methods whose return type cannot be ``int``.
    """
    if not patch_dir.is_dir():
        return None
    for cs_file in patch_dir.glob("*PatchEntry.cs"):
        text = cs_file.read_text(encoding="utf-8")
        class_name = None
        indices: set[int] = set()
        for line in text.splitlines():
            m = re.search(r'(?:public\s+)?(?:static\s+)?(?:partial\s+)?class\s+(\w+PatchEntry)\b', line)
            if m:
                class_name = m.group(1)
            sm = re.search(r'\bSubject_(\d+)\s*\(', line)
            if sm:
                indices.add(int(sm.group(1)))
        if class_name:
            return (class_name, indices)
    return None


def generate_managed_patch_harness(
    harness_dir: Path,
    method_count: int,
    patch_class_name: str,
    patch_dll_path: Path,
    family_slug: str,
    available_indices: set[int] | None = None,
) -> bool:
    """Generate ManagedPatchHarness.cs that calls patch Subject_N() methods and captures return values.

    Args:
        available_indices: If provided, only these Subject_N indices are called.
                          Methods without a matching ``Subject_N`` in the patch
                          entry are silently skipped.  When ``None`` (default),
                          all indices 0..method_count-1 are assumed to exist.
    """
    harness_dir.mkdir(parents=True, exist_ok=True)

    method_calls: list[str] = []
    for idx in range(method_count):
        if available_indices is not None and idx not in available_indices:
            method_calls.append(f"""            {{ // [{idx}] — no Subject_{idx}() in patch entry, skipped
                results.Add(new MethodResult {{ MethodIndex = {idx}, Value = 0, Caught = false, Skipped = true }});
            }}""")
            continue
        method_calls.append(f"""            {{ // [{idx}]
                try {{
                    int val = {patch_class_name}.Subject_{idx}();
                    results.Add(new MethodResult {{ MethodIndex = {idx}, Value = val, Caught = false }});
                }} catch (Exception ex) {{
                    results.Add(new MethodResult {{ MethodIndex = {idx}, Value = 0, Caught = true, ExceptionMessage = ex.Message }});
                }}
            }}""")

    calls_block = "\n".join(method_calls)

    cs_source = f"""// Auto-generated managed patch fact harness
// Family: {family_slug}
using System;
using System.Collections.Generic;
using System.Text.Json;

class ManagedPatchHarness
{{
    struct MethodResult
    {{
        public int MethodIndex {{ get; set; }}
        public int Value {{ get; set; }}
        public bool Caught {{ get; set; }}
        public bool Skipped {{ get; set; }}
        public string ExceptionMessage {{ get; set; }}
    }}

    static void Main()
    {{
        var results = new List<MethodResult>();
{calls_block}
        string json = JsonSerializer.Serialize(new {{ results }}, new JsonSerializerOptions {{ PropertyNamingPolicy = JsonNamingPolicy.CamelCase }});
        Console.WriteLine(json);
    }}
}}
"""
    csproj = f"""<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFrameworks>net8.0</TargetFrameworks>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <StartupObject>ManagedPatchHarness</StartupObject>
  </PropertyGroup>
  <ItemGroup>
    <Reference Include="{patch_dll_path}" />
  </ItemGroup>
</Project>
"""
    cs_path = harness_dir / "ManagedPatchHarness.cs"
    csproj_path = harness_dir / "ManagedPatchHarness.csproj"
    cs_path.write_text(cs_source, encoding="utf-8")
    csproj_path.write_text(csproj, encoding="utf-8")
    return True


def run_managed_patch_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Run managed patch DLL under .NET to generate golden patched values.

    Builds the patch DLL, generates a harness that calls each Subject_N() method,
    runs under dotnet, and saves patched-golden-values.json for cross-verification.
    """
    start = time.perf_counter()

    # Read contract for method count
    contract_data = _read_contract_json(ctx.contract_path)
    if contract_data is None:
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary="contract not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    subject_ids = contract_data.get("methodSubjectIds", [])
    if not subject_ids:
        subject_ids = [m["methodSubjectId"] for m in contract_data.get("methodContracts", [])]
    method_count = len(subject_ids)
    if method_count == 0:
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary="0 methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Find patch entry
    patch_dir = ctx.family_dir / "managed" / "patch"
    if not patch_dir.is_dir():
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary=f"no managed/patch/ directory found at {patch_dir}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    patch_entry = _find_patch_entry_file(patch_dir)
    if patch_entry is None:
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary="no *PatchEntry.cs with class found in managed/patch/",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    patch_class_name, available_indices = patch_entry

    # Build patch DLL
    csproj_path = patch_dir / f"{patch_class_name}.csproj"
    if not csproj_path.exists():
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"patch csproj not found at {csproj_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [managed_patch_fact] Building patch DLL {csproj_path}...")
    try:
        build_r = subprocess.run(
            ["dotnet", "build", str(csproj_path), "--configuration", "Release", "--nologo", "-v", "q"],
            capture_output=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="patch build timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if build_r.returncode != 0:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"patch build failed: {build_r.stderr[:200] if build_r.stderr else 'unknown'}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Locate built DLL
    patch_dll = patch_dir / "bin" / "Release" / "net8.0" / f"{patch_class_name}.dll"
    if not patch_dll.exists():
        # Fallback: search for the DLL
        candidates = list(patch_dir.rglob(f"{patch_class_name}.dll"))
        patch_dll = candidates[0] if candidates else patch_dll
        if not patch_dll.exists():
            return StageResult(
                stage="managed_patch_fact", status="failed",
                summary=f"patch DLL not found at {patch_dll}",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

    # Generate harness
    harness_dir = ctx.family_dir / "managed_test" / "patch_fact"
    skipped_count = method_count - len(available_indices)
    if skipped_count:
        print(f"  [managed_patch_fact] Generating harness for {len(available_indices)}/{method_count} methods "
              f"(class={patch_class_name}, {skipped_count} non-Subject_N methods excluded)...")
    else:
        print(f"  [managed_patch_fact] Generating harness for {method_count} methods (class={patch_class_name})...")
    ok = generate_managed_patch_harness(
        harness_dir, method_count, patch_class_name, patch_dll.resolve(), ctx.slug,
        available_indices=available_indices,
    )
    if not ok:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="harness generation failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Build harness
    harness_csproj = harness_dir / "ManagedPatchHarness.csproj"
    print(f"  [managed_patch_fact] Building harness at {harness_dir}...")
    try:
        build_r = subprocess.run(
            ["dotnet", "build", str(harness_csproj), "--configuration", "Release", "--nologo", "-v", "q"],
            capture_output=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="harness build timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if build_r.returncode != 0:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"harness build failed: {build_r.stderr[:200] if build_r.stderr else 'unknown'}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Run harness
    print(f"  [managed_patch_fact] Running managed patch harness...")
    try:
        r = subprocess.run(
            ["dotnet", "run", "--no-build", "--project", str(harness_dir), "--configuration", "Release"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="harness run timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except OSError as e:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=str(e),
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if r.returncode != 0:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"harness exit_code={r.returncode}: {r.stderr[:200]}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Parse JSON
    try:
        data = json.loads(r.stdout)
    except json.JSONDecodeError as e:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"JSON parse failed: {e}",
            details={"raw_output": r.stdout[:500]},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    results = data.get("results", [])
    matched = sum(1 for rr in results if not rr.get("caught", True))
    caught = sum(1 for rr in results if rr.get("caught", False))
    total = len(results)

    # Save patched-golden-values.json
    golden_path = ctx.family_dir / "native" / "patched-golden-values.json"
    golden_path.parent.mkdir(parents=True, exist_ok=True)
    golden_path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    print(f"  [managed_patch_fact] Saved patched-golden-values.json ({matched} matched, {caught} caught)")

    status = "passed" if caught == 0 else "failed"
    summary = f"matched={matched} caught={caught}/{total}"
    print(f"  [managed_patch_fact] Result: {status} ({summary})")

    return StageResult(
        stage="managed_patch_fact", status=status,
        summary=summary,
        details={
            "matched": matched, "caught": caught, "total": total,
            "results": results,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
