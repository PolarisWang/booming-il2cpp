from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
GENERATOR_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "execution_solution_generator.py"
)


def test_generate_execution_solution_wires_formal_solution_and_family_entries() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_execution_solution_generator")
    repo_root = make_temp_repo_root("verification", "foundation-dll-execution-solution")

    try:
        families = [
            {
                "assemblyName": "System.Private.CoreLib",
                "familyId": "family/System.Private.CoreLib/convert/char",
                "displayName": "Convert.Char Conversions",
            },
            {
                "assemblyName": "System.Collections.Immutable",
                "familyId": "family/System.Collections.Immutable/immutable-array",
                "displayName": "Immutable Array",
            },
        ]
        for item in families:
            family_slug = "-".join(item["familyId"].split("/")[2:])
            family_root = repo_root / "verification" / "foundation-dll" / item["assemblyName"] / family_slug
            for subdir, filename in [
                ("test", "ConvertChar.Tests.csproj" if item["assemblyName"] == "System.Private.CoreLib" else "ImmutableArray.Tests.csproj"),
                ("native", "ConvertChar.Native.csproj" if item["assemblyName"] == "System.Private.CoreLib" else "ImmutableArray.Native.csproj"),
                ("benchmark", "ConvertChar.Benchmark.csproj" if item["assemblyName"] == "System.Private.CoreLib" else "ImmutableArray.Benchmark.csproj"),
                ("host", "ConvertChar.Host.csproj" if item["assemblyName"] == "System.Private.CoreLib" else "ImmutableArray.Host.csproj"),
                ("patch", "ConvertChar.Patch.csproj" if item["assemblyName"] == "System.Private.CoreLib" else "ImmutableArray.Patch.csproj"),
            ]:
                project_path = family_root / subdir / filename
                project_path.parent.mkdir(parents=True, exist_ok=True)
                project_path.write_text("<Project Sdk=\"Microsoft.NET.Sdk\" />\n", encoding="utf-8")

        result = generator_module.generate_execution_solution(
            repo_root,
            solution_path=repo_root
            / "subjects"
            / "SolutionCorePack"
            / "source"
            / "EngineeringScenarios"
            / "FoundationDllTranslationSolution"
            / "FoundationDllTranslationSolution.sln",
            families=families,
        )

        solution_path = (
            repo_root
            / "subjects"
            / "SolutionCorePack"
            / "source"
            / "EngineeringScenarios"
            / "FoundationDllTranslationSolution"
            / "FoundationDllTranslationSolution.sln"
        )
        assert solution_path.is_file()
        solution_text = solution_path.read_text(encoding="utf-8")
        assert "System.Private.CoreLib" in solution_text
        assert "System.Collections.Immutable" in solution_text
        assert result["dllCount"] == 2
        assert len(result["entries"]) == 2
        assert result["entries"][0]["familyExecutionEntries"]["test"].endswith("/test/ConvertChar.Tests.csproj")
        assert result["entries"][0]["familyExecutionEntries"]["benchmark"].endswith("/benchmark/ConvertChar.Benchmark.csproj")
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)


def test_generate_execution_solution_can_cover_all_14_dll_scopes() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_execution_solution_generator_all")
    repo_root = make_temp_repo_root("verification", "foundation-dll-execution-solution-all")

    try:
        assemblies = [
            "System.Private.CoreLib",
            "System.Collections.Immutable",
            "System.Formats.Asn1",
            "System.IO.Compression.Brotli",
            "System.IO.Compression.ZipFile",
            "System.IO.Pipelines",
            "System.Linq",
            "System.Net.ServerSentEvents",
            "System.ObjectModel",
            "System.Runtime.InteropServices",
            "System.Runtime.Serialization.Formatters",
            "System.Security.Principal.Windows",
            "System.Text.Json",
            "System.Threading.Tasks.Parallel",
        ]
        families = []
        for assembly_name in assemblies:
            family_id = f"family/{assembly_name}/sample"
            family_slug = "-".join(family_id.split("/")[2:])
            family_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug
            project_base = "Sample"
            for subdir, filename in [
                ("test", f"{project_base}.Tests.csproj"),
                ("native", f"{project_base}.Native.csproj"),
                ("benchmark", f"{project_base}.Benchmark.csproj"),
                ("host", f"{project_base}.Host.csproj"),
                ("patch", f"{project_base}.Patch.csproj"),
            ]:
                project_path = family_root / subdir / filename
                project_path.parent.mkdir(parents=True, exist_ok=True)
                project_path.write_text("<Project Sdk=\"Microsoft.NET.Sdk\" />\n", encoding="utf-8")
            families.append(
                {
                    "assemblyName": assembly_name,
                    "familyId": family_id,
                    "displayName": f"{assembly_name} Sample",
                }
            )

        result = generator_module.generate_execution_solution(
            repo_root,
            solution_path=repo_root
            / "subjects"
            / "SolutionCorePack"
            / "source"
            / "EngineeringScenarios"
            / "FoundationDllTranslationSolution"
            / "FoundationDllTranslationSolution.sln",
            families=families,
        )

        solution_path = (
            repo_root
            / "subjects"
            / "SolutionCorePack"
            / "source"
            / "EngineeringScenarios"
            / "FoundationDllTranslationSolution"
            / "FoundationDllTranslationSolution.sln"
        )
        solution_text = solution_path.read_text(encoding="utf-8")
        assert result["dllCount"] == 14
        for assembly_name in assemblies:
            assert assembly_name in solution_text
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
