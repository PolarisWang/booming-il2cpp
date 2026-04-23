from __future__ import annotations

import json
import shutil
import unittest
import uuid
from pathlib import Path

from tests.support import load_module, make_temp_repo_root, materialize_subject_manifest


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"

def load_subjects_module(module_name: str):
    return load_module(SUBJECTS_MODULE_PATH, module_name)


def expected_matrix_for_goal(manifest: dict, goal_id: str) -> dict:
    default_matrix_id = str(manifest["defaultMatrix"])
    for matrix in list(manifest.get("environmentMatrices") or []):
        if str(matrix.get("matrixId") or "") == default_matrix_id and goal_id in list(matrix.get("supportedGoals") or []):
            return dict(matrix)
    for matrix in list(manifest.get("environmentMatrices") or []):
        if goal_id in list(matrix.get("supportedGoals") or []):
            return dict(matrix)
    raise AssertionError(f"no matrix supports goal: {goal_id}")


def find_matrix_for_goal(subjects_module, manifest: dict, goal_id: str, *, required_stage_kind: str | None = None) -> dict:
    for matrix in list(manifest.get("environmentMatrices") or []):
        if goal_id not in list(matrix.get("supportedGoals") or []):
            continue
        if required_stage_kind is not None:
            pipeline = subjects_module.find_pipeline(manifest, str(matrix.get("pipelineId") or ""))
            if required_stage_kind not in subjects_module.pipeline_stage_kinds(pipeline):
                continue
        return dict(matrix)
    raise AssertionError(f"no matrix matched goal={goal_id}, required_stage_kind={required_stage_kind}")


def find_goal_matrix_mismatch(manifest: dict) -> tuple[str, str]:
    matrices = [dict(matrix) for matrix in list(manifest.get("environmentMatrices") or [])]
    for candidate_matrix in matrices:
        candidate_goals = set(candidate_matrix.get("supportedGoals") or [])
        for other_matrix in matrices:
            if other_matrix["matrixId"] == candidate_matrix["matrixId"]:
                continue
            for goal_id in list(other_matrix.get("supportedGoals") or []):
                if goal_id not in candidate_goals:
                    return str(candidate_matrix["matrixId"]), str(goal_id)
    raise AssertionError("no mismatched goal/matrix pair found")


def create_subject_repo(prefix: str, manifest: dict) -> tuple[Path, dict]:
    repo_root = make_temp_repo_root("subject-planner", prefix)
    materialize_subject_manifest(repo_root, manifest)
    return repo_root, manifest


