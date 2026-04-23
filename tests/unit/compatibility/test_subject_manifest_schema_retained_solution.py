from tests.unit.compatibility.subject_manifest_schema_test_support import *


class TestSubjectManifestSchemaRetainedSolution(SubjectManifestSchemaTestSupport):
    def test_retained_solution_subjects_declare_solution_mode_and_require_explicit_test_contract(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_retained_solution_mode")

        records = [
            record
            for record in subjects_module.load_subject_records(REPO_ROOT)
            if str(record["capabilities"]["sourceModel"]) == "dotnet-solution"
        ]

        self.assertEqual(RETAINED_SOLUTION_SUBJECT_IDS, {str(record["subjectId"]) for record in records})

        for record in records:
            manifest = record["manifest"]
            capabilities = record["capabilities"]
            primary_project_path = REPO_ROOT / str(manifest["source"]["primaryProjectPath"])

            self.assertEqual("require", capabilities["testDeclarationMode"])
            self.assertEqual("dotnet-solution", manifest["sourceModel"])
            self.assertTrue(str(manifest["source"]["path"]).endswith(".sln"))
            self.assertTrue(primary_project_path.is_file(), msg=f"missing primary project: {primary_project_path}")

    def test_retained_solution_subject_manifests_project_expected_thin_fields(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_retained_thin_fields")

        expected_contracts = {
            "SolutionCorePack": {
                "sourceModel": "dotnet-solution",
                "dependencyModel": "mixed",
                "executablePlan": "generated-native",
                "engineeringProfile": "native-executable",
                "availability": {"macos-arm64": "ready", "windows-x64": "ready"},
            },
            "HotUpdateHostPack": {
                "sourceModel": "dotnet-solution",
                "dependencyModel": "project-reference",
                "executablePlan": "managed-host",
                "engineeringProfile": "hot-update-host",
                "availability": {
                    "linux-x64": "ready",
                    "macos-arm64": "ready",
                    "windows-x64": "ready",
                },
            },
            "MixedExecutionFeaturePack": {
                "sourceModel": "dotnet-solution",
                "dependencyModel": "project-reference",
                "executablePlan": "managed-host",
                "engineeringProfile": "managed-output",
                "availability": {"windows-x64": "ready"},
            },
        }

        for subject_id, expected in expected_contracts.items():
            manifest = subjects_module.load_subject_manifest(REPO_ROOT, subject_id)
            capabilities = subjects_module.manifest_capabilities(manifest)

            self.assertEqual(expected["sourceModel"], manifest["sourceModel"])
            self.assertEqual(expected["dependencyModel"], manifest["dependencyModel"])
            self.assertEqual(expected["executablePlan"], manifest["executablePlan"])
            self.assertEqual(expected["engineeringProfile"], manifest["engineeringProfile"])
            self.assertEqual(expected["availability"], manifest["availability"])
            self.assertEqual(expected["sourceModel"], capabilities["sourceModel"])
            self.assertEqual(expected["dependencyModel"], capabilities["dependencyModel"])
            self.assertEqual(expected["executablePlan"], capabilities["executablePlan"])
            self.assertEqual(expected["engineeringProfile"], capabilities["engineeringProfile"])

    def test_retained_subject_sources_declare_chaos_attributes_in_csharp_api(self) -> None:
        expected_sources = {
            "subjects/SolutionCorePack/source/Proofs/CoreRuntimeFeatures/AsyncAndThreading/TaskAndValueTaskFlowProof.cs": [
                "[ChaosUnitTest(",
                'Alias = "task-valuetask-flow-proof"',
            ],
            "subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/PrimitivesAndOps/ArithmeticBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "arithmetic-bench"',
            ],
            "subjects/HotUpdateHostPack/source/Host/Proofs/HotUpdateSkeletonProofEntry.cs": [
                "[ChaosUnitTest(",
                'Alias = "hot-update-skeleton-proof"',
            ],
            "subjects/HotUpdateHostPack/source/Host/Benchmarks/HotUpdateLoadBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "hot-update-load-bench"',
            ],
            "subjects/MixedExecutionFeaturePack/source/Proofs/MixedExecutionProofEntry.cs": [
                "[ChaosUnitTest(",
                'Alias = "mixed-execution-proof"',
            ],
            "subjects/MixedExecutionFeaturePack/source/Benchmarks/MixedExecutionBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "mixed-execution-bench"',
            ],
            "subjects/MixedExecutionFeaturePack/source/Benchmarks/MixedExecutionNativeBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "mixed-execution-native-bench"',
            ],
        }

        for relative_path, required_fragments in expected_sources.items():
            source = (REPO_ROOT / relative_path).read_text(encoding="utf-8")
            for fragment in required_fragments:
                self.assertIn(fragment, source, msg=relative_path)

    def test_interpreter_arithmetic_subject_projects_support_assembly_from_mixed_execution_feature_pack(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_interpreter_arithmetic")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MixedExecutionFeaturePack")
        project_path = (
            REPO_ROOT
            / "subjects"
            / "MixedExecutionFeaturePack"
            / "source"
            / "EngineeringScenarios"
            / "MixedBridgeSolution"
            / "InterpreterArithmeticProof"
            / "InterpreterArithmeticProof.csproj"
        )
        program_path = (
            REPO_ROOT
            / "subjects"
            / "MixedExecutionFeaturePack"
            / "source"
            / "EngineeringScenarios"
            / "MixedBridgeSolution"
            / "InterpreterArithmeticProof"
            / "Program.cs"
        )
        program_source = program_path.read_text(encoding="utf-8")

        self.assertTrue(project_path.is_file(), msg=f"missing nested interpreter arithmetic project: {project_path}")
        self.assertTrue(program_path.is_file(), msg=f"missing nested interpreter arithmetic program: {program_path}")
        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln",
            manifest["source"]["path"],
        )
        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", manifest["source"]["primaryProjectPath"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])
        self.assertIn("namespace InterpreterArithmeticProof;", program_source)
        self.assertIn("public static int Main()", program_source)
