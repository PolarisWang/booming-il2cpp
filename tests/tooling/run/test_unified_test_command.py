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
