from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
GENERATOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll_audit_generator.py"


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _corelib_fixture_coverage() -> dict:
    convert_char = [
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
    ]
    buffer_memory = [
        "System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)",
        "System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)",
        "System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)",
        "System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)",
        "System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)",
        "System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)",
        "System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)",
        "System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)",
        "System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)",
        "System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)",
    ]
    enum_parsing = [
        "System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)",
        "System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)",
        "System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)",
        "System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)",
        "System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)",
        "System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)",
        "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)",
        "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)",
        "System.Private.CoreLib/System.Enum::ToString:System.String()",
        "System.Private.CoreLib/System.Enum::ToString:System.String(System.String)",
        "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)",
        "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)",
    ]
    uncovered = [*buffer_memory, *enum_parsing]
    return {
        "formatVersion": "v0",
        "artifactKind": "nativeReferenceRuntimeSkeletonCoverage",
        "assemblyName": "System.Private.CoreLib",
        "planKind": "assembly-full-closure-runtime-skeleton",
        "translationUnitMode": "runtime-skeleton",
        "requestedMethodCount": 40,
        "emittedMethodCount": len(convert_char),
        "uncoveredMethodCount": len(uncovered),
        "uncoveredReasonCounts": {"unsupportedShapeOrCapability": len(uncovered)},
        "uncoveredMethodSubjectIds": uncovered,
        "uncoveredMethods": [
            {"subjectId": subject_id, "reasonCode": "unsupportedShapeOrCapability"}
            for subject_id in uncovered
        ],
    }


def _corelib_fixture_native_reference_plan() -> dict:
    convert_char = [
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)",
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
    ]
    buffer_memory = [
        "System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)",
        "System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)",
        "System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)",
        "System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)",
        "System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)",
        "System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)",
        "System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)",
        "System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)",
        "System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)",
        "System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)",
    ]
    enum_parsing = [
        "System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)",
        "System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)",
        "System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)",
        "System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)",
        "System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)",
        "System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)",
        "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)",
        "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)",
        "System.Private.CoreLib/System.Enum::ToString:System.String()",
        "System.Private.CoreLib/System.Enum::ToString:System.String(System.String)",
        "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)",
        "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)",
    ]
    all_ids = [*convert_char, *buffer_memory, *enum_parsing]
    return {
        "formatVersion": "v0",
        "artifactKind": "nativeReferencePlan",
        "planKind": "assembly-full-closure-runtime-skeleton",
        "assemblyName": "System.Private.CoreLib",
        "translationUnitMode": "runtime-skeleton",
        "translationUnitMethodSubjectIds": all_ids,
        "translationUnitMethodCount": len(all_ids),
        "translationUnitPageSize": len(all_ids),
        "translationUnitPageCount": 1,
        "translationUnitPages": [],
        "auditStatus": "runtime-skeleton",
        "auditMessage": "",
    }


def _program_manifest() -> dict:
    return {
        "schemaVersion": 1,
        "programId": "foundation-dll-translation-audit",
        "title": "Foundation DLL Translation Audit",
        "roadmapPath": "docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md",
        "designPath": "docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md",
        "statusPath": "docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md",
        "authorityTaskId": "20260427-04-dll-capability-verification-closure-roadmap",
        "authorityPhase": "roadmap-closed",
        "scopeSource": "capability-family-ledger",
        "subjectEntry": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
        "projectTemplates": [
            {
                "code": "audit-input-and-ledger",
                "displayName": "Audit Input And Ledger",
                "defaultPolicyState": "required",
                "verificationMethod": "刷新 audit input manifest，并校验 surface / semantic / nativeization plan。",
                "artifactKeywords": ["input-manifest", "surface-ledger", "semantic-ledger", "nativeization-plan", "audit"],
            },
            {
                "code": "managed-proof",
                "displayName": "Managed Proof",
                "defaultPolicyState": "required",
                "verificationMethod": "运行 managed proof，验证语义基线。",
                "artifactKeywords": ["managed-proof", "proof"],
            },
            {
                "code": "native-proof",
                "displayName": "Native Proof",
                "defaultPolicyState": "required",
                "verificationMethod": "运行 native / AOT proof，验证 generated native code 可执行。",
                "artifactKeywords": ["native-proof", "native-aot", "native-reference", "runtime-skeleton", "generated/runtime"],
            },
            {
                "code": "hotupdate-proof",
                "displayName": "HotUpdate Proof",
                "defaultPolicyState": "conditional",
                "verificationMethod": "运行 patch / host proof，验证 hotupdate consumer 路径。",
                "artifactKeywords": ["hotupdate-proof", "hot-update", "hotupdate"],
            },
            {
                "code": "benchmark",
                "displayName": "Benchmark",
                "defaultPolicyState": "conditional",
                "verificationMethod": "运行 benchmark，归档性能结果。",
                "artifactKeywords": ["benchmark"],
            },
            {
                "code": "codegen-review",
                "displayName": "Codegen Review",
                "defaultPolicyState": "required",
                "verificationMethod": "审核 generated code、dispatch/helper/binding/coverage。",
                "artifactKeywords": ["codegen-review", "generated/audit", "generated/runtime"],
            },
            {
                "code": "completion-certification",
                "displayName": "Completion Certification",
                "defaultPolicyState": "required",
                "verificationMethod": "汇总前置 project 结果，给出 DLL completion 结论。",
                "artifactKeywords": ["completion-report", "completion", "STATUS.md"],
            },
        ],
        "assemblies": [
            {
                "assemblyName": "System.Private.CoreLib",
                "orderIndex": 1,
                "phase": "phase-1",
                "roadmapTaskId": "20260419-03-system-private-corelib-full-verification",
                "followupTaskIds": [
                    "20260419-20-corelib-supplemental-full-closure-audit-integration",
                    "20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening",
                ],
                "riskTags": ["core-foundation"],
                "projectPolicies": {
                    "hotupdate-proof": "required",
                    "benchmark": "conditional",
                },
            },
            {
                "assemblyName": "System.Collections.Immutable",
                "orderIndex": 2,
                "phase": "phase-2",
                "roadmapTaskId": "20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable",
                "riskTags": [],
                "projectPolicies": {
                    "hotupdate-proof": "conditional",
                    "benchmark": "conditional",
                },
            },
        ],
    }


