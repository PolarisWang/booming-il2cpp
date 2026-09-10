from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
CLAIMS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "family_verification_claims.py"


def _native_reference_plan_payload() -> dict:
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


def test_build_family_verification_claims_snapshot_prefers_truth_or_inline_method_authority() -> None:
    claims_module = load_module(CLAIMS_MODULE_PATH, "chaos_family_verification_claims")
    repo_root = make_temp_repo_root("verification", "family-verification-claims")

    try:
        plan_path = repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260426-fixture" / "analysis" / "generated" / "supplemental-full-closures" / "system-private-corelib" / "native-reference" / "native-reference.plan.json"
        write_json(plan_path, _native_reference_plan_payload())

        projects = [
            {
                "projectCode": "native-proof",
                "artifacts": [
                    {
                        "path": plan_path.relative_to(repo_root).as_posix(),
                        "artifactKind": "report-json",
                        "linkTargetType": "file",
                    }
                ],
            }
        ]
        families = [
            {
                "familyId": "family/System.Private.CoreLib/convert/char",
                "displayName": "Convert.Char Conversions",
                "methodCount": 18,
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
                "verificationGates": {"native-proof": "in-progress"},
            },
            {
                "familyId": "family/System.Private.CoreLib/buffer/memory",
                "displayName": "Buffer.Memory Operations",
                "methodCount": 10,
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
                "verificationGates": {"native-proof": "in-progress"},
            },
            {
                "familyId": "family/System.Private.CoreLib/enum/parsing",
                "displayName": "Enum Parsing And Formatting",
                "methodCount": 12,
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
                "verificationGates": {"native-proof": "in-progress"},
            },
        ]

        snapshot = claims_module.build_family_verification_claims_snapshot(
            repo_root,
            assembly_name="System.Private.CoreLib",
            owner_subject_id="SolutionCorePack",
            families=families,
            projects=projects,
        )
        by_claim = {(item["familyId"], item["gateCode"]): item for item in snapshot["claims"]}
        assert len(snapshot["claims"]) == 15
        assert len(by_claim[("family/System.Private.CoreLib/convert/char", "native-proof")]["methodSubjectIds"]) == 18
        assert len(by_claim[("family/System.Private.CoreLib/buffer/memory", "native-proof")]["methodSubjectIds"]) == 10
        assert len(by_claim[("family/System.Private.CoreLib/enum/parsing", "native-proof")]["methodSubjectIds"]) == 12
        assert by_claim[("family/System.Private.CoreLib/convert/char", "native-proof")]["derivationRule"] == "inline-family:methodSubjectIds"
        assert by_claim[("family/System.Private.CoreLib/convert/char", "managed-proof")]["denominator"] == 1
        assert by_claim[("family/System.Private.CoreLib/convert/char", "test-code")]["denominator"] == 18
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
