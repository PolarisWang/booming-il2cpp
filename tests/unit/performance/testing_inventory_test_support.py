from __future__ import annotations

import json
import shutil
import unittest
from pathlib import Path
from typing import Any

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module
from build.toolchains.run.core.common import read_json
from build.toolchains.run.testing import verification_layout as vlayout


REPO_ROOT = Path(__file__).resolve().parents[3]
INVENTORY_SOURCE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "inventory_source.py"
INVENTORY_GENERATOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "inventory_generator.py"
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"


def load_inventory_source_module(module_name: str):
    return load_module(INVENTORY_SOURCE_MODULE_PATH, module_name)


def load_inventory_generator_module(module_name: str):
    return load_module(INVENTORY_GENERATOR_MODULE_PATH, module_name)


def inventory_fixture() -> dict[str, Any]:
    subject_id = "FixtureSubject"
    capability_family = 7
    capability_item = 31
    orphan_capability_family = 12
    orphan_capability_item = 52
    unit_stable_id = f"{subject_id}::Fixture.Tests::Fixture.Proofs.NativeInteropProof::Run()"
    benchmark_stable_id = (
        f"{subject_id}::Fixture.Benchmarks::Fixture.Benchmarks.NativeInteropBenchmark::RunWorkload()"
    )
    workload_entry = "Fixture.Benchmarks/NativeInteropBenchmark::RunWorkload()"
    source_entry = "Fixture.Tests/NativeInteropProof::Run()"
    device_id = "fixture-device"
    return {
        "subjectId": subject_id,
        "displayName": "Fixture Subject",
        "capabilityFamily": capability_family,
        "capabilityItem": capability_item,
        "orphanCapabilityFamily": orphan_capability_family,
        "orphanCapabilityItem": orphan_capability_item,
        "unitStableId": unit_stable_id,
        "benchmarkStableId": benchmark_stable_id,
        "unitAlias": "native-interop-proof",
        "benchmarkAlias": "native-interop-bench",
        "unitAssemblyName": "Fixture.Tests",
        "unitDeclaringType": "Fixture.Proofs.NativeInteropProof",
        "benchmarkAssemblyName": "Fixture.Benchmarks",
        "benchmarkDeclaringType": "Fixture.Benchmarks.NativeInteropBenchmark",
        "sourceEntry": source_entry,
        "workloadEntry": workload_entry,
        "deviceId": device_id,
        "deviceName": "Fixture Device",
        "platformId": "windows-x64",
        "gitCommit": "fixture123",
        "recordedAt": "2026-04-22T07:32:00Z",
    }


def benchmark_mode_status_payload(fixture: dict[str, Any]) -> dict[str, Any]:
    return {
        "managed": {
            "mode": "managed",
            "status": "recorded",
            "deviceId": fixture["deviceId"],
            "deviceName": fixture["deviceName"],
            "recordedAt": fixture["recordedAt"],
            "gitCommit": fixture["gitCommit"],
            "metrics": {
                "meanDurationMs": 12.5,
                "meanOpsPerSecond": 80.0,
            },
            "isStale": False,
            "staleReasonCode": "",
            "staleReasonLabel": "",
            "reasonCode": "recorded",
            "reasonLabel": "Benchmark record captured.",
        },
        "native": {
            "mode": "native",
            "status": "missing",
            "reasonCode": "missing-record",
            "reasonLabel": "Declared by case contract, but no benchmark record was found.",
        },
        "interpreter": {
            "mode": "interpreter",
            "status": "unsupported",
            "reasonCode": "unsupported-by-contract",
            "reasonLabel": "This mode is not declared by the subject manifest.",
        },
    }


def benchmark_key_metrics_payload(fixture: dict[str, Any]) -> dict[str, Any]:
    return {
        "managedBaseline": {
            "mode": "managed",
            "status": "recorded",
            "reasonCode": "recorded",
            "reasonLabel": "Benchmark record captured.",
            "durationMs": 12.5,
            "opsPerSecond": 80.0,
            "recordedAt": fixture["recordedAt"],
            "gitCommit": fixture["gitCommit"],
            "isStale": False,
            "staleReasonCode": "",
            "staleReasonLabel": "",
        },
        "relativeToManaged": {
            "native": {
                "mode": "native",
                "status": "missing",
                "reasonCode": "missing-record",
                "reasonLabel": "Declared by case contract, but no benchmark record was found.",
                "direction": "faster",
                "ratio": None,
                "durationMs": None,
                "opsPerSecond": None,
                "recordedAt": None,
                "gitCommit": None,
                "baselineStatus": "recorded",
                "baselineReasonCode": "recorded",
                "baselineReasonLabel": "Benchmark record captured.",
                "isStale": False,
                "staleReasonCode": "",
                "staleReasonLabel": "",
            },
            "interpreter": {
                "mode": "interpreter",
                "status": "unsupported",
                "reasonCode": "unsupported-by-contract",
                "reasonLabel": "This mode is not declared by the subject manifest.",
                "direction": "slower",
                "ratio": None,
                "durationMs": None,
                "opsPerSecond": None,
                "recordedAt": None,
                "gitCommit": None,
                "baselineStatus": "recorded",
                "baselineReasonCode": "recorded",
                "baselineReasonLabel": "Benchmark record captured.",
                "isStale": False,
                "staleReasonCode": "",
                "staleReasonLabel": "",
            },
        },
        "nativeSpeedup": None,
        "interpreterOverhead": None,
    }


