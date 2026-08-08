from __future__ import annotations

import json
from unittest.mock import patch

from tests.tooling.run.subject_command_test_support import *
from tests.unit.performance.testing_inventory_test_support import (
    cleanup_repo_root,
    inventory_fixture,
    write_inventory_fixture_repo,
)


class TestSubjectCommandInventory(SubjectCommandTestSupport):
    def test_test_inventory_command_writes_authority_json_csv_and_html_outputs(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_inventory_outputs")
        repo_root = make_temp_repo_root("subject-command", "inventory-command")
        fixture = inventory_fixture()
        write_inventory_fixture_repo(repo_root, fixture)

        inventory_payload = {
            "outputRoot": "verification/projections/testing-inventory",
            "artifacts": [
                "verification/projections/testing-inventory/inventory-source.json",
                "verification/projections/testing-inventory/capability-inventory.json",
                "verification/projections/testing-inventory/unit-test-inventory.json",
                "verification/projections/testing-inventory/benchmark-inventory.json",
                "verification/projections/testing-inventory/capability-inventory.csv",
                "verification/projections/testing-inventory/unit-test-inventory.csv",
                "verification/projections/testing-inventory/benchmark-inventory.csv",
                "verification/projections/testing-inventory/inventory.html",
            ],
            "tables": {
                "capability": {"rowCount": 2},
                "unitTest": {"rowCount": 6},
                "benchmark": {"rowCount": 1},
            },
        }

        try:
            with patch.object(
                test_module,
                "inventory_generator_module",
                create=True,
            ) as generator_mock:
                generator_mock.refresh_inventory_outputs.return_value = inventory_payload
                result = test_module.handle(
                    {
                        "id": "test-inventory",
                        "handler": "test.dispatch",
                    },
                    repo_root,
                    "windows",
                    "test inventory",
                    {},
                    {},
                )

            self.assertEqual("ok", result.status)
            self.assertEqual("testing-inventory", result.target)
            self.assertEqual("verification/projections/testing-inventory", result.payload["outputRoot"])
            self.assertEqual(8, len(result.payload["artifacts"]))
            self.assertIn("verification/projections/testing-inventory/inventory.html", result.payload["artifacts"])
            generator_mock.refresh_inventory_outputs.assert_called_once_with(
                repo_root,
                host_platform="windows",
                output_root=None,
            )
        finally:
            cleanup_repo_root(repo_root)

    def test_test_inventory_command_materializes_formal_source_without_legacy_benchmark_inputs(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_inventory_formal_source")
        repo_root = make_temp_repo_root("subject-command", "inventory-command-formal-source")
        fixture = inventory_fixture()
        write_inventory_fixture_repo(repo_root, fixture)

        legacy_docs_root = repo_root / "docs" / "benchmark"
        legacy_subjects_root = legacy_docs_root / "subjects"
        legacy_subjects_root.mkdir(parents=True, exist_ok=True)
        (legacy_docs_root / "overview.json").write_text(
            json.dumps(
                {
                    "subjects": {
                        str(fixture["subjectId"]): {
                            "displayName": "Legacy Subject",
                        }
                    }
                },
                ensure_ascii=False,
                indent=2,
            ),
            encoding="utf-8",
        )
        (legacy_subjects_root / f"{fixture['subjectId']}.json").write_text(
            json.dumps(
                {
                    "subjectId": fixture["subjectId"],
                    "benchmarkCasesByDevice": {
                        "legacy-device": {
                            str(fixture["benchmarkStableId"]): {
                                "stableId": fixture["benchmarkStableId"],
                                "deviceId": "legacy-device",
                                "modeStatus": {
                                    "managed": {
                                        "status": "missing",
                                    }
                                },
                            }
                        }
                    },
                },
                ensure_ascii=False,
                indent=2,
            ),
            encoding="utf-8",
        )

        try:
            result = test_module.handle(
                {
                    "id": "test-inventory",
                    "handler": "test.dispatch",
                },
                repo_root,
                "windows",
                "test inventory",
                {},
                {},
            )

            self.assertEqual("ok", result.status)
            self.assertIn("artifact/verification-catalog/archive/master/result-master.json", result.payload["artifacts"])
            self.assertTrue(
                (repo_root / "artifact" / "verification-catalog" / "archive" / "master" / "result-master.json").is_file()
            )
            self.assertTrue(result.payload["validated"])
            self.assertEqual(
                "artifact/verification-catalog/archive/latest/result-snapshot.json",
                result.payload["verificationData"]["latestResultPath"],
            )
            self.assertEqual(
                "artifact/verification-catalog/archive/master/result-master.json",
                result.payload["verificationData"]["masterResultPath"],
            )
            self.assertEqual(
                "artifact/verification-catalog/archive/reports/completed/testing-inventory/summary.md",
                result.payload["verificationData"]["reportSummaryPath"],
            )
            self.assertEqual(0, result.payload["sourceSummary"]["codegenStubCount"])
            self.assertEqual([], result.payload["verificationData"]["codegenStubPaths"])

            benchmark_inventory_text = (
                repo_root / "artifact" / "verification-catalog" / "projections" / "testing-inventory" / "benchmark-inventory.json"
            ).read_text(encoding="utf-8")
            self.assertIn(
                "artifact/verification-catalog/archive/master/evidence-claims-master.json",
                benchmark_inventory_text,
            )
            self.assertNotIn("docs/benchmark/overview.json", benchmark_inventory_text)
            self.assertNotIn("docs/benchmark/subjects", benchmark_inventory_text)

            source_payload = json.loads(
                (repo_root / "artifact" / "verification-catalog" / "projections" / "testing-inventory" / "inventory-source.json").read_text(encoding="utf-8")
            )
            evidence_paths = {
                str(item.get("sourceSubjectPath") or "")
                for item in list(source_payload.get("benchmarkEvidence") or [])
            }
            self.assertEqual(
                {f"artifact/verification/benchmark-records/{fixture['subjectId']}/records.jsonl"},
                evidence_paths,
            )
            verification_index = (repo_root / "artifact" / "verification-catalog" / "INDEX.md").read_text(encoding="utf-8")
            verification_manifest = json.loads(
                (repo_root / "artifact" / "verification-catalog" / "verification.manifest.json").read_text(encoding="utf-8")
            )
            self.assertIn("python build/toolchains/run/run.py test inventory --json", verification_index)
            self.assertIn("python build/toolchains/run/run.py generate project all --json", verification_index)
            self.assertNotIn("python build/toolchains/run/run.py verify verification-v1 --json", verification_index)
            self.assertNotIn("python build/toolchains/run/run.py project all-workspaces --json", verification_index)
            self.assertEqual(
                "python build/toolchains/run/run.py test inventory --json",
                verification_manifest["commands"]["verify"],
            )
            self.assertEqual(
                "python build/toolchains/run/run.py generate project all --json",
                verification_manifest["commands"]["workspace"],
            )
            self.assertNotIn("inventory", verification_manifest["commands"])
        finally:
            cleanup_repo_root(repo_root)

    def test_test_inventory_command_supports_custom_output_path(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_inventory_custom_output")
        repo_root = make_temp_repo_root("subject-command", "inventory-command-custom-output")
        fixture = inventory_fixture()
        write_inventory_fixture_repo(repo_root, fixture)

        try:
            with patch.object(
                test_module,
                "inventory_generator_module",
                create=True,
            ) as generator_mock:
                generator_mock.refresh_inventory_outputs.return_value = {
                    "outputRoot": "custom/testing-inventory",
                    "artifacts": ["custom/testing-inventory/inventory.html"],
                    "tables": {},
                }
                result = test_module.handle(
                    {
                        "id": "test-inventory",
                        "handler": "test.dispatch",
                    },
                    repo_root,
                    "windows",
                    "test inventory --output custom/testing-inventory",
                    {},
                    {"output": "custom/testing-inventory"},
                )

            self.assertEqual("ok", result.status)
            self.assertEqual("custom/testing-inventory", result.payload["outputRoot"])
            generator_mock.refresh_inventory_outputs.assert_called_once_with(
                repo_root,
                host_platform="windows",
                output_root="custom/testing-inventory",
            )
        finally:
            cleanup_repo_root(repo_root)

    def test_test_inventory_command_reports_generator_failures(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_inventory_failure")
        repo_root = make_temp_repo_root("subject-command", "inventory-command-failure")
        fixture = inventory_fixture()
        write_inventory_fixture_repo(repo_root, fixture)

        try:
            with patch.object(
                test_module,
                "inventory_generator_module",
                create=True,
            ) as generator_mock:
                generator_mock.refresh_inventory_outputs.side_effect = RuntimeError("inventory generation failed")
                result = test_module.handle(
                    {
                        "id": "test-inventory",
                        "handler": "test.dispatch",
                    },
                    repo_root,
                    "windows",
                    "test inventory",
                    {},
                    {},
                )

            self.assertEqual("error", result.status)
            self.assertEqual(["inventory generation failed"], result.errors)
            self.assertEqual(1, result.payload["exitCode"])
        finally:
            cleanup_repo_root(repo_root)
