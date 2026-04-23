from __future__ import annotations

import json
import shutil
import unittest
from pathlib import Path
from typing import Any

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module


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


def make_registry_index(registry_module, fixture: dict[str, Any]):
    subject_id = str(fixture["subjectId"])
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
        declared_unit_tests=[
            {
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
                "entryIndex": 2,
                "assemblyName": "Fixture.Tests",
                "declaringType": "Fixture.Proofs.NativeInteropProof",
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
                "archetype": 4,
                "archetypeLabel": "Reference Assembly Solution",
                "hotUpdateCapability": 0,
                "hotUpdateCapabilityLabels": [],
                "requires": 16,
                "requirementLabels": ["Native Interop"],
                "evidence": 4,
                "evidenceLabels": ["Native Symbol"],
                "priority": 2,
            }
        ],
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
                "assemblyName": "Fixture.Benchmarks",
                "declaringType": "Fixture.Benchmarks.NativeInteropBenchmark",
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
        pipelines=[],
        errors=[],
        warnings=[],
    )


def write_inventory_fixture_repo(repo_root: Path, fixture: dict[str, Any]) -> None:
    subject_id = str(fixture["subjectId"])
    features_path = repo_root / "subjects" / subject_id / "subject.features.json"
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
        repo_root / "subjects" / subject_id / "subject.manifest.json",
        {
            "subjectId": subject_id,
            "displayName": str(fixture["displayName"]),
            "source": {
                "type": "dotnet-project",
                "path": f"subjects/{subject_id}/source",
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
    (repo_root / "subjects" / subject_id / "source").mkdir(parents=True, exist_ok=True)
    (repo_root / "subjects" / subject_id / "baselines" / "codegen" / "windows-native-check").mkdir(
        parents=True,
        exist_ok=True,
    )

    collection_path = (
        repo_root / "solutions" / "subjects" / subject_id / "managed-tests" / "Generated" / "declared-tests.collection.json"
    )
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
                    "assemblyName": "Fixture.Tests",
                    "declaringType": "Fixture.Proofs.NativeInteropProof",
                    "methodName": "Run",
                    "methodSignature": "Run()",
                }
            ],
            "declaredBenchmarks": [
                {
                    "stableId": fixture["benchmarkStableId"],
                    "entryIndex": 7,
                    "alias": fixture["benchmarkAlias"],
                    "assemblyName": "Fixture.Benchmarks",
                    "declaringType": "Fixture.Benchmarks.NativeInteropBenchmark",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                }
            ],
        },
    )
    write_json(
        repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json",
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
    records_path = repo_root / "subjects" / subject_id / "benchmark-records" / "records.jsonl"
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
                "manifestPath": f"solutions/subjects/{subject_id}/workspace.manifest.json",
                "collectionPath": (
                    f"solutions/subjects/{subject_id}/managed-tests/Generated/declared-tests.collection.json"
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
                "sourceSubjectPath": f"subjects/{subject_id}/benchmark-records/records.jsonl",
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
                        "path": f"subjects/{subject_id}/source/Proofs/NativeInteropProofEntry.cs",
                        "label": fixture["sourceEntry"],
                    }
                ],
                "stubRefs": [
                    {
                        "kind": "stub-dir",
                        "path": f"subjects/{subject_id}/baselines/codegen/windows-native-check",
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
