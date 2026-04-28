from __future__ import annotations

import os
from pathlib import Path
from typing import Any


def _relative(base: Path, path: Path) -> str:
    return os.path.relpath(path.resolve(), start=base.resolve()).replace("\\", "/")


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return str(family_id).replace("/", "-")


def _project_basename(family_id: str) -> str:
    slug = _family_slug(family_id)
    return "".join(part[:1].upper() + part[1:] for part in slug.split("-") if part)


def _write_solution(solution_path: Path, project_paths: list[Path]) -> None:
    solution_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "Microsoft Visual Studio Solution File, Format Version 12.00",
        "# Visual Studio Version 17",
        "VisualStudioVersion = 17.0.31903.59",
        "MinimumVisualStudioVersion = 10.0.40219.1",
    ]
    guids: list[str] = []
    for index, project_path in enumerate(project_paths, start=1):
        project_name = project_path.parent.parent.name + "." + project_path.parent.name
        relative = _relative(solution_path.parent, project_path)
        guid = f"00000000-0000-0000-0000-{index:012d}"
        guids.append(guid)
        lines.append(
            f'Project("{{9A19103F-16F7-4668-BE54-9A1E7A4F7556}}") = "{project_name}", "{relative}", "{{{guid}}}"'
        )
        lines.append("EndProject")
    lines.append("Global")
    lines.append("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution")
    lines.append("\t\tDebug|Any CPU = Debug|Any CPU")
    lines.append("\t\tRelease|Any CPU = Release|Any CPU")
    lines.append("\tEndGlobalSection")
    lines.append("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution")
    for guid in guids:
        lines.append(f"\t\t{{{guid}}}.Debug|Any CPU.ActiveCfg = Debug|Any CPU")
        lines.append(f"\t\t{{{guid}}}.Debug|Any CPU.Build.0 = Debug|Any CPU")
        lines.append(f"\t\t{{{guid}}}.Release|Any CPU.ActiveCfg = Release|Any CPU")
        lines.append(f"\t\t{{{guid}}}.Release|Any CPU.Build.0 = Release|Any CPU")
    lines.append("\tEndGlobalSection")
    lines.append("\tGlobalSection(SolutionProperties) = preSolution")
    lines.append("\t\tHideSolutionNode = FALSE")
    lines.append("\tEndGlobalSection")
    lines.append("EndGlobal")
    solution_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def generate_execution_solution(
    repo_root: Path,
    *,
    solution_path: Path,
    families: list[dict[str, Any]],
) -> dict[str, Any]:
    project_paths: list[Path] = []
    entries: list[dict[str, Any]] = []
    for family in families:
        assembly_name = str(family.get("assemblyName") or "")
        family_id = str(family.get("familyId") or "")
        family_slug = _family_slug(family_id)
        project_base = _project_basename(family_id)
        family_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug
        family_entries = {
            "test": _relative(repo_root, family_root / "test" / f"{project_base}.Tests.csproj"),
            "native": _relative(repo_root, family_root / "native" / f"{project_base}.Native.csproj"),
            "benchmark": _relative(repo_root, family_root / "benchmark" / f"{project_base}.Benchmark.csproj"),
            "host": _relative(repo_root, family_root / "host" / f"{project_base}.Host.csproj"),
            "patch": _relative(repo_root, family_root / "patch" / f"{project_base}.Patch.csproj"),
        }
        project_paths.extend(
            [
                family_root / "test" / f"{project_base}.Tests.csproj",
                family_root / "native" / f"{project_base}.Native.csproj",
                family_root / "benchmark" / f"{project_base}.Benchmark.csproj",
                family_root / "host" / f"{project_base}.Host.csproj",
                family_root / "patch" / f"{project_base}.Patch.csproj",
            ]
        )
        entries.append(
            {
                "assemblyName": assembly_name,
                "familyId": family_id,
                "familyExecutionEntries": family_entries,
            }
        )

    _write_solution(solution_path, project_paths)
    return {
        "solutionPath": _relative(repo_root, solution_path),
        "dllCount": len({str(item.get('assemblyName') or '') for item in families}),
        "entries": entries,
    }