def benchmark_summary_case_payload(fixture: dict[str, Any]) -> dict[str, Any]:
    subject_id = str(fixture["subjectId"])
    return {
        "caseId": fixture["benchmarkStableId"],
        "stableId": fixture["benchmarkStableId"],
        "alias": fixture["benchmarkAlias"],
        "displayName": fixture["benchmarkAlias"],
        "workloadEntry": fixture["workloadEntry"],
        "entryIndex": 7,
        "assemblyName": "Fixture.Benchmarks",
        "declaringType": "Fixture.Benchmarks.NativeInteropBenchmark",
        "methodName": "RunWorkload",
        "methodSignature": "RunWorkload()",
        "category": 1,
        "categoryLabel": "Runtime Dispatch",
        "metricLabels": ["Wall Clock", "Managed Alloc"],
        "requirementLabels": ["Native Interop"],
        "archetype": 4,
        "archetypeLabel": "Reference Assembly Solution",
        "hotUpdateCapability": 0,
        "hotUpdateCapabilityLabels": [],
        "capabilityFamily": int(fixture["capabilityFamily"]),
        "capabilityFamilyLabel": "Interop And Marshaling",
        "capabilityItem": int(fixture["capabilityItem"]),
        "capabilityItemLabel": "Native Call Interop",
        "ownerSubjectId": subject_id,
        "supportStates": [1, 5],
        "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
        "proofRequired": True,
        "benchmarkRequired": True,
        "supportedModes": ["managed", "native"],
    }


def benchmark_case_payload(fixture: dict[str, Any]) -> dict[str, Any]:
    subject_id = str(fixture["subjectId"])
    return {
        "caseId": fixture["benchmarkStableId"],
        "stableId": fixture["benchmarkStableId"],
        "alias": fixture["benchmarkAlias"],
        "displayName": fixture["benchmarkAlias"],
        "workloadEntry": fixture["workloadEntry"],
        "entryIndex": 7,
        "assemblyName": "Fixture.Benchmarks",
        "declaringType": "Fixture.Benchmarks.NativeInteropBenchmark",
        "methodName": "RunWorkload",
        "methodSignature": "RunWorkload()",
        "category": 1,
        "categoryLabel": "Runtime Dispatch",
        "metrics": 3,
        "metricLabels": ["Wall Clock", "Managed Alloc"],
        "modes": 3,
        "supportedModes": ["managed", "native"],
        "requires": 16,
        "requirementLabels": ["Native Interop"],
        "archetype": 4,
        "archetypeLabel": "Reference Assembly Solution",
        "hotUpdateCapability": 0,
        "hotUpdateCapabilityLabels": [],
        "capabilityFamily": int(fixture["capabilityFamily"]),
        "capabilityFamilyLabel": "Interop And Marshaling",
        "capabilityItem": int(fixture["capabilityItem"]),
        "capabilityItemLabel": "Native Call Interop",
        "ownerSubjectId": subject_id,
        "supportStates": [1, 5],
        "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
        "proofRequired": True,
        "benchmarkRequired": True,
        "recordedModes": ["managed"],
        "missingModes": ["native"],
        "staleModes": [],
        "unsupportedModes": ["interpreter"],
        "modeStatus": benchmark_mode_status_payload(fixture),
        "coverage": {
            "supportedModeCount": 2,
            "recordedModeCount": 1,
            "missingModeCount": 1,
            "staleModeCount": 0,
            "unsupportedModeCount": 1,
            "isComplete": False,
            "needsAttention": True,
        },
        "lastRecordedAt": fixture["recordedAt"],
        "gitCommit": fixture["gitCommit"],
        "isStale": False,
        "deviceId": fixture["deviceId"],
        "deviceName": fixture["deviceName"],
        "platformId": fixture["platformId"],
        "keyMetrics": {
            "relativeToManaged": {
                "native": {
                    "status": "missing",
                    "reasonCode": "missing-record",
                    "reasonLabel": "Declared by case contract, but no benchmark record was found.",
                },
                "interpreter": {
                    "status": "unsupported",
                    "reasonCode": "unsupported-by-contract",
                    "reasonLabel": "This mode is not declared by the subject manifest.",
                },
            }
        },
    }


def benchmark_subject_payload(fixture: dict[str, Any]) -> dict[str, Any]:
    subject_id = str(fixture["subjectId"])
    platform_id = str(fixture["platformId"])
    device_id = str(fixture["deviceId"])
    case_id = str(fixture["benchmarkStableId"])
    case_payload = benchmark_case_payload(fixture)
    return {
        "subjectId": subject_id,
        "displayName": str(fixture["displayName"]),
        "availablePlatforms": [platform_id],
        "summaryWorkloadEntry": str(fixture["workloadEntry"]),
        "summaryBenchmarkCase": benchmark_summary_case_payload(fixture),
        "supportedModesByPlatform": {
            platform_id: ["managed", "native"],
        },
        "platforms": {
            platform_id: {
                "subjectId": subject_id,
                "displayName": str(fixture["displayName"]),
                "platformId": platform_id,
                "deviceId": device_id,
                "deviceName": str(fixture["deviceName"]),
                "supportedModes": ["managed", "native"],
                "recordedModes": ["managed"],
                "missingModes": ["native"],
                "staleModes": [],
                "unsupportedModes": ["interpreter"],
                "modeStatus": benchmark_mode_status_payload(fixture),
                "coverage": {
                    "supportedModeCount": 2,
                    "recordedModeCount": 1,
                    "missingModeCount": 1,
                    "staleModeCount": 0,
                    "unsupportedModeCount": 1,
                    "isComplete": False,
                    "needsAttention": True,
                },
                "lastRecordedAt": fixture["recordedAt"],
                "gitCommit": fixture["gitCommit"],
                "isStale": False,
                "comparison": None,
                "verdict": None,
                "keyMetrics": benchmark_key_metrics_payload(fixture),
            }
        },
        "latestByDevice": {
            device_id: {
                case_id: {
                    "modeStatus": benchmark_mode_status_payload(fixture),
                    "lastRecordedAt": fixture["recordedAt"],
                    "gitCommit": fixture["gitCommit"],
                }
            }
        },
        "benchmarkCasesByDevice": {
            device_id: {
                case_id: case_payload,
            }
        },
        "caseSummaryByDevice": {
            device_id: {
                "caseCount": 1,
                "crossModeCaseCount": 1,
                "managedOnlyCaseCount": 0,
                "fullyRecordedCaseCount": 0,
                "missingCaseCount": 1,
                "staleCaseCount": 0,
                "attentionCaseCount": 1,
            }
        },
    }


