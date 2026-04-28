from __future__ import annotations

from pathlib import Path
from typing import Any


_SOLUTION_RELATIVE_PATH = Path(
    "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln"
)
_KIND_TO_PROJECT = {
    "test": ("test", "Tests.csproj"),
    "native": ("native", "Native.csproj"),
    "benchmark": ("benchmark", "Benchmark.csproj"),
    "host": ("host", "Host.csproj"),
    "patch": ("patch", "Patch.csproj"),
}


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return str(family_id).replace("/", "-")


def _project_basename(family_id: str) -> str:
    slug = _family_slug(family_id)
    return "".join(part[:1].upper() + part[1:] for part in slug.split("-") if part)


def execute_entry(
    repo_root: Path,
    *,
    assembly_name: str,
    family_id: str,
    kind: str,
) -> dict[str, Any]:
    normalized_kind = str(kind or "test").strip().lower()
    if normalized_kind not in _KIND_TO_PROJECT:
        allowed = ", ".join(sorted(_KIND_TO_PROJECT))
        raise ValueError(f"unsupported foundation-dll execution kind '{kind}'. expected one of: {allowed}")
    family_root = repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id)
    project_dir, project_name = _KIND_TO_PROJECT[normalized_kind]
    project_base = _project_basename(family_id)
    actual_project_name = f"{project_base}.{project_name}" if "." in project_name else project_name
    project_path = family_root / project_dir / actual_project_name
    solution_path = repo_root / _SOLUTION_RELATIVE_PATH
    if not project_path.is_file():
        raise FileNotFoundError(project_path.relative_to(repo_root).as_posix())
    if not solution_path.is_file():
        raise FileNotFoundError(solution_path.relative_to(repo_root).as_posix())
    suggested_command = (
        f"dotnet test {project_path.relative_to(repo_root).as_posix()}"
        if normalized_kind in {"test", "benchmark"}
        else f"dotnet build {project_path.relative_to(repo_root).as_posix()}"
    )
    return {
        "entryKind": "family",
        "assemblyName": assembly_name,
        "scope": family_id,
        "kind": normalized_kind,
        "repoRoot": str(repo_root),
        "familyRoot": family_root.relative_to(repo_root).as_posix(),
        "projectPath": project_path.relative_to(repo_root).as_posix(),
        "solutionPath": solution_path.relative_to(repo_root).as_posix(),
        "suggestedCommand": suggested_command,
    }
