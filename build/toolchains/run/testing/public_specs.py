from __future__ import annotations

from typing import Any


MODULE_MAP_DOC = "wiki/06-测试验证/模块映射.md"
PLATFORM_GATES_DOC = "wiki/06-测试验证/整体验证场景/platform-gates.md"
TEST_TAXONOMY_DOC = "wiki/06-测试验证/测试分层.md"
TRACE_EXPORT_DOC = "wiki/06-测试验证/模块/trace-export.md"
SOLUTION_CORE_PACK = "SolutionCorePack"


def _suite_spec(
    *,
    suite_id: str,
    family: str,
    suite: str,
    stages: list[str],
    supported_hosts: list[str],
    level: str,
    primary_module_id: str,
    module_ids: list[str],
    subsystem_ids: list[str],
    doc_refs: list[str],
    execution: dict[str, Any],
) -> dict[str, Any]:
    return {
        "id": suite_id,
        "family": family,
        "suite": suite,
        "stages": list(stages),
        "supported_hosts": list(supported_hosts),
        "level": level,
        "primaryModuleId": primary_module_id,
        "moduleIds": list(module_ids),
        "subsystemIds": list(subsystem_ids),
        "docRefs": list(doc_refs),
        "execution": dict(execution),
    }


def _solution_core_smoke_suite(
    *,
    suite: str,
    project_name: str,
    primary_module_id: str,
    module_ids: list[str],
    subsystem_ids: list[str],
    expected_patterns: list[str],
) -> dict[str, Any]:
    project_root = f"subjects/{SOLUTION_CORE_PACK}/source/Slices/{project_name}"
    dll_path = f"{project_root}/bin/Release/net8.0/{project_name}.dll"
    return _suite_spec(
        suite_id=f"smoke/{suite}",
        family="smoke",
        suite=suite,
        stages=["all", "build", "run"],
        supported_hosts=["windows", "macos", "linux"],
        level="code",
        primary_module_id=primary_module_id,
        module_ids=module_ids,
        subsystem_ids=subsystem_ids,
        doc_refs=[MODULE_MAP_DOC],
        execution={
            "kind": "smoke-run",
            "targetSubjectId": SOLUTION_CORE_PACK,
            "project_path": f"{project_root}/{project_name}.csproj",
            "artifact_path": dll_path,
            "dll_path": dll_path,
            "expected_patterns": list(expected_patterns),
        },
    )