def _declared_unit_test_record(fixture: dict[str, Any]) -> dict[str, Any]:
    subject_id = str(fixture["subjectId"])
    return {
        "id": f"declared-unit-test/{fixture['unitStableId']}",
        "type": "declared-unit-test",
        "displayName": str(fixture["unitAlias"]),
        "subjectId": subject_id,
        "defaultGoalId": "correctness.dev",
        "defaultMatrixId": "windows-native-check",
        "goalIds": ["correctness.dev"],
        "matrixIds": ["windows-native-check"],
        "supportedHosts": ["windows"],
        "level": "subject",
        "primaryModuleId": None,
        "moduleIds": [],
        "subsystemIds": [],
        "docRefs": [],
        "canonicalCommand": f"run test declared-unit-test --id declared-unit-test/{fixture['unitStableId']}",
        "stableId": str(fixture["unitStableId"]),
        "alias": str(fixture["unitAlias"]),
        "entryIndex": int(fixture.get("unitEntryIndex") or 2),
        "assemblyName": str(fixture.get("unitAssemblyName") or "Fixture.Tests"),
        "declaringType": str(fixture.get("unitDeclaringType") or "Fixture.Proofs.NativeInteropProof"),
        "methodName": "Run",
        "methodSignature": "Run()",
        "sourceEntry": str(fixture["sourceEntry"]),
        "workloadEntry": "",
        "category": 3,
        "categoryLabel": "Interop Contract",
        "capabilityFamily": int(fixture["capabilityFamily"]),
        "capabilityFamilyLabel": "Interop And Marshaling",
        "capabilityItem": int(fixture["capabilityItem"]),
        "capabilityItemLabel": "Native Call Interop",
        "ownerSubjectId": subject_id,
        "supportStates": [1, 5],
        "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
        "proofRequired": True,
        "benchmarkRequired": True,
        "archetype": int(fixture.get("archetype") or 4),
        "archetypeLabel": str(fixture.get("archetypeLabel") or "Reference Assembly Solution"),
        "hotUpdateCapability": int(fixture.get("hotUpdateCapability") or 0),
        "hotUpdateCapabilityLabels": list(fixture.get("hotUpdateCapabilityLabels") or []),
        "requires": int(fixture.get("requires") or 16),
        "requirementLabels": list(fixture.get("requirementLabels") or ["Native Interop"]),
        "evidence": int(fixture.get("evidence") or 4),
        "evidenceLabels": list(fixture.get("evidenceLabels") or ["Native Symbol"]),
        "priority": int(fixture.get("priority") or 2),
    }


def make_registry_index(
    registry_module,
    fixture: dict[str, Any],
    *,
    declared_unit_test_fixtures: list[dict[str, Any]] | None = None,
):
    subject_id = str(fixture["subjectId"])
    declared_unit_test_fixtures = list(declared_unit_test_fixtures or [fixture])
    return registry_module.RegistryIndex(
        host_platform="windows",
        suites=[],
        subjects=[
            {
                "id": f"subject/{subject_id}",
                "type": "subject",
                "displayName": str(fixture["displayName"]),
                "subjectId": subject_id,
                "defaultGoalId": "correctness.dev",
                "defaultMatrixId": "windows-native-check",
                "goalIds": ["correctness.dev", "perf.release"],
                "matrixIds": ["windows-native-check", "windows-native-perf"],
                "supportedHosts": ["windows"],
                "level": "subject",
                "primaryModuleId": None,
                "moduleIds": [],
                "subsystemIds": [],
                "docRefs": [],
                "canonicalCommand": f"run test subject --id subject/{subject_id}",
            }
        ],
        engineering_validations=[],
        engineering_workloads=[],
        declared_unit_tests=[_declared_unit_test_record(item) for item in declared_unit_test_fixtures],
        declared_benchmarks=[
            {
                "id": f"declared-benchmark/{fixture['benchmarkStableId']}",
                "type": "declared-benchmark",
                "displayName": str(fixture["benchmarkAlias"]),
                "subjectId": subject_id,
                "defaultGoalId": "perf.release",
                "defaultMatrixId": "windows-native-perf",
                "goalIds": ["perf.release"],
                "matrixIds": ["windows-native-perf"],
                "supportedHosts": ["windows"],
                "level": "subject",
                "primaryModuleId": None,
                "moduleIds": [],
                "subsystemIds": [],
                "docRefs": [],
                "canonicalCommand": (
                    f"run test declared-benchmark --id declared-benchmark/{fixture['benchmarkStableId']}"
                ),
                "stableId": str(fixture["benchmarkStableId"]),
                "alias": str(fixture["benchmarkAlias"]),
                "entryIndex": 7,
                "assemblyName": str(fixture.get("benchmarkAssemblyName") or "Fixture.Benchmarks"),
                "declaringType": str(
                    fixture.get("benchmarkDeclaringType") or "Fixture.Benchmarks.NativeInteropBenchmark"
                ),
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
                "sourceEntry": str(fixture["sourceEntry"]),
                "workloadEntry": str(fixture["workloadEntry"]),
                "category": 1,
                "categoryLabel": "Runtime Dispatch",
                "capabilityFamily": int(fixture["capabilityFamily"]),
                "capabilityFamilyLabel": "Interop And Marshaling",
                "capabilityItem": int(fixture["capabilityItem"]),
                "capabilityItemLabel": "Native Call Interop",
                "ownerSubjectId": subject_id,
                "supportStates": [1, 5],
                "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
                "proofRequired": True,
                "benchmarkRequired": True,
                "archetype": 4,
                "archetypeLabel": "Reference Assembly Solution",
                "hotUpdateCapability": 0,
                "hotUpdateCapabilityLabels": [],
                "requires": 16,
                "requirementLabels": ["Native Interop"],
                "metrics": 3,
                "metricLabels": ["Wall Clock", "Managed Alloc"],
                "modes": 3,
                "supportedModes": ["managed", "native"],
                "warmupCount": 2,
                "iterationCount": 5,
                "invocationCount": 10,
            }
        ],
        module_verifications=[],
        system_scenarios=[],
        errors=[],
        warnings=[],
    )


