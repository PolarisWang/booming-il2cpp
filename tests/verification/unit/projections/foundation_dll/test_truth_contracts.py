from __future__ import annotations

from pathlib import Path

from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
TRUTH_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "truth_contracts.py"
)


def test_build_truth_contracts_emits_dll_family_and_method_authority() -> None:
    truth_module = load_module(TRUTH_MODULE_PATH, "chaos_foundation_dll_truth_contracts")

    assembly_name = "System.Private.CoreLib"
    owner_subject_id = "SolutionCorePack"
    families = [
        {
            "familyId": "family/System.Private.CoreLib/convert/char",
            "displayName": "Convert.Char Conversions",
            "methodCount": 18,
            "verificationGates": {
                "native-proof": "in-progress",
                "managed-proof": "passed",
                "hotupdate-proof": "not-required",
                "benchmark": "not-required",
            },
            "testCode": {
                "testCodeStatus": "present",
                "testProjectPath": "tests/families/System.Private.CoreLib/convert-char",
            },
        },
        {
            "familyId": "family/System.Private.CoreLib/buffer/memory",
            "displayName": "Buffer.Memory Operations",
            "methodCount": 10,
            "verificationGates": {
                "native-proof": "in-progress",
                "managed-proof": "passed",
                "hotupdate-proof": "not-required",
                "benchmark": "not-required",
            },
            "testCode": {
                "testCodeStatus": "present",
                "testProjectPath": "tests/families/System.Private.CoreLib/buffer-memory",
            },
        },
    ]
    method_contracts = {
        "family/System.Private.CoreLib/convert/char": [
            {
                "methodSubjectId": "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                "functionalRequired": True,
                "benchmarkRequired": False,
                "hotupdateFunctionalRequired": False,
                "hotupdateBenchmarkRequired": False,
                "routeCodes": ["managed", "native"],
                "reviewRequired": True,
            },
            {
                "methodSubjectId": "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
                "functionalRequired": True,
                "benchmarkRequired": False,
                "hotupdateFunctionalRequired": False,
                "hotupdateBenchmarkRequired": False,
                "routeCodes": ["managed", "native"],
                "reviewRequired": True,
            },
        ],
        "family/System.Private.CoreLib/buffer/memory": [
            {
                "methodSubjectId": "System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)",
                "functionalRequired": True,
                "benchmarkRequired": True,
                "hotupdateFunctionalRequired": False,
                "hotupdateBenchmarkRequired": False,
                "routeCodes": ["managed", "native"],
                "reviewRequired": True,
            }
        ],
    }

    payload = truth_module.build_truth_contracts_snapshot(
        assembly_name=assembly_name,
        owner_subject_id=owner_subject_id,
        families=families,
        method_contracts_by_family=method_contracts,
    )

    assert payload["schemaVersion"] == 1
    assert payload["dllCapabilityManifest"]["assemblyName"] == assembly_name
    assert payload["dllCapabilityManifest"]["ownerSubjectId"] == owner_subject_id
    assert payload["dllCapabilityManifest"]["capabilityFamilies"] == [
        "family/System.Private.CoreLib/convert/char",
        "family/System.Private.CoreLib/buffer/memory",
    ]
    assert payload["dllCapabilityManifest"]["methodUniverseCount"] == 3
    family_contracts = {item["familyId"]: item for item in payload["capabilityFamilyVerificationContracts"]}
    assert family_contracts["family/System.Private.CoreLib/convert/char"]["methodSubjectCount"] == 2
    assert family_contracts["family/System.Private.CoreLib/buffer/memory"]["performanceObligation"]["required"] is True
    method_items = {item["methodSubjectId"]: item for item in payload["methodCapabilityContracts"]}
    assert method_items["System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)"]["functionalRequired"] is True
    assert method_items["System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)"]["benchmarkRequired"] is True
