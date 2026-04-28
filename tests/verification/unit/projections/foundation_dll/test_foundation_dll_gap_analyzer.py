from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
GAP_ANALYZER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "gap_analyzer.py"


def _write_ledger(repo_root: Path) -> None:
    payload = {
        "schemaVersion": "1.0",
        "snapshotId": "snap-gap-candidate",
        "denominatorStrategy": "capability-family",
        "denominatorStatus": "candidate-derived",
        "dlls": [
            {
                "assemblyName": "System.Private.CoreLib",
                "families": [
                    {
                        "familyId": "family/System.Private.CoreLib/convert/char",
                        "displayName": "Convert.Char Conversions",
                        "denominatorStatus": "candidate-derived",
                        "closureStatus": "in-progress",
                        "methodCount": 18,
                        "capabilityFamilyEnum": "SystemPrivateCoreLib_ConvertChar",
                        "methodSubjectIds": [
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
                        ],
                        "testCode": {
                            "testCodeStatus": "present",
                        },
                        "verificationGates": {
                            "audit-input-and-ledger": "passed",
                            "managed-proof": "pending",
                            "native-proof": "pending",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                    },
                    {
                        "familyId": "family/System.Private.CoreLib/buffer/memory",
                        "displayName": "Buffer.Memory Operations",
                        "denominatorStatus": "candidate-derived",
                        "closureStatus": "in-progress",
                        "methodCount": 10,
                        "capabilityFamilyEnum": "SystemPrivateCoreLib_BufferMemory",
                        "methodSubjectIds": [
                            "System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)",
                        ],
                        "testCode": {
                            "testCodeStatus": "needs-tests",
                        },
                        "verificationGates": {
                            "audit-input-and-ledger": "passed",
                            "managed-proof": "pending",
                            "native-proof": "pending",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                    },
                    {
                        "familyId": "family/System.Private.CoreLib/object/equality-identity",
                        "displayName": "Object Equality And Identity",
                        "denominatorStatus": "candidate-derived",
                        "closureStatus": "in-progress",
                        "methodCount": 0,
                        "verificationGates": {
                            "audit-input-and-ledger": "passed",
                            "managed-proof": "pending",
                            "native-proof": "pending",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                    },
                ],
            }
        ],
    }
    write_json(
        repo_root / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.candidate.json",
        payload,
    )
    write_json(
        repo_root / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json",
        payload,
    )
    write_json(
        repo_root
        / "artifacts"
        / "subjects"
        / "SolutionCorePack"
        / "runs"
        / "20260427-fixture"
        / "analysis"
        / "generated"
        / "supplemental-full-closures"
        / "system-private-corelib"
        / "native-reference"
        / "native-reference.plan.json",
        {
            "formatVersion": "v0",
            "artifactKind": "nativeReferencePlan",
            "assemblyName": "System.Private.CoreLib",
            "translationUnitMethodSubjectIds": [
                "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
                "System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)",
            ],
        },
    )


def test_analyze_gaps_derives_test_code_status_and_can_write_back() -> None:
    gap_module = load_module(GAP_ANALYZER_MODULE_PATH, "chaos_foundation_dll_gap_analyzer")
    repo_root = make_temp_repo_root("verification", "foundation-dll-gap-analyzer")

    try:
        _write_ledger(repo_root)

        tests_root = repo_root / "tests" / "families" / "System.Private.CoreLib"
        (tests_root / "convert-char").mkdir(parents=True, exist_ok=True)
        (tests_root / "buffer-memory").mkdir(parents=True, exist_ok=True)

        result = gap_module.analyze_gaps(repo_root, scope="all", update_ledger=True)

        assert result["dllCount"] == 1
        assert result["familyCount"] == 3
        assert result["statusCounts"] == {
            "present": 1,
            "coverage-widened": 1,
            "needs-tests": 0,
            "no-coverage": 1,
        }

        by_family = {item["familyId"]: item for item in result["families"]}
        assert by_family["family/System.Private.CoreLib/convert/char"]["testCode"]["testCodeStatus"] == "present"
        assert by_family["family/System.Private.CoreLib/buffer/memory"]["testCode"]["testCodeStatus"] == "coverage-widened"
        assert by_family["family/System.Private.CoreLib/object/equality-identity"]["testCode"]["testCodeStatus"] == "no-coverage"

        ledger = json.loads(
            (repo_root / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.candidate.json").read_text(
                encoding="utf-8"
            )
        )
        families = {family["familyId"]: family for family in ledger["dlls"][0]["families"]}
        assert families["family/System.Private.CoreLib/convert/char"]["testCode"]["testCodeStatus"] == "present"
        assert families["family/System.Private.CoreLib/buffer/memory"]["testCode"]["testCodeStatus"] == "coverage-widened"
        assert families["family/System.Private.CoreLib/object/equality-identity"]["testCode"]["testCodeStatus"] == "no-coverage"
        formal_ledger = json.loads(
            (repo_root / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json").read_text(
                encoding="utf-8"
            )
        )
        formal_families = {family["familyId"]: family for family in formal_ledger["dlls"][0]["families"]}
        assert formal_families["family/System.Private.CoreLib/buffer/memory"]["testCode"]["testCodeStatus"] == "coverage-widened"
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)


def test_analyze_gaps_can_filter_single_family() -> None:
    gap_module = load_module(GAP_ANALYZER_MODULE_PATH, "chaos_foundation_dll_gap_analyzer_single")
    repo_root = make_temp_repo_root("verification", "foundation-dll-gap-analyzer-single")

    try:
        _write_ledger(repo_root)
        tests_root = repo_root / "tests" / "families" / "System.Private.CoreLib"
        (tests_root / "convert-char").mkdir(parents=True, exist_ok=True)

        result = gap_module.analyze_gaps(repo_root, scope="family/System.Private.CoreLib/convert/char", update_ledger=False)

        assert result["dllCount"] == 1
        assert result["familyCount"] == 1
        assert result["families"][0]["familyId"] == "family/System.Private.CoreLib/convert/char"
        assert result["families"][0]["testCode"]["testCodeStatus"] == "present"
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)


def test_analyze_gaps_auto_generate_also_emits_truth_skeleton() -> None:
    gap_module = load_module(GAP_ANALYZER_MODULE_PATH, "chaos_foundation_dll_gap_analyzer_truth")
    repo_root = make_temp_repo_root("verification", "foundation-dll-gap-analyzer-truth")

    try:
        _write_ledger(repo_root)

        result = gap_module.analyze_gaps(
            repo_root,
            scope="family/System.Private.CoreLib/convert/char",
            update_ledger=False,
            auto_generate=True,
        )

        truth_root = (
            repo_root
            / "verification"
            / "foundation-dll"
            / "System.Private.CoreLib"
            / "convert-char"
        )
        assert (truth_root / "dll-capability-manifest.json").is_file()
        assert (truth_root / "capability-family-contract.json").is_file()
        assert (truth_root / "method-capability-contracts.json").is_file()
        review_root = truth_root / "review"
        assert (review_root / "bundle.json").is_file()
        assert (review_root / "notes.md").is_file()
        assert (truth_root / "method-test-case-index.json").is_file()
        assert (truth_root / "method-benchmark-case-index.json").is_file()
        assert (truth_root / "method-hotupdate-case-index.json").is_file()
        assert (truth_root / "test" / "ConvertChar.Tests.csproj").is_file()
        assert (truth_root / "native" / "ConvertChar.Native.csproj").is_file()
        assert (truth_root / "benchmark" / "ConvertChar.Benchmark.csproj").is_file()
        assert (truth_root / "host" / "ConvertChar.Host.csproj").is_file()
        assert (truth_root / "patch" / "ConvertChar.Patch.csproj").is_file()
        solution_path = (
            repo_root
            / "subjects"
            / "SolutionCorePack"
            / "source"
            / "EngineeringScenarios"
            / "FoundationDllTranslationSolution"
            / "FoundationDllTranslationSolution.sln"
        )
        assert solution_path.is_file()
        solution_text = solution_path.read_text(encoding="utf-8")
        assert "convert-char.test" in solution_text
        assert "convert-char.benchmark" in solution_text
        assert "convert-char.host" in solution_text
        method_contracts = json.loads((truth_root / "method-capability-contracts.json").read_text(encoding="utf-8"))
        assert method_contracts["methods"]
        assert "verification/foundation-dll/System.Private.CoreLib/convert-char/dll-capability-manifest.json" in result["generatedArtifacts"]
        assert "verification/foundation-dll/System.Private.CoreLib/convert-char/review/bundle.json" in result["generatedArtifacts"]
        assert "verification/foundation-dll/System.Private.CoreLib/convert-char/method-test-case-index.json" in result["generatedArtifacts"]
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)


def test_analyze_gaps_auto_generate_scope_all_wires_multiple_families_into_solution() -> None:
    gap_module = load_module(GAP_ANALYZER_MODULE_PATH, "chaos_foundation_dll_gap_analyzer_all_solution")
    repo_root = make_temp_repo_root("verification", "foundation-dll-gap-analyzer-all-solution")

    try:
        _write_ledger(repo_root)
        tests_root = repo_root / "tests" / "families" / "System.Private.CoreLib"
        (tests_root / "convert-char").mkdir(parents=True, exist_ok=True)
        (tests_root / "buffer-memory").mkdir(parents=True, exist_ok=True)

        result = gap_module.analyze_gaps(
            repo_root,
            scope="all",
            update_ledger=False,
            auto_generate=True,
        )

        solution_path = (
            repo_root
            / "subjects"
            / "SolutionCorePack"
            / "source"
            / "EngineeringScenarios"
            / "FoundationDllTranslationSolution"
            / "FoundationDllTranslationSolution.sln"
        )
        assert solution_path.is_file()
        solution_text = solution_path.read_text(encoding="utf-8")
        assert "convert-char.test" in solution_text
        assert "buffer-memory.test" in solution_text
        assert result["familyCount"] == 3
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