PUBLIC_TEST_SPECS = [
    _solution_core_smoke_suite(
        suite="managed-entry-basic",
        project_name="HelloWorld",
        primary_module_id="managed-smoke",
        module_ids=["managed-smoke"],
        subsystem_ids=["managed-execution"],
        expected_patterns=[
            "HelloWorld smoke entry reached.",
            "register:Main",
        ],
    ),
    _solution_core_smoke_suite(
        suite="managed-generics-basic",
        project_name="GenericEcho",
        primary_module_id="managed-smoke",
        module_ids=["managed-smoke"],
        subsystem_ids=["managed-execution"],
        expected_patterns=[
            "roadmap0",
            "42",
            "roadmap0:roadmap0",
        ],
    ),
    _solution_core_smoke_suite(
        suite="reflection-basic",
        project_name="ReflectionLite",
        primary_module_id="reflection",
        module_ids=["reflection"],
        subsystem_ids=["managed-execution"],
        expected_patterns=[
            "field=BackingField:Int32",
            "generic-method=String",
        ],
    ),
    _solution_core_smoke_suite(
        suite="native-interop-basic",
        project_name="PInvokeLite",
        primary_module_id="interop",
        module_ids=["interop"],
        subsystem_ids=["native-bridge"],
        expected_patterns=[
            "marshal=interop-smoke",
            "export=chaos_smoke_add:7",
            "symbol=True",
        ],
    ),
    _solution_core_smoke_suite(
        suite="host-embedding-basic",
        project_name="HostEmbeddingLite",
        primary_module_id="hosted-runtime",
        module_ids=["hosted-runtime", "managed-smoke"],
        subsystem_ids=["hosting", "managed-execution"],
        expected_patterns=[
            "HostEmbeddingSession:InvokeManagedEntry:True",
            "guards=invalid-detach:True|double-start:True|unattached-entry:True",
        ],
    ),
    _suite_spec(
        suite_id="contract/analysis-schema",
        family="contract",
        suite="analysis-schema",
        stages=["all"],
        supported_hosts=["windows", "macos", "linux"],
        level="code",
        primary_module_id="analysis",
        module_ids=["analysis"],
        subsystem_ids=["analysis-contracts"],
        doc_refs=[MODULE_MAP_DOC],
        execution={
            "kind": "contract-check",
            "target": "analysis-schema",
            "contract_check": "analysis-schema",
        },
    ),
    _suite_spec(
        suite_id="contract/managed-closure-bundle",
        family="contract",
        suite="managed-closure-bundle",
        stages=["all"],
        supported_hosts=["windows", "macos", "linux"],
        level="code",
        primary_module_id="managed-closure",
        module_ids=["managed-closure"],
        subsystem_ids=["managed-pipeline"],
        doc_refs=[MODULE_MAP_DOC],
        execution={
            "kind": "python-unittest",
            "target": "managed-closure-bundle",
            "test_module": "tests.unit.run.test_managed_closure_contract_bundle",
            "artifacts": [
                "tests/contracts/analysis/v0/samples",
            ],
        },
    ),
    _suite_spec(
        suite_id="contract/trace-schema",
        family="contract",
        suite="trace-schema",
        stages=["all"],
        supported_hosts=["windows", "macos", "linux"],
        level="code",
        primary_module_id="trace-export",
        module_ids=["trace-export"],
        subsystem_ids=["hosting"],
        doc_refs=[TRACE_EXPORT_DOC],
        execution={
            "kind": "contract-check",
            "target": "trace-schema",
            "contract_check": "trace-schema",
        },
    ),
    _suite_spec(
        suite_id="contract/native-abi",
        family="contract",
        suite="native-abi",
        stages=["all"],
        supported_hosts=["windows", "macos", "linux"],
        level="code",
        primary_module_id="interop",
        module_ids=["interop"],
        subsystem_ids=["native-bridge"],
        doc_refs=[MODULE_MAP_DOC],
        execution={
            "kind": "native-contract",
            "target": "abi",
            "source_dir": "tests/contracts/native/abi",
            "binary_dir": "artifacts/run/native-contract-abi",
        },
    ),
    _suite_spec(
        suite_id="contract/native-bridge",
        family="contract",
        suite="native-bridge",
        stages=["all"],
        supported_hosts=["windows", "macos", "linux"],
        level="code",
        primary_module_id="interop",
        module_ids=["interop"],
        subsystem_ids=["native-bridge"],
        doc_refs=[MODULE_MAP_DOC],
        execution={
            "kind": "native-contract",
            "target": "bridge",
            "source_dir": "tests/contracts/native/bridge",
            "binary_dir": "artifacts/run/native-contract-bridge",
        },
    ),
    _suite_spec(
        suite_id="contract/trace-compare-windows",
        family="contract",
        suite="trace-compare-windows",
        stages=["all", "run"],
        supported_hosts=["windows"],
        level="code",
        primary_module_id="trace-export",
        module_ids=["trace-export"],
        subsystem_ids=["hosting"],
        doc_refs=[MODULE_MAP_DOC],
        execution={
            "kind": "trace-compare",
            "target": "windows",
            "subject_id": SOLUTION_CORE_PACK,
            "goal_id": "correctness.platform",
            "matrix_id": "windows-managed-trace",
        },
    ),
    _suite_spec(
        suite_id="contract/trace-compare-macos",
        family="contract",
        suite="trace-compare-macos",
        stages=["all", "run"],
        supported_hosts=["macos"],
        level="code",
        primary_module_id="trace-export",
        module_ids=["trace-export"],
        subsystem_ids=["hosting"],
        doc_refs=[MODULE_MAP_DOC],
        execution={
            "kind": "trace-compare",
            "target": "macos",
            "subject_id": SOLUTION_CORE_PACK,
            "goal_id": "correctness.platform",
            "matrix_id": "macos-managed-trace",
        },
    ),
    _suite_spec(
        suite_id="gate/windows-reference-desktop",
        family="gate",
        suite="windows-reference-desktop",
        stages=["all"],
        supported_hosts=["windows"],
        level="system",
        primary_module_id="runtime-baseline",
        module_ids=["runtime-baseline"],
        subsystem_ids=["workflow"],
        doc_refs=[PLATFORM_GATES_DOC],
        execution={
            "kind": "reference-desktop-gate",
            "target": "windows-reference-desktop",
            "preset": "windows-x64-reference",
            "binary_dir": "artifacts/presets/windows-x64-reference",
            "subject_id": SOLUTION_CORE_PACK,
            "goal_id": "correctness.platform",
            "matrix_id": "windows-managed-trace",
            "gate_record_path": "artifacts/verify-runtime-baseline/windows/windows-reference-desktop.gate.json",
            "gate_name": "windows-reference-desktop",
            "gate_preset": "windows-x64-reference",
            "gate_notes": "Windows reference desktop gate passed with warmup trace compare.",
        },
    ),
    _suite_spec(
        suite_id="gate/macos-reference-desktop",
        family="gate",
        suite="macos-reference-desktop",
        stages=["all"],
        supported_hosts=["macos"],
        level="system",
        primary_module_id="runtime-baseline",
        module_ids=["runtime-baseline"],
        subsystem_ids=["workflow"],
        doc_refs=[PLATFORM_GATES_DOC],
        execution={
            "kind": "reference-desktop-gate",
            "target": "macos-reference-desktop",
            "preset": "macos-reference",
            "binary_dir": "artifacts/presets/macos-reference",
            "subject_id": SOLUTION_CORE_PACK,
            "goal_id": "correctness.platform",
            "matrix_id": "macos-managed-trace",
            "gate_record_path": "artifacts/verify-runtime-baseline/macos/macos-reference-desktop.gate.json",
            "gate_name": "macos-reference-desktop",
            "gate_preset": "macos-reference",
            "gate_notes": "macOS reference desktop gate passed with warmup trace compare.",
        },
    ),
    _suite_spec(
        suite_id="gate/android-arm64-smoke",
        family="gate",
        suite="android-arm64-smoke",
        stages=["all"],
        supported_hosts=["windows"],
        level="system",
        primary_module_id="runtime-baseline",
        module_ids=["runtime-baseline"],
        subsystem_ids=["workflow"],
        doc_refs=[PLATFORM_GATES_DOC],
        execution={
            "kind": "platform-gate",
            "target": "android-arm64-smoke",
            "preset_target": "android-arm64-smoke",
            "toolchain_file": "build/toolchains/android-arm64.cmake",
            "binary_dir": "artifacts/run/platform/android-arm64-smoke",
        },
    ),
    _suite_spec(
        suite_id="gate/ios-arm64-packaging",
        family="gate",
        suite="ios-arm64-packaging",
        stages=["all"],
        supported_hosts=["macos"],
        level="system",
        primary_module_id="runtime-baseline",
        module_ids=["runtime-baseline"],
        subsystem_ids=["workflow"],
        doc_refs=[PLATFORM_GATES_DOC],
        execution={
            "kind": "platform-gate",
            "target": "ios-arm64-packaging",
            "preset_target": "ios-arm64-packaging",
            "toolchain_file": "build/toolchains/ios-arm64.cmake",
            "binary_dir": "artifacts/run/platform/ios-arm64-packaging",
        },
    ),
    _suite_spec(
        suite_id="gate/linux-x64-packaging",
        family="gate",
        suite="linux-x64-packaging",
        stages=["all"],
        supported_hosts=["windows", "macos", "linux"],
        level="system",
        primary_module_id="runtime-baseline",
        module_ids=["runtime-baseline"],
        subsystem_ids=["workflow"],
        doc_refs=[PLATFORM_GATES_DOC],
        execution={
            "kind": "platform-gate",
            "target": "linux-x64-packaging",
            "preset_target": "linux-x64-packaging",
            "toolchain_file": "build/toolchains/linux-x64.cmake",
            "binary_dir": "artifacts/run/platform/linux-x64-packaging",
        },
    ),
    _suite_spec(
        suite_id="workflow/runtime-baseline-windows",
        family="workflow",
        suite="runtime-baseline-windows",
        stages=["all"],
        supported_hosts=["windows"],
        level="system",
        primary_module_id="runtime-baseline",
        module_ids=["runtime-baseline"],
        subsystem_ids=["workflow"],
        doc_refs=[TEST_TAXONOMY_DOC],
        execution={
            "kind": "registry-object",
            "object_kind": "system",
            "object_id": "system/runtime-baseline-windows",
        },
    ),
    _suite_spec(
        suite_id="workflow/runtime-baseline-macos",
        family="workflow",
        suite="runtime-baseline-macos",
        stages=["all"],
        supported_hosts=["macos"],
        level="system",
        primary_module_id="runtime-baseline",
        module_ids=["runtime-baseline"],
        subsystem_ids=["workflow"],
        doc_refs=[TEST_TAXONOMY_DOC],
        execution={
            "kind": "registry-object",
            "object_kind": "system",
            "object_id": "system/runtime-baseline-macos",
        },
    ),
]


