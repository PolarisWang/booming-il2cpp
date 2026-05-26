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


def _write_solution(solution_path: Path, project_paths: list[tuple[str, Path, str]]) -> None:
    """Write a .sln file with solution folders.

    Args:
        solution_path: Path to the .sln file.
        project_paths: List of (folder_name, project_path, guid_str) tuples.
            folder_name is the solution folder name (e.g. family slug).
    """
    solution_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "Microsoft Visual Studio Solution File, Format Version 12.00",
        "# Visual Studio Version 17",
        "VisualStudioVersion = 17.0.31903.59",
        "MinimumVisualStudioVersion = 10.0.40219.1",
    ]

    project_guids: list[str] = []
    folder_guids: dict[str, str] = {}
    folder_children: dict[str, list[str]] = {}

    for folder_name, project_path, guid in project_paths:
        relative = _relative(solution_path.parent, project_path)
        project_name = project_path.parent.name  # e.g. "tests", "src", "patch"
        # Use folder_name as prefix if it's not already the default
        if folder_name:
            display_name = f"{folder_name}.{project_name}"
        else:
            display_name = project_name
        lines.append(
            f'Project("{{9A19103F-16F7-4668-BE54-9A1E7A4F7556}}") = "{display_name}", "{relative}", "{{{guid}}}"'
        )
        lines.append("EndProject")
        project_guids.append(guid)

        # Track folder membership
        if folder_name:
            if folder_name not in folder_guids:
                folder_guids[folder_name] = f"{{{guid.replace('{', '').replace('}', '')[:8]}-0000-0000-0000-000000000000}}"
            folder_children.setdefault(folder_name, []).append(guid)

    # Solution folders
    for folder_name in folder_guids:
        folder_guid = folder_guids[folder_name]
        lines.append(
            f'Project("{{2150E333-8FDC-42A3-9474-1A3956D46DE8}}") = "{folder_name}", "{folder_name}", "{folder_guid}"'
        )
        lines.append("EndProject")

    lines.append("Global")
    lines.append("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution")
    lines.append("\t\tDebug|Any CPU = Debug|Any CPU")
    lines.append("\t\tRelease|Any CPU = Release|Any CPU")
    lines.append("\tEndGlobalSection")
    lines.append("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution")
    for guid in project_guids:
        lines.append(f"\t\t{{{guid}}}.Debug|Any CPU.ActiveCfg = Debug|Any CPU")
        lines.append(f"\t\t{{{guid}}}.Debug|Any CPU.Build.0 = Debug|Any CPU")
        lines.append(f"\t\t{{{guid}}}.Release|Any CPU.ActiveCfg = Release|Any CPU")
        lines.append(f"\t\t{{{guid}}}.Release|Any CPU.Build.0 = Release|Any CPU")
    lines.append("\tEndGlobalSection")
    lines.append("\tGlobalSection(SolutionProperties) = preSolution")
    lines.append("\t\tHideSolutionNode = FALSE")
    lines.append("\tEndGlobalSection")

    # NestedProjects section to organize projects into folders
    if folder_children:
        lines.append("\tGlobalSection(NestedProjects) = preSolution")
        for folder_name, child_guids in folder_children.items():
            folder_guid = folder_guids[folder_name]
            for child_guid in child_guids:
                lines.append(f"\t\t{{{child_guid}}} = {folder_guid}")
        lines.append("\tEndGlobalSection")

    lines.append("EndGlobal")
    solution_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def generate_execution_solution(
    repo_root: Path,
    *,
    solution_path: Path,
    families: list[dict[str, Any]],
) -> dict[str, Any]:
    project_paths: list[tuple[str, Path, str]] = []
    entries: list[dict[str, Any]] = []
    guid_index = 1

    for family in families:
        assembly_name = str(family.get("assemblyName") or "")
        family_id = str(family.get("familyId") or "")
        family_slug = _family_slug(family_id)
        project_base = _project_basename(family_id)

        # csproj are in solution/<assembly>/<slug>/
        family_solution_dir = repo_root / "solution" / assembly_name / family_slug
        folder_name = f"{assembly_name}.{family_slug}"

        family_project_entries: dict[str, str] = {}

        for subdir, suffix, key in [
            ("src", "", "src"),
            ("patch", ".Patch", "patch"),
            ("tests", ".Tests", "tests"),
            ("benchmarks", ".Benchmarks", "benchmarks"),
        ]:
            csproj = family_solution_dir / subdir / f"{project_base}{suffix}.csproj"
            guid = f"00000000-0000-0000-0000-{guid_index:012d}"
            guid_index += 1
            project_paths.append((folder_name, csproj, guid))
            family_project_entries[key] = _relative(repo_root, csproj)

        entries.append(
            {
                "assemblyName": assembly_name,
                "familyId": family_id,
                "familyExecutionEntries": family_project_entries,
            }
        )

    _write_solution(solution_path, project_paths)
    return {
        "solutionPath": _relative(repo_root, solution_path),
        "dllCount": len({str(item.get('assemblyName') or '') for item in families}),
        "entries": entries,
    }