def write_inventory_fixture_repo(repo_root: Path, fixture: dict[str, Any]) -> None:
    subject_id = str(fixture["subjectId"])
    owner_root = vlayout.owner_root(repo_root, subject_id)
    workspace_root = vlayout.subject_workspace_root(repo_root, subject_id)
    raw_records_root = vlayout.raw_benchmark_records_root(repo_root) / subject_id
    codegen_root = vlayout.owner_codegen_stubs_root(repo_root, subject_id)
    features_path = owner_root / "owner.features.json"
    write_json(
        features_path,
        {
            "schemaVersion": 1,
            "subjectId": subject_id,
            "features": [
                {
                    "capabilityFamily": int(fixture["capabilityFamily"]),
                    "capabilityItem": int(fixture["capabilityItem"]),
                    "supportStates": [1, 5],
                    "proofRequired": True,
                    "benchmarkRequired": True,
                },
                {
                    "capabilityFamily": int(fixture["orphanCapabilityFamily"]),
                    "capabilityItem": int(fixture["orphanCapabilityItem"]),
                    "supportStates": [1],
                    "proofRequired": True,
                    "benchmarkRequired": True,
                },
            ],
        },
    )
    write_json(
        owner_root / "owner.manifest.json",
        {
            "subjectId": subject_id,
            "displayName": str(fixture["displayName"]),
            "source": {
                "type": "dotnet-project",
                "path": f"artifact/verification-catalog/catalog/owners/{subject_id}/support/host/{subject_id}.csproj",
            },
            "workloadEntry": str(fixture["workloadEntry"]),
            "executionPipelines": [
                {
                    "pipelineId": "managed-benchmark",
                    "stages": [
                        {
                            "stageId": "managed-stage",
                            "kind": "runtime-perf-collect",
                            "scope": "matrix",
                            "bucket": "runtime",
                            "dependsOn": [],
                        }
                    ],
                },
                {
                    "pipelineId": "native-benchmark",
                    "stages": [
                        {
                            "stageId": "native-stage",
                            "kind": "native-runtime-perf",
                            "scope": "matrix",
                            "bucket": "runtime",
                            "dependsOn": [],
                        }
                    ],
                },
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-perf",
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": str(fixture["platformId"]),
                        "targetPlatform": str(fixture["platformId"]),
                        "runtimeProfile": "managed-perf-release",
                    },
                },
                {
                    "matrixId": "windows-native-perf",
                    "pipelineId": "native-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": str(fixture["platformId"]),
                        "targetPlatform": str(fixture["platformId"]),
                        "runtimeProfile": "native-perf-release",
                    },
                },
            ],
        },
    )
    (owner_root / "proofs").mkdir(parents=True, exist_ok=True)
    (owner_root / "support" / "host").mkdir(parents=True, exist_ok=True)
    raw_records_root.mkdir(parents=True, exist_ok=True)
    (codegen_root / "windows-native-check").mkdir(parents=True, exist_ok=True)

    collection_path = workspace_root / "managed-tests" / "Generated" / "declared-tests.collection.json"
    write_json(
        collection_path,
        {
            "subjectId": subject_id,
            "frameworkReferenced": True,
            "subjectKind": "declared-test",
            "warningCodes": [],
            "declaredUnitTests": [
                {
                    "stableId": fixture["unitStableId"],
                    "entryIndex": 2,
                    "alias": fixture["unitAlias"],
                    "assemblyName": str(fixture.get("unitAssemblyName") or "Fixture.Tests"),
                    "declaringType": str(fixture.get("unitDeclaringType") or "Fixture.Proofs.NativeInteropProof"),
                    "methodName": "Run",
                    "methodSignature": "Run()",
                }
            ],
            "declaredBenchmarks": [
                {
                    "stableId": fixture["benchmarkStableId"],
                    "entryIndex": 7,
                    "alias": fixture["benchmarkAlias"],
                    "assemblyName": str(fixture.get("benchmarkAssemblyName") or "Fixture.Benchmarks"),
                    "declaringType": str(
                        fixture.get("benchmarkDeclaringType") or "Fixture.Benchmarks.NativeInteropBenchmark"
                    ),
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                }
            ],
        },
    )
    write_json(
        workspace_root / "workspace.manifest.json",
        {
            "workspaceVersion": 2,
            "kind": "subject-workspace",
            "subjectId": subject_id,
            "defaultMatrixId": "windows-native-check",
            "managedTestProjects": [
                {
                    "projectId": f"managed-test/{subject_id}/proof-host",
                    "hostKind": "proof-host",
                    "collectionPath": collection_path.relative_to(repo_root).as_posix(),
                },
                {
                    "projectId": f"managed-test/{subject_id}/benchmark-host",
                    "hostKind": "benchmark-host",
                    "collectionPath": collection_path.relative_to(repo_root).as_posix(),
                },
            ],
            "nativeTestProjects": [
                {
                    "projectId": f"native-test/{subject_id}/windows-native-check/proof-host",
                    "matrixId": "windows-native-check",
                    "hostKind": "proof-host",
                }
            ],
            "matrices": [
                {
                    "matrixId": "windows-native-check",
                    "goalIds": ["correctness.dev"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                }
            ],
        },
    )
    append_benchmark_record(
        repo_root,
        fixture,
        mode="managed",
        mean_duration_ms=12.5,
        mean_ops_per_second=80.0,
    )


def write_inventory_fixture_repo_with_declared_units(
    repo_root: Path,
    fixture: dict[str, Any],
    declared_unit_test_fixtures: list[dict[str, Any]],
) -> None:
    write_inventory_fixture_repo(repo_root, fixture)
    subject_id = str(fixture["subjectId"])
    collection_path = (
        vlayout.subject_workspace_root(repo_root, subject_id)
        / "managed-tests"
        / "Generated"
        / "declared-tests.collection.json"
    )
    collection_payload = read_json(collection_path)
    collection_payload["declaredUnitTests"] = [
        {
            "stableId": str(item["unitStableId"]),
            "entryIndex": int(item.get("unitEntryIndex") or 2),
            "alias": str(item["unitAlias"]),
            "assemblyName": str(item.get("unitAssemblyName") or "Fixture.Tests"),
            "declaringType": str(item.get("unitDeclaringType") or "Fixture.Proofs.NativeInteropProof"),
            "methodName": "Run",
            "methodSignature": "Run()",
        }
        for item in declared_unit_test_fixtures
    ]
    write_json(collection_path, collection_payload)


def append_benchmark_record(
    repo_root: Path,
    fixture: dict[str, Any],
    *,
    mode: str,
    mean_duration_ms: float,
    mean_ops_per_second: float,
    device_id: str | None = None,
    device_name: str | None = None,
    platform_id: str | None = None,
    recorded_at: str | None = None,
    git_commit: str | None = None,
    benchmark_case_overrides: dict[str, Any] | None = None,
) -> None:
    subject_id = str(fixture["subjectId"])
    records_path = vlayout.raw_benchmark_records_path(repo_root, subject_id)
    records_path.parent.mkdir(parents=True, exist_ok=True)

    benchmark_case = benchmark_case_payload(fixture)
    if benchmark_case_overrides:
        benchmark_case.update(dict(benchmark_case_overrides))

    payload = {
        "subject": subject_id,
        "mode": mode,
        "platform": str(platform_id or fixture["platformId"]),
        "device": {
            "id": str(device_id or fixture["deviceId"]),
            "name": str(device_name or fixture["deviceName"]),
        },
        "metrics": {
            "meanDurationMs": mean_duration_ms,
            "meanOpsPerSecond": mean_ops_per_second,
        },
        "recordedAt": str(recorded_at or fixture["recordedAt"]),
        "gitCommit": str(git_commit or fixture["gitCommit"]),
        "benchmarkCase": benchmark_case,
    }
    with records_path.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(payload, ensure_ascii=False) + "\n")


