"""SDK test skeleton generator for foundation-dll families.

Reads the capability-family-contract.json from each family and generates
SDK-based skeleton test files that reference the subjects DLL via _exitCode.

Generated tests use:
  - [CapabilityTest(assembly/family)] on the class
  - [MethodSubjectId(methodSubjectId)] on each test method
  - Assert.Equal(0, SubjectsClass._exitCode) for verification
"""

from __future__ import annotations

import json
import os
from pathlib import Path


def _pascal_case(slug: str) -> str:
    """Convert kebab-case-slug to PascalCase."""
    return "".join(part[:1].upper() + part[1:] for part in slug.split("-") if part)


def _family_id_from_contract(contract_path: Path) -> str | None:
    """Extract the familyId from a capability-family-contract.json."""
    if not contract_path.exists():
        return None
    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)
    return contract.get("familyId")


def _method_subject_ids(contract: dict) -> list[str]:
    """Extract method subject IDs from a parsed contract."""
    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
    return mids


def generate_skeleton_tests(
    repo_root: Path,
    *,
    assembly_name: str = "System.Private.CoreLib",
    family_slug: str,
) -> dict[str, object]:
    """Generate SDK skeleton test files for a single family.

    Args:
        repo_root: Repository root path.
        assembly_name: Assembly name (e.g. "System.Private.CoreLib").
        family_slug: Family slug (e.g. "convert-char").

    Returns:
        Dictionary with results including paths of generated files.
    """
    # Paths
    verification_root = repo_root / "testing" / "foundation-dll" / assembly_name / family_slug
    contract_path = verification_root / "capability-family-contract.json"

    contract = _load_contract(contract_path)
    if contract is None:
        return {"family": family_slug, "generated": [], "error": "no contract found"}

    mids = _method_subject_ids(contract)
    if not mids:
        return {"family": family_slug, "generated": [], "error": "no method subject IDs in contract"}

    family_id = contract.get("familyId", f"{assembly_name}/{family_slug}")
    subjects_class_name = f"{_pascal_case(family_slug)}Subjects"

    # Output directories
    tests_dir = verification_root / "managed_test" / "tests"
    benchmarks_dir = verification_root / "managed_test" / "benchmarks"

    # Generate test file
    tests_dir.mkdir(parents=True, exist_ok=True)
    test_file = tests_dir / f"{subjects_class_name}Tests.cs"
    test_code = _build_test_class(subjects_class_name, family_id, mids)
    test_file.write_text(test_code, encoding="utf-8")

    # Generate benchmark file
    benchmarks_dir.mkdir(parents=True, exist_ok=True)
    benchmark_file = benchmarks_dir / f"{subjects_class_name}Benchmarks.cs"
    benchmark_code = _build_benchmark_class(subjects_class_name, family_id, mids)
    benchmark_file.write_text(benchmark_code, encoding="utf-8")

    generated = [
        str(test_file.relative_to(repo_root)),
        str(benchmark_file.relative_to(repo_root)),
    ]

    return {
        "family": family_slug,
        "generated": generated,
        "method_count": len(mids),
        "error": None,
    }


def _load_contract(contract_path: Path) -> dict | None:
    """Load and return a capability family contract, or None."""
    if not contract_path.exists():
        return None
    with open(contract_path, encoding="utf-8") as f:
        return json.load(f)


def _build_test_class(class_name: str, family_id: str, mids: list[str]) -> str:
    """Build the C# test class skeleton."""
    lines = [
        "// Auto-generated SDK test skeleton",
        "// Do not modify — re-generate via foundation-dll pipeline",
        "",
        "using Chaos.TestFramework;",
        "using Xunit;",
        "",
        f"namespace {class_name}Tests;",
        "",
        f'[CapabilityTest("{family_id}")]',
        f"public class {class_name}Tests",
        "{",
    ]

    for i, mid in enumerate(mids):
        method_name = f"Subject_{i}"
        lines.extend([
            "    [Fact]",
            f"    [MethodSubjectId(\"{mid}\")]",
            f"    public void {method_name}()",
            "    {",
            f"        {class_name}._exitCode = 0;",
            f"        {class_name}.{method_name}();",
            f"        Assert.Equal(0, {class_name}._exitCode);",
            "    }",
            "",
        ])

    lines.append("}")
    return "\n".join(lines)


def _build_benchmark_class(class_name: str, family_id: str, mids: list[str]) -> str:
    """Build the C# benchmark class skeleton."""
    lines = [
        "// Auto-generated SDK benchmark skeleton",
        "// Do not modify — re-generate via foundation-dll pipeline",
        "",
        "using Chaos.TestFramework;",
        "",
        f"namespace {class_name}Benchmarks;",
        "",
        f'[CapabilityTest("{family_id}")]',
        f"public class {class_name}Benchmarks",
        "{",
    ]

    for i, mid in enumerate(mids):
        method_name = f"Subject_{i}"
        label = mid.split("::", 1)[1] if "::" in mid else mid
        lines.extend([
            "    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,",
            "        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]",
            f"    [BenchmarkSubjectId(\"{mid}\")]",
            f"    public void {method_name}()",
            "    {",
            f"        {class_name}.{method_name}();",
            "    }",
            "",
        ])

    lines.append("}")
    return "\n".join(lines)


def generate_all_families(
    repo_root: Path,
    *,
    assembly_name: str = "System.Private.CoreLib",
) -> list[dict[str, object]]:
    """Generate skeleton tests for all families in an assembly.

    Scans for capability-family-contract.json files under the assembly's
    verification directory.
    """
    asm_dir = repo_root / "testing" / "foundation-dll" / assembly_name
    if not asm_dir.exists():
        return []

    results = []
    for d in sorted(asm_dir.iterdir()):
        if d.is_dir() and (d / "capability-family-contract.json").exists():
            result = generate_skeleton_tests(
                repo_root,
                assembly_name=assembly_name,
                family_slug=d.name,
            )
            results.append(result)

    return results


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate SDK test skeletons")
    parser.add_argument("--repo-root", default=str(Path(__file__).resolve().parent.parent.parent.parent.parent))
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--family", help="Single family slug (omit for all)")
    args = parser.parse_args()

    repo_root = Path(args.repo_root)
    if args.family:
        results = [generate_skeleton_tests(repo_root, assembly_name=args.assembly, family_slug=args.family)]
    else:
        results = generate_all_families(repo_root, assembly_name=args.assembly)

    total = 0
    for r in results:
        if r.get("error"):
            print(f"  {r['family']:35s} SKIPPED: {r['error']}")
        else:
            print(f"  {r['family']:35s} {r['method_count']:4d} methods -> {len(r['generated'])} files")
            total += r['method_count']
    print(f"\nGenerated skeletons for {total} methods across {len(results)} families")
