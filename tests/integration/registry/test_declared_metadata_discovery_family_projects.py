from __future__ import annotations

import shutil
import uuid
from pathlib import Path

from tests.integration.registry.declared_metadata_discovery_test_support import *


class TestDeclaredMetadataDiscoveryFamilyProjects(DeclaredMetadataDiscoveryTestSupport):
    def test_family_test_project_declares_capability_test_and_benchmark_metadata(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_family_project")

        build_root = TEST_TMP_ROOT / "family-project" / uuid.uuid4().hex
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            project_path = REPO_ROOT / "tests" / "families" / "System.Private.CoreLib" / "convert-char" / "ConvertCharTests.csproj"
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    *dotnet_intermediate_args(build_root / "obj"),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = project_path.parent / "bin" / "Release" / "net8.0" / "ConvertCharTests.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled family test dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="ConvertCharFamilyProject",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual(1, len(catalog["declaredUnitTests"]))
            self.assertEqual(1, len(catalog["declaredBenchmarks"]))
            self.assertEqual("ConvertCharTests", catalog["declaredUnitTests"][0]["assemblyName"])
            self.assertEqual("ConvertCharTests", catalog["declaredBenchmarks"][0]["assemblyName"])
            self.assertEqual("dispatch-bench", catalog["declaredBenchmarks"][0]["alias"])
        finally:
            shutil.rmtree(build_root, ignore_errors=True)
