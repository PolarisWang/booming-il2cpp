from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
GENERATED_MANAGED_HOSTS_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "generated_managed_hosts.py"
)
TEMPLATES_ROOT = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "templates"


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


def make_declared_entry(
    *,
    stable_id: str,
    alias: str,
    declaring_type: str,
    method_name: str,
    category: int = 1,
) -> dict:
    return {
        "stableId": stable_id,
        "alias": alias,
        "assemblyName": "FixtureSubject",
        "declaringType": declaring_type,
        "methodName": method_name,
        "methodSignature": f"{method_name}()",
        "category": category,
        "capabilityFamily": 1,
        "capabilityItem": 1,
        "archetype": 1,
        "hotUpdateCapability": 0,
        "requires": 0,
        "evidence": 1,
        "priority": 1,
        "metrics": 1,
        "modes": 1,
        "warmupCount": 2,
        "iterationCount": 5,
        "invocationCount": 10,
    }


class GeneratedManagedHostsTests(unittest.TestCase):
    def test_generated_managed_host_renderers_are_backed_by_template_assets(self) -> None:
        module_source = GENERATED_MANAGED_HOSTS_MODULE_PATH.read_text(encoding="utf-8")

        self.assertIn("templates/managed-proof-host.cs.tmpl", module_source)
        self.assertIn("templates/managed-benchmark-host.cs.tmpl", module_source)
        self.assertIn("templates/managed-declared-host.csproj.tmpl", module_source)
        self.assertTrue((TEMPLATES_ROOT / "managed-proof-host.cs.tmpl").is_file())
        self.assertTrue((TEMPLATES_ROOT / "managed-benchmark-host.cs.tmpl").is_file())
        self.assertTrue((TEMPLATES_ROOT / "managed-declared-host.csproj.tmpl").is_file())

    def test_assign_entry_indexes_sorts_each_family_by_stable_id(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_MANAGED_HOSTS_MODULE_PATH,
            "chaos_generated_managed_hosts_entry_index",
        )

        entries = [
            make_declared_entry(
                stable_id="FixtureSubject::FixtureSubject::FixtureSubject.ZetaProofs::Verify()",
                alias="zeta-proof",
                declaring_type="FixtureSubject.ZetaProofs",
                method_name="Verify",
            ),
            make_declared_entry(
                stable_id="FixtureSubject::FixtureSubject::FixtureSubject.AlphaProofs::Verify()",
                alias="alpha-proof",
                declaring_type="FixtureSubject.AlphaProofs",
                method_name="Verify",
            ),
        ]

        indexed_entries = generated_hosts_module.assign_entry_indexes(entries)

        self.assertEqual(
            [
                "FixtureSubject::FixtureSubject::FixtureSubject.AlphaProofs::Verify()",
                "FixtureSubject::FixtureSubject::FixtureSubject.ZetaProofs::Verify()",
            ],
            [entry["stableId"] for entry in indexed_entries],
        )
        self.assertEqual([0, 1], [entry["entryIndex"] for entry in indexed_entries])

    def test_assign_entry_indexes_preserves_explicit_entry_indexes_when_present(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_MANAGED_HOSTS_MODULE_PATH,
            "chaos_generated_managed_hosts_explicit_entry_index",
        )

        entries = [
            {
                **make_declared_entry(
                    stable_id="FixtureSubject::FixtureSubject::FixtureSubject.ZetaBenchmarks::Run()",
                    alias="zeta-bench",
                    declaring_type="FixtureSubject.ZetaBenchmarks",
                    method_name="Run",
                ),
                "entryIndex": 11,
            },
            {
                **make_declared_entry(
                    stable_id="FixtureSubject::FixtureSubject::FixtureSubject.AlphaBenchmarks::Run()",
                    alias="alpha-bench",
                    declaring_type="FixtureSubject.AlphaBenchmarks",
                    method_name="Run",
                ),
                "entryIndex": 7,
            },
        ]

        indexed_entries = generated_hosts_module.assign_entry_indexes(entries)

        self.assertEqual(
            [
                "FixtureSubject::FixtureSubject::FixtureSubject.AlphaBenchmarks::Run()",
                "FixtureSubject::FixtureSubject::FixtureSubject.ZetaBenchmarks::Run()",
            ],
            [entry["stableId"] for entry in indexed_entries],
        )
        self.assertEqual([7, 11], [entry["entryIndex"] for entry in indexed_entries])

    def test_render_declared_proof_host_source_uses_numeric_dispatch_and_direct_wrappers(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_MANAGED_HOSTS_MODULE_PATH,
            "chaos_generated_managed_proof_host",
        )

        proof_entries = generated_hosts_module.assign_entry_indexes(
            [
                make_declared_entry(
                    stable_id="FixtureSubject::FixtureSubject::FixtureSubject.AlphaProofs::Verify()",
                    alias="alpha-proof",
                    declaring_type="FixtureSubject.AlphaProofs",
                    method_name="Verify",
                ),
                make_declared_entry(
                    stable_id="FixtureSubject::FixtureSubject::FixtureSubject.BetaProofs::Run()",
                    alias="beta-proof",
                    declaring_type="FixtureSubject.BetaProofs",
                    method_name="Run",
                ),
            ]
        )

        source_text = generated_hosts_module.render_declared_test_host_source(
            subject_id="FixtureSubject",
            host_kind="proof-host",
            entries=proof_entries,
        )

        self.assertIn("ChaosManagedHostArguments.Parse(args)", source_text)
        self.assertIn("ChaosTestCollectionLoader.EnsureEntryExists", source_text)
        self.assertIn("ChaosManagedHostKind.Proof", source_text)
        self.assertIn("ChaosAssertState.Reset();", source_text)
        self.assertIn("return ChaosAssertState.Complete();", source_text)
        self.assertIn("public static int Execute(int entryIndex)", source_text)
        self.assertIn("switch (entryIndex)", source_text)
        self.assertIn("case 0:", source_text)
        self.assertIn("case 1:", source_text)
        self.assertIn("global::FixtureSubject.AlphaProofs.Verify();", source_text)
        self.assertIn("global::FixtureSubject.BetaProofs.Run();", source_text)
        self.assertNotIn("sourceEntry", source_text)
        self.assertNotIn("Dictionary<string, Func<int>>", source_text)

    def test_render_declared_benchmark_host_source_dispatches_without_embedded_metadata_table(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_MANAGED_HOSTS_MODULE_PATH,
            "chaos_generated_managed_benchmark_host",
        )

        benchmark_entries = generated_hosts_module.assign_entry_indexes(
            [
                make_declared_entry(
                    stable_id="FixtureSubject::FixtureSubject::FixtureSubject.GenericBenchmarks::Run()",
                    alias="generic-bench",
                    declaring_type="FixtureSubject.GenericBenchmarks",
                    method_name="Run",
                    category=3,
                ),
            ]
        )

        source_text = generated_hosts_module.render_declared_test_host_source(
            subject_id="FixtureSubject",
            host_kind="benchmark-host",
            entries=benchmark_entries,
        )

        self.assertIn("ChaosManagedHostKind.Benchmark", source_text)
        self.assertIn("ChaosTestCollectionLoader.EnsureEntryExists", source_text)
        self.assertIn("public static int Execute(int entryIndex)", source_text)
        self.assertIn("case 0:", source_text)
        self.assertIn("global::FixtureSubject.GenericBenchmarks.Run();", source_text)
        self.assertNotIn("DeclaredBenchmarkEntry", source_text)
        self.assertNotIn("public static IReadOnlyList", source_text)
        self.assertNotIn('"generic-bench"', source_text)
        self.assertNotIn("WarmupCount:", source_text)

    def test_render_declared_test_host_source_rejects_empty_catalog(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_MANAGED_HOSTS_MODULE_PATH,
            "chaos_generated_managed_host_empty_catalog",
        )

        with self.assertRaisesRegex(ValueError, "requires at least one declared entry"):
            generated_hosts_module.render_declared_test_host_source(
                subject_id="FixtureSubject",
                host_kind="proof-host",
                entries=[],
            )

    def test_render_declared_test_host_source_rejects_incomplete_entry_metadata(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_MANAGED_HOSTS_MODULE_PATH,
            "chaos_generated_managed_host_incomplete_entry",
        )

        with self.assertRaisesRegex(ValueError, "declaringType"):
            generated_hosts_module.render_declared_test_host_source(
                subject_id="FixtureSubject",
                host_kind="proof-host",
                entries=generated_hosts_module.assign_entry_indexes(
                    [
                        make_declared_entry(
                            stable_id="FixtureSubject::FixtureSubject::FixtureSubject.BrokenProofs::Verify()",
                            alias="broken-proof",
                            declaring_type="",
                            method_name="Verify",
                        ),
                    ]
                ),
            )

    def test_render_managed_host_project_includes_framework_and_subject_references(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_MANAGED_HOSTS_MODULE_PATH,
            "chaos_generated_managed_host_project",
        )

        project_text = generated_hosts_module.render_declared_test_host_project(
            subject_id="FixtureSubject",
            host_kind="proof-host",
            project_references=[
                "../../../../verification/catalog/owners/FixtureSubject/support/host/FixtureSubject.csproj",
                "../../../../verification/catalog/owners/FixtureSubject/support/host/FixtureSubject.Support.csproj",
            ],
            generated_source_path="Generated/ChaosGeneratedDeclaredTests.g.cs",
            assembly_name="FixtureSubject.DeclaredProofHost",
        )

        self.assertIn(
            "../../../../../src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj",
            project_text,
        )
        self.assertIn(
            "../../../../../src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj",
            project_text,
        )
        self.assertIn(
            "../../../../verification/catalog/owners/FixtureSubject/support/host/FixtureSubject.csproj",
            project_text,
        )
        self.assertIn(
            "../../../../verification/catalog/owners/FixtureSubject/support/host/FixtureSubject.Support.csproj",
            project_text,
        )
        self.assertIn("<TargetFramework>net8.0</TargetFramework>", project_text)
        self.assertIn("<OutputType>Exe</OutputType>", project_text)
        self.assertIn("<EnableDefaultCompileItems>false</EnableDefaultCompileItems>", project_text)
        self.assertIn('<Compile Include="Generated/ChaosGeneratedDeclaredTests.g.cs" />', project_text)


if __name__ == "__main__":
    unittest.main()

