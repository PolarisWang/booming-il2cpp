from __future__ import annotations

from pathlib import Path
from typing import Any


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return str(family_id).replace("/", "-")


def _families_to_lines(assembly_name: str, families: list[dict[str, Any]], *, indent: str = "") -> list[str]:
    """Generate sorted add_subdirectory lines for all families."""
    slugs = []
    for family in families:
        family_id = str(family.get("familyId") or "")
        slugs.append(_family_slug(family_id))

    # Deduplicate and sort
    slugs = sorted(set(slugs))
    return [f"{indent}add_subdirectory({assembly_name}/{slug})" for slug in slugs]


def generate_cmake_solution(
    repo_root: Path,
    *,
    solution_path: Path,
    families: list[dict[str, Any]],
) -> dict[str, Any]:
    """Generate the root solution_native/CMakeLists.txt.

    This aggregates all family targets under a single cmake project.
    """
    # Group families by assembly
    assembly_groups: dict[str, list[dict[str, Any]]] = {}
    for family in families:
        assembly_name = str(family.get("assemblyName") or "")
        if assembly_name not in assembly_groups:
            assembly_groups[assembly_name] = []
        assembly_groups[assembly_name].append(family)

    lines = [
        "cmake_minimum_required(VERSION 3.20)",
        "project(FoundationDllNativeBuild LANGUAGES CXX)",
        "",
        "# ── Foundation DLL family native targets ──",
        "# Each forwarding CMakeLists.txt delegates to the real source locations",
        "# under testing/foundation-dll/<assembly>/<slug>/",
        "",
    ]

    for assembly_name in sorted(assembly_groups):
        family_lines = _families_to_lines(assembly_name, assembly_groups[assembly_name], indent="")
        lines.extend(family_lines)
        lines.append("")

    solution_path.parent.mkdir(parents=True, exist_ok=True)
    solution_path.write_text("\n".join(lines) + "\n", encoding="utf-8")

    return {
        "solutionPath": solution_path.relative_to(repo_root).as_posix(),
        "familyCount": len(families),
    }
