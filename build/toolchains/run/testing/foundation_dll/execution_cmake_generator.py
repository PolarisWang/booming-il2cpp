from __future__ import annotations

import os
from pathlib import Path


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return str(family_id).replace("/", "-")


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _relative_from(base: Path, target: Path) -> str:
    """Compute relative path from base to target, using forward slashes."""
    return os.path.relpath(target.resolve(), start=base.resolve()).replace("\\", "/")


def _forwarding_cmakelists(family_slug: str) -> str:
    """Generate a forwarding CMakeLists.txt that delegates to actual source locations."""
    # CMAKE_SOURCE_DIR is solution_native/ (the root of the cmake project).
    # From there, the family dir is ../System.Private.CoreLib/<slug>/.
    return (
        f"# ── Foundation DLL: {family_slug} ──\n"
        f"# Forward to actual source locations in verification/\n"
        "\n"
        "# Native AOT generated code (il2cpp_dist/)\n"
        f"add_subdirectory(\n"
        f"    ${{CMAKE_SOURCE_DIR}}/../System.Private.CoreLib/{family_slug}/il2cpp_dist\n"
        f"    ${{CMAKE_CURRENT_BINARY_DIR}}/il2cpp_dist\n"
        ")\n"
        "\n"
        "# Benchmark native entry (if exists)\n"
        f"if(EXISTS ${{CMAKE_SOURCE_DIR}}/../System.Private.CoreLib/{family_slug}/native_test/benchmark/CMakeLists.txt)\n"
        "    add_subdirectory(\n"
        f"        ${{CMAKE_SOURCE_DIR}}/../System.Private.CoreLib/{family_slug}/native_test/benchmark\n"
        "        ${CMAKE_CURRENT_BINARY_DIR}/benchmark\n"
        "    )\n"
        "endif()\n"
        "\n"
        "# Hotupdate native skeleton (if exists)\n"
        f"if(EXISTS ${{CMAKE_SOURCE_DIR}}/../System.Private.CoreLib/{family_slug}/native_test/hotupdate/CMakeLists.txt)\n"
        "    add_subdirectory(\n"
        f"        ${{CMAKE_SOURCE_DIR}}/../System.Private.CoreLib/{family_slug}/native_test/hotupdate\n"
        "        ${CMAKE_CURRENT_BINARY_DIR}/hotupdate\n"
        "    )\n"
        "endif()\n"
    )


def generate_cmake_projects(repo_root: Path, *, assembly_name: str, family_id: str) -> dict[str, object]:
    """Generate per-family forwarding CMakeLists.txt under solution_native/.

    Each CMakeLists.txt uses add_subdirectory() to forward to the real source
    locations (il2cpp_dist/, native_test/benchmark/, native_test/hotupdate/)
    under verification/foundation-dll/<assembly>/<slug>/.
    """
    family_slug = _family_slug(family_id)
    solution_native_base = repo_root / "solution_native"

    family_solution_dir = solution_native_base / assembly_name / family_slug

    cmake_text = _forwarding_cmakelists(family_slug)
    cmake_path = family_solution_dir / "CMakeLists.txt"
    _write_text(cmake_path, cmake_text)

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": [cmake_path.relative_to(repo_root).as_posix()],
    }