def find_public_test_suite_spec(family: str | None, suite: str | None) -> dict[str, Any] | None:
    for item in PUBLIC_TEST_SPECS:
        if item["family"] == family and item["suite"] == suite:
            return item
    return None


def find_public_test_suite_by_id(suite_id: str) -> dict[str, Any] | None:
    for item in PUBLIC_TEST_SPECS:
        if item["id"] == suite_id:
            return item
    return None


def list_public_test_suites(host_platform: str) -> list[dict[str, Any]]:
    return [
        {
            "id": item["id"],
            "type": "suite",
            "family": item["family"],
            "suite": item["suite"],
            "stages": list(item["stages"]),
            "level": item.get("level", "code"),
            "primaryModuleId": item.get("primaryModuleId"),
            "moduleIds": list(item.get("moduleIds", [])),
            "subsystemIds": list(item.get("subsystemIds", [])),
            "docRefs": list(item.get("docRefs", [])),
            "supportedHosts": list(item["supported_hosts"]),
            "executionKind": str(dict(item.get("execution") or {}).get("kind") or ""),
            "targetSubjectId": str(dict(item.get("execution") or {}).get("targetSubjectId") or ""),
        }
        for item in PUBLIC_TEST_SPECS
        if host_platform in item["supported_hosts"]
    ]


def resolve_public_test_execution_spec(
    family: str,
    suite: str,
    *,
    host_platform: str,
) -> dict[str, Any] | None:
    suite_spec = find_public_test_suite_spec(family, suite)
    if suite_spec is None:
        return None
    if host_platform not in suite_spec["supported_hosts"]:
        return None
    return dict(suite_spec.get("execution") or {})
