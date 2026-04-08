from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

from tests.support import select_public_suite_spec, select_subject_record


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "manifest.py"
RUN_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run.py"
RESULT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "result.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


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


class UnifiedTestCommandTests(unittest.TestCase):
    def test_public_suite_resolution_maps_to_legacy_commands(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command")
        smoke_spec = select_public_suite_spec(
            "chaos_unified_test_command_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["build", "all"],
        )
        legacy_commands = dict(smoke_spec.get("legacy_commands") or {})

        self.assertEqual(
            str(legacy_commands["build"]),
            test_module.resolve_legacy_test_command_id(
                str(smoke_spec["family"]),
                str(smoke_spec["suite"]),
                stage="build",
                host_platform="macos",
            ),
        )
        self.assertEqual(
            str(legacy_commands["all"]),
            test_module.resolve_legacy_test_command_id(
                str(smoke_spec["family"]),
                str(smoke_spec["suite"]),
                stage="all",
                host_platform="macos",
            ),
        )
        self.assertEqual(
            "test-workflow-runtime-baseline-macos",
            test_module.resolve_legacy_test_command_id("workflow", "runtime-baseline-macos", stage="all", host_platform="macos"),
        )

    def test_public_test_list_surfaces_unified_suite_ids(self) -> None:
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_for_public_test")
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_for_public_list")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        smoke_spec = select_public_suite_spec(
            "chaos_unified_test_list_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["all"],
        )

        items = test_module.list_public_test_suites(manifest, "macos")
        item_ids = {item["id"] for item in items}

        self.assertIn(str(smoke_spec["id"]), item_ids)
        self.assertIn("workflow/runtime-baseline-macos", item_ids)
        self.assertNotIn(str(dict(smoke_spec.get("legacy_commands") or {}).get("all") or ""), item_ids)
        self.assertNotIn("verify-roadmap-0-macos", item_ids)

    def test_test_all_includes_all_current_subjects(self) -> None:
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_for_test_all_subjects")
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_for_test_all_subjects")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_record = select_subject_record(
            "chaos_unified_test_all_subjects",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
        )
        expected_subject_id = f"subject/{subject_record['subjectId']}"
        index = test_module._scan_registry(REPO_ROOT, "windows")
        expected_subject_ids = {item["id"] for item in index.subjects}
        subject_calls: list[str] = []

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = test_module.session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return test_module.session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        def fake_subject_object(
            *,
            index: object,
            selected_object: dict,
            normalized_options: dict,
            repo_root: Path,
            host_platform: str,
            command_text: str,
        ):
            del index
            del normalized_options
            del repo_root
            del host_platform
            subject_calls.append(str(selected_object["id"]))
            subject_id = str(selected_object["subjectId"])
            return test_module.CommandResult.success(
                command=command_text,
                host_platform="windows",
                target=str(selected_object["id"]),
                payload={
                    "artifacts": [],
                    "exitCode": 0,
                    "subjectResults": [
                        {
                            "subjectId": subject_id,
                            "status": "ok",
                            "subjectSummaryPath": f"artifacts/subjects/{subject_id}/runs/fake-run/subject-report/summary.json",
                        }
                    ],
                },
                text=f"{subject_id} ok\n",
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            with patch.object(test_module, "_run_subject_object", side_effect=fake_subject_object):
                result = test_module.handle(
                    {"id": "test-all", "handler": "test.dispatch"},
                    REPO_ROOT,
                    "windows",
                    "test all",
                    manifest,
                    {},
                )

        self.assertEqual("ok", result.status)
        item_ids = {item["id"] for item in result.payload["items"]}
        self.assertTrue(expected_subject_ids.issubset(item_ids))
        self.assertIn(expected_subject_id, item_ids)
        self.assertEqual(expected_subject_ids, set(subject_calls))
        self.assertEqual(expected_subject_ids, {f"subject/{item['subjectId']}" for item in result.payload["subjectResults"]})

    def test_test_all_payload_includes_batch_counts_and_failure_digest(self) -> None:
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_for_test_all_payload_summary")
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_for_test_all_payload_summary")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_record = select_subject_record(
            "chaos_unified_test_all_payload_summary",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
        )
        subject_id = str(subject_record["subjectId"])
        fake_index = test_module.registry_module.RegistryIndex(
            host_platform="windows",
            suites=[],
            subjects=[
                {
                    "id": f"subject/{subject_id}",
                    "type": "subject",
                    "displayName": subject_id,
                    "subjectId": subject_id,
                    "defaultGoalId": "debug.native",
                    "defaultMatrixId": "windows-x64-default",
                    "goalIds": ["debug.native"],
                    "matrixIds": ["windows-x64-default"],
                    "supportedHosts": ["windows"],
                    "level": "subject",
                    "primaryModuleId": None,
                    "moduleIds": [],
                    "subsystemIds": [],
                    "docRefs": [],
                }
            ],
            module_verifications=[],
            system_scenarios=[],
            pipelines=[],
            errors=[],
            warnings=[],
        )
        public_suites = [
            {
                "id": "smoke/HelloWorld",
                "type": "suite",
                "family": "smoke",
                "suite": "HelloWorld",
                "level": "code",
                "primaryModuleId": "managed-smoke",
                "moduleIds": ["managed-smoke"],
                "subsystemIds": ["managed-execution"],
                "docRefs": [],
                "supportedHosts": ["windows"],
            },
            {
                "id": "contract/trace-schema",
                "type": "suite",
                "family": "contract",
                "suite": "trace-schema",
                "level": "code",
                "primaryModuleId": "trace-export",
                "moduleIds": ["trace-export"],
                "subsystemIds": ["hosting"],
                "docRefs": [],
                "supportedHosts": ["windows"],
            },
        ]

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del host_platform
            del manifest_payload
            suite_id = f"{family}/{suite}"
            status = "ok" if family == "smoke" else "fail"
            return test_module.session_module.SessionResult(
                request=test_module.session_module.TestRequest(
                    family=family,
                    suite=suite,
                    stage=stage,
                    command_text=command_text,
                ),
                host_platform="windows",
                status=status,
                suite_results=[
                    {
                        "suiteId": suite_id,
                        "status": status,
                        "stageResults": {"all": status},
                        "caseCounts": {"total": 1, "ok": 1 if status == "ok" else 0, "fail": 0 if status == "ok" else 1, "skip": 0, "aborted": 0},
                        "trafficLightCounts": {
                            "green": {"total": 1, "ok": 1 if status == "ok" else 0, "fail": 0, "skip": 0, "aborted": 0},
                            "yellow": {"total": 0, "ok": 0, "fail": 0, "skip": 0, "aborted": 0},
                            "red": {"total": 0 if status == "ok" else 1, "ok": 0, "fail": 0 if status == "ok" else 1, "skip": 0, "aborted": 0},
                        },
                        "artifacts": [],
                    }
                ],
                text=f"{suite_id} {status}\n",
                artifacts=[],
                exit_code=0 if status == "ok" else 1,
                errors=[] if status == "ok" else [f"{suite_id} failed"],
            )

        with patch.object(test_module, "_scan_registry", return_value=fake_index):
            with patch.object(test_module, "list_public_test_suites", return_value=public_suites):
                with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
                    with patch.object(test_module, "_run_subject_object") as run_subject_object:
                        result = test_module.handle(
                            {"id": "test-all", "handler": "test.dispatch"},
                            REPO_ROOT,
                            "windows",
                            "test all",
                            manifest,
                            {},
                        )

        self.assertEqual("error", result.status)
        self.assertEqual({"totalUnits": 3, "suiteCount": 2, "subjectCount": 1}, result.payload["plannedCounts"])
        self.assertEqual(1, result.payload["countsByFamily"]["smoke"]["ok"])
        self.assertEqual(1, result.payload["countsByFamily"]["contract"]["fail"])
        self.assertEqual(1, result.payload["countsByType"]["subject"]["aborted"])
        self.assertEqual("contract/trace-schema", result.payload["failureItems"][0]["id"])
        self.assertEqual("run test contract trace-schema", result.payload["failureItems"][0]["rerunCommand"])
        run_subject_object.assert_not_called()

    def test_render_summary_surfaces_overall_breakdown_and_failures(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_for_summary_render")

        summary_text = test_module._render_summary(
            {
                "runId": "run-1",
                "command": "test all",
                "hostPlatform": "windows",
                "finalStatus": "fail",
                "plannedCounts": {"totalUnits": 3, "suiteCount": 2, "subjectCount": 1},
                "countsByType": {
                    "suite": {"total": 2, "ok": 1, "fail": 1, "skip": 0, "aborted": 0},
                    "subject": {"total": 1, "ok": 0, "fail": 0, "skip": 0, "aborted": 1},
                },
                "countsByFamily": {
                    "smoke": {"total": 1, "ok": 1, "fail": 0, "skip": 0, "aborted": 0},
                    "contract": {"total": 1, "ok": 0, "fail": 1, "skip": 0, "aborted": 0},
                },
                "subjectResults": [
                    {
                        "subjectId": "HelloWorldObject",
                        "requestedGoalId": "debug.native",
                        "status": "aborted",
                        "matrixStatusCounts": {"total": 1, "ok": 0, "fail": 0, "skip": 0, "aborted": 1},
                        "subjectSummaryPath": "artifacts/subjects/HelloWorldObject/runs/run-1/subject-report/summary.json",
                    }
                ],
                "failureItems": [
                    {
                        "id": "contract/trace-schema",
                        "type": "suite",
                        "status": "fail",
                        "rerunCommand": "run test contract trace-schema",
                        "reportPath": "artifacts/logs/tests/run-1/suites/contract/trace-schema/report.json",
                    }
                ],
            }
        )

        self.assertIn("Overall:", summary_text)
        self.assertIn("Total units: 3", summary_text)
        self.assertIn("Suite count: 2", summary_text)
        self.assertIn("Subject count: 1", summary_text)
        self.assertIn("Suite Breakdown:", summary_text)
        self.assertIn("contract: total 1 | ok 0 | fail 1 | skip 0 | aborted 0", summary_text)
        self.assertIn("Subject Breakdown:", summary_text)
        self.assertIn("HelloWorldObject", summary_text)
        self.assertIn("Failure Digest:", summary_text)
        self.assertIn("rerun: run test contract trace-schema", summary_text)

    def test_legacy_test_commands_gain_migration_guidance(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "chaos_run_main_for_legacy_migration")
        result_module = load_module(RESULT_MODULE_PATH, "chaos_run_result_for_legacy_migration")
        smoke_spec = select_public_suite_spec(
            "chaos_unified_test_legacy_migration_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["build"],
        )
        suite_name = str(smoke_spec["suite"])
        legacy_build_command = str(dict(smoke_spec.get("legacy_commands") or {}).get("build") or "")
        replacement_syntax = f"test {smoke_spec['family']} {suite_name} --stage build"
        result = result_module.CommandResult.success(
            command=f"build {smoke_spec['family']} {suite_name}",
            host_platform="macos",
            target=suite_name,
            payload={"artifacts": [f"artifacts/smoke/bin/{suite_name}/Release/net8.0/{suite_name}.dll"]},
            text="build completed\n",
        )

        wrapped = run_module.add_legacy_test_migration_guidance(
            {
                "id": legacy_build_command,
                "public": False,
                "replacement_syntax": replacement_syntax,
            },
            result,
        )

        self.assertIn("Deprecated test command", wrapped.text)
        self.assertEqual(replacement_syntax, wrapped.payload["migration"]["replacementSyntax"])

    def test_removed_verify_entrypoint_returns_migration_guidance(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "chaos_run_main_for_removed_verify_migration")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_for_removed_verify_migration")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        result = run_module.execute_command(
            None,
            "verify roadmap-0",
            None,
            "windows",
            manifest,
            REPO_ROOT,
            {"host": "windows"},
        )

        self.assertEqual("error", result.status)
        self.assertIn("Removed command", result.text or "")
        self.assertIn("run test workflow runtime-baseline-windows", result.text or "")
        self.assertEqual("verify roadmap-0", result.payload["migration"]["removedCommand"])
        self.assertEqual("test workflow runtime-baseline-windows", result.payload["migration"]["replacementSyntax"])


if __name__ == "__main__":
    unittest.main()
