from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
GENERATOR_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "truth_skeleton_generator.py"
)


def test_generate_truth_skeleton_creates_family_contract_bundle() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_truth_skeleton_generator")
    repo_root = make_temp_repo_root("verification", "foundation-dll-truth-skeleton")

    try:
        result = generator_module.generate_truth_skeleton(
            repo_root,
            assembly_name="System.Private.CoreLib",
            owner_subject_id="SolutionCorePack",
            family={
                "familyId": "family/System.Private.CoreLib/convert/char",
                "displayName": "Convert.Char Conversions",
                "methodCount": 18,
            },
            method_subject_ids=[
                "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
            ],
        )

        output_root = (
            repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char"
        )
        assert output_root.is_dir()
        dll_manifest = json.loads((output_root / "dll-capability-manifest.json").read_text(encoding="utf-8"))
        family_contract = json.loads((output_root / "capability-family-contract.json").read_text(encoding="utf-8"))
        method_contracts = json.loads((output_root / "method-capability-contracts.json").read_text(encoding="utf-8"))

        assert dll_manifest["assemblyName"] == "System.Private.CoreLib"
        assert dll_manifest["ownerSubjectId"] == "SolutionCorePack"
        assert dll_manifest["capabilityFamilies"] == ["family/System.Private.CoreLib/convert/char"]
        assert family_contract["familyId"] == "family/System.Private.CoreLib/convert/char"
        assert family_contract["methodSubjectCount"] == 2
        assert len(method_contracts["methods"]) == 2
        assert method_contracts["methods"][0]["methodSubjectId"].startswith("System.Private.CoreLib/System.Convert::ToChar:")
        assert "verification/foundation-dll/System.Private.CoreLib/convert-char/dll-capability-manifest.json" in result["artifacts"]
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)


def test_generate_truth_skeleton_for_dll_creates_all_family_bundles() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_truth_skeleton_generator_dll")
    repo_root = make_temp_repo_root("verification", "foundation-dll-truth-skeleton-dll")

    try:
        result = generator_module.generate_truth_skeleton_for_dll(
            repo_root,
            assembly_name="System.Private.CoreLib",
            owner_subject_id="SolutionCorePack",
            families=[
                {
                    "familyId": "family/System.Private.CoreLib/convert/char",
                    "displayName": "Convert.Char Conversions",
                    "methodCount": 18,
                },
                {
                    "familyId": "family/System.Private.CoreLib/buffer/memory",
                    "displayName": "Buffer.Memory Operations",
                    "methodCount": 10,
                },
            ],
            method_subject_ids_by_family={
                "family/System.Private.CoreLib/convert/char": [
                    "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)",
                    "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)",
                ],
                "family/System.Private.CoreLib/buffer/memory": [
                    "System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)"
                ],
            },
        )

        convert_root = repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char"
        buffer_root = repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib" / "buffer-memory"
        assert (convert_root / "dll-capability-manifest.json").is_file()
        assert (buffer_root / "dll-capability-manifest.json").is_file()
        assert result["familyCount"] == 2
        assert len(result["artifacts"]) == 6
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
