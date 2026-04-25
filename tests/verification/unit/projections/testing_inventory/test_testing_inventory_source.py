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
                f".artifact/verification/benchmark-records/{fixture['subjectId']}/records.jsonl",
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
                f"verification/evidence/owners/{subject_id}/codegen-stubs/windows-native-check",
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

    def test_collect_inventory_source_inherits_legacy_test_subject_proof_evidence(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_legacy_proof")
        fixture = inventory_fixture()
        repo_root = make_temp_repo_root("testing-inventory", "source-legacy-proof")
        write_inventory_fixture_repo(repo_root, fixture)
        write_legacy_subject_proof_run(
            repo_root,
            fixture,
            run_id="legacy-managed-proof",
            stage_kind="managed-proof",
            status="ok",
        )
        write_legacy_subject_proof_run(
            repo_root,
            fixture,
            run_id="legacy-native-proof",
            stage_kind="native-proof",
            status="fail",
        )

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            proof_rows = {
                (item["stableId"], item["stageKind"]): item
                for item in payload["proofEvidence"]
            }
            self.assertEqual("ok", proof_rows[(fixture["unitStableId"], "managed-proof")]["status"])
            self.assertEqual("fail", proof_rows[(fixture["unitStableId"], "native-proof")]["status"])
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_inherits_legacy_perf_summary(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_legacy_perf")
        fixture = inventory_fixture()
        repo_root = make_temp_repo_root("testing-inventory", "source-legacy-perf")
        write_inventory_fixture_repo(repo_root, fixture)
        records_path = repo_root / ".artifact" / "verification" / "benchmark-records" / fixture["subjectId"] / "records.jsonl"
        records_path.unlink()
        write_legacy_subject_perf_run(
            repo_root,
            fixture,
            run_id="legacy-native-perf",
            mode="native",
            mean_duration_ms=8.5,
            mean_ops_per_second=200.0,
        )

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            self.assertEqual(1, len(payload["benchmarkEvidence"]))
            evidence = payload["benchmarkEvidence"][0]
            self.assertEqual("recorded", evidence["modeStatus"]["native"]["status"])
            self.assertEqual(8.5, evidence["modeStatus"]["native"]["metrics"]["meanDurationMs"])
            self.assertEqual(
                "artifacts/subjects/FixtureSubject/runs/legacy-native-perf/matrices/windows-native-perf/validations/perf/summary.json",
                evidence["sourceSubjectPath"],
            )
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_maps_legacy_proof_from_generated_dispatch_subject(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_legacy_dispatch_subject")
        fixture = inventory_fixture()
        fixture["unitStableId"] = "FixtureSubject::Fixture.Tests::Fixture.Proofs.ArrayBoxingProofEntry::Run()"
        fixture["unitAlias"] = "array-boxing-proof"
        fixture["unitDeclaringType"] = "Fixture.Proofs.ArrayBoxingProofEntry"
        repo_root = make_temp_repo_root("testing-inventory", "source-legacy-dispatch-subject")
        write_inventory_fixture_repo(repo_root, fixture)
        write_legacy_subject_proof_run(
            repo_root,
            fixture,
            run_id="legacy-native-proof-dispatch-subject",
            stage_kind="native-proof",
            status="ok",
            include_entry_selection=False,
            preferred_dispatch_subject_id="ArrayBoxingReferenceArray.App/ArrayBoxingProofEntry::Run:System.Int32()",
        )

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            proof_rows = {
                (item["stableId"], item["stageKind"]): item
                for item in payload["proofEvidence"]
            }
            self.assertEqual("ok", proof_rows[(fixture["unitStableId"], "native-proof")]["status"])
            self.assertEqual(
                "ArrayBoxingReferenceArray.App/ArrayBoxingProofEntry::Run:System.Int32()",
                proof_rows[(fixture["unitStableId"], "native-proof")]["dispatchSubjectId"],
            )
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_inherits_declared_unit_report_evidence(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_declared_unit_report")
        fixture = inventory_fixture()
        repo_root = make_temp_repo_root("testing-inventory", "source-declared-unit-report")
        write_inventory_fixture_repo(repo_root, fixture)
        write_legacy_declared_unit_report(
            repo_root,
            fixture,
            run_id="legacy-declared-unit-managed",
            stage_kind="managed-proof",
            status="ok",
        )

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            proof_rows = {
                (item["stableId"], item["stageKind"]): item
                for item in payload["proofEvidence"]
            }
            self.assertEqual("ok", proof_rows[(fixture["unitStableId"], "managed-proof")]["status"])
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_routes_declared_unit_test_runs_to_actual_native_stage(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_declared_unit_native_stage")
        fixture = inventory_fixture()
        repo_root = make_temp_repo_root("testing-inventory", "source-declared-unit-native-stage")
        write_inventory_fixture_repo(repo_root, fixture)
        write_legacy_subject_proof_run(
            repo_root,
            fixture,
            run_id="declared-unit-native-proof",
            stage_kind="native-proof",
            status="ok",
        )

        run_root = (
            repo_root
            / "artifacts"
            / "subjects"
            / str(fixture["subjectId"])
            / "runs"
            / "declared-unit-native-proof"
        )
        summary_path = run_root / "run-report" / "summary.json"
        summary_payload = read_json(summary_path)
        summary_payload["command"] = "test declared-unit-test"
        summary_payload["subjectResults"][0]["entrySelection"] = {
            "family": "declared-unit-test",
            "stableId": str(fixture["unitStableId"]),
            "alias": str(fixture["unitAlias"]),
            "entryIndex": 2,
        }
        summary_payload["subjectResults"][0]["subjectSummaryPath"] = ""
        write_json(summary_path, summary_payload)

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            proof_rows = {
                (item["stableId"], item["stageKind"]): item
                for item in payload["proofEvidence"]
            }
            self.assertEqual("ok", proof_rows[(fixture["unitStableId"], "native-proof")]["status"])
            self.assertNotIn((fixture["unitStableId"], "managed-proof"), proof_rows)
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_maps_legacy_managed_output_source_entry_to_declared_unit(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_legacy_managed_entry")
        fixture = inventory_fixture()
        fixture["subjectId"] = "MixedExecutionFeaturePack"
        fixture["unitStableId"] = (
            "MixedExecutionFeaturePack::MixedExecutionFeaturePack::MixedExecutionFeaturePack.MixedExecutionProofEntry::Run()"
        )
        fixture["unitAlias"] = "mixed-execution-proof"
        fixture["unitAssemblyName"] = "MixedExecutionFeaturePack"
        fixture["unitDeclaringType"] = "MixedExecutionFeaturePack.MixedExecutionProofEntry"
        fixture["sourceEntry"] = "MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()"
        repo_root = make_temp_repo_root("testing-inventory", "source-legacy-managed-entry")
        write_inventory_fixture_repo(repo_root, fixture)
        write_legacy_managed_output_proof_run(
            repo_root,
            fixture,
            run_id="legacy-managed-output-proof",
            source_entry="MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()",
            status="ok",
        )

        try:
            registry_index = make_registry_index(source_module.registry_module, fixture)
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            proof_rows = {
                (item["stableId"], item["stageKind"]): item
                for item in payload["proofEvidence"]
            }
            self.assertEqual("ok", proof_rows[(fixture["unitStableId"], "managed-proof")]["status"])
        finally:
            cleanup_repo_root(repo_root)

    def test_collect_inventory_source_maps_legacy_managed_output_stdout_signals_to_mixed_execution_declared_units(self) -> None:
        source_module = load_inventory_source_module("chaos_testing_inventory_source_legacy_managed_stdout_signals")
        fixture = inventory_fixture()
        fixture["subjectId"] = "MixedExecutionFeaturePack"
        fixture["displayName"] = "MixedExecutionFeaturePack"
        fixture["unitStableId"] = (
            "MixedExecutionFeaturePack::MixedExecutionFeaturePack::MixedExecutionFeaturePack.MixedExecutionProofEntry::Run()"
        )
        fixture["unitAlias"] = "mixed-execution-proof"
        fixture["unitAssemblyName"] = "MixedExecutionFeaturePack"
        fixture["unitDeclaringType"] = "MixedExecutionFeaturePack.MixedExecutionProofEntry"
        fixture["unitEntryIndex"] = 4
        fixture["sourceEntry"] = "MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()"
        fixture["capabilityFamily"] = 14
        fixture["capabilityItem"] = 61
        fixture["archetype"] = 11
        fixture["archetypeLabel"] = "Mixed Bridge Solution"
        fixture["requires"] = 40
        fixture["requirementLabels"] = ["HotUpdate", "ExceptionFlow"]
        fixture["priority"] = 1

        declared_unit_fixtures = [
            {
                **fixture,
                "unitStableId": (
                    "MixedExecutionFeaturePack::MixedExecutionFeaturePack::"
                    "MixedExecutionFeaturePack.InterpreterArithmeticProofEntry::Run()"
                ),
                "unitAlias": "interpreter-arithmetic-proof",
                "unitDeclaringType": "MixedExecutionFeaturePack.InterpreterArithmeticProofEntry",
                "unitEntryIndex": 0,
                "capabilityItem": 63,
                "requires": 0,
                "requirementLabels": [],
            },
            {
                **fixture,
                "unitStableId": (
                    "MixedExecutionFeaturePack::MixedExecutionFeaturePack::"
                    "MixedExecutionFeaturePack.InterpreterLoweringProofEntry::Run()"
                ),
                "unitAlias": "interpreter-lowering-proof",
                "unitDeclaringType": "MixedExecutionFeaturePack.InterpreterLoweringProofEntry",
                "unitEntryIndex": 1,
                "capabilityItem": 62,
                "requires": 0,
                "requirementLabels": [],
            },
            {
                **fixture,
                "unitStableId": (
                    "MixedExecutionFeaturePack::MixedExecutionFeaturePack::"
                    "MixedExecutionFeaturePack.MixedDelegateFlowProofEntry::Run()"
                ),
                "unitAlias": "mixed-delegate-flow-proof",
                "unitDeclaringType": "MixedExecutionFeaturePack.MixedDelegateFlowProofEntry",
                "unitEntryIndex": 2,
                "capabilityItem": 66,
                "requires": 4,
                "requirementLabels": ["Delegate"],
            },
            {
                **fixture,
                "unitStableId": (
                    "MixedExecutionFeaturePack::MixedExecutionFeaturePack::"
                    "MixedExecutionFeaturePack.MixedExceptionFlowProofEntry::Run()"
                ),
                "unitAlias": "mixed-exception-flow-proof",
                "unitDeclaringType": "MixedExecutionFeaturePack.MixedExceptionFlowProofEntry",
                "unitEntryIndex": 3,
                "capabilityItem": 65,
                "requires": 8,
                "requirementLabels": ["ExceptionFlow"],
            },
            fixture,
            {
                **fixture,
                "unitStableId": (
                    "MixedExecutionFeaturePack::MixedExecutionFeaturePack::"
                    "MixedExecutionFeaturePack.MixedGenericFlowProofEntry::Run()"
                ),
                "unitAlias": "mixed-generic-flow-proof",
                "unitDeclaringType": "MixedExecutionFeaturePack.MixedGenericFlowProofEntry",
                "unitEntryIndex": 5,
                "capabilityItem": 64,
                "requires": 1,
                "requirementLabels": ["GenericSharing"],
            },
        ]

        repo_root = make_temp_repo_root("testing-inventory", "source-legacy-managed-stdout-signals")
        write_inventory_fixture_repo_with_declared_units(repo_root, fixture, declared_unit_fixtures)
        write_legacy_managed_output_proof_run(
            repo_root,
            fixture,
            run_id="legacy-managed-output-proof",
            source_entry="MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()",
            status="ok",
            stdout_lines=[
                "mixed-aot-to-interpreter-before-load=22",
                "mixed-interpreter-local-call=42",
                "mixed-interpreter-local-call-ops=ldarg,ldarg,call,ret",
                "mixed-interpreter-string-bridge=5",
                "mixed-interpreter-real-catch=42",
                "mixed-interpreter-real-rethrow-caught=ok",
                "mixed-interpreter-real-leave-finally=42",
                "mixed-interpreter-real-leave-finally-opcodes=leave,endfinally",
                "mixed-interpreter-throw-caught=ok",
                "mixed-interpreter-rethrow-caught=ok",
                "mixed-interpreter-leave-finally=ok",
                "mixed-aot-to-interpreter=42",
                "mixed-interpreter-to-aot=30",
                "mixed-interpreter-to-engine=7",
                "mixed-aot-to-interpreter-after-unload=22",
            ],
        )

        try:
            registry_index = make_registry_index(
                source_module.registry_module,
                fixture,
                declared_unit_test_fixtures=declared_unit_fixtures,
            )
            with patch.object(source_module.registry_module, "scan_registry", return_value=registry_index):
                payload = source_module.collect_inventory_source(repo_root, host_platform="windows")

            managed_rows = {
                item["stableId"]: item
                for item in payload["proofEvidence"]
                if item["stageKind"] == "managed-proof"
            }

            self.assertEqual("ok", managed_rows[declared_unit_fixtures[0]["unitStableId"]]["status"])
            self.assertEqual("ok", managed_rows[declared_unit_fixtures[2]["unitStableId"]]["status"])
            self.assertEqual("ok", managed_rows[declared_unit_fixtures[3]["unitStableId"]]["status"])
            self.assertEqual("ok", managed_rows[declared_unit_fixtures[4]["unitStableId"]]["status"])
            self.assertEqual("ok", managed_rows[declared_unit_fixtures[5]["unitStableId"]]["status"])
            self.assertNotIn(declared_unit_fixtures[1]["unitStableId"], managed_rows)
        finally:
            cleanup_repo_root(repo_root)