def write_legacy_subject_proof_run(
    repo_root: Path,
    fixture: dict[str, Any],
    *,
    run_id: str,
    stage_kind: str,
    status: str = "ok",
    stable_id: str | None = None,
    alias: str | None = None,
    entry_index: int = 2,
    matrix_id: str | None = None,
    runtime_profile: str | None = None,
    include_entry_selection: bool = True,
    preferred_dispatch_subject_id: str | None = None,
) -> None:
    subject_id = str(fixture["subjectId"])
    stable_id = str(stable_id or fixture["unitStableId"])
    alias = str(alias or fixture["unitAlias"])
    matrix_id = str(matrix_id or f"windows-{stage_kind}")
    runtime_profile = str(
        runtime_profile
        or {
            "managed-proof": "managed-proof-output",
            "native-proof": "native-proof-output",
            "hotupdate-proof": "native-hotupdate-proof-output",
        }.get(stage_kind, "managed-proof-output")
    )
    run_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id
    pipeline_report_path = run_root / "matrices" / matrix_id / "pipeline-report" / "report.json"
    generated_manifest_path = run_root / "analysis" / "generated" / "native-reference.manifest.json"
    write_json(
        run_root / "run-report" / "summary.json",
        {
            "runId": run_id,
            "command": "test subject",
            "hostPlatform": "windows",
            "finalStatus": status,
            "exitCode": 0 if status == "ok" else 1,
            "errors": [] if status == "ok" else [f"{stage_kind} failed"],
            "artifacts": [pipeline_report_path.relative_to(repo_root).as_posix()],
            "subjectResults": [
                {
                    "subjectId": subject_id,
                    "requestedGoalId": "correctness.dev",
                    "status": status,
                    "subjectSummaryPath": "",
                }
            ],
        },
    )
    selection = {
        "subjectId": subject_id,
        "goalId": "correctness.dev",
        "matrixId": matrix_id,
        "pipelineId": f"{stage_kind}-pipeline",
        "workloadEntry": str(fixture["workloadEntry"]),
        "executionContext": {
            "hostPlatform": str(fixture["platformId"]),
            "targetPlatform": str(fixture["platformId"]),
            "runtimeProfile": runtime_profile,
        },
    }
    if include_entry_selection:
        selection["entrySelection"] = {
            "family": "declared-unit-test",
            "stableId": stable_id,
            "alias": alias,
            "entryIndex": int(entry_index),
        }
    stage_results = []
    if preferred_dispatch_subject_id:
        write_json(
            generated_manifest_path,
            {
                "artifactKind": "nativeReferenceManifest",
                "preferredAssemblyDispatchSubjectId": str(preferred_dispatch_subject_id),
            },
        )
        stage_results.append(
            {
                "stageId": "generated-native-proof",
                "kind": "generated-native-proof",
                "bucket": "generated",
                "status": status,
                "manifestPath": (
                    run_root / "analysis" / "generated" / "generated.manifest.json"
                ).relative_to(repo_root).as_posix(),
                "primaryEvidencePaths": [
                    generated_manifest_path.relative_to(repo_root).as_posix(),
                ],
            }
        )
    write_json(
        pipeline_report_path,
        {
            "reportVersion": "v1",
            "runId": run_id,
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "correctness.dev",
            "status": status,
            "selection": selection,
            "stageResults": stage_results,
            "matrixProofLinkage": {
                "proofKind": stage_kind,
            },
        },
    )


