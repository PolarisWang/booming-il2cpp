from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
GENERATED_HOTUPDATE_HOSTS_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "generated_hotupdate_hosts.py"
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


class GeneratedHotUpdateHostsTests(unittest.TestCase):
    def test_generated_hotupdate_host_renderers_are_backed_by_template_assets(self) -> None:
        module_source = GENERATED_HOTUPDATE_HOSTS_MODULE_PATH.read_text(encoding="utf-8")

        self.assertIn("templates/hotupdate-proof-host.cs.tmpl", module_source)
        self.assertIn("templates/hotupdate-benchmark-host.cs.tmpl", module_source)
        self.assertIn("templates/hotupdate-host.csproj.tmpl", module_source)
        self.assertTrue((TEMPLATES_ROOT / "hotupdate-proof-host.cs.tmpl").is_file())
        self.assertTrue((TEMPLATES_ROOT / "hotupdate-benchmark-host.cs.tmpl").is_file())
        self.assertTrue((TEMPLATES_ROOT / "hotupdate-host.csproj.tmpl").is_file())

    def test_render_declared_hotupdate_benchmark_host_source_exposes_invoke_workload_surface(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_HOTUPDATE_HOSTS_MODULE_PATH,
            "chaos_generated_hotupdate_benchmark_host_source",
        )

        source_text = generated_hosts_module.render_declared_hotupdate_host_source(
            subject_id="FixtureSubject",
            host_kind="benchmark-host",
        )

        self.assertIn("public static object? InvokeWorkload", source_text)
        self.assertIn("ChaosHotUpdateExecutor.ExecuteBenchmarkWorkload(", source_text)
        self.assertIn("ResolveHostBaseDirectory()", source_text)
        self.assertIn("Path.GetDirectoryName(typeof(FixtureSubjectHotUpdateBenchmarkHost).Assembly.Location)", source_text)
        self.assertIn("var request = ChaosHotUpdateHostArguments.Parse(args);", source_text)

    def test_render_declared_hotupdate_host_project_limits_compile_inputs_to_generated_source(self) -> None:
        generated_hosts_module = load_module(
            GENERATED_HOTUPDATE_HOSTS_MODULE_PATH,
            "chaos_generated_hotupdate_host_project",
        )

        project_text = generated_hosts_module.render_declared_hotupdate_host_project(
            assembly_name="FixtureSubject.HotUpdateProofHost",
            generated_source_path="Generated/ChaosGeneratedHotUpdateProofHost.g.cs",
        )

        self.assertIn("<TargetFramework>net8.0</TargetFramework>", project_text)
        self.assertIn("<OutputType>Exe</OutputType>", project_text)
        self.assertIn("<EnableDefaultCompileItems>false</EnableDefaultCompileItems>", project_text)
        self.assertIn("Chaos.TestFramework.Sdk.csproj", project_text)
        self.assertIn("Chaos.TestFramework.Runtime.csproj", project_text)
        self.assertIn('<Compile Include="Generated/ChaosGeneratedHotUpdateProofHost.g.cs" />', project_text)


if __name__ == "__main__":
    unittest.main()
