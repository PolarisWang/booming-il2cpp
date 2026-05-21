from __future__ import annotations

import json
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
FORBIDDEN_TEST_PATH_SNIPPETS = [
    "C:" + "\\" + "Users" + "\\",
    "AppData" + "\\" + "Local" + "\\" + "Temp",
]
FORBIDDEN_ACTIVE_PREFIX_SNIPPETS = [
    "boo" + "ming_",
    "boo" + "ming-",
]
EXPECTED_PYTEST_INI_SNIPPETS = [
    "[pytest]",
    "testpaths = tests",
    "norecursedirs = artifacts",
]

EXPECTED_MANAGED_PROJECTS: dict[str, list[str]] = {
    "Chaos.IL2CPP.Contracts": [],
    "Chaos.IL2CPP.EngineBinding": [
        "Chaos.IL2CPP.Contracts",
    ],
    "Chaos.IL2CPP.HotUpdate": [
        "Chaos.IL2CPP.Contracts",
    ],
    "Chaos.IL2CPP.Driver": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Diagnostics",
        "Chaos.IL2CPP.Generator",
        "Chaos.IL2CPP.Pipeline",
        "Chaos.IL2CPP.ProjectGraph",
    ],
    "Chaos.IL2CPP.Loader": [
        "Chaos.IL2CPP.Contracts",
    ],
    "Chaos.IL2CPP.ProjectGraph": [
        "Chaos.IL2CPP.Contracts",
    ],
    "Chaos.IL2CPP.SemanticWorld": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Loader",
    ],
    "Chaos.IL2CPP.Pipeline": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Loader",
        "Chaos.IL2CPP.SemanticWorld",
        "Chaos.IL2CPP.Linker",
        "Chaos.IL2CPP.MetadataWriter",
        "Chaos.IL2CPP.Generator",
    ],
    "Chaos.IL2CPP.Linker": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.SemanticWorld",
    ],
    "Chaos.IL2CPP.MetadataWriter": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Linker",
    ],
    "Chaos.IL2CPP.Generator": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Generator.Generators",
        "Chaos.IL2CPP.Loader",
        "Chaos.IL2CPP.MetadataWriter",
        "Scriban",
    ],
}
ACTIVE_CONTRACT_DOC_PATHS = [
    REPO_ROOT / "contracts" / "docs" / "v0" / "overview.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "schema-pack-v0.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "hello-world-proof-spec-v0.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "local-verification.md",
    REPO_ROOT / "docs" / "architecture" / "subject-test-framework-v1" / "foundation-and-windows-cutover-v1.md",
]
ACTIVE_ANALYSIS_RETIREMENT_DOC_PATHS = [
    REPO_ROOT / "contracts" / "docs" / "v0" / "overview.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "schema-pack-v0.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "repo-layout.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "ownership-map.md",
    REPO_ROOT / "docs" / "architecture" / "subject-test-framework-v1" / "foundation-and-windows-cutover-v1.md",
    REPO_ROOT / "wiki" / "06-测试验证" / "模块" / "analysis.md",
]
RETIRED_RUN_TEST_NAMES = [
    "test_phase2_aot_runtime_completeness.py",
    "test_phase2_async_await_proof.py",
    "test_phase2_core_contract_cutover.py",
    "test_phase2_linker_stripping_proof.py",
    "test_phase2_marshaling_proof.py",
    "test_phase2_nested_exception_proof.py",
    "test_phase2_perf_governance.py",
    "test_phase2_threading_gc_proof.py",
    "test_phase3_capability_driven_lowering.py",
    "test_phase3_engine_binding_codegen_baseline.py",
    "test_phase3_engine_bridge_minimal.py",
    "test_phase3_engine_contract_freeze.py",
    "test_phase3_engine_host_proof.py",
    "test_phase3_host_embedding_ownership_protocol.py",
    "test_phase4_mainline_feature_pack_onboarding.py",
    "test_phase4_mobile_hello_world_proof.py",
    "test_phase4_mobile_runtime_host_batch1.py",
    "test_phase4_mobile_runtime_host_batch2_android.py",
    "test_phase4_mobile_runtime_host_batch3_ios.py",
    "test_phase5_capability_batch_a.py",
    "test_phase5_hot_update_skeleton.py",
    "test_phase6_capability_batch_b.py",
    "test_phase6_metadata_supplement_bridge.py",
    "test_phase7_engine_binding.py",
    "test_phase7_interpreter_mixed_execution.py",
    "test_phase8_mobile_foundation.py",
    "test_phase8_productization_gates.py",
    "test_phase9_hot_update_e2e.py",
    "test_phase_b_aot_contract_split.py",
    "test_phase_b_native_aot_workload_entry_bundle.py",
    "test_stage3_managed_minimal_closure.py",
    "test_stage4_bootstrap_support_skeleton.py",
    "test_stage4_native_reference_proof.py",
    "test_stage4_runtime_core_skeleton.py",
    "test_stage5a_generic_echo_windows_reference.py",
    "test_stage5b_reflection_lite_windows_reference.py",
    "test_stage5c_pinvoke_lite_windows_reference.py",
]
MIGRATED_COMPATIBILITY_TESTS = [
    "test_interface_dispatch_aot_runtime_completeness.py",
    "test_async_await_proof_subject.py",
    "test_core_contract_cutover.py",
    "test_linker_stripping_proof_subject.py",
    "test_marshaling_proof_subject.py",
    "test_nested_exception_proof_subject.py",
    "test_threading_gc_proof_subject.py",
    "test_capability_driven_lowering.py",
    "test_engine_binding_codegen_baseline.py",
    "test_engine_bridge_minimal.py",
    "test_engine_contract_freeze.py",
    "test_engine_host_proof_subject.py",
    "test_mainline_feature_pack_onboarding.py",
    "test_mobile_hello_world_proof_subject.py",
    "test_mobile_runtime_host_foundation.py",
    "test_mobile_runtime_host_android.py",
    "test_mobile_runtime_host_ios.py",
    "test_mainline_capability_batch_a.py",
    "test_hot_update_skeleton_subject.py",
    "test_mainline_capability_batch_b.py",
    "test_metadata_supplement_bridge_subjects.py",
    "test_engine_binding_subjects.py",
    "test_interpreter_mixed_execution_subjects.py",
    "test_mobile_foundation.py",
    "test_hot_update_productization_gates.py",
    "test_hot_update_e2e_flows.py",
]
MIGRATED_PERFORMANCE_TESTS = [
    "test_perf_governance.py",
    "test_native_aot_contract_split.py",
    "test_native_aot_workload_entry_bundle.py",
]
def parse_project_references(project_path: Path) -> list[str]:
    root = ET.fromstring(project_path.read_text(encoding="utf-8"))
    references: list[str] = []
    for element in root.findall(".//ProjectReference"):
        include_value = element.attrib.get("Include", "")
        references.append(Path(include_value).stem)
    return sorted(references)


class RepoLayoutTestSupport(unittest.TestCase):
    pass