def write_legacy_declared_unit_report(
    repo_root: Path,
    fixture: dict[str, Any],
    *,
    run_id: str,
    stage_kind: str,
    status: str = "ok",
    matrix_id: str | None = None,
) -> None:
    subject_id = str(fixture["subjectId"])
    matrix_id = str(matrix_id or "windows-native-check")
    alias = str(fixture["unitAlias"])
    run_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id
    declared_report_path = run_root / "matrices" / matrix_id / "declared" / "unit" / alias / "report.json"
    write_json(
        run_root / "run-report" / "summary.json",
        {
            "runId": run_id,
            "command": "run test subject --id subject/FixtureSubject --matrix windows-native-check --json",
            "hostPlatform": "windows",
            "finalStatus": status,
            "exitCode": 0 if status == "ok" else 1,
            "errors": [] if status == "ok" else [f"{stage_kind} failed"],
            "artifacts": [declared_report_path.relative_to(repo_root).as_posix()],
            "subjectResults": [
                {
                    "subjectId": subject_id,
                    "requestedGoalId": "correctness.dev",
                    "status": status,
                    "subjectSummaryPath": "",
                }
            ],
        },
    )
    write_json(
        declared_report_path,
        {
            "reportVersion": "v1",
            "runId": run_id,
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "correctness.dev",
            "status": status,
            "selection": {
                "subjectId": subject_id,
                "goalId": "correctness.dev",
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": str(fixture["unitStableId"]),
                    "alias": alias,
                    "entryIndex": 2,
                },
                "executionContext": {
                    "hostPlatform": str(fixture["platformId"]),
                    "targetPlatform": str(fixture["platformId"]),
                    "runtimeProfile": "managed-proof-output" if stage_kind == "managed-proof" else "native-proof-output",
                },
            },
            "matrixProofLinkage": {
                "proofKind": stage_kind,
            },
        },
    )


def write_legacy_managed_output_proof_run(
    repo_root: Path,
    fixture: dict[str, Any],
    *,
    run_id: str,
    source_entry: str,
    status: str = "ok",
    matrix_id: str = "windows-managed-output",
    stdout_lines: list[str] | None = None,
) -> None:
    subject_id = str(fixture["subjectId"])
    run_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id
    pipeline_report_path = run_root / "matrices" / matrix_id / "pipeline-report" / "report.json"
    stdout_path = run_root / "matrices" / matrix_id / "runtime" / "stdout.log"
    if stdout_lines is not None:
        stdout_path.parent.mkdir(parents=True, exist_ok=True)
        stdout_path.write_text("\n".join(stdout_lines) + "\n", encoding="utf-8")
    write_json(
        run_root / "run-report" / "summary.json",
        {
            "runId": run_id,
            "command": f"run test subject --id subject/{subject_id} --matrix {matrix_id} --json",
            "hostPlatform": "windows",
            "finalStatus": status,
            "exitCode": 0 if status == "ok" else 1,
            "errors": [] if status == "ok" else ["managed proof failed"],
            "artifacts": [pipeline_report_path.relative_to(repo_root).as_posix()],
            "subjectResults": [
                {
                    "subjectId": subject_id,
                    "requestedGoalId": "correctness.dev",
                    "status": status,
                    "subjectSummaryPath": "",
                }
            ],
        },
    )
    write_json(
        pipeline_report_path,
        {
            "reportVersion": "v1",
            "runId": run_id,
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "correctness.dev",
            "validationKinds": ["proof"],
            "status": status,
            "selection": {
                "subjectId": subject_id,
                "goalId": "correctness.dev",
                "matrixId": matrix_id,
                "pipelineId": "managed-runtime-output",
                "source": {
                    "type": "dotnet-project",
                    "path": f"artifact/verification-catalog/catalog/owners/{subject_id}/support/host/{subject_id}.sln",
                    "primaryProjectPath": f"artifact/verification-catalog/catalog/owners/{subject_id}/support/host/{subject_id}.csproj",
                    "entry": source_entry,
                },
                "executionContext": {
                    "hostPlatform": str(fixture["platformId"]),
                    "targetPlatform": str(fixture["platformId"]),
                    "runtimeProfile": "managed-output",
                },
            },
            "stageResults": [
                {
                    "stageId": "runtime-managed-output",
                    "kind": "runtime-managed-output",
                    "bucket": "runtime",
                    "status": status,
                    "primaryEvidencePaths": [stdout_path.relative_to(repo_root).as_posix()] if stdout_lines is not None else [],
                    "diagnostics": {
                        "stdoutPath": stdout_path.relative_to(repo_root).as_posix() if stdout_lines is not None else None,
                        "stderrPath": None,
                    },
                }
            ],
        },
    )


