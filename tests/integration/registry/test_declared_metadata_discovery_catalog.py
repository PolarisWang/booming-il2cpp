from tests.integration.registry.declared_metadata_discovery_test_support import *


class TestDeclaredMetadataDiscoveryCatalog(DeclaredMetadataDiscoveryTestSupport):
    def test_compiled_catalog_distinguishes_runtime_only_framework_only_and_declared_entries(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_state_matrix")

        runtime_only = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="RuntimeOnlySubject",
            declaration_mode="auto",
            assembly_paths=[self.runtime_only_dll],
        )
        framework_only = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="FrameworkOnlySubject",
            declaration_mode="auto",
            assembly_paths=[self.framework_no_entries_dll],
        )
        declared = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="DeclaredEntriesSubject",
            declaration_mode="auto",
            assembly_paths=[self.declared_entries_dll],
        )

        self.assertEqual(1, runtime_only["schemaVersion"])
        self.assertEqual(1, framework_only["schemaVersion"])
        self.assertEqual(1, declared["schemaVersion"])
        self.assertEqual(
            {
                "schemaVersion": 1,
                "subjectId": "RuntimeOnlySubject",
                "frameworkReferenced": False,
                "subjectKind": "runtime-only",
                "warningCodes": [],
                "declaredUnitTests": [],
                "declaredBenchmarks": [],
            },
            runtime_only,
        )
        self.assertEqual(
            {
                "schemaVersion": 1,
                "subjectId": "FrameworkOnlySubject",
                "frameworkReferenced": True,
                "subjectKind": "runtime-only",
                "warningCodes": ["framework-without-declarations"],
                "declaredUnitTests": [],
                "declaredBenchmarks": [],
            },
            framework_only,
        )
        self.assertEqual("DeclaredEntriesSubject", declared["subjectId"])
        self.assertTrue(declared["frameworkReferenced"])
        self.assertEqual("declared-test", declared["subjectKind"])
        self.assertEqual([], declared["warningCodes"])
        self.assertEqual(1, len(declared["declaredUnitTests"]))
        self.assertEqual(1, len(declared["declaredBenchmarks"]))

    def test_compiled_catalog_emits_stable_ids_and_compact_numeric_metadata(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_payload")

        catalog = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="DeclaredEntriesSubject",
            declaration_mode="require",
            assembly_paths=[self.declared_entries_dll],
        )

        unit_entry = catalog["declaredUnitTests"][0]
        benchmark_entry = catalog["declaredBenchmarks"][0]

        self.assertEqual(
            "DeclaredEntriesSubject::DeclaredEntries::DeclaredEntries.DeclaredProofs::VerifyOutput()",
            unit_entry["stableId"],
        )
        self.assertEqual(0, unit_entry["entryIndex"])
        self.assertEqual("runtime-contract", unit_entry["alias"])
        self.assertEqual("DeclaredEntries", unit_entry["assemblyName"])
        self.assertEqual("DeclaredEntries.DeclaredProofs", unit_entry["declaringType"])
        self.assertEqual("VerifyOutput", unit_entry["methodName"])
        self.assertEqual("VerifyOutput()", unit_entry["methodSignature"])
        self.assertEqual(1, unit_entry["category"])
        self.assertEqual(6, unit_entry["capabilityFamily"])
        self.assertEqual(23, unit_entry["capabilityItem"])
        self.assertEqual(2, unit_entry["requires"])
        self.assertEqual(4, unit_entry["archetype"])
        self.assertEqual(8, unit_entry["hotUpdateCapability"])
        self.assertEqual(1, unit_entry["evidence"])
        self.assertEqual(3, unit_entry["priority"])

        self.assertEqual(
            "DeclaredEntriesSubject::DeclaredEntries::DeclaredEntries.DeclaredProofs::BenchmarkDispatch()",
            benchmark_entry["stableId"],
        )
        self.assertEqual(0, benchmark_entry["entryIndex"])
        self.assertEqual("dispatch-bench", benchmark_entry["alias"])
        self.assertEqual("DeclaredEntries", benchmark_entry["assemblyName"])
        self.assertEqual("DeclaredEntries.DeclaredProofs", benchmark_entry["declaringType"])
        self.assertEqual("BenchmarkDispatch", benchmark_entry["methodName"])
        self.assertEqual("BenchmarkDispatch()", benchmark_entry["methodSignature"])
        self.assertEqual(1, benchmark_entry["category"])
        self.assertEqual(2, benchmark_entry["capabilityFamily"])
        self.assertEqual(10, benchmark_entry["capabilityItem"])
        self.assertEqual(3, benchmark_entry["metrics"])
        self.assertEqual(1, benchmark_entry["requires"])
        self.assertEqual(8, benchmark_entry["archetype"])
        self.assertEqual(3, benchmark_entry["hotUpdateCapability"])
        self.assertEqual(2, benchmark_entry["warmupCount"])
        self.assertEqual(12, benchmark_entry["iterationCount"])
        self.assertEqual(100, benchmark_entry["invocationCount"])

    def test_compiled_catalog_scans_metadata_without_loading_framework_binary(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_metadata_only")

        isolated_dll_path = self._copy_fixture_output(self.declared_entries_dll)
        framework_dll_path = isolated_dll_path.parent / "Chaos.TestFramework.Sdk.dll"
        self.assertTrue(framework_dll_path.is_file(), msg=f"missing copied framework dll: {framework_dll_path}")
        framework_dll_path.unlink()

        catalog = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="DeclaredEntriesSubject",
            declaration_mode="require",
            assembly_paths=[isolated_dll_path],
        )

        self.assertEqual(1, len(catalog["declaredUnitTests"]))
        self.assertEqual(1, len(catalog["declaredBenchmarks"]))

    def test_compiled_catalog_skips_discovery_when_declaration_mode_is_none(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_mode_none")

        catalog = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="CompatibilityShellSubject",
            declaration_mode="none",
            assembly_paths=[self.declared_entries_dll],
        )

        self.assertEqual(1, catalog["schemaVersion"])
        self.assertEqual(
            {
                "schemaVersion": 1,
                "subjectId": "CompatibilityShellSubject",
                "frameworkReferenced": False,
                "subjectKind": "runtime-only",
                "warningCodes": [],
                "declaredUnitTests": [],
                "declaredBenchmarks": [],
            },
            catalog,
        )
