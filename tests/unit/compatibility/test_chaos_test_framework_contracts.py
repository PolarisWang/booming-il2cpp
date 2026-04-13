from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
FRAMEWORK_ROOT = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework"
FRAMEWORK_PROJECT_PATH = FRAMEWORK_ROOT / "Chaos.TestFramework.csproj"
FRAMEWORK_CONTRACT_PATH = FRAMEWORK_ROOT / "Chaos.TestFramework.cs"
DECLARATIONS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "declarations.py"


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


class ChaosTestFrameworkContractTests(unittest.TestCase):
    def test_reference_test_framework_project_exposes_compact_contract_surface(self) -> None:
        self.assertTrue(FRAMEWORK_ROOT.is_dir(), msg=f"missing framework root: {FRAMEWORK_ROOT}")
        self.assertTrue(FRAMEWORK_PROJECT_PATH.is_file(), msg=f"missing framework project: {FRAMEWORK_PROJECT_PATH}")
        self.assertTrue(FRAMEWORK_CONTRACT_PATH.is_file(), msg=f"missing framework contract file: {FRAMEWORK_CONTRACT_PATH}")

        project_text = FRAMEWORK_PROJECT_PATH.read_text(encoding="utf-8")
        contract_text = FRAMEWORK_CONTRACT_PATH.read_text(encoding="utf-8")

        self.assertIn("<TargetFramework>net8.0</TargetFramework>", project_text)
        self.assertIn("namespace Chaos.TestFramework;", contract_text)
        self.assertIn("public enum ChaosUnitCategory : byte", contract_text)
        self.assertIn("public enum ChaosBenchmarkCategory : byte", contract_text)
        self.assertIn("public enum ChaosMetric : ushort", contract_text)
        self.assertIn("public enum ChaosRuntimeFeature : uint", contract_text)
        self.assertIn("public enum ChaosEvidenceKind : ushort", contract_text)
        self.assertIn("public sealed class ChaosUnitTestAttribute : Attribute", contract_text)
        self.assertIn("public sealed class ChaosBenchmarkAttribute : Attribute", contract_text)
        self.assertIn("public byte Priority { get; init; }", contract_text)
        self.assertIn("public byte WarmupCount { get; init; }", contract_text)
        self.assertIn("public ushort IterationCount { get; init; }", contract_text)
        self.assertIn("public ushort InvocationCount { get; init; }", contract_text)
        self.assertIn("public string? Alias { get; init; }", contract_text)
        self.assertIn("/// <summary>", contract_text)
        self.assertIn("RuntimeContract = 1", contract_text)
        self.assertIn("RuntimeDispatch = 1", contract_text)
        self.assertIn("WallClockUs = 1 << 0", contract_text)

    def test_declaration_contract_module_normalizes_modes(self) -> None:
        declarations_module = load_module(DECLARATIONS_MODULE_PATH, "chaos_declaration_contract_modes")

        self.assertEqual("auto", declarations_module.test_declaration_mode({}).value)
        self.assertEqual("none", declarations_module.test_declaration_mode({"testDeclarationMode": "none"}).value)
        self.assertEqual("require", declarations_module.test_declaration_mode({"testDeclarationMode": "require"}).value)

        with self.assertRaises(ValueError):
            declarations_module.test_declaration_mode({"testDeclarationMode": "invalid"})

    def test_declaration_contract_module_builds_stable_ids_and_scan_summary(self) -> None:
        declarations_module = load_module(DECLARATIONS_MODULE_PATH, "chaos_declaration_contract_summary")

        stable_id = declarations_module.stable_declared_test_id(
            subject_id="SolutionMultiProject",
            assembly_name="SolutionMultiProject.Tests",
            declaring_type="SolutionMultiProject.Proofs.DispatchProofs",
            method_signature="Run()",
        )
        self.assertEqual(
            "SolutionMultiProject::SolutionMultiProject.Tests::SolutionMultiProject.Proofs.DispatchProofs::Run()",
            stable_id,
        )

        runtime_only = declarations_module.summarize_declaration_scan(
            mode=declarations_module.TestDeclarationMode.AUTO,
            framework_referenced=False,
            declared_unit_tests=[],
            declared_benchmarks=[],
        )
        self.assertEqual("runtime-only", runtime_only.subject_kind.value)
        self.assertEqual([], runtime_only.warning_codes)

        framework_without_entries = declarations_module.summarize_declaration_scan(
            mode=declarations_module.TestDeclarationMode.AUTO,
            framework_referenced=True,
            declared_unit_tests=[],
            declared_benchmarks=[],
        )
        self.assertEqual("runtime-only", framework_without_entries.subject_kind.value)
        self.assertEqual(
            [declarations_module.DeclarationWarningCode.FRAMEWORK_WITHOUT_DECLARATIONS],
            framework_without_entries.warning_codes,
        )

        with self.assertRaises(ValueError):
            declarations_module.summarize_declaration_scan(
                mode=declarations_module.TestDeclarationMode.REQUIRE,
                framework_referenced=False,
                declared_unit_tests=[],
                declared_benchmarks=[],
            )


if __name__ == "__main__":
    unittest.main()
