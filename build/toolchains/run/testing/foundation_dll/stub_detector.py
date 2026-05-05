"""
Stub Detector: Scans generated native C++ files and classifies methods
as stub (skeleton) or real (genuine IL translation).

Detection patterns:
  - RuntimeSkeletonPage*.cpp → always stub
  - native-aot.generated.cpp with "NativeReferenceStub_" → stub
  - BenchmarkNativeEntry.cpp with "return 42" → stub
  - Any generated file with "CHAOS_BRIDGE_STATUS_OK" → stub

Usage:
    python stub_detector.py --family <family-dir>          # single family
    python stub_detector.py --assembly <assembly-dir>      # whole DLL
    python stub_detector.py --all                           # all foundation DLLs
    python stub_detector.py --report                        # output JSON report
"""

from __future__ import annotations

import json
import re
import sys
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any

_VERIFICATION_BASE = Path(__file__).resolve().parents[5] / "verification" / "foundation-dll"

# ── Stub detection patterns ────────────────────────────────────────────

STUB_FUNCTION_PATTERNS: list[re.Pattern] = [
    re.compile(r"return\s+CHAOS_BRIDGE_STATUS_OK\s*;"),
    re.compile(r"NativeReferenceStub_\w+\s*\("),
    re.compile(r"return\s+static_cast<CHAOS_IL2CPP_INT32>\s*\(\s*42\s*\)\s*;"),
    re.compile(r"//\s*SKIPPED:"),
]

REAL_CODE_PATTERNS: list[re.Pattern] = [
    re.compile(r"chaos_eval_stack\s*\["),
    re.compile(r"chaos_stack_top"),
    re.compile(r"Emit\w+\("),
    re.compile(r"callvirt|ldfld|stfld"),
]

SKELETON_FILE_PATTERNS: list[re.Pattern] = [
    re.compile(r"RuntimeSkeletonPage\d+\.cpp$"),
    re.compile(r"RuntimeSkeletonSummary\.cpp$"),
]


@dataclass
class StubMethodInfo:
    method_name: str
    subject_id: str | None = None
    is_stub: bool = True
    stub_pattern: str = ""
    line_number: int = 0


@dataclass
class StubFileResult:
    file_path: str
    file_kind: str  # "native-aot" | "runtime-skeleton" | "benchmark" | "hotupdate" | "other"
    total_methods: int = 0
    stub_methods: int = 0
    real_methods: int = 0
    methods: list[StubMethodInfo] = field(default_factory=list)

    @property
    def is_pure_stub(self) -> bool:
        return self.total_methods > 0 and self.stub_methods == self.total_methods

    @property
    def is_pure_real(self) -> bool:
        return self.total_methods > 0 and self.real_methods == self.total_methods


@dataclass
class FamilyStubResult:
    family_id: str
    assembly_name: str
    family_slug: str
    files: list[StubFileResult] = field(default_factory=list)
    has_native_aot: bool = False
    has_runtime_skeleton: bool = False
    has_benchmark: bool = False
    has_hotupdate: bool = False

    @property
    def native_aot_file(self) -> StubFileResult | None:
        for f in self.files:
            if f.file_kind == "native-aot":
                return f
        return None

    @property
    def classification(self) -> str:
        """Return one of: genuine, partial, skeleton-only, no-code."""
        native = self.native_aot_file
        if native is None:
            return "no-code"
        if native.is_pure_real:
            return "genuine"
        if native.is_pure_stub:
            return "skeleton-only"
        return "partial"


def _classify_file(path: Path) -> str:
    name = path.name
    for pat in SKELETON_FILE_PATTERNS:
        if pat.search(str(path)):
            return "runtime-skeleton"
    if "BenchmarkNativeEntry" in name:
        return "benchmark"
    if "HotUpdateTest" in name:
        return "hotupdate"
    if "native-aot.generated" in name:
        return "native-aot"
    if "native-aot.patch.generated" in name:
        return "hotupdate-patch"
    return "other"


def _extract_method_name(line: str) -> str:
    """Extract C++ function name from a definition line."""
    m = re.match(r"^(?:static\s+)?(?:inline\s+)?(?:\w+(?:::\w+)*\s+)?(\w+)\s*\(", line)
    if m:
        return m.group(1)
    # Match CHAOS_RUNTIME_ABI_CALL style
    m = re.search(r"(?:NativeReferenceStub_|BenchmarkEntry_|HotUpdateStub_|Run)\w*", line)
    if m:
        return m.group(0)
    return ""


def _extract_subject_id(lines: list[str], func_start: int) -> str | None:
    """Look for a subject ID comment or comment above the function."""
    for i in range(max(0, func_start - 3), func_start):
        m = re.search(r"(?:Stub for|Method:)\s+(.+)$", lines[i])
        if m:
            return m.group(1).strip()
    return None