def write_legacy_subject_perf_run(
    repo_root: Path,
    fixture: dict[str, Any],
    *,
    run_id: str,
    mode: str,
    mean_duration_ms: float,
    mean_ops_per_second: float,
    matrix_id: str | None = None,
) -> None:
    subject_id = str(fixture["subjectId"])
    matrix_id = str(matrix_id or f"windows-{mode}-perf")
    run_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id
    pipeline_report_path = run_root / "matrices" / matrix_id / "pipeline-report" / "report.json"
    perf_summary_path = run_root / "matrices" / matrix_id / "validations" / "perf" / "summary.json"
    write_json(
        run_root / "run-report" / "summary.json",
        {
            "runId": run_id,
            "command": "test subject",
            "hostPlatform": "windows",
            "finalStatus": "ok",
            "exitCode": 0,
            "errors": [],
            "artifacts": [
                pipeline_report_path.relative_to(repo_root).as_posix(),
                perf_summary_path.relative_to(repo_root).as_posix(),
            ],
            "subjectResults": [
                {
                    "subjectId": subject_id,
                    "requestedGoalId": "perf.release",
                    "status": "ok",
                    "subjectSummaryPath": "",
                }
            ],
        },
    )
    write_json(
        pipeline_report_path,
        {
            "reportVersion": "v1",
            "runId": run_id,
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "perf.release",
            "status": "ok",
            "selection": {
                "subjectId": subject_id,
                "goalId": "perf.release",
                "matrixId": matrix_id,
                "pipelineId": f"{mode}-benchmark",
                "workloadEntry": str(fixture["workloadEntry"]),
                "executionContext": {
                    "hostPlatform": str(fixture["platformId"]),
                    "targetPlatform": str(fixture["platformId"]),
                    "runtimeProfile": f"{mode}-perf-release",
                },
            },
            "metrics": {
                "sampleCount": 3,
                "meanDurationMs": mean_duration_ms,
                "meanOpsPerSecond": mean_ops_per_second,
            },
            "performance": {
                "metrics": {
                    "sampleCount": 3,
                    "meanDurationMs": mean_duration_ms,
                    "meanOpsPerSecond": mean_ops_per_second,
                }
            },
            "releaseReportPaths": [
                perf_summary_path.relative_to(repo_root).as_posix(),
            ],
        },
    )
    write_json(
        perf_summary_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "perf.release",
            "status": "ok",
            "metrics": {
                "sampleCount": 3,
                "meanDurationMs": mean_duration_ms,
                "meanOpsPerSecond": mean_ops_per_second,
            },
            "regressionStatus": "no-baseline",
        },
    )