def build_native_proof_subject_manifest(subject_id: str = "FixtureNativeProofSubject") -> dict:
    return {
        "subjectId": subject_id,
        "displayName": subject_id,
        "category": "canonical",
        "defaultGoal": "correctness.dev",
        "defaultMatrix": "windows-dev-output",
        "defaultValidationProfile": "proof-dev",
        "source": {
            "type": "dotnet-project",
            "path": f"subjects/{subject_id}/source/{subject_id}.csproj",
            "entry": f"{subject_id}/Program::Main()",
        },
        "validationProfiles": {
            "proof-dev": ["proof"],
            "trace-platform": ["proof"],
        },
        "validation": {
            "proof": {
                "kind": "proof",
                "defaultVariant": "CHECK",
            }
        },
        "executionPipelines": [
            {
                "pipelineId": "proof-runtime-output",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis", "dependsOn": ["host-input-build"]},
                    {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated", "dependsOn": ["analysis-frontend"]},
                    {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["generated-native-proof"]},
                    {"stageId": "runtime-observe", "kind": "runtime-observe", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                    {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-observe"]},
                ],
            },
            {
                "pipelineId": "proof-runtime-trace",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis", "dependsOn": ["host-input-build"]},
                    {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated", "dependsOn": ["analysis-frontend"]},
                    {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["generated-native-proof"]},
                    {"stageId": "runtime-observe", "kind": "runtime-observe", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                    {"stageId": "runtime-trace-compare", "kind": "runtime-trace-compare", "scope": "matrix", "bucket": "runtime", "dependsOn": ["runtime-observe"]},
                    {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-trace-compare"]},
                ],
            },
        ],
        "environmentMatrices": [
            {
                "matrixId": "windows-dev-output",
                "pipelineId": "proof-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "validationIntent": {
                    "validationMode": "output",
                    "adaptationLevel": "observable-output",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "runtime",
                },
            },
            {
                "matrixId": "windows-reference-trace",
                "pipelineId": "proof-runtime-trace",
                "supportedGoals": ["correctness.platform"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "validationIntent": {
                    "validationMode": "trace",
                    "adaptationLevel": "traceable",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "runtime",
                },
            },
        ],
    }


def build_managed_perf_subject_manifest(subject_id: str = "FixtureManagedPerfSubject") -> dict:
    return {
        "subjectId": subject_id,
        "displayName": subject_id,
        "category": "canonical",
        "defaultGoal": "perf.release",
        "defaultMatrix": "windows-managed-perf",
        "defaultValidationProfile": "perf-profile",
        "source": {
            "type": "dotnet-project",
            "path": f"subjects/{subject_id}/source/{subject_id}.csproj",
            "entry": f"{subject_id}/Program::RunWorkload()",
        },
        "workloadEntry": f"{subject_id}/Program::RunWorkload()",
        "validationProfiles": {
            "perf-profile": ["perf"],
        },
        "validation": {
            "perf": {
                "kind": "perf",
                "driver": "csharp-perf-harness",
                "defaultVariant": "PROFILE",
            }
        },
        "executionPipelines": [
            {
                "pipelineId": "managed-benchmark",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                    {"stageId": "runtime-perf-collect", "kind": "runtime-perf-collect", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                    {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-perf-collect"]},
                ],
            }
        ],
        "environmentMatrices": [
            {
                "matrixId": "windows-managed-perf",
                "pipelineId": "managed-benchmark",
                "supportedGoals": ["perf.release"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                },
                "validationIntent": {
                    "validationMode": "perf",
                    "adaptationLevel": "managed-runtime",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "runtime", "report"],
                    "evidenceTerminalBucket": "report",
                },
            }
        ],
    }


def build_managed_output_subject_manifest(subject_id: str = "FixtureManagedOutputSubject") -> dict:
    return {
        "subjectId": subject_id,
        "displayName": subject_id,
        "category": "canonical",
        "defaultGoal": "correctness.dev",
        "defaultMatrix": "windows-managed-output",
        "defaultValidationProfile": "proof-dev",
        "source": {
            "type": "dotnet-project",
            "path": f"subjects/{subject_id}/source/{subject_id}.csproj",
            "entry": f"{subject_id}/Program::Main()",
        },
        "validationProfiles": {
            "proof-dev": ["proof"],
        },
        "validation": {
            "proof": {
                "kind": "proof",
                "defaultVariant": "CHECK",
            }
        },
        "executionPipelines": [
            {
                "pipelineId": "managed-runtime-output",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                    {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                    {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-managed-output"]},
                ],
            }
        ],
        "environmentMatrices": [
            {
                "matrixId": "windows-managed-output",
                "pipelineId": "managed-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                },
                "validationIntent": {
                    "validationMode": "output",
                    "adaptationLevel": "observable-output",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "runtime", "report"],
                    "evidenceTerminalBucket": "runtime",
                },
            }
        ],
    }


def build_solution_style_managed_perf_subject_manifest(
    subject_id: str = "FixtureSolutionStylePerfSubject",
    *,
    source_entry: str | None = None,
    workload_entry: str | None = None,
    matrix_source_entry: str | None = None,
    matrix_workload_entry: str | None = None,
) -> dict:
    manifest = build_managed_perf_subject_manifest(subject_id)
    manifest["source"]["path"] = f"subjects/{subject_id}/source/{subject_id}.sln"
    manifest["source"]["primaryProjectPath"] = f"subjects/{subject_id}/source/{subject_id}.csproj"
    if source_entry is not None:
        manifest["source"]["entry"] = source_entry
    if workload_entry is not None:
        manifest["workloadEntry"] = workload_entry
    matrix = manifest["environmentMatrices"][0]
    if matrix_source_entry is not None:
        matrix["source"] = {"entry": matrix_source_entry}
    if matrix_workload_entry is not None:
        matrix["workloadEntry"] = matrix_workload_entry
    return manifest


def build_solution_style_managed_output_subject_manifest(
    subject_id: str = "FixtureSolutionStyleOutputSubject",
    *,
    source_entry: str | None = None,
    matrix_source_entry: str | None = None,
) -> dict:
    manifest = build_managed_output_subject_manifest(subject_id)
    manifest["source"]["path"] = f"subjects/{subject_id}/source/{subject_id}.sln"
    manifest["source"]["primaryProjectPath"] = f"subjects/{subject_id}/source/{subject_id}.csproj"
    if source_entry is not None:
        manifest["source"]["entry"] = source_entry
    matrix = manifest["environmentMatrices"][0]
    if matrix_source_entry is not None:
        matrix["source"] = {"entry": matrix_source_entry}
    return manifest


class SubjectPlannerTestSupport(unittest.TestCase):
    pass
