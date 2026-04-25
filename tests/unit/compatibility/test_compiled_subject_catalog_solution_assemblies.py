from tests.unit.compatibility.compiled_subject_catalog_test_support import *


class TestCompiledSubjectCatalogSolutionAssemblies(CompiledSubjectCatalogTestSupport):
    def test_declared_catalog_resolves_primary_project_assembly_from_solution_source(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_solution_source")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "solution-source"
        subject_id = "FixtureSolutionCatalog"
        project_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "App"
            / "FixtureSolutionCatalog.App.csproj"
        )
        assembly_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "App"
            / "bin"
            / "Debug"
            / "net8.0"
            / "FixtureSolutionCatalog.App.dll"
        )
        solution_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "FixtureSolutionCatalog.sln"
        )

        manifest = {
            "subjectId": "FixtureSolutionCatalog",
            "displayName": "FixtureSolutionCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/FixtureSolutionCatalog.sln",
                "primaryProjectPath": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/App/FixtureSolutionCatalog.App.csproj",
                "entry": "FixtureSolutionCatalog.App/Program::Main()",
            },
            "sourceModel": "dotnet-solution",
            "engineeringProfile": "managed-output",
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
                }
            ],
        }

        try:
            solution_path.parent.mkdir(parents=True, exist_ok=True)
            solution_path.write_text("", encoding="utf-8")
            project_path.parent.mkdir(parents=True, exist_ok=True)
            project_path.write_text("<Project Sdk=\"Microsoft.NET.Sdk\" />\n", encoding="utf-8")
            assembly_path.parent.mkdir(parents=True, exist_ok=True)
            assembly_path.write_text("", encoding="utf-8")
            manifest_path = write_owner_manifest(repo_root, subject_id, manifest)
            os.utime(solution_path, (1000, 1000))
            os.utime(project_path, (1000, 1000))
            os.utime(manifest_path, (1000, 1000))
            os.utime(assembly_path, (2000, 2000))

            assembly_paths = compiled_catalog_module._resolve_subject_assembly_paths(
                repo_root,
                manifest,
                build_if_missing=False,
            )

            self.assertEqual([assembly_path.resolve()], assembly_paths)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_catalog_resolves_all_solution_assemblies_from_primary_output_directory(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_solution_multi_assembly")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "solution-multi-assembly"
        subject_id = "FixtureSolutionCatalog"
        solution_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "FixtureSolutionCatalog.sln"
        )
        app_project_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "App"
            / "FixtureSolutionCatalog.App.csproj"
        )
        support_project_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "Support"
            / "FixtureSolutionCatalog.Support.csproj"
        )
        app_assembly_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "App"
            / "bin"
            / "Debug"
            / "net8.0"
            / "FixtureSolutionCatalog.App.dll"
        )
        support_assembly_path = app_assembly_path.parent / "FixtureSolutionCatalog.Support.dll"

        manifest = {
            "subjectId": "FixtureSolutionCatalog",
            "displayName": "FixtureSolutionCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/FixtureSolutionCatalog.sln",
                "primaryProjectPath": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/App/FixtureSolutionCatalog.App.csproj",
                "entry": "FixtureSolutionCatalog.App/Program::Main()",
            },
            "sourceModel": "dotnet-solution",
            "engineeringProfile": "managed-output",
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
                }
            ],
        }

        solution_text = """
Microsoft Visual Studio Solution File, Format Version 12.00
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "FixtureSolutionCatalog.App", "App\\FixtureSolutionCatalog.App.csproj", "{11111111-1111-1111-1111-111111111111}"
EndProject
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "FixtureSolutionCatalog.Support", "Support\\FixtureSolutionCatalog.Support.csproj", "{22222222-2222-2222-2222-222222222222}"
EndProject
Global
EndGlobal
""".lstrip()

        try:
            solution_path.parent.mkdir(parents=True, exist_ok=True)
            app_project_path.parent.mkdir(parents=True, exist_ok=True)
            support_project_path.parent.mkdir(parents=True, exist_ok=True)
            solution_path.write_text(solution_text, encoding="utf-8")
            app_project_path.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\"><PropertyGroup><AssemblyName>FixtureSolutionCatalog.App</AssemblyName></PropertyGroup></Project>\n",
                encoding="utf-8",
            )
            support_project_path.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\"><PropertyGroup><AssemblyName>FixtureSolutionCatalog.Support</AssemblyName></PropertyGroup></Project>\n",
                encoding="utf-8",
            )
            app_assembly_path.parent.mkdir(parents=True, exist_ok=True)
            app_assembly_path.write_text("", encoding="utf-8")
            support_assembly_path.write_text("", encoding="utf-8")
            manifest_path = write_owner_manifest(repo_root, subject_id, manifest)
            os.utime(solution_path, (1000, 1000))
            os.utime(app_project_path, (1000, 1000))
            os.utime(support_project_path, (1000, 1000))
            os.utime(manifest_path, (1000, 1000))
            os.utime(app_assembly_path, (2000, 2000))
            os.utime(support_assembly_path, (2000, 2000))

            assembly_paths = compiled_catalog_module._resolve_subject_assembly_paths(
                repo_root,
                manifest,
                build_if_missing=False,
            )

            self.assertEqual(
                [
                    app_assembly_path.resolve(),
                    support_assembly_path.resolve(),
                ],
                assembly_paths,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_catalog_resolves_solution_assemblies_from_external_project_output_directories(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_solution_external_assembly")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "solution-external-assembly"
        subject_id = "FixtureSolutionCatalog"
        solution_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "FixtureSolutionCatalog.sln"
        )
        app_project_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "Host"
            / "FixtureSolutionCatalog.Host.csproj"
        )
        external_project_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "Patch"
            / "FixtureSolutionCatalog.Patch.csproj"
        )
        app_assembly_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "Host"
            / "bin"
            / "Debug"
            / "net8.0"
            / "FixtureSolutionCatalog.Host.dll"
        )
        external_assembly_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "Patch"
            / "bin"
            / "Debug"
            / "net8.0"
            / "FixtureSolutionCatalog.Patch.dll"
        )

        manifest = {
            "subjectId": "FixtureSolutionCatalog",
            "displayName": "FixtureSolutionCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/FixtureSolutionCatalog.sln",
                "primaryProjectPath": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/Host/FixtureSolutionCatalog.Host.csproj",
                "entry": "FixtureSolutionCatalog.Host/Program::Main()",
            },
            "sourceModel": "dotnet-solution",
            "engineeringProfile": "managed-output",
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
                }
            ],
        }

        solution_text = """
Microsoft Visual Studio Solution File, Format Version 12.00
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "FixtureSolutionCatalog.Host", "Host\\FixtureSolutionCatalog.Host.csproj", "{11111111-1111-1111-1111-111111111111}"
EndProject
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "FixtureSolutionCatalog.Patch", "Patch\\FixtureSolutionCatalog.Patch.csproj", "{22222222-2222-2222-2222-222222222222}"
EndProject
Global
EndGlobal
""".lstrip()

        try:
            solution_path.parent.mkdir(parents=True, exist_ok=True)
            app_project_path.parent.mkdir(parents=True, exist_ok=True)
            external_project_path.parent.mkdir(parents=True, exist_ok=True)
            solution_path.write_text(solution_text, encoding="utf-8")
            app_project_path.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\"><PropertyGroup><AssemblyName>FixtureSolutionCatalog.Host</AssemblyName></PropertyGroup></Project>\n",
                encoding="utf-8",
            )
            external_project_path.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\"><PropertyGroup><AssemblyName>FixtureSolutionCatalog.Patch</AssemblyName></PropertyGroup></Project>\n",
                encoding="utf-8",
            )
            app_assembly_path.parent.mkdir(parents=True, exist_ok=True)
            external_assembly_path.parent.mkdir(parents=True, exist_ok=True)
            app_assembly_path.write_text("", encoding="utf-8")
            external_assembly_path.write_text("", encoding="utf-8")
            manifest_path = write_owner_manifest(repo_root, subject_id, manifest)
            os.utime(solution_path, (1000, 1000))
            os.utime(app_project_path, (1000, 1000))
            os.utime(external_project_path, (1000, 1000))
            os.utime(manifest_path, (1000, 1000))
            os.utime(app_assembly_path, (2000, 2000))
            os.utime(external_assembly_path, (2000, 2000))

            assembly_paths = compiled_catalog_module._resolve_subject_assembly_paths(
                repo_root,
                manifest,
                build_if_missing=False,
            )

            self.assertEqual(
                [
                    app_assembly_path.resolve(),
                    external_assembly_path.resolve(),
                ],
                assembly_paths,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_catalog_rebuilds_stale_solution_assemblies_even_when_output_exists(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_solution_stale")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "solution-stale"
        subject_id = "FixtureSolutionCatalog"
        solution_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "FixtureSolutionCatalog.sln"
        )
        project_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "App"
            / "FixtureSolutionCatalog.App.csproj"
        )
        source_file_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "App"
            / "Slice.cs"
        )
        assembly_path = (
            repo_root
            / "verification"
            / "catalog"
            / "scenarios"
            / "FixtureSolutionCatalog"
            / "FixtureSolutionCatalogSolution"
            / "App"
            / "bin"
            / "Debug"
            / "net8.0"
            / "FixtureSolutionCatalog.App.dll"
        )

        manifest = {
            "subjectId": "FixtureSolutionCatalog",
            "displayName": "FixtureSolutionCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/FixtureSolutionCatalog.sln",
                "primaryProjectPath": "verification/catalog/scenarios/FixtureSolutionCatalog/FixtureSolutionCatalogSolution/App/FixtureSolutionCatalog.App.csproj",
                "entry": "FixtureSolutionCatalog.App/Program::Main()",
            },
            "sourceModel": "dotnet-solution",
            "engineeringProfile": "managed-output",
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
                }
            ],
        }

        solution_text = """
Microsoft Visual Studio Solution File, Format Version 12.00
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "FixtureSolutionCatalog.App", "App\\FixtureSolutionCatalog.App.csproj", "{11111111-1111-1111-1111-111111111111}"
EndProject
Global
EndGlobal
""".lstrip()

        build_calls: list[list[str]] = []

        def fake_run_process(arguments: list[str], cwd: Path):
            build_calls.append(list(arguments))
            assembly_path.parent.mkdir(parents=True, exist_ok=True)
            assembly_path.write_text("", encoding="utf-8")
            os.utime(assembly_path, (3000, 3000))

            class CompletedProcess:
                returncode = 0
                stdout = ""
                stderr = ""

            return CompletedProcess()

        try:
            solution_path.parent.mkdir(parents=True, exist_ok=True)
            project_path.parent.mkdir(parents=True, exist_ok=True)
            solution_path.write_text(solution_text, encoding="utf-8")
            project_path.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\"><PropertyGroup><AssemblyName>FixtureSolutionCatalog.App</AssemblyName></PropertyGroup></Project>\n",
                encoding="utf-8",
            )
            source_file_path.write_text("internal static class Slice { }\n", encoding="utf-8")
            assembly_path.parent.mkdir(parents=True, exist_ok=True)
            assembly_path.write_text("", encoding="utf-8")
            manifest_path = write_owner_manifest(repo_root, subject_id, manifest)

            os.utime(assembly_path, (1000, 1000))
            os.utime(solution_path, (2000, 2000))
            os.utime(project_path, (2000, 2000))
            os.utime(source_file_path, (2000, 2000))
            os.utime(manifest_path, (2000, 2000))

            with mock.patch.object(compiled_catalog_module, "run_process", side_effect=fake_run_process):
                assembly_paths = compiled_catalog_module._resolve_subject_assembly_paths(
                    repo_root,
                    manifest,
                    build_if_missing=False,
                )

            self.assertEqual([assembly_path.resolve()], assembly_paths)
            self.assertEqual(1, len(build_calls))
            self.assertEqual("dotnet", build_calls[0][0])
            self.assertEqual("build", build_calls[0][1])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
