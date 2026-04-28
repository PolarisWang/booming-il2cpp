from __future__ import annotations

from unittest import TestCase
from unittest.mock import patch

from tests._support.module_loading import load_module

from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
FOUNDATION_DLL_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "foundation_dll.py"


class FoundationDllExecuteCommandTests(TestCase):
    def test_foundation_dll_execute_family_entry_invokes_execution_handler(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_execute")

        with patch.object(command_module, "execution_entry_module", create=True) as execution_mock:
            execution_mock.execute_entry.return_value = {
                "entryKind": "family",
                "scope": "family/System.Private.CoreLib/convert/char",
                "kind": "test",
            }

            result = command_module.handle(
                {"id": "foundation-dll-execute", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll execute",
                {
                    "assembly": "System.Private.CoreLib",
                    "family": "family/System.Private.CoreLib/convert/char",
                    "kind": "test",
                },
            )

            self.assertEqual(result.status, "ok")
            self.assertEqual(result.payload["scope"], "family/System.Private.CoreLib/convert/char")
            execution_mock.execute_entry.assert_called_once()

    def test_foundation_dll_execute_family_entry_returns_execution_project_paths(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_execute_paths")

        with patch.object(command_module, "execution_entry_module", create=True) as execution_mock:
            execution_mock.execute_entry.return_value = {
                "entryKind": "family",
                "assemblyName": "System.Private.CoreLib",
                "scope": "family/System.Private.CoreLib/convert/char",
                "kind": "benchmark",
                "projectPath": "verification/foundation-dll/System.Private.CoreLib/convert-char/benchmark/ConvertChar.Benchmark.csproj",
                "solutionPath": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
            }

            result = command_module.handle(
                {"id": "foundation-dll-execute", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll execute",
                {
                    "assembly": "System.Private.CoreLib",
                    "family": "family/System.Private.CoreLib/convert/char",
                    "kind": "benchmark",
                },
            )

            self.assertEqual(result.status, "ok")
            self.assertTrue(result.payload["projectPath"].endswith("/benchmark/ConvertChar.Benchmark.csproj"))
            self.assertTrue(result.payload["solutionPath"].endswith("FoundationDllTranslationSolution.sln"))