def scan_file(path: Path) -> StubFileResult:
    """Scan a single .cpp file for stub methods."""
    file_kind = _classify_file(path)
    result = StubFileResult(
        file_path=str(path.as_posix()),
        file_kind=file_kind,
    )

    try:
        text = path.read_text(encoding="utf-8")
    except (OSError, UnicodeDecodeError):
        return result

    lines = text.split("\n")

    # For skeleton files, every function is a stub
    if file_kind == "runtime-skeleton":
        func_starts: list[int] = []
        for i, line in enumerate(lines):
            stripped = line.strip()
            if "NativeReferenceStub_" in stripped or "NativeReferenceAssemblyStubFn" in stripped:
                continue
            if stripped.startswith("int32_t CHAOS_RUNTIME_ABI_CALL") or "NativeReferenceStub_" in stripped:
                func_starts.append(i)
                method_name = _extract_method_name(stripped)
                if method_name:
                    subject_id = _extract_subject_id(lines, i)
                    result.methods.append(StubMethodInfo(
                        method_name=method_name,
                        subject_id=subject_id,
                        is_stub=True,
                        stub_pattern="runtime-skeleton",
                        line_number=i + 1,
                    ))
                    result.total_methods += 1
                    result.stub_methods += 1
        return result

    # For native-aot / benchmark / other files, scan function by function
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Skip non-function lines
        if not any(x in stripped for x in [
            "NativeReferenceStub_", "BenchmarkEntry_", "HotUpdateStub_",
            "RunNativeAot", "RunNativeReference", "extern \"C\"",
        ]):
            i += 1
            continue

        # Skip type aliases and comments
        if "NativeReferenceAssemblyStubFn" in stripped:
            i += 1
            continue

        method_name = _extract_method_name(stripped)
        if not method_name:
            i += 1
            continue

        subject_id = _extract_subject_id(lines, i)
        func_start = i

        # Find the function body and analyze it
        brace_depth = 0
        in_body = False
        body_start = i
        for j in range(i, len(lines)):
            if "{" in lines[j]:
                if not in_body:
                    in_body = True
                    body_start = j
                brace_depth += lines[j].count("{")
            if "}" in lines[j]:
                brace_depth -= lines[j].count("}")
                if in_body and brace_depth <= 0:
                    body_end = j
                    break
        else:
            body_end = len(lines) - 1

        body_text = "\n".join(lines[body_start:body_end + 1]) if in_body else ""

        is_stub = False
        stub_pattern = ""
        for pat in STUB_FUNCTION_PATTERNS:
            if pat.search(body_text):
                is_stub = True
                stub_pattern = pat.pattern
                break

        result.methods.append(StubMethodInfo(
            method_name=method_name,
            subject_id=subject_id,
            is_stub=is_stub,
            stub_pattern=stub_pattern,
            line_number=func_start + 1,
        ))
        result.total_methods += 1
        if is_stub:
            result.stub_methods += 1
        else:
            result.real_methods += 1

        i = body_end + 1

    return result


def scan_family(family_path: Path) -> FamilyStubResult:
    """Scan all .cpp files in a family directory."""
    # Determine family_id from the path structure
    try:
        rel = family_path.relative_to(_VERIFICATION_BASE)
        parts = rel.parts
    except ValueError:
        parts = family_path.parts[-2:] if len(family_path.parts) >= 2 else (family_path.name,)
    assembly_name = parts[0] if parts else family_path.name
    family_slug = parts[1] if len(parts) > 1 else family_path.name
    family_id = f"family/{assembly_name}/{family_slug}"

    result = FamilyStubResult(
        family_id=family_id,
        assembly_name=assembly_name,
        family_slug=family_slug,
    )

    for cpp_file in sorted(family_path.rglob("*.cpp")):
        # Skip CMake-generated or build artifacts
        if "_build" in str(cpp_file) or "_phase_c_build" in str(cpp_file):
            continue

        file_result = scan_file(cpp_file)
        if file_result.total_methods > 0:
            result.files.append(file_result)

            if file_result.file_kind == "native-aot":
                result.has_native_aot = True
            elif file_result.file_kind == "runtime-skeleton":
                result.has_runtime_skeleton = True
            elif file_result.file_kind == "benchmark":
                result.has_benchmark = True
            elif file_result.file_kind in ("hotupdate", "hotupdate-patch"):
                result.has_hotupdate = True

    return result


def scan_assembly(assembly_path: Path) -> dict[str, FamilyStubResult]:
    """Scan all families in a DLL assembly directory."""
    results: dict[str, FamilyStubResult] = {}
    for item in sorted(assembly_path.iterdir()):
        if item.is_dir() and not item.name.startswith("_"):
            family_result = scan_family(item)
            if family_result.files:
                results[family_result.family_slug] = family_result
    return results


def scan_all() -> dict[str, dict[str, FamilyStubResult]]:
    """Scan all foundation DLLs."""
    results: dict[str, dict[str, FamilyStubResult]] = {}
    if not _VERIFICATION_BASE.exists():
        return results
    for item in sorted(_VERIFICATION_BASE.iterdir()):
        if item.is_dir() and not item.name.startswith("_"):
            assembly_results = scan_assembly(item)
            if assembly_results:
                results[item.name] = assembly_results
    return results


