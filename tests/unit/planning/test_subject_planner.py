from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path

from tests.support import make_temp_repo_root, materialize_subject_manifest


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


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


class SubjectPlannerTests(unittest.TestCase):
    def test_planner_uses_subject_defaults_and_subject_artifact_layout(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_default")
        subjects_module = load_subjects_module("chaos_subject_planner_default_subjects")
        repo_root, manifest = create_subject_repo("default", build_native_proof_subject_manifest())
        subject_id = str(manifest["subjectId"])
        run_id = "20260407-fixture-default-001"

        try:
            plan = planner_module.build_plan(repo_root, subject_id, run_id=run_id)
            expected_matrix = subjects_module.find_matrix(manifest, str(manifest["defaultMatrix"]))
            expected_pipeline = subjects_module.find_pipeline(manifest, str(expected_matrix["pipelineId"]))

            self.assertEqual("v1", plan["planVersion"])
            self.assertEqual(subject_id, plan["request"]["subjectId"])
            self.assertIsNone(plan["request"]["goalId"])
            self.assertIsNone(plan["request"]["matrixId"])
            self.assertEqual(run_id, plan["request"]["runId"])
            self.assertEqual(subject_id, plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["defaultGoal"]), plan["selection"]["goalId"])
            self.assertEqual(str(manifest["defaultMatrix"]), plan["selection"]["matrixId"])
            self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
            self.assertEqual(
                str(dict(expected_matrix["artifactPlan"])["evidenceTerminalBucket"]),
                plan["selection"]["artifactPlan"]["evidenceTerminalBucket"],
            )
            self.assertEqual(f"artifacts/subjects/{subject_id}", plan["artifactsRoot"]["subjectRoot"])
            self.assertEqual(f"artifacts/subjects/{subject_id}/runs/{run_id}", plan["artifactsRoot"]["runRoot"])
            self.assertEqual(
                [str(stage["stageId"]) for stage in list(expected_pipeline.get("stages") or [])],
                [stage["stageId"] for stage in plan["stagePlan"]],
            )
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(expected_matrix["matrixId"]), run_id=run_id, bucket="source", scope="shared", kind="source-resolve")["manifestPath"],
                plan["stagePlan"][0]["paths"]["manifestPath"],
            )
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(expected_matrix["matrixId"]), run_id=run_id, bucket="report", scope="matrix", kind="report-assemble")["manifestPath"],
                plan["stagePlan"][-1]["paths"]["manifestPath"],
            )
            self.assertTrue(all(stage["executionMode"] in {"executed", "reused", "invalidated"} for stage in plan["stagePlan"]))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_requested_trace_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_trace")
        subjects_module = load_subjects_module("chaos_subject_planner_trace_subjects")
        repo_root, manifest = create_subject_repo("trace", build_native_proof_subject_manifest("FixtureTraceSubject"))
        subject_id = str(manifest["subjectId"])
        trace_matrix = find_matrix_for_goal(subjects_module, manifest, "correctness.platform", required_stage_kind="runtime-trace-compare")
        run_id = "20260407-fixture-trace-001"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="correctness.platform",
                matrix_id=str(trace_matrix["matrixId"]),
                run_id=run_id,
            )
            expected_pipeline = subjects_module.find_pipeline(manifest, str(trace_matrix["pipelineId"]))

            self.assertEqual(str(trace_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(trace_matrix["pipelineId"]), plan["selection"]["pipelineId"])
            self.assertEqual(
                [str(stage["stageId"]) for stage in list(expected_pipeline.get("stages") or [])],
                [stage["stageId"] for stage in plan["stagePlan"]],
            )
            trace_stage = next(stage for stage in plan["stagePlan"] if stage["kind"] == "runtime-trace-compare")
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(trace_matrix["matrixId"]), run_id=run_id, bucket="runtime", scope="matrix", kind="runtime-trace-compare")["manifestPath"],
                trace_stage["paths"]["manifestPath"],
            )
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(trace_matrix["matrixId"]), run_id=run_id, bucket="runtime", scope="matrix", kind="runtime-trace-compare")["reportPaths"],
                trace_stage["paths"]["reportPaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_rejects_goal_matrix_mismatch(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mismatch")
        repo_root, manifest = create_subject_repo("mismatch", build_native_proof_subject_manifest("FixtureMismatchSubject"))
        subject_id = str(manifest["subjectId"])
        matrix_id, unsupported_goal_id = find_goal_matrix_mismatch(manifest)

        try:
            with self.assertRaisesRegex(ValueError, "does not support goal"):
                planner_module.build_plan(repo_root, subject_id, goal_id=unsupported_goal_id, matrix_id=matrix_id)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_perf_defaults_without_benchmark_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_perf_default")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"perf-default-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureBenchDefaults" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        manifest = {
            "subjectId": "FixtureBenchDefaults",
            "displayName": "FixtureBenchDefaults",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureBenchDefaults/source/FixtureBenchDefaults.csproj",
                "entry": "FixtureBenchDefaults/Program::Main()",
            },
            "workloadEntry": "FixtureBenchDefaults/Program::RunWorkload()",
            "validationProfiles": {
                "perf-profile": ["perf"],
            },
            "validation": {
                "perf": {
                    "kind": "perf",
                    "driver": "native-runtime-perf",
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
                        "runtimeProfile": "managed-perf-release",
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

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            run_id = "20260407-fixture-perf-default-001"
            plan = planner_module.build_plan(repo_root, "FixtureBenchDefaults", run_id=run_id)

            self.assertEqual("FixtureBenchDefaults", plan["selection"]["subjectId"])
            self.assertEqual("perf.release", plan["selection"]["goalId"])
            self.assertEqual("windows-managed-perf", plan["selection"]["matrixId"])
            self.assertEqual("managed-benchmark", plan["selection"]["pipelineId"])
            self.assertEqual("report", plan["selection"]["artifactPlan"]["evidenceTerminalBucket"])
            self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
            self.assertEqual("PROFILE", plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_release_perf_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_perf_release")
        subjects_module = load_subjects_module("chaos_subject_planner_perf_release_subjects")
        repo_root, manifest = create_subject_repo("perf-release", build_managed_perf_subject_manifest())
        subject_id = str(manifest["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        run_id = "20260407-fixture-perf-release-001"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                run_id=run_id,
            )

            self.assertEqual(str(selected_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(selected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(selected_matrix["matrixId"]), run_id=run_id, bucket="report", scope="matrix", kind="report-assemble")["manifestPath"],
                plan["stagePlan"][-1]["paths"]["manifestPath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_workload_entry_for_solution_style_benchmark_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_workload_entry")
        subject_id = "FixtureSolutionStyleBenchmarkSubject"
        workload_entry = f"{subject_id}/Benchmarks::RunDefault()"
        repo_root, manifest = create_subject_repo(
            "solution-style-benchmark",
            build_solution_style_managed_perf_subject_manifest(
                subject_id,
                source_entry=workload_entry,
                workload_entry=workload_entry,
            ),
        )

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="20260413-solution-style-benchmark-workload-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(workload_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(workload_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_explicit_workload_entry_as_effective_source_entry_for_solution_style_perf_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_explicit_solution_workload_entry_only")
        subject_id = "FixtureExplicitPerfSubject"
        repo_root, manifest = create_subject_repo(
            "explicit-solution-workload",
            build_solution_style_managed_perf_subject_manifest(subject_id),
        )
        explicit_entry = f"{subject_id}/Benchmarks::RunHotPath()"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="20260413-explicit-solution-workload-entry-001",
                workload_entry=explicit_entry,
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(explicit_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(explicit_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_stage_fingerprint_changes_when_declared_entry_selection_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_declared_entry_fingerprint")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"declared-entry-selection-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureDeclaredEntrySelection" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureDeclaredEntrySelection",
            "displayName": "FixtureDeclaredEntrySelection",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureDeclaredEntrySelection/source/FixtureDeclaredEntrySelection.csproj",
                "entry": "FixtureDeclaredEntrySelection/Program::Main()",
            },
            "workloadEntry": "FixtureDeclaredEntrySelection/Program::RunWorkload()",
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
                        "runtimeProfile": "managed-perf-release",
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

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            first_plan = planner_module.build_plan(
                repo_root,
                "FixtureDeclaredEntrySelection",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-declared-entry-selection",
                entry_selection={
                    "family": "declared-benchmark",
                    "stableId": "bench/a",
                    "alias": "bench-a",
                    "entryIndex": 1,
                },
            )
            second_plan = planner_module.build_plan(
                repo_root,
                "FixtureDeclaredEntrySelection",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-declared-entry-selection",
                entry_selection={
                    "family": "declared-benchmark",
                    "stableId": "bench/b",
                    "alias": "bench-b",
                    "entryIndex": 2,
                },
            )

            first_runtime_stage = next(stage for stage in first_plan["stagePlan"] if stage["kind"] == "runtime-perf-collect")
            second_runtime_stage = next(stage for stage in second_plan["stagePlan"] if stage["kind"] == "runtime-perf-collect")

            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": "bench/a",
                    "alias": "bench-a",
                    "entryIndex": 1,
                },
                first_plan["selection"]["entrySelection"],
            )
            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": "bench/b",
                    "alias": "bench-b",
                    "entryIndex": 2,
                },
                second_plan["selection"]["entrySelection"],
            )
            self.assertNotEqual(first_runtime_stage["fingerprint"], second_runtime_stage["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_matrix_workload_entry_for_solution_style_perf_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_matrix_workload_entry")
        subject_id = "FixtureMatrixPerfSubject"
        matrix_entry = f"{subject_id}/Benchmarks::RunMatrixDefault()"
        repo_root, manifest = create_subject_repo(
            "matrix-solution-workload",
            build_solution_style_managed_perf_subject_manifest(
                subject_id,
                matrix_source_entry=matrix_entry,
                matrix_workload_entry=matrix_entry,
            ),
        )

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="20260413-matrix-solution-workload-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(matrix_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(matrix_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_native_perf_override_for_solution_style_perf_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_native_perf_entry")
        subject_id = "FixtureNativePerfSubject"
        native_entry = f"{subject_id}/Benchmarks::RunNativePerf()"
        manifest = build_solution_style_managed_perf_subject_manifest(subject_id)
        manifest["executionPipelines"].append(
            {
                "pipelineId": "native-benchmark",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                    {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["host-input-build"]},
                    {"stageId": "native-runtime-perf", "kind": "native-runtime-perf", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                    {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["native-runtime-perf"]},
                ],
            }
        )
        manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-native-perf",
                "pipelineId": "native-benchmark",
                "supportedGoals": ["perf.release"],
                "source": {"entry": native_entry},
                "workloadEntry": native_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "native-perf-profile",
                },
                "validationIntent": {
                    "validationMode": "perf",
                    "adaptationLevel": "native-runtime",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "report",
                },
            }
        )
        repo_root, manifest = create_subject_repo("solution-style-native-perf", manifest)

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-native-perf",
                run_id="20260413-solution-style-native-perf-workload-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(native_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(native_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_explicit_proof_entry_for_solution_style_output_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_explicit_proof_entry")
        subject_id = "FixtureExplicitProofSubject"
        repo_root, manifest = create_subject_repo(
            "explicit-solution-proof",
            build_solution_style_managed_output_subject_manifest(subject_id),
        )
        explicit_entry = f"{subject_id}/Proofs::RunExplicit()"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="correctness.dev",
                matrix_id="windows-managed-output",
                run_id="20260413-explicit-solution-proof-entry-001",
                source_entry=explicit_entry,
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(explicit_entry, plan["selection"]["source"]["entry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_matrix_proof_entry_for_solution_style_output_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_matrix_proof_entry")
        subject_id = "FixtureMatrixProofSubject"
        matrix_entry = f"{subject_id}/Proofs::RunMatrixDefault()"
        repo_root, manifest = create_subject_repo(
            "matrix-solution-proof",
            build_solution_style_managed_output_subject_manifest(
                subject_id,
                matrix_source_entry=matrix_entry,
            ),
        )

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="correctness.dev",
                matrix_id="windows-managed-output",
                run_id="20260413-matrix-solution-proof-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(matrix_entry, plan["selection"]["source"]["entry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_runtime_stage_fingerprint_changes_when_subject_entry_selection_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_subject_entry_fingerprint")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"subject-entry-selection-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureSubjectEntrySelection" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureSubjectEntrySelection",
            "displayName": "FixtureSubjectEntrySelection",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "managed-output",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSubjectEntrySelection/source/FixtureSubjectEntrySelection.csproj",
                "entry": "FixtureSubjectEntrySelection/Program::Main()",
            },
            "validationProfiles": {
                "managed-output": ["proof"],
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
                        {
                            "stageId": "source-resolve",
                            "kind": "source-resolve",
                            "scope": "shared",
                            "bucket": "source",
                            "dependsOn": [],
                        },
                        {
                            "stageId": "host-input-build",
                            "kind": "host-input-build",
                            "scope": "shared",
                            "bucket": "host-input",
                            "dependsOn": ["source-resolve"],
                        },
                        {
                            "stageId": "runtime-managed-output",
                            "kind": "runtime-managed-output",
                            "scope": "matrix",
                            "bucket": "runtime",
                            "dependsOn": ["host-input-build"],
                        },
                        {
                            "stageId": "report-assemble",
                            "kind": "report-assemble",
                            "scope": "matrix",
                            "bucket": "report",
                            "dependsOn": ["runtime-managed-output"],
                        },
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-output",
                    "pipelineId": "managed-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "source": {
                        "entrySelection": {
                            "entryKind": 1,
                            "entrySlice": 1,
                        },
                    },
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-output",
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
                },
            ],
        }

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            first_plan = planner_module.build_plan(
                repo_root,
                "FixtureSubjectEntrySelection",
                matrix_id="windows-managed-output",
                run_id="fixture-subject-entry-selection-001",
            )
            manifest["environmentMatrices"][0]["source"]["entrySelection"]["entrySlice"] = 2
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            second_plan = planner_module.build_plan(
                repo_root,
                "FixtureSubjectEntrySelection",
                matrix_id="windows-managed-output",
                run_id="fixture-subject-entry-selection-001",
            )

            first_runtime_stage = next(stage for stage in first_plan["stagePlan"] if stage["kind"] == "runtime-managed-output")
            second_runtime_stage = next(stage for stage in second_plan["stagePlan"] if stage["kind"] == "runtime-managed-output")

            self.assertEqual(
                {
                    "entryKind": 1,
                    "entrySlice": 1,
                },
                first_plan["selection"]["source"]["entrySelection"],
            )
            self.assertEqual(
                {
                    "entryKind": 1,
                    "entrySlice": 2,
                },
                second_plan["selection"]["source"]["entrySelection"],
            )
            self.assertNotEqual(first_runtime_stage["fingerprint"], second_runtime_stage["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_first_matrix_supporting_requested_goal_when_matrix_is_omitted(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_goal_only")
        repo_root, manifest = create_subject_repo("goal-only", build_managed_perf_subject_manifest("FixtureGoalOnlySubject"))
        subject_id = str(manifest["subjectId"])
        expected_matrix = expected_matrix_for_goal(manifest, "perf.release")

        try:
            plan = planner_module.build_plan(repo_root, subject_id, goal_id="perf.release", run_id="20260407-fixture-auto-001")

            self.assertEqual("perf.release", plan["selection"]["goalId"])
            self.assertEqual(str(expected_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_managed_output_defaults_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_managed_output")
        repo_root, manifest = create_subject_repo("managed-output", build_managed_output_subject_manifest())
        subject_id = str(manifest["subjectId"])
        expected_matrix = expected_matrix_for_goal(manifest, "correctness.dev")
        run_id = "20260407-fixture-managed-output-001"

        try:
            plan = planner_module.build_plan(repo_root, subject_id, run_id=run_id)

            self.assertEqual(subject_id, plan["selection"]["subjectId"])
            self.assertEqual("correctness.dev", plan["selection"]["goalId"])
            self.assertEqual(str(expected_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_default_validation_profile_and_variant(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_default")
        subjects_module = load_subjects_module("chaos_subject_planner_variant_default_subjects")
        repo_root, manifest = create_subject_repo("variant-default", build_managed_output_subject_manifest("FixtureVariantDefaultSubject"))
        subject_id = str(manifest["subjectId"])
        validation_selection = subjects_module.resolve_validation_selection(manifest)

        try:
            plan = planner_module.build_plan(repo_root, subject_id, run_id="20260407-fixture-variant-default-001")

            self.assertIsNone(plan["request"].get("validationProfileId"))
            self.assertIsNone(plan["request"].get("validationKind"))
            self.assertIsNone(plan["request"].get("variant"))
            self.assertEqual(str(validation_selection["validationProfileId"]), plan["selection"]["validationProfileId"])
            self.assertEqual(list(validation_selection["validationKinds"]), plan["selection"]["validationKinds"])
            self.assertEqual(str(validation_selection["variant"]), plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_allows_explicit_validation_profile_and_variant_override(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_override")
        subjects_module = load_subjects_module("chaos_subject_planner_variant_override_subjects")
        repo_root, manifest = create_subject_repo("variant-override", build_managed_perf_subject_manifest("FixtureVariantOverrideSubject"))
        subject_id = str(manifest["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        profile_id = "perf-profile"
        validation_kind = str(list(dict(manifest["validationProfiles"])[profile_id])[0])

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                validation_profile_id=profile_id,
                validation_kind=validation_kind,
                variant="SHIP",
                run_id="20260407-fixture-variant-override-001",
            )

            self.assertEqual(profile_id, plan["request"]["validationProfileId"])
            self.assertEqual(validation_kind, plan["request"]["validationKind"])
            self.assertEqual("SHIP", plan["request"]["variant"])
            self.assertEqual(profile_id, plan["selection"]["validationProfileId"])
            self.assertEqual([validation_kind], plan["selection"]["validationKinds"])
            self.assertEqual(validation_kind, plan["selection"]["validationKind"])
            self.assertEqual("SHIP", plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_stage_fingerprint_changes_when_variant_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_fingerprint")
        subjects_module = load_subjects_module("chaos_subject_planner_variant_fingerprint_subjects")
        repo_root, manifest = create_subject_repo("variant-fingerprint", build_managed_perf_subject_manifest("FixtureVariantFingerprintSubject"))
        subject_id = str(manifest["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        profile_id = "perf-profile"

        try:
            default_plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                validation_profile_id=profile_id,
                run_id="20260407-fixture-variant-fingerprint-001",
            )
            ship_plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                validation_profile_id=profile_id,
                variant="SHIP",
                run_id="20260407-fixture-variant-fingerprint-001",
            )

            self.assertNotEqual(default_plan["selection"]["variant"], ship_plan["selection"]["variant"])
            self.assertNotEqual(default_plan["stagePlan"][0]["fingerprint"], ship_plan["stagePlan"][0]["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_stage_fingerprint_changes_when_workload_entry_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_workload_fingerprint")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"workload-fingerprint-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureBench" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureBench",
            "displayName": "FixtureBench",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureBench/source/FixtureBench.csproj",
                "entry": "FixtureBench/Program::Main()",
            },
            "workloadEntry": "FixtureBench/Program::RunWorkload()",
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
                        "runtimeProfile": "managed-perf-release",
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

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            plan = planner_module.build_plan(
                repo_root,
                "FixtureBench",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-workload-fingerprint",
            )

            manifest["workloadEntry"] = "FixtureBench/Program::RunHotPath()"
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            updated_plan = planner_module.build_plan(
                repo_root,
                "FixtureBench",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-workload-fingerprint",
            )

            self.assertNotEqual(plan["selection"]["workloadEntry"], updated_plan["selection"]["workloadEntry"])
            self.assertNotEqual(plan["stagePlan"][0]["fingerprint"], updated_plan["stagePlan"][0]["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_goal_matching_perf_profile_when_goal_changes_perf_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_goal_matching_perf_profile")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"goal-matching-perf-profile-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixturePerfProfile" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixturePerfProfile",
            "displayName": "FixturePerfProfile",
            "category": "canonical",
            "defaultGoal": "perf.dev",
            "defaultMatrix": "windows-managed-perf-dev",
            "defaultValidationProfile": "perf-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixturePerfProfile/source/FixturePerfProfile.csproj",
                "entry": "FixturePerfProfile/Program::Main()",
            },
            "workloadEntry": "FixturePerfProfile/Program::RunWorkload()",
            "validationProfiles": {
                "perf-dev": ["perf"],
                "perf-release": ["perf"],
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
                    "matrixId": "windows-managed-perf-dev",
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-perf-dev",
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
                },
                {
                    "matrixId": "windows-managed-perf-release",
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-perf-release",
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
                },
            ],
        }

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            plan = planner_module.build_plan(
                repo_root,
                "FixturePerfProfile",
                goal_id="perf.release",
                run_id="20260413-fixture-goal-matching-perf-profile-001",
            )

            self.assertEqual("perf.release", plan["selection"]["goalId"])
            self.assertEqual("windows-managed-perf-release", plan["selection"]["matrixId"])
            self.assertEqual("perf-release", plan["selection"]["validationProfileId"])
            self.assertEqual("perf", plan["selection"]["validationKind"])
            self.assertEqual("PROFILE", plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_expands_shared_orchestration_profiles_before_building_plan(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_shared_profiles")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"shared-profiles-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureSharedPlanner" / "subject.manifest.json"
        pipeline_profile_path = repo_root / "testing" / "orchestration" / "pipelines" / "proof-core.json"
        matrix_profile_path = repo_root / "testing" / "orchestration" / "matrices" / "proof-core.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        pipeline_profile_path.parent.mkdir(parents=True, exist_ok=True)
        matrix_profile_path.parent.mkdir(parents=True, exist_ok=True)

        pipeline_profile = {
            "executionPipelines": [
                {
                    "pipelineId": "proof-runtime-output",
                    "displayName": "Profile Runtime Output",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis", "dependsOn": ["host-input-build"]},
                        {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated", "dependsOn": ["analysis-frontend"]},
                        {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["generated-native-proof"]},
                        {"stageId": "runtime-observe", "kind": "runtime-observe", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-observe"]},
                    ],
                }
            ]
        }
        matrix_profile = {
            "environmentMatrices": [
                {
                    "matrixId": "windows-dev-output",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "runtimeProfile": "native-proof-output",
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
                }
            ]
        }
        manifest = {
            "subjectId": "FixtureSharedPlanner",
            "displayName": "FixtureSharedPlanner",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-dev-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSharedPlanner/source/FixtureSharedPlanner.csproj",
                "entry": "FixtureSharedPlanner/Program::Main()",
            },
            "orchestration": {
                "matrixProfile": "proof-core",
                "pipelineProfile": "proof-core",
                "budgetProfile": "budget-standard",
                "baselineProfile": "baseline-standard",
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
        }

        try:
            pipeline_profile_path.write_text(json.dumps(pipeline_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            matrix_profile_path.write_text(json.dumps(matrix_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            plan = planner_module.build_plan(
                repo_root,
                "FixtureSharedPlanner",
                run_id="fixture-shared-planner",
            )

            self.assertEqual("FixtureSharedPlanner", plan["selection"]["subjectId"])
            self.assertEqual("windows-dev-output", plan["selection"]["matrixId"])
            self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
            self.assertEqual(
                {
                    "matrixProfile": "proof-core",
                    "pipelineProfile": "proof-core",
                    "budgetProfile": "budget-standard",
                    "baselineProfile": "baseline-standard",
                },
                plan["selection"]["orchestration"],
            )
            self.assertEqual(
                [
                    "source-resolve",
                    "host-input-build",
                    "analysis-frontend",
                    "generated-native-proof",
                    "build-target",
                    "runtime-observe",
                    "report-assemble",
                ],
                [stage["stageId"] for stage in plan["stagePlan"]],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