def _write_fixture_repo(repo_root: Path) -> None:
    write_json(
        repo_root / "verification" / "catalog" / "programs" / "foundation-dll-translation-audit.program.json",
        _program_manifest(),
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-01-foundation-dll-translation-audit-roadmap" / "design-v1-01.md",
        "# design\n",
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-01-foundation-dll-translation-audit-roadmap" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-01-foundation-dll-translation-audit-roadmap",
                "title: Foundation DLL Translation Audit Roadmap",
                "lifecycle_status: in-progress",
                "phase: roadmap",
                "---",
                "",
                "## 当前判断",
                "",
                "- DLL-first 审核与报告模型已拍板。",
                "",
            ]
        )
        + "\n",
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-01-foundation-dll-translation-audit-roadmap" / "roadmap-v1-01.md",
        "\n".join(
            [
                "# Roadmap",
                "",
                "| task_id | phase | status | owner | purpose | depends_on |",
                "| --- | --- | --- | --- | --- | --- |",
                "| `20260419-03-system-private-corelib-full-verification` | `phase-1` | `completed-blocked` | `codex` | CoreLib full verification remains blocked by translated coverage gaps. | `20260419-02` |",
                "| `20260419-20-corelib-supplemental-full-closure-audit-integration` | `phase-1` | `completed` | `codex` | Supplemental full-closure evidence connected. | `20260419-07` |",
                "| `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening` | `phase-1` | `in-progress` | `codex` | Continue widening translated coverage. | `20260419-20` |",
                "| `20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable` | `phase-2` | `blocked` | `codex` | Waits for CoreLib full verification. | `20260419-03` |",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "docs" / "dev" / "completed" / "20260419-03-system-private-corelib-full-verification" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-03-system-private-corelib-full-verification",
                "title: System.Private.CoreLib Full Verification",
                "---",
                "",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/supplemental-full-closures/system-private-corelib/native-aot/native-aot.plan.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/supplemental-full-closures/system-private-corelib/hot-update/`",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "docs" / "dev" / "completed" / "20260419-20-corelib-supplemental-full-closure-audit-integration" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-20-corelib-supplemental-full-closure-audit-integration",
                "title: CoreLib Supplemental Full Closure Audit Integration",
                "---",
                "",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/audit/System.Private.CoreLib/managed-proof-report.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/audit/System.Private.CoreLib/codegen-review.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/audit/System.Private.CoreLib/codegen-review.md`",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening",
                "title: CoreLib Supplemental Runtime Skeleton Coverage Widening",
                "---",
                "",
                "next_step: Continue widening translated coverage.",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "subjects" / "SolutionCorePack" / "source" / "EngineeringScenarios" / "FoundationDllTranslationSolution" / "FoundationDllTranslationSolution.sln",
        "Microsoft Visual Studio Solution File\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "managed-proof-report.json",
        "{}\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "codegen-review.json",
        "{}\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "codegen-review.md",
        "# review\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "analysis" / "generated" / "supplemental-full-closures" / "system-private-corelib" / "native-reference" / "generated" / "runtime" / "native-reference.runtime-skeleton.coverage.json",
        json.dumps(_corelib_fixture_coverage(), ensure_ascii=False, indent=2) + "\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "analysis" / "generated" / "supplemental-full-closures" / "system-private-corelib" / "native-reference" / "native-reference.plan.json",
        json.dumps(_corelib_fixture_native_reference_plan(), ensure_ascii=False, indent=2) + "\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "analysis" / "generated" / "supplemental-full-closures" / "system-private-corelib" / "native-aot" / "native-aot.plan.json",
        "{}\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "hotupdate-proof-report.json",
        "{}\n",
    )
    write_json(
        repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char" / "method-test-case-index.json",
        {
            "schemaVersion": 1,
            "cases": [
                {
                    "methodSubjectId": "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                    "familyId": "family/System.Private.CoreLib/convert/char",
                    "caseKind": "functional-test",
                    "projectPath": "verification/foundation-dll/System.Private.CoreLib/convert-char/test",
                    "sourceFile": "verification/foundation-dll/System.Private.CoreLib/convert-char/test/ConvertCharTests.AutoGenerated.cs",
                    "className": "ConvertCharTests",
                    "memberName": "Method_System_Convert_ToChar_System_Boolean",
                    "routeCode": "Native",
                    "profileCode": "",
                }
            ],
        },
    )
    write_json(
        repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char" / "method-benchmark-case-index.json",
        {
            "schemaVersion": 1,
            "cases": [
                {
                    "methodSubjectId": "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                    "familyId": "family/System.Private.CoreLib/convert/char",
                    "caseKind": "benchmark",
                    "projectPath": "verification/foundation-dll/System.Private.CoreLib/convert-char/benchmark",
                    "sourceFile": "verification/foundation-dll/System.Private.CoreLib/convert-char/benchmark/ConvertCharBenchmarks.AutoGenerated.cs",
                    "className": "ConvertCharBenchmarks",
                    "memberName": "Method_System_Convert_ToChar_System_Boolean",
                    "routeCode": "Native",
                    "profileCode": "Default",
                }
            ],
        },
    )
    write_json(
        repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char" / "method-hotupdate-case-index.json",
        {
            "schemaVersion": 1,
            "cases": [
                {
                    "methodSubjectId": "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                    "familyId": "family/System.Private.CoreLib/convert/char",
                    "caseKind": "hotupdate",
                    "projectPath": "verification/foundation-dll/System.Private.CoreLib/convert-char/host",
                    "sourceFile": "verification/foundation-dll/System.Private.CoreLib/convert-char/host/ConvertCharHotUpdate.AutoGenerated.cs",
                    "className": "ConvertCharHotUpdate",
                    "memberName": "Method_System_Convert_ToChar_System_Boolean",
                    "direction": "HostToPatch",
                }
            ],
        },
    )
    write_json(
        repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char" / "review" / "bundle.json",
        {
            "schemaVersion": 1,
            "assemblyName": "System.Private.CoreLib",
            "familyId": "family/System.Private.CoreLib/convert/char",
            "displayName": "Convert.Char Conversions",
            "generatedArtifacts": [
                "verification/foundation-dll/System.Private.CoreLib/convert-char/method-test-case-index.json"
            ],
            "solutionPath": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
            "notesPath": "verification/foundation-dll/System.Private.CoreLib/convert-char/review/notes.md",
        },
    )
    _write_text(
        repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char" / "review" / "notes.md",
        "# Review Notes\n",
    )


def _capability_ledger() -> dict:
    return {
        "schemaVersion": "1.0",
        "snapshotId": "snap-20260426-manual",
        "snapshotAt": "2026-04-26T00:00:00+08:00",
        "authoritySnapshotOf": "foundation-dll-translation-audit",
        "denominatorStrategy": "capability-family",
        "denominatorStatus": "candidate-derived",
        "originalAuthorityDigest": {
            "inputManifestVersion": "v1",
            "surfaceLedgerVersion": "v1",
            "semanticLedgerVersion": "v1",
            "nativeizationPlanVersion": "v1",
        },
        "dlls": [
            {
                "assemblyName": "System.Private.CoreLib",
                "denominatorStatus": "candidate-derived",
                "sourceLinks": {
                    "subjectSource": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
                    "generatedCode": "artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/",
                    "evidence": "artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/",
                    "verificationSource": "verification/catalog/programs/foundation-dll-translation-audit.program.json",
                },
                "families": [
                    {
                        "familyId": "family/System.Private.CoreLib/convert/char",
                        "displayName": "Convert.Char Conversions",
                        "description": "char-related type conversions in System.Convert",
                        "denominatorStatus": "candidate-derived",
                        "closureStatus": "in-progress",
                        "verificationGates": {
                            "audit-input-and-ledger": "passed",
                            "managed-proof": "passed",
                            "native-proof": "in-progress",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                        "methodCount": 18,
                        "capabilityFamilyEnum": "SystemPrivateCoreLib_ConvertChar",
                        "methodSubjectIds": [
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)",
                            "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
                        ],
                        "testCode": {
                            "testCodeStatus": "present",
                            "requestedMethodCount": 18,
                            "emittedMethodCount": 18,
                            "allMethodsCovered": True,
                            "action": "none",
                        },
                        "implementationFamilies": [
                            {"familyId": "impl/convert/char", "displayName": "Char conversions", "handlerName": "ConvertChar"}
                        ],
                        "sourceGroups": [{"groupId": "src/System/Convert", "typeNames": ["Convert"], "methodCount": 18}],
                    },
                    {
                        "familyId": "family/System.Private.CoreLib/buffer/memory",
                        "displayName": "Buffer.Memory Operations",
                        "description": "Buffer and Memory-related operations",
                        "denominatorStatus": "candidate-derived",
                        "closureStatus": "in-progress",
                        "verificationGates": {
                            "audit-input-and-ledger": "passed",
                            "managed-proof": "passed",
                            "native-proof": "in-progress",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                        "methodCount": 10,
                        "capabilityFamilyEnum": "SystemPrivateCoreLib_BufferMemory",
                        "methodSubjectIds": [
                            "System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)",
                            "System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)",
                            "System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)",
                            "System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)",
                            "System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)",
                            "System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)",
                            "System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)",
                            "System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)",
                            "System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)",
                            "System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)",
                        ],
                        "testCode": {
                            "testCodeStatus": "coverage-widened",
                            "requestedMethodCount": 10,
                            "emittedMethodCount": 10,
                            "allMethodsCovered": False,
                            "action": "update-tests",
                        },
                        "implementationFamilies": [
                            {"familyId": "impl/buffer/memory", "displayName": "Buffer memory ops", "handlerName": "BufferMemory"}
                        ],
                    },
                    {
                        "familyId": "family/System.Private.CoreLib/enum/parsing",
                        "displayName": "Enum Parsing And Formatting",
                        "description": "Enum.ToString, Enum.Parse, Enum.IsDefined and related",
                        "denominatorStatus": "candidate-derived",
                        "closureStatus": "in-progress",
                        "verificationGates": {
                            "audit-input-and-ledger": "passed",
                            "managed-proof": "passed",
                            "native-proof": "in-progress",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                        "methodCount": 12,
                        "capabilityFamilyEnum": "SystemPrivateCoreLib_EnumParsing",
                        "methodSubjectIds": [
                            "System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)",
                            "System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)",
                            "System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)",
                            "System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)",
                            "System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)",
                            "System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)",
                            "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)",
                            "System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)",
                            "System.Private.CoreLib/System.Enum::ToString:System.String()",
                            "System.Private.CoreLib/System.Enum::ToString:System.String(System.String)",
                            "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)",
                            "System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)",
                        ],
                        "testCode": {
                            "testCodeStatus": "needs-tests",
                            "requestedMethodCount": 12,
                            "emittedMethodCount": 12,
                            "allMethodsCovered": False,
                            "action": "write-tests",
                        },
                        "implementationFamilies": [
                            {"familyId": "impl/enum/parsing", "displayName": "Enum parsing", "handlerName": "EnumParsing"}
                        ],
                    },
                ],
            },
            {
                "assemblyName": "System.Collections.Immutable",
                "denominatorStatus": "audit-confirmed",
                "families": [
                    {
                        "familyId": "family/System.Collections.Immutable/immutable-array",
                        "displayName": "Immutable Array",
                        "description": "ImmutableArray construction, builders, enumeration, and interop",
                        "denominatorStatus": "audit-confirmed",
                        "authorityRefs": [
                            "verification/catalog/programs/foundation-dll-translation-audit.program.json",
                            "docs/dev/in-progress/20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable/STATUS.md",
                        ],
                        "derivationBasis": [
                            "audit-input-manifest",
                            "surface-ledger",
                            "semantic-ledger",
                            "nativeization-plan",
                        ],
                        "closureStatus": "in-progress",
                        "verificationGates": {
                            "audit-input-and-ledger": "pending",
                            "managed-proof": "pending",
                            "native-proof": "pending",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                        "methodCount": 0,
                        "implementationFamilies": [],
                    },
                    {
                        "familyId": "family/System.Collections.Immutable/immutable-dictionary",
                        "displayName": "Immutable Dictionary",
                        "description": "ImmutableDictionary construction, mutation, lookup, and enumeration",
                        "denominatorStatus": "audit-confirmed",
                        "authorityRefs": [
                            "verification/catalog/programs/foundation-dll-translation-audit.program.json",
                            "docs/dev/in-progress/20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable/STATUS.md",
                        ],
                        "derivationBasis": [
                            "audit-input-manifest",
                            "surface-ledger",
                            "semantic-ledger",
                            "nativeization-plan",
                        ],
                        "closureStatus": "in-progress",
                        "verificationGates": {
                            "audit-input-and-ledger": "pending",
                            "managed-proof": "pending",
                            "native-proof": "pending",
                            "hotupdate-proof": "not-required",
                            "benchmark": "not-required",
                            "codegen-review": "pending",
                        },
                        "methodCount": 0,
                        "implementationFamilies": [],
                    },
                ],
            },
        ],
    }


def _write_ledger(repo_root: Path) -> None:
    write_json(
        repo_root / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json",
        _capability_ledger(),
    )


def test_repository_capability_family_ledger_uses_candidate_denominator_not_full_surface_placeholders() -> None:
    ledger_path = REPO_ROOT / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json"
    ledger = json.loads(ledger_path.read_text(encoding="utf-8"))

    expected_counts = {
        "System.Private.CoreLib": 24,
        "System.Collections.Immutable": 10,
        "System.Formats.Asn1": 6,
        "System.IO.Compression.Brotli": 4,
        "System.IO.Compression.ZipFile": 2,
        "System.IO.Pipelines": 5,
        "System.Linq": 12,
        "System.Net.ServerSentEvents": 4,
        "System.ObjectModel": 6,
        "System.Runtime.InteropServices": 14,
        "System.Runtime.Serialization.Formatters": 6,
        "System.Security.Principal.Windows": 6,
        "System.Text.Json": 13,
        "System.Threading.Tasks.Parallel": 4,
    }
    actual_counts = {entry["assemblyName"]: len(entry.get("families") or []) for entry in ledger["dlls"]}

    assert ledger["denominatorStrategy"] == "capability-family"
    assert ledger["denominatorStatus"] == "audit-confirmed"
    assert actual_counts == expected_counts
    assert sum(actual_counts.values()) == 116
    assert all(not family["familyId"].endswith("/all") for entry in ledger["dlls"] for family in entry["families"])
    assert all(entry["denominatorStatus"] == "audit-confirmed" for entry in ledger["dlls"])
    assert all(family["denominatorStatus"] == "audit-confirmed" for entry in ledger["dlls"] for family in entry["families"])


def test_write_foundation_dll_audit_outputs_materializes_program_matrix_and_report_files() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_audit_generator_outputs")
    repo_root = make_temp_repo_root("verification", "foundation-dll-audit")
    _write_fixture_repo(repo_root)

    try:
        payload = generator_module.write_foundation_dll_audit_outputs(repo_root)

        projection_root = repo_root / "verification" / "projections" / "foundation-dll-audit"
        report_root = repo_root / "verification" / "archive" / "reports" / "completed" / "foundation-dll-audit"
        docs_root = repo_root / "docs" / "verification" / "foundation-dll-audit"
        assert (projection_root / "program.json").is_file()
        assert (projection_root / "family-verification.json").is_file()
        assert (projection_root / "family-verification-claims.json").is_file()
        assert (projection_root / "dll-matrix.json").is_file()
        assert (projection_root / "artifact-index.json").is_file()
        assert (projection_root / "dashboard.html").is_file()
        assert (projection_root / "dashboard.css").is_file()
        assert (projection_root / "dlls" / "System.Private.CoreLib.json").is_file()
        assert (projection_root / "dlls" / "System.Private.CoreLib.html").is_file()
        assert (report_root / "summary.md").is_file()
        assert (docs_root / "program.json").is_file()
        assert (docs_root / "family-verification.json").is_file()
        assert (docs_root / "family-verification-claims.json").is_file()
        assert (docs_root / "dll-matrix.json").is_file()
        assert (docs_root / "artifact-index.json").is_file()
        assert (docs_root / "dashboard.html").is_file()
        assert (docs_root / "dashboard.css").is_file()
        assert (docs_root / "summary.md").is_file()
        assert (docs_root / "dlls" / "System.Private.CoreLib.json").is_file()
        assert (docs_root / "dlls" / "System.Private.CoreLib.html").is_file()

        program_payload = json.loads((projection_root / "program.json").read_text(encoding="utf-8"))
        assert program_payload["programId"] == "foundation-dll-translation-audit"
        assert program_payload["summary"]["dllCount"] == 2
        assert program_payload["summary"]["completedCount"] == 0
        assert program_payload["summary"]["inProgressCount"] == 1
        assert program_payload["summary"]["blockedCount"] == 1
        assert program_payload["summary"]["activeAssembly"] == "System.Private.CoreLib"

        matrix_payload = json.loads((projection_root / "dll-matrix.json").read_text(encoding="utf-8"))
        rows = {row["assemblyName"]: row for row in matrix_payload["rows"]}
        assert rows["System.Private.CoreLib"]["dllState"] == "in-progress"
        assert rows["System.Private.CoreLib"]["currentProject"] == "native-proof"
        assert rows["System.Collections.Immutable"]["dllState"] == "blocked"

        detail_payload = json.loads(
            (projection_root / "dlls" / "System.Private.CoreLib.json").read_text(encoding="utf-8")
        )
        assert detail_payload["phase"] == "roadmap-closed"
        assert detail_payload["roadmapTaskId"] == "20260427-04-dll-capability-verification-closure-roadmap"
        projects = {item["projectCode"]: item for item in detail_payload["projects"]}
        assert projects["managed-proof"]["executionState"] == "passed"
        assert projects["native-proof"]["executionState"] == "passed"
        assert projects["codegen-review"]["executionState"] == "passed"
        assert projects["completion-certification"]["executionState"] == "in-progress"
        assert projects["managed-proof"]["supportRefs"]
        assert all(path.startswith("docs/") or path.startswith("subjects/") for path in projects["managed-proof"]["supportRefs"])
        assert any(
            artifact["path"].endswith("managed-proof-report.json") and artifact["linkTargetType"] == "file"
            for artifact in projects["managed-proof"]["artifacts"]
        )
        assert all(path.startswith("artifacts/") for path in (artifact["path"] for artifact in projects["managed-proof"]["artifacts"]))

        immutable_payload = json.loads(
            (projection_root / "dlls" / "System.Collections.Immutable.json").read_text(encoding="utf-8")
        )
        assert immutable_payload["phase"] == "roadmap-closed"
        assert immutable_payload["roadmapTaskId"] == "20260427-04-dll-capability-verification-closure-roadmap"
        immutable_projects = {item["projectCode"]: item for item in immutable_payload["projects"]}
        assert immutable_projects["audit-input-and-ledger"]["executionState"] == "blocked"
        assert immutable_projects["managed-proof"]["executionState"] == "pending"
        assert immutable_projects["native-proof"]["executionState"] == "pending"
        assert immutable_projects["completion-certification"]["executionState"] == "blocked"
        assert immutable_projects["managed-proof"]["artifacts"] == []
        assert immutable_projects["managed-proof"]["supportRefs"]

        artifact_payload = json.loads((projection_root / "artifact-index.json").read_text(encoding="utf-8"))
        assert any(
            item["assemblyName"] == "System.Private.CoreLib"
            and item["projectCode"] == "native-proof"
            and item["linkTargetType"] == "file"
            for item in artifact_payload["rows"]
        )
        assert all(not item["path"].startswith("docs/") for item in artifact_payload["rows"])
        assert all(not item["path"].startswith("subjects/") for item in artifact_payload["rows"])
        dashboard_html = (projection_root / "dashboard.html").read_text(encoding="utf-8")
        dashboard_css = (projection_root / "dashboard.css").read_text(encoding="utf-8")
        assert "{{" not in dashboard_html
        assert 'href="./dashboard.css"' in dashboard_html
        assert 'href="./dlls/System.Private.CoreLib.html"' in dashboard_html
        assert '<div class="dll-card"' in dashboard_html
        assert '<section id="details">' in dashboard_html
        assert "Support Refs" in dashboard_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in dashboard_html
        projection_dll_html = (projection_root / "dlls" / "System.Private.CoreLib.html").read_text(encoding="utf-8")
        assert 'href="../dashboard.css"' in projection_dll_html
        assert "Verification Projects" in projection_dll_html
        assert "Managed Proof" in projection_dll_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in projection_dll_html
        docs_dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
        assert "{{" not in docs_dashboard_html
        assert 'href="./dashboard.css"' in docs_dashboard_html
        assert 'href="./dlls/System.Private.CoreLib.html"' in docs_dashboard_html
        assert '<div class="dll-card"' in docs_dashboard_html
        assert '<section id="details">' in docs_dashboard_html
        assert "Support Refs" in docs_dashboard_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in docs_dashboard_html
        docs_dll_html = (docs_root / "dlls" / "System.Private.CoreLib.html").read_text(encoding="utf-8")
        assert 'href="../dashboard.css"' in docs_dll_html
        assert "Verification Projects" in docs_dll_html
        assert "Managed Proof" in docs_dll_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in docs_dll_html
        assert "20260427-04-dll-capability-verification-closure-roadmap" in docs_dll_html
        assert "20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable" not in docs_dashboard_html
        assert "phase-2" not in docs_dashboard_html
        assert "verification/projections/foundation-dll-audit/dashboard.html" in payload["artifacts"]
        assert "verification/projections/foundation-dll-audit/dashboard.css" in payload["artifacts"]
        assert "verification/projections/foundation-dll-audit/dlls/System.Private.CoreLib.html" in payload["artifacts"]
        assert "docs/verification/foundation-dll-audit/dashboard.html" in payload["artifacts"]
        assert "docs/verification/foundation-dll-audit/dashboard.css" in payload["artifacts"]
        assert "docs/verification/foundation-dll-audit/dlls/System.Private.CoreLib.html" in payload["artifacts"]
        assert payload["reportSummaryPath"] == "verification/archive/reports/completed/foundation-dll-audit/summary.md"
        assert payload["docsDashboardPath"] == "docs/verification/foundation-dll-audit/dashboard.html"
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)


def test_write_foundation_dll_audit_outputs_with_ledger_produces_dual_axis_reporting() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_audit_generator_ledger")
    repo_root = make_temp_repo_root("verification", "foundation-dll-audit-ledger")
    _write_fixture_repo(repo_root)
    _write_ledger(repo_root)

    try:
        payload = generator_module.write_foundation_dll_audit_outputs(repo_root)

        projection_root = repo_root / "verification" / "projections" / "foundation-dll-audit"
        docs_root = repo_root / "docs" / "verification" / "foundation-dll-audit"

        # -- schemaVersion=2 at program level --
        program_payload = json.loads((projection_root / "program.json").read_text(encoding="utf-8"))
        assert program_payload["schemaVersion"] == 2
        assert program_payload["programId"] == "foundation-dll-translation-audit"
        assert program_payload["scopeAssemblies"] == [
            "System.Private.CoreLib",
            "System.Collections.Immutable",
        ]
        family_verification_payload = json.loads((projection_root / "family-verification.json").read_text(encoding="utf-8"))
        assert family_verification_payload["schemaVersion"] == 1
        corelib_family_snapshot = next(item for item in family_verification_payload["assemblies"] if item["assemblyName"] == "System.Private.CoreLib")
        assert len(corelib_family_snapshot["families"]) == 3
        family_verification_claims_payload = json.loads((projection_root / "family-verification-claims.json").read_text(encoding="utf-8"))
        assert family_verification_claims_payload["schemaVersion"] == 1
        corelib_claims = next(item for item in family_verification_claims_payload["assemblies"] if item["assemblyName"] == "System.Private.CoreLib")
        assert len(corelib_claims["claims"]) == 15
        native_claim = next(item for item in corelib_claims["claims"] if item["familyId"] == "family/System.Private.CoreLib/convert/char" and item["gateCode"] == "native-proof")
        assert native_claim["methodSubjectIds"]
        truth_contracts_payload = json.loads((projection_root / "truth-contracts.json").read_text(encoding="utf-8"))
        assert truth_contracts_payload["schemaVersion"] == 1
        corelib_truth = next(item for item in truth_contracts_payload["assemblies"] if item["dllCapabilityManifest"]["assemblyName"] == "System.Private.CoreLib")
        assert corelib_truth["dllCapabilityManifest"]["methodUniverseCount"] >= 40

        # -- summary has dual-axis fields --
        summary = program_payload["summary"]
        assert "capabilityClosure" in summary
        assert summary["capabilityClosure"]["totalFamilies"] == 5
        assert summary["capabilityClosure"]["closedFamilies"] == 0
        assert summary["capabilityClosure"]["inProgressFamilies"] == 5
        assert summary["capabilityClosure"]["closurePercent"] == 0.0
        assert "workflowProgress" in summary
        assert summary["workflowProgress"]["totalRequiredGates"] == 20
        assert summary["workflowProgress"]["passedGates"] == 6
        assert summary["workflowProgress"]["progressPercent"] == 30.0
        assert "dllCompletion" in summary
        assert summary["dllCompletion"]["completedDllCount"] == 0

        # -- authoritySnapshot --
        assert "authoritySnapshot" in program_payload
        assert program_payload["authoritySnapshot"]["snapshotId"] == "snap-20260426-manual"
        assert program_payload["authoritySnapshot"]["denominatorStatus"] == "candidate-derived"

        # -- dll-matrix has capabilityClosure and workflowProgress columns --
        matrix_payload = json.loads((projection_root / "dll-matrix.json").read_text(encoding="utf-8"))
        assert matrix_payload["schemaVersion"] == 2
        rows = {row["assemblyName"]: row for row in matrix_payload["rows"]}
        assert "capabilityClosure" in rows["System.Private.CoreLib"]
        assert "workflowProgress" in rows["System.Private.CoreLib"]
        assert rows["System.Private.CoreLib"]["capabilityClosure"]["totalFamilies"] == 3
        assert rows["System.Private.CoreLib"]["workflowProgress"]["totalRequiredGates"] == 12
        assert rows["System.Private.CoreLib"]["denominatorStatus"] == "candidate-derived"
        assert rows["System.Collections.Immutable"]["capabilityClosure"]["totalFamilies"] == 2
        assert rows["System.Collections.Immutable"]["workflowProgress"]["totalRequiredGates"] == 8
        assert rows["System.Collections.Immutable"]["denominatorStatus"] == "audit-confirmed"

        # -- DLL detail has capabilityFamilies, sourceLinks, waiverSummary --
        detail_payload = json.loads(
            (projection_root / "dlls" / "System.Private.CoreLib.json").read_text(encoding="utf-8")
        )
        assert detail_payload["schemaVersion"] == 2
        assert detail_payload["phase"] == "roadmap-closed"
        assert detail_payload["roadmapTaskId"] == "20260427-04-dll-capability-verification-closure-roadmap"
        assert "truthContracts" in detail_payload
        assert detail_payload["truthContracts"]["dllCapabilityManifest"]["assemblyName"] == "System.Private.CoreLib"
        assert len(detail_payload["capabilityFamilies"]) == 3
        assert detail_payload["capabilityClosure"]["totalFamilies"] == 3
        assert detail_payload["workflowProgress"]["totalRequiredGates"] == 12
        assert detail_payload["waiverSummary"]["totalWaivers"] == 0
        assert "subjectSource" in detail_payload.get("sourceLinks", {})
        assert "familySource" not in detail_payload  # CoreLib uses real families, not auto-derived
        assert detail_payload["denominatorStatus"] == "candidate-derived"
        assert detail_payload["capabilityFamilies"][0]["testCode"]["testCodeStatus"] == "present"
        assert detail_payload["capabilityFamilies"][1]["testCode"]["testCodeStatus"] == "coverage-widened"
        assert detail_payload["capabilityFamilies"][2]["testCode"]["testCodeStatus"] == "needs-tests"
        assert detail_payload["capabilityFamilies"][0]["nativeProof"]["status"] == "in-progress"
        assert detail_payload["capabilityFamilies"][0]["nativeProof"]["numerator"] == 18
        assert detail_payload["capabilityFamilies"][0]["nativeProof"]["denominator"] == 18
        assert detail_payload["capabilityFamilies"][0]["nativeProof"]["progressPercent"] == 100.0
        assert detail_payload["capabilityFamilies"][0]["nativeProof"]["evidence"]
        assert detail_payload["capabilityFamilies"][0]["nativeProof"]["runs"]
        assert detail_payload["capabilityFamilies"][1]["nativeProof"]["numerator"] == 0
        assert detail_payload["capabilityFamilies"][1]["nativeProof"]["denominator"] == 10
        assert detail_payload["capabilityFamilies"][1]["nativeProof"]["progressPercent"] == 0.0
        assert detail_payload["capabilityFamilies"][2]["nativeProof"]["numerator"] == 0
        assert detail_payload["capabilityFamilies"][2]["nativeProof"]["denominator"] == 12
        assert detail_payload["capabilityFamilies"][2]["nativeProof"]["progressPercent"] == 0.0
        immutable_payload = json.loads(
            (projection_root / "dlls" / "System.Collections.Immutable.json").read_text(encoding="utf-8")
        )
        assert immutable_payload["phase"] == "roadmap-closed"
        assert immutable_payload["roadmapTaskId"] == "20260427-04-dll-capability-verification-closure-roadmap"
        assert len(immutable_payload.get("capabilityFamilies")) == 2
        assert immutable_payload["capabilityFamilies"][0]["familyId"] == "family/System.Collections.Immutable/immutable-array"
        assert immutable_payload["capabilityFamilies"][0]["closureStatus"] == "in-progress"
        assert immutable_payload["capabilityClosure"]["totalFamilies"] == 2
        assert immutable_payload["capabilityClosure"]["inProgressFamilies"] == 2
        assert immutable_payload["workflowProgress"]["totalRequiredGates"] == 8
        assert immutable_payload["workflowProgress"]["passedGates"] == 0
        assert "familySource" not in immutable_payload
        assert immutable_payload["denominatorStatus"] == "audit-confirmed"
        assert immutable_payload["capabilityFamilies"][0]["denominatorStatus"] == "audit-confirmed"
        assert immutable_payload["capabilityFamilies"][0]["authorityRefs"]

        # -- artifact-index.html exists --
        assert (projection_root / "artifact-index.html").is_file()
        artifact_html = (projection_root / "artifact-index.html").read_text(encoding="utf-8")
        assert "Back To Dashboard" in artifact_html
        assert 'href="./dashboard.html"' in artifact_html
        assert "Artifact Index" in artifact_html

        # -- dashboard HTML --
        dashboard_html = (projection_root / "dashboard.html").read_text(encoding="utf-8")
        dashboard_css = (projection_root / "dashboard.css").read_text(encoding="utf-8")
        assert "{{" not in dashboard_html
        assert 'href="./dashboard.css"' in dashboard_html
        # Artifact link points to independent page, not hash anchor
        assert 'href="./artifact-index.html"' in dashboard_html
        # Triple-axis progress bars present
        assert "DLL Completion" in dashboard_html
        assert "Capability Closure" in dashboard_html
        assert "Workflow Progress" in dashboard_html
        # Condensed card: no full project grid in detail cards
        assert "Verification Projects" not in dashboard_html
        # Mini bars in matrix
        assert 'class="mini-bar"' in dashboard_html
        assert "20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable" not in dashboard_html
        assert "phase-2" not in dashboard_html

        # -- DLL detail HTML has dual-axis bars and family table --
        dll_html = (projection_root / "dlls" / "System.Private.CoreLib.html").read_text(encoding="utf-8")
        assert "Back To Dashboard" in dll_html
        assert "Capability Closure" in dll_html
        assert "Workflow Progress" in dll_html
        assert "Capability Families" in dll_html
        assert "Convert.Char Conversions" in dll_html
        assert "Test Code" in dll_html
        assert "Native Proof Detail" not in dll_html
        assert "native-reference.runtime-skeleton.coverage.json" in dll_html
        assert "coverage-widened" in dll_html
        assert "needs-tests" in dll_html
        assert "18/18" in dll_html
        assert "0/10" in dll_html
        assert "0/12" in dll_html
        assert 'class="native-proof-tooltip"' in dll_html
        assert 'class="native-proof-trigger"' in dll_html
        assert 'data-tooltip-delay-ms="500"' in dll_html
        assert 'class="native-proof-links"' in dll_html
        assert 'class="native-proof-methods"' in dll_html
        assert "System.Convert::ToChar(System.Boolean)" in dll_html
        assert "System.Convert::ToChar(System.UInt64)" in dll_html
        assert "System.Convert::ToChar:System.Char(System.Boolean)" not in dll_html
        assert 'class="native-proof-runs"' not in dll_html
        assert "Tests" in dll_html
        assert "Benchmarks" in dll_html
        assert "HotUpdate" in dll_html
        assert "Method_System_Convert_ToChar_System_Boolean" in dll_html
        assert "HostToPatch" in dll_html
        assert "native-proof-trigger.is-visible .native-proof-tooltip" in dashboard_css
        assert "window.setTimeout" in dll_html
        assert "user-select: text;" in dashboard_css
        assert "white-space: nowrap;" in dashboard_css
        assert "width: max-content;" in dashboard_css
        assert "Source Links" in dll_html
        # CoreLib has real families, NOT auto-derived
        assert "auto-derived" not in dll_html.lower()

        # -- Immutable detail HTML uses curated capability families --
        immutable_html = (projection_root / "dlls" / "System.Collections.Immutable.html").read_text(encoding="utf-8")
        assert "Back To Dashboard" in immutable_html
        assert "auto-derived" not in immutable_html.lower()
        assert "Immutable Array" in immutable_html
        assert "Immutable Dictionary" in immutable_html
        assert "audit-confirmed" in immutable_html
        # Still has everything a detail page needs
        assert "Verification Projects" in immutable_html
        assert "Blocking Context" in immutable_html

        # -- docs bundle is also generated --
        assert (docs_root / "dashboard.html").is_file()
        assert (docs_root / "artifact-index.html").is_file()
        assert (docs_root / "dlls" / "System.Private.CoreLib.html").is_file()

        # -- payload artifacts includes artifact-index.html --
        assert "verification/projections/foundation-dll-audit/artifact-index.html" in payload["artifacts"]
        assert "docs/verification/foundation-dll-audit/artifact-index.html" in payload["artifacts"]
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