def sample_inventory_source() -> dict[str, Any]:
    fixture = inventory_fixture()
    subject_id = str(fixture["subjectId"])
    capability_id = f"capability/{fixture['capabilityFamily']}/{fixture['capabilityItem']}"
    return {
        "schemaVersion": 1,
        "generatedAt": "2026-04-22T07:40:00Z",
        "hostPlatform": "windows",
        "declaredUnitTests": [
            {
                "subjectId": subject_id,
                "stableId": fixture["unitStableId"],
                "alias": fixture["unitAlias"],
                "assemblyName": "Fixture.Tests",
                "declaringType": "Fixture.Proofs.NativeInteropProof",
                "methodName": "Run",
                "methodSignature": "Run()",
                "sourceEntry": fixture["sourceEntry"],
                "entryIndex": 2,
                "category": 3,
                "categoryLabel": "Interop Contract",
                "capabilityFamily": fixture["capabilityFamily"],
                "capabilityFamilyLabel": "Interop And Marshaling",
                "capabilityItem": fixture["capabilityItem"],
                "capabilityItemLabel": "Native Call Interop",
                "ownerSubjectId": subject_id,
                "supportStates": [1, 5],
                "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
                "proofRequired": True,
                "benchmarkRequired": True,
                "archetype": 4,
                "archetypeLabel": "Reference Assembly Solution",
                "hotUpdateCapability": 0,
                "hotUpdateCapabilityLabels": [],
                "requires": 16,
                "requirementLabels": ["Native Interop"],
                "evidence": 4,
                "evidenceLabels": ["Native Symbol"],
                "priority": 2,
                "defaultGoalId": "correctness.dev",
                "defaultMatrixId": "windows-native-check",
                "goalIds": ["correctness.dev"],
                "matrixIds": ["windows-native-check"],
            }
        ],
        "declaredBenchmarks": [
            {
                "subjectId": subject_id,
                "stableId": fixture["benchmarkStableId"],
                "alias": fixture["benchmarkAlias"],
                "assemblyName": "Fixture.Benchmarks",
                "declaringType": "Fixture.Benchmarks.NativeInteropBenchmark",
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
                "sourceEntry": fixture["sourceEntry"],
                "workloadEntry": fixture["workloadEntry"],
                "entryIndex": 7,
                "category": 1,
                "categoryLabel": "Runtime Dispatch",
                "capabilityFamily": fixture["capabilityFamily"],
                "capabilityFamilyLabel": "Interop And Marshaling",
                "capabilityItem": fixture["capabilityItem"],
                "capabilityItemLabel": "Native Call Interop",
                "ownerSubjectId": subject_id,
                "supportStates": [1, 5],
                "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
                "proofRequired": True,
                "benchmarkRequired": True,
                "metricLabels": ["Wall Clock", "Managed Alloc"],
                "supportedModes": ["managed", "native"],
                "warmupCount": 2,
                "iterationCount": 5,
                "invocationCount": 10,
            }
        ],
        "capabilityContracts": [
            {
                "subjectId": subject_id,
                "capabilityFamily": fixture["capabilityFamily"],
                "capabilityFamilyLabel": "Interop And Marshaling",
                "capabilityItem": fixture["capabilityItem"],
                "capabilityItemLabel": "Native Call Interop",
                "ownerSubjectId": subject_id,
                "supportStates": [1, 5],
                "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
                "proofRequired": True,
                "benchmarkRequired": True,
            },
            {
                "subjectId": subject_id,
                "capabilityFamily": fixture["orphanCapabilityFamily"],
                "capabilityFamilyLabel": "Linker And Aot Closure",
                "capabilityItem": fixture["orphanCapabilityItem"],
                "capabilityItemLabel": "Generic Sharing Boundary",
                "ownerSubjectId": subject_id,
                "supportStates": [1],
                "supportStateLabels": ["NativeGenerated"],
                "proofRequired": True,
                "benchmarkRequired": True,
            },
        ],
        "workspaceCollections": [
            {
                "subjectId": subject_id,
                "manifestPath": f"artifact/verification-catalog/workspaces/subjects/{subject_id}/workspace.manifest.json",
                "collectionPath": (
                    f"artifact/verification-catalog/workspaces/subjects/{subject_id}/managed-tests/Generated/declared-tests.collection.json"
                ),
                "declaredUnitTests": [
                    {
                        "stableId": fixture["unitStableId"],
                        "alias": fixture["unitAlias"],
                        "entryIndex": 2,
                    }
                ],
                "declaredBenchmarks": [
                    {
                        "stableId": fixture["benchmarkStableId"],
                        "alias": fixture["benchmarkAlias"],
                        "entryIndex": 7,
                    }
                ],
            }
        ],
        "benchmarkSubjects": {
            subject_id: benchmark_subject_payload(fixture),
        },
        "benchmarkEvidence": [
            {
                "subjectId": subject_id,
                "stableId": fixture["benchmarkStableId"],
                "caseId": fixture["benchmarkStableId"],
                "alias": fixture["benchmarkAlias"],
                "displayName": fixture["benchmarkAlias"],
                "workloadEntry": fixture["workloadEntry"],
                "entryIndex": 7,
                "assemblyName": "Fixture.Benchmarks",
                "declaringType": "Fixture.Benchmarks.NativeInteropBenchmark",
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
                "capabilityFamily": fixture["capabilityFamily"],
                "capabilityFamilyLabel": "Interop And Marshaling",
                "capabilityItem": fixture["capabilityItem"],
                "capabilityItemLabel": "Native Call Interop",
                "ownerSubjectId": subject_id,
                "supportStates": [1, 5],
                "supportStateLabels": ["NativeGenerated", "ExternalRuntime"],
                "proofRequired": True,
                "benchmarkRequired": True,
                "supportedModes": ["managed", "native"],
                "recordedModes": ["managed"],
                "missingModes": ["native"],
                "staleModes": [],
                "unsupportedModes": ["interpreter"],
                "deviceId": fixture["deviceId"],
                "deviceName": fixture["deviceName"],
                "platformId": fixture["platformId"],
                "modeStatus": benchmark_mode_status_payload(fixture),
                "lastRecordedAt": fixture["recordedAt"],
                "gitCommit": fixture["gitCommit"],
                "isStale": False,
                "sourceSubjectPath": f"artifact/verification/benchmark-records/{subject_id}/records.jsonl",
            }
        ],
        "codegenStubs": [
            {
                "ownerSubjectId": subject_id,
                "capabilityId": capability_id,
                "routeCode": "native",
                "profileCode": "windows-native-check",
                "managedSourceRefs": [
                    {
                        "kind": "managed-source",
                        "path": f"artifact/verification-catalog/catalog/owners/{subject_id}/proofs/NativeInteropProofEntry.cs",
                        "label": fixture["sourceEntry"],
                    }
                ],
                "stubRefs": [
                    {
                        "kind": "stub-dir",
                        "path": f"artifact/verification-catalog/evidence/owners/{subject_id}/codegen-stubs/windows-native-check",
                        "label": "windows-native-check",
                    }
                ],
                "generatedFromEvidenceIds": [
                    f"evidence-claim/unit/{fixture['unitStableId']}/native-proof",
                ],
            }
        ],
    }


def assert_has_detailed_hint(testcase: unittest.TestCase, header: dict[str, Any]) -> None:
    hint = dict(header.get("hint") or {})
    for key in ("summary", "definition", "why", "source", "values", "rule", "emptyMeans"):
        testcase.assertIn(key, hint)
        testcase.assertTrue(hint[key])
    testcase.assertRegex(str(hint["summary"]), r"[\u4e00-\u9fff]")
    testcase.assertRegex(str(hint["definition"]), r"[\u4e00-\u9fff]")


def cleanup_repo_root(repo_root: Path) -> None:
    shutil.rmtree(repo_root, ignore_errors=True)


class TestingInventoryTestSupport(unittest.TestCase):
    pass