def _family_to_dict(result: FamilyStubResult) -> dict[str, Any]:
    return {
        "familyId": result.family_id,
        "assemblyName": result.assembly_name,
        "familySlug": result.family_slug,
        "classification": result.classification,
        "hasNativeAot": result.has_native_aot,
        "hasRuntimeSkeleton": result.has_runtime_skeleton,
        "hasBenchmark": result.has_benchmark,
        "hasHotupdate": result.has_hotupdate,
        "files": [asdict(f) for f in result.files],
    }


def generate_report(results: dict[str, dict[str, FamilyStubResult]]) -> dict[str, Any]:
    """Generate a summary report from scan results."""
    summary = {
        "generatedAt": __import__("datetime").datetime.now().isoformat(),
        "totalAssemblies": 0,
        "totalFamilies": 0,
        "genuine": 0,
        "partial": 0,
        "skeletonOnly": 0,
        "noCode": 0,
        "assemblies": {},
    }

    for assembly_name, families in sorted(results.items()):
        summary["totalAssemblies"] += 1
        asm_entry = {
            "totalFamilies": len(families),
            "families": {},
        }
        for slug, family_result in sorted(families.items()):
            summary["totalFamilies"] += 1
            cls = family_result.classification
            if cls == "genuine":
                summary["genuine"] += 1
            elif cls == "partial":
                summary["partial"] += 1
            elif cls == "skeleton-only":
                summary["skeletonOnly"] += 1
            else:
                summary["noCode"] += 1
            asm_entry["families"][slug] = _family_to_dict(family_result)
        summary["assemblies"][assembly_name] = asm_entry

    return summary


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Stub Detector for foundation DLLs")
    parser.add_argument("--family", help="Path to a single family directory")
    parser.add_argument("--assembly", help="Path to an assembly directory")
    parser.add_argument("--all", action="store_true", help="Scan all foundation DLLs")
    parser.add_argument("--report", action="store_true", help="Output JSON report")
    parser.add_argument("-o", "--output", help="Output file for JSON report")

    args = parser.parse_args()

    if args.family:
        family_path = Path(args.family)
        if not family_path.is_absolute():
            family_path = family_path.resolve()
        result = scan_family(family_path)
        print(f"Family: {result.family_id}")
        print(f"  Classification: {result.classification}")
        print(f"  Native AOT: {'yes' if result.has_native_aot else 'no'}")
        print(f"  Runtime Skeleton: {'yes' if result.has_runtime_skeleton else 'no'}")
        print(f"  Benchmark: {'yes' if result.has_benchmark else 'no'}")
        print(f"  HotUpdate: {'yes' if result.has_hotupdate else 'no'}")
        print(f"  Files with methods: {len(result.files)}")
        for f in result.files:
            print(f"    {f.file_kind}: {f.file_path}")
            print(f"      {f.total_methods} methods, {f.stub_methods} stubs, {f.real_methods} real")
        if args.report:
            report_entry = _family_to_dict(result)
            report = {"singleFamily": report_entry}
            print(json.dumps(report, indent=2, ensure_ascii=False))

    elif args.assembly:
        asm_path = Path(args.assembly)
        if not asm_path.is_absolute():
            asm_path = asm_path.resolve()
        results = scan_assembly(asm_path)
        print(f"Assembly: {asm_path.name}")
        for slug, result in sorted(results.items()):
            print(f"  {slug}: {result.classification} (native-aot={result.has_native_aot}, skeleton={result.has_runtime_skeleton})")
        if args.report:
            report = generate_report({asm_path.name: results})
            print(json.dumps(report, indent=2, ensure_ascii=False))

    elif args.all:
        results = scan_all()
        report = generate_report(results)
        total = report["totalFamilies"]
        genuine = report["genuine"]
        partial = report["partial"]
        skeleton = report["skeletonOnly"]
        no_code = report["noCode"]
        print(f"Foundation DLL Stub Scan Report")
        print(f"{'=' * 60}")
        print(f"Total families: {total}")
        print(f"  Genuine:       {genuine:3d} ({genuine * 100 // total if total else 0:2d}%)")
        print(f"  Partial:       {partial:3d} ({partial * 100 // total if total else 0:2d}%)")
        print(f"  Skeleton-only: {skeleton:3d} ({skeleton * 100 // total if total else 0:2d}%)")
        print(f"  No code:       {no_code:3d} ({no_code * 100 // total if total else 0:2d}%)")
        print()
        for asm_name, asm_data in sorted(report["assemblies"].items()):
            fams = asm_data["families"]
            cls_counts = {}
            for f in fams.values():
                cls_counts[f["classification"]] = cls_counts.get(f["classification"], 0) + 1
            print(f"  {asm_name}: {asm_data['totalFamilies']} families {cls_counts}")
        print()

        if args.report:
            if args.output:
                Path(args.output).write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
                print(f"Report written to: {args.output}")
            else:
                print(json.dumps(report, indent=2, ensure_ascii=False))

    else:
        parser.print_help()


if __name__ == "__main__":
    main()
