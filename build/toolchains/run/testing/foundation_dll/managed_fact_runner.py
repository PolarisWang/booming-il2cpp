"""Managed Fact Runner — .NET8 fact verification for foundation-dll families.

Generates a C# harness, compiles and runs it with `dotnet run -f net8.0`,
then reports pass/fail for each method subject.

Usage:
    from managed_fact_runner import run_managed_fact
    result = run_managed_fact("enum-parsing", "System.Private.CoreLib")
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

from managed_harness_generator import generate_call_expr, CallExprMode

_VERIFICATION_BASE = Path(__file__).resolve().parents[5] / "testing" / "foundation-dll"


def _generate_fact_harness(family_slug: str, assembly: str,
                           method_subject_ids: list[str],
                           tfm: str = "net8.0") -> Path | None:
    """Generate a .NET fact harness that calls each method and reports pass/fail.

    Returns Path to the generated .csproj directory, or None if generation fails.
    """
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    harness_dir = family_dir / "managed_test" / "fact"
    harness_dir.mkdir(parents=True, exist_ok=True)

    harness_cs = harness_dir / "ManagedFactHarness.cs"
    harness_csproj = harness_dir / "ManagedFactHarness.csproj"

    # Generate .csproj with multi-targeting for net8.0 + net10.0
    harness_csproj.write_text(
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        '  <PropertyGroup>\n'
        '    <OutputType>Exe</OutputType>\n'
        f'    <TargetFrameworks>{tfm}</TargetFrameworks>\n'
        '    <Nullable>enable</Nullable>\n'
        '    <ImplicitUsings>enable</ImplicitUsings>\n'
        '    <StartupObject>ManagedFactHarness</StartupObject>\n'
        '  </PropertyGroup>\n'
        '</Project>\n',
        encoding="utf-8",
    )

    # ── Generate method sections ────────────────────────────────────────
    method_sections: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        call_expr, always_throws = generate_call_expr(mid, idx, mode=CallExprMode.FACT)
        method_id_escaped = mid.replace('"', '\\"')

        if not call_expr:
            # Unsupported signature — mark as skipped
            method_sections.append(
                f'            {{ // [{idx}] unsupported signature\n'
                f'                results.Add(new MethodResult {{\n'
                f'                    MethodIndex = {idx},\n'
                f'                    MethodSubjectId = "{method_id_escaped}",\n'
                f'                    Status = "skipped",\n'
                f'                    ExceptionMessage = "unsupported_signature",\n'
                f'                }});\n'
                f'            }}'
            )
            continue

        if always_throws:
            # Expect exception
            method_sections.append(
                f'            {{ // [{idx}] always_throws\n'
                f'                try\n'
                f'                {{\n'
                f'                    {call_expr};\n'
                f'                    results.Add(new MethodResult {{\n'
                f'                        MethodIndex = {idx},\n'
                f'                        MethodSubjectId = "{method_id_escaped}",\n'
                f'                        Status = "unexpected_pass",\n'
                f'                        ExceptionMessage = "",\n'
                f'                    }});\n'
                f'                }}\n'
                f'                catch (System.Exception ex)\n'
                f'                {{\n'
                f'                    results.Add(new MethodResult {{\n'
                f'                        MethodIndex = {idx},\n'
                f'                        MethodSubjectId = "{method_id_escaped}",\n'
                f'                        Status = "passed",\n'
                f'                        ExceptionMessage = ex.GetType().Name,\n'
                f'                    }});\n'
                f'                }}\n'
                f'            }}'
            )
        else:
            # Expect normal execution
            method_sections.append(
                f'            {{ // [{idx}] normal\n'
                f'                try\n'
                f'                {{\n'
                f'                    {call_expr};\n'
                f'                    results.Add(new MethodResult {{\n'
                f'                        MethodIndex = {idx},\n'
                f'                        MethodSubjectId = "{method_id_escaped}",\n'
                f'                        Status = "passed",\n'
                f'                        ExceptionMessage = "",\n'
                f'                    }});\n'
                f'                }}\n'
                f'                catch (System.Exception ex)\n'
                f'                {{\n'
                f'                    results.Add(new MethodResult {{\n'
                f'                        MethodIndex = {idx},\n'
                f'                        MethodSubjectId = "{method_id_escaped}",\n'
                f'                        Status = "failed",\n'
                f'                        ExceptionMessage = ex.GetType().Name + ": " + ex.Message.Substring(0, Math.Min(ex.Message.Length, 100)),\n'
                f'                    }});\n'
                f'                }}\n'
                f'            }}'
            )

    sections_code = "\n".join(method_sections)

    # ── Generate harness code ───────────────────────────────────────────
    harness_code = (
        '// Auto-generated managed fact harness\n'
        f'// Family: {family_slug}, Assembly: {assembly}\n'
        'using System;\n'
        'using System.Collections.Generic;\n'
        'using System.Text.Json;\n'
        '\n'
        'class ManagedFactHarness\n'
        '{\n'
        '    struct MethodResult\n'
        '    {\n'
        '        public int MethodIndex { get; set; }\n'
        '        public string MethodSubjectId { get; set; }\n'
        '        public string Status { get; set; }\n'
        '        public string ExceptionMessage { get; set; }\n'
        '    }\n'
        '\n'
        '    static void Main()\n'
        '    {\n'
        '        var results = new List<MethodResult>();\n'
        f'{sections_code}\n'
        '        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });\n'
        '        Console.WriteLine(json);\n'
        '    }\n'
        '}\n'
    )
    harness_cs.write_text(harness_code, encoding="utf-8")

    return harness_csproj


def run_managed_fact(family_slug: str, assembly: str,
                     method_subject_ids: list[str],
                     tfm: str = "net8.0",
                     timeout: int = 300) -> dict[str, Any]:
    """Run .NET fact verification for the given family.

    Returns a dict with:
        status: "passed" | "failed" | "skipped" | "error"
        totalMethods: int
        passedMethods: int
        failedMethods: int
        skippedMethods: int
        methodResults: list[dict]
        tfm: str
        summary: str
    """
    start = time.perf_counter()

    result: dict[str, Any] = {
        "status": "error",
        "totalMethods": len(method_subject_ids),
        "passedMethods": 0,
        "failedMethods": 0,
        "skippedMethods": 0,
        "methodResults": [],
        "tfm": tfm,
        "summary": "",
        "duration_ms": 0,
    }

    # Generate harness
    csproj_path = _generate_fact_harness(family_slug, assembly, method_subject_ids, tfm=tfm)
    if csproj_path is None:
        result["summary"] = "Failed to generate harness"
        return result

    # Check if dotnet is available
    try:
        subprocess.run(["dotnet", "--version"], capture_output=True, text=True, timeout=30)
    except (OSError, subprocess.TimeoutExpired):
        result["status"] = "skipped"
        result["summary"] = "dotnet CLI not available"
        result["duration_ms"] = int((time.perf_counter() - start) * 1000)
        return result

    # Build and run
    try:
        r = subprocess.run(
            ["dotnet", "run", "--project", str(csproj_path), "--configuration", "Release", "-f", tfm],
            capture_output=True, text=True, timeout=timeout)
        if r.returncode != 0:
            result["status"] = "error"
            result["summary"] = f"dotnet run failed: {r.stderr[:200]}"
            result["duration_ms"] = int((time.perf_counter() - start) * 1000)
            return result

        # Parse JSON output
        output = r.stdout.strip()
        # Find JSON in output (handles potential extra output)
        json_start = output.find('{')
        if json_start >= 0:
            output = output[json_start:]
        parsed = json.loads(output)
        method_results = parsed.get("results", [])

        passed = sum(1 for mr in method_results if mr.get("status") == "passed")
        failed = sum(1 for mr in method_results if mr.get("status") in ("failed", "unexpected_pass"))
        skipped = sum(1 for mr in method_results if mr.get("status") == "skipped")

        result["status"] = "passed" if failed == 0 else "failed"
        result["passedMethods"] = passed
        result["failedMethods"] = failed
        result["skippedMethods"] = skipped
        result["methodResults"] = method_results
        result["summary"] = f"Fact {tfm}: passed={passed} failed={failed} skipped={skipped}"
        result["duration_ms"] = int((time.perf_counter() - start) * 1000)

    except subprocess.TimeoutExpired:
        result["status"] = "error"
        result["summary"] = f"dotnet run timed out after {timeout}s"
    except json.JSONDecodeError as e:
        result["status"] = "error"
        result["summary"] = f"JSON parse error: {e}"
    except OSError as e:
        result["status"] = "skipped"
        result["summary"] = f"OS error: {e}"

    result["duration_ms"] = int((time.perf_counter() - start) * 1000)
    return result


def run_managed_fact_for_family(family_slug: str, assembly: str,
                                tfm: str = "net8.0",
                                timeout: int = 300) -> dict[str, Any]:
    """Load contract methods and run managed fact verification."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    contract_path = family_dir / "capability-family-contract.json"

    if not contract_path.exists():
        return {
            "status": "skipped",
            "summary": f"Contract not found: {contract_path}",
            "totalMethods": 0,
        }

    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]

    if not mids:
        return {
            "status": "skipped",
            "summary": "No methods in contract",
            "totalMethods": 0,
        }

    return run_managed_fact(family_slug, assembly, mids, tfm=tfm, timeout=timeout)
