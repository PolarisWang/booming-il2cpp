"""Managed subject record — golden record generation stage.

Runs the subjects DLL (which includes both auto-generated Subject_N methods
and handwritten CustomEntrySubject_N methods from Custom.cs) under managed
.NET, captures each method's pass/fail result, and saves as golden-record.json
for cross-verification with AOT entry.exe --fact-json.

Design decisions:
  - Uses the pre-built subjects DLL directly (not regenerating call expressions),
    ensuring handwritten Custom.cs methods are included in the golden record.
  - Calls each Subject_N() / CustomEntrySubject_N() directly — methods either
    return normally (pass) or throw an exception (fail).
  - Subjects use Assert.* + [Fact] annotations; no _exitCode field needed.
  - Matches the AOT fact-json semantics: both measure "did the subject method
    complete without an unhandled exception?"
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult

_REPO_ROOT = Path(__file__).resolve().parents[4]


def _find_subjects_project(family_dir: Path) -> tuple[str, Path] | None:
    """Find the subjects .csproj file for ProjectReference.

    Returns (class_name, csproj_path) or None if not found.
    """
    subjects_dir = family_dir / "managed" / "subjects"
    if not subjects_dir.is_dir():
        return None

    # The subjects .csproj always matches the class name pattern
    csproj_files = list(subjects_dir.glob("*Subjects.csproj"))
    if csproj_files:
        csproj = csproj_files[0]
        return (csproj.stem, csproj)

    # Fallback: scan for any .csproj that has 'Subjects' in name
    for csproj in sorted(subjects_dir.glob("*.csproj")):
        if "Subjects" in csproj.stem or "subjects" in csproj.stem:
            return (csproj.stem, csproj)

    return None


def _read_contract(ctx: FamilyContext) -> dict | None:
    """Read the capability-family-contract.json."""
    if not ctx.contract_path.exists():
        return None
    try:
        return json.loads(ctx.contract_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _read_target_framework(subjects_csproj: Path) -> str:
    """Read the TargetFramework from the subjects .csproj file."""
    if not subjects_csproj.exists():
        return "net8.0"
    text = subjects_csproj.read_text(encoding="utf-8")
    m = re.search(r'<TargetFramework[^>]*>(net[.\d]+)</TargetFramework>', text)
    return m.group(1) if m else "net8.0"


def _detect_namespace(subjects_dir: Path, class_name: str) -> str | None:
    """Detect if the subjects class is inside a namespace."""
    cs_file = subjects_dir / f"{class_name}.cs"
    if not cs_file.exists():
        return None
    text = cs_file.read_text(encoding="utf-8")
    m = re.search(r'^namespace\s+([\w.]+)\s*{', text, re.MULTILINE)
    return m.group(1) if m else None


def run_managed_record(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage: Managed Subject Record — golden record generation.

    Flow:
      1. Read contract for methodSubjectIds and customEntryIndices.
      2. Find the pre-built subjects DLL.
      3. Generate a minimal C# harness that calls each Subject_N() /
         CustomEntrySubject_N() and captures pass/fail via try/catch.
      4. Build and run the harness under managed .NET.
      5. Save golden-record.json to native/golden-record.json.
    """
    start = time.perf_counter()

    # ── 1. Read contract ──
    contract = _read_contract(ctx)
    if contract is None:
        return StageResult(
            stage="managed_record", status="skipped",
            summary="contract not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    subject_ids = contract.get("methodSubjectIds", [])
    if not subject_ids:
        subject_ids = [m["methodSubjectId"] for m in contract.get("methodContracts", [])]
    method_count = len(subject_ids)
    if method_count == 0:
        return StageResult(
            stage="managed_record", status="n/a",
            summary="0 methods — no record needed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    custom_indices = set(contract.get("customEntryIndices", []))

    # ── 2. Find subjects .csproj ──
    found = _find_subjects_project(ctx.family_dir)
    if found is None:
        return StageResult(
            stage="managed_record", status="failed",
            summary="subjects .csproj not found in managed/subjects/",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    class_name, csproj_path = found

    # ── 3. Detect namespace and TFM ──
    subjects_dir = ctx.family_dir / "managed" / "subjects"
    namespace = _detect_namespace(subjects_dir, class_name)
    full_class_ref = f"{namespace}.{class_name}" if namespace else class_name
    tfm = _read_target_framework(subjects_dir / f"{class_name}.csproj")

    print(f"  [managed_record] Class={full_class_ref}, .csproj={csproj_path.name}, TFM={tfm}, "
          f"methods={method_count}, custom={len(custom_indices)}")

    # ── 4. Generate harness ──
    harness_dir = ctx.family_dir / "managed_test" / "subject_record"
    harness_dir.mkdir(parents=True, exist_ok=True)

    method_entries = []
    for idx in range(method_count):
        is_custom = idx in custom_indices
        method_name = f"CustomEntrySubject_{idx}" if is_custom else f"Subject_{idx}"
        method_entries.append(
            f"            {{ // [{idx}] {method_name}\n"
            f"                try {{\n"
            f"                    {full_class_ref}.{method_name}();\n"
            f"                    results.Add(new MethodResult {{ MethodIndex = {idx}, SubjectName = \"{method_name}\", IsCustom = {str(is_custom).lower()}, Passed = true, ExitCode = 0, ExceptionMessage = null }});\n"
            f"                }}\n"
            f"                catch (System.Exception ex) {{\n"
            f"                    results.Add(new MethodResult {{ MethodIndex = {idx}, SubjectName = \"{method_name}\", IsCustom = {str(is_custom).lower()}, Passed = false, ExitCode = -1, ExceptionMessage = ex.GetType().Name + \": \" + ex.Message }});\n"
            f"                }}\n"
            f"            }}"
        )

    methods_block = "\n".join(method_entries)

    cs_source = (
        "// Auto-generated managed subject record harness\n"
        f"// Family: {ctx.slug}, Assembly: {ctx.assembly}\n"
        "using System;\n"
        "using System.Collections.Generic;\n"
        "using System.Text.Json;\n"
        "\n"
        "class ManagedSubjectRecord\n"
        "{\n"
        "    struct MethodResult\n"
        "    {\n"
        "        public int MethodIndex { get; set; }\n"
        "        public string SubjectName { get; set; }\n"
        "        public bool IsCustom { get; set; }\n"
        "        public bool Passed { get; set; }\n"
        "        public int ExitCode { get; set; }\n"
        "        public string ExceptionMessage { get; set; }\n"
        "    }\n"
        "\n"
        "    struct GoldenRecord\n"
        "    {\n"
        "        public string Family { get; set; }\n"
        "        public string Assembly { get; set; }\n"
        "        public int MethodCount { get; set; }\n"
        "        public System.Collections.Generic.List<MethodResult> Results { get; set; }\n"
        "    }\n"
        "\n"
        "    static void Main()\n"
        "    {\n"
        "        var results = new System.Collections.Generic.List<MethodResult>();\n"
        f"{methods_block}\n"
        "        var record = new GoldenRecord\n"
        "        {\n"
        f"            Family = \"{ctx.slug}\",\n"
        f"            Assembly = \"{ctx.assembly}\",\n"
        f"            MethodCount = {method_count},\n"
        "            Results = results,\n"
        "        };\n"
        "        string json = JsonSerializer.Serialize(record, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase, WriteIndented = false });\n"
        "        Console.WriteLine(json);\n"
        "    }\n"
        "}\n"
    )

    cs_path = harness_dir / "ManagedSubjectRecord.cs"
    cs_path.write_text(cs_source, encoding="utf-8")

    # Generate .csproj — use ProjectReference so MSBuild handles all transitive dependencies
    csproj = (
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        '  <PropertyGroup>\n'
        '    <OutputType>Exe</OutputType>\n'
        f'    <TargetFramework>{tfm}</TargetFramework>\n'
        '    <Nullable>enable</Nullable>\n'
        '    <ImplicitUsings>disable</ImplicitUsings>\n'
        '    <StartupObject>ManagedSubjectRecord</StartupObject>\n'
        '    <NoWarn>$(NoWarn);CS8600;CS8602;CS8604</NoWarn>\n'
        '  </PropertyGroup>\n'
        '  <ItemGroup>\n'
        f'    <ProjectReference Include="{csproj_path.resolve()}" />\n'
        '  </ItemGroup>\n'
        '</Project>\n'
    )
    csproj_path = harness_dir / "ManagedSubjectRecord.csproj"
    csproj_path.write_text(csproj, encoding="utf-8")

    # ── 5. Build harness ──
    print(f"  [managed_record] Building harness ({tfm})...")
    try:
        build_r = subprocess.run(
            ["dotnet", "build", str(csproj_path), "--nologo", "-v", "q"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_record", status="failed",
            summary="harness build timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if build_r.returncode != 0:
        err = build_r.stderr[:300] if build_r.stderr else (build_r.stdout[:300] or "unknown")
        return StageResult(
            stage="managed_record", status="failed",
            summary=f"harness build failed: {err}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── 6. Run harness ──
    print(f"  [managed_record] Running harness...")
    try:
        run_r = subprocess.run(
            ["dotnet", "run", "--no-build", "--project", str(csproj_path)],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_record", status="failed",
            summary="harness run timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if run_r.returncode != 0:
        return StageResult(
            stage="managed_record", status="failed",
            summary=f"harness run failed (rc={run_r.returncode}): {run_r.stderr[:200]}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Parse JSON from stdout
    data = None
    for line in run_r.stdout.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                data = json.loads(line)
                break
            except json.JSONDecodeError:
                continue

    if data is None:
        return StageResult(
            stage="managed_record", status="failed",
            summary="harness output contained no valid JSON",
            details={"raw_output": run_r.stdout[:500]},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── 7. Save golden-record.json ──
    results = data.get("results", [])
    passed = sum(1 for r in results if r.get("passed"))
    failed = sum(1 for r in results if not r.get("passed"))

    golden_path = ctx.family_dir / "native" / "golden-record.json"
    golden_path.parent.mkdir(parents=True, exist_ok=True)
    golden_path.write_text(json.dumps(data, indent=2, ensure_ascii=False), encoding="utf-8")

    print(f"  [managed_record] Saved golden-record.json ({passed} passed, {failed} failed / {len(results)} total)")

    status = "passed" if failed == 0 else "failed"
    summary = f"{status} ({passed}/{len(results)} passed)"

    return StageResult(
        stage="managed_record", status=status,
        summary=summary,
        details={
            "passed": passed,
            "failed": failed,
            "total": len(results),
            "methodCount": method_count,
            "customCount": len(custom_indices),
            "goldenRecordPath": str(golden_path.resolve()),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
