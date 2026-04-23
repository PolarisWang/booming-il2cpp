from __future__ import annotations

from unittest.mock import patch

from tests.unit.performance.testing_inventory_test_support import *


class TestTestingInventorySource(TestingInventoryTestSupport):
    def test_collect_inventory_source_aggregates_registry_workspace_and_raw_benchmark_records(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_collect")
        fixture = inventory_fixture()
        repo_root = make_temp_repo_root("testing-inventory", "source-aggregate")
        write_inventory_fixture_repo(repo_root, fixture)
        write_json(
            repo_root / "docs" / "benchmark" / "overview.json",
            {
                "subjects": {
                    str(fixture["subjectId"]): {
                        "displayName": "Legacy Fixture Subject",
                    }
                }
            },
        )
        write_json(
            repo_root / "docs" / "benchmark" / "subjects" / f"{fixture['subjectId']}.json",
            {
                "subjectId": fixture["subjectId"],
                "benchmarkCasesByDevice": {
                    "legacy-device": {
                        str(fixture["benchmarkStableId"]): {
                            "stableId": fixture["benchmarkStableId"],
                            "deviceId": "legacy-device",
                            "deviceName": "Legacy Device",
                            "modeStatus": {
                                "managed": {
                                    "status": "missing",
                                }
                            },
                            "lastRecordedAt": "2026-01-01T00:00:00Z",
                            "gitCommit": "legacy000",
                        }
                    }
                },
            },
        )

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            self.assertEqual(1, payload["schemaVersion"])
            self.assertEqual("windows", payload["hostPlatform"])
            self.assertEqual(1, len(payload["declaredUnitTests"]))
            self.assertEqual(fixture["unitStableId"], payload["declaredUnitTests"][0]["stableId"])
            self.assertEqual(1, len(payload["declaredBenchmarks"]))
            self.assertEqual(fixture["benchmarkStableId"], payload["declaredBenchmarks"][0]["stableId"])
            self.assertEqual(2, len(payload["capabilityContracts"]))
            orphan = next(
                item for item in payload["capabilityContracts"] if item["capabilityItem"] == fixture["orphanCapabilityItem"]
            )
            self.assertEqual("Generic Sharing Boundary", orphan["capabilityItemLabel"])
            self.assertEqual(1, len(payload["workspaceCollections"]))
            self.assertEqual(1, len(payload["benchmarkEvidence"]))
            evidence = payload["benchmarkEvidence"][0]
            self.assertEqual(fixture["deviceId"], evidence["deviceId"])
            self.assertEqual(
                f"subjects/{fixture['subjectId']}/benchmark-records/records.jsonl",
                evidence["sourceSubjectPath"],
            )
            self.assertEqual("recorded", evidence["modeStatus"]["managed"]["status"])
            self.assertEqual("missing", evidence["modeStatus"]["native"]["status"])
            self.assertEqual("unsupported", evidence["modeStatus"]["interpreter"]["status"])
            self.assertEqual(1, len(payload["codegenStubs"]))
            stub = payload["codegenStubs"][0]
            self.assertEqual(subject_id := str(fixture["subjectId"]), stub["ownerSubjectId"])
            self.assertEqual("native", stub["routeCode"])
            self.assertEqual("windows-native-check", stub["profileCode"])
            self.assertEqual(
                f"subjects/{subject_id}/baselines/codegen/windows-native-check",
                stub["stubRefs"][0]["path"],
            )
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_resolves_capability_contracts_from_passed_repo_root(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_repo_root")
        fixture = inventory_fixture()
        repo_root = make_temp_repo_root("testing-inventory", "source-repo-root")
        write_inventory_fixture_repo(repo_root, fixture)

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            capability = next(
                item for item in payload["declaredUnitTests"] if item["stableId"] == fixture["unitStableId"]
            )
            self.assertEqual(str(fixture["subjectId"]), capability["ownerSubjectId"])
            self.assertEqual([1, 5], capability["supportStates"])
            self.assertEqual(["NativeGenerated", "ExternalRuntime"], capability["supportStateLabels"])
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_preserves_device_granularity_for_benchmark_evidence(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_device_rows")
        fixture = inventory_fixture()
        repo_root = make_temp_repo_root("testing-inventory", "source-device-granularity")
        write_inventory_fixture_repo(repo_root, fixture)
        append_benchmark_record(
            repo_root,
            fixture,
            mode="managed",
            mean_duration_ms=10.0,
            mean_ops_per_second=100.0,
            device_id="fixture-device-b",
            device_name="Fixture Device B",
            recorded_at="2026-04-22T07:40:01Z",
            git_commit="fixture124",
        )
        append_benchmark_record(
            repo_root,
            fixture,
            mode="native",
            mean_duration_ms=5.0,
            mean_ops_per_second=200.0,
            device_id="fixture-device-b",
            device_name="Fixture Device B",
            recorded_at="2026-04-22T07:40:02Z",
            git_commit="fixture124",
        )

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            rows = [item for item in payload["benchmarkEvidence"] if item["stableId"] == fixture["benchmarkStableId"]]
            self.assertEqual(2, len(rows))
            second_device_row = next(row for row in rows if row["deviceId"] == "fixture-device-b")
            self.assertEqual("recorded", second_device_row["modeStatus"]["native"]["status"])
            self.assertEqual(
                {str(fixture["deviceId"]), "fixture-device-b"},
                {row["deviceId"] for row in rows},
            )
        finally:
            cleanup_repo_root(repo_root)
