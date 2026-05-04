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


def _project_basename(family_id: str) -> str:
    slug = _family_slug(family_id)
    return "".join(part[:1].upper() + part[1:] for part in slug.split("-") if part)


def _relative_from(base: Path, target: Path) -> str:
    """Compute relative path from base to target, using forward slashes."""
    return os.path.relpath(target.resolve(), start=base.resolve()).replace("\\", "/")


def _compile_glob(base: Path, target_dir: Path, pattern: str) -> str:
    """Compute relative glob path from base to target_dir/pattern."""
    return _relative_from(base, target_dir) + "/" + pattern


def _src_project_template(name: str, ref_relative: str, src_glob: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{name}</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{ref_relative}\" />\n"
        "  </ItemGroup>\n"
        "  <ItemGroup>\n"
        f'    <Compile Include="{src_glob}" />\n'
        "  </ItemGroup>\n"
        "</Project>\n"
    )


def _patch_project_template(name: str, src_glob: str, ref_relative: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{name}Patch</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{ref_relative}\" />\n"
        "  </ItemGroup>\n"
        "  <ItemGroup>\n"
        f'    <Compile Include="{src_glob}" />\n'
        "  </ItemGroup>\n"
        "</Project>\n"
    )


def _tests_project_template(name: str, src_ref_relative: str, src_glob: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <OutputType>Exe</OutputType>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{name}Tests</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{src_ref_relative}\" />\n"
        "  </ItemGroup>\n"
        "  <ItemGroup>\n"
        f'    <Compile Include="{src_glob}" />\n'
        "  </ItemGroup>\n"
        "  <ItemGroup>\n"
        "    <PackageReference Include=\"Microsoft.NET.Test.Sdk\" Version=\"17.11.1\" />\n"
        "    <PackageReference Include=\"xunit\" Version=\"2.9.0\" />\n"
        "    <PackageReference Include=\"xunit.runner.visualstudio\" Version=\"2.8.2\">\n"
        "      <PrivateAssets>all</PrivateAssets>\n"
        "      <IncludeAssets>runtime; build; native; contentfiles; analyzers; buildtransitive</IncludeAssets>\n"
        "    </PackageReference>\n"
        "  </ItemGroup>\n"
        "</Project>\n"
    )


def _benchmarks_project_template(name: str, src_ref_relative: str, src_glob: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <OutputType>Exe</OutputType>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{name}Benchmarks</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{src_ref_relative}\" />\n"
        "  </ItemGroup>\n"
        "  <ItemGroup>\n"
        f'    <Compile Include="{src_glob}" />\n'
        "  </ItemGroup>\n"
        "</Project>\n"
    )


def generate_execution_projects(repo_root: Path, *, assembly_name: str, family_id: str) -> dict[str, object]:
    family_slug = _family_slug(family_id)
    project_base = _project_basename(family_id)
    solution_base = repo_root / "solution"

    # Solution directories for this family's csproj files
    family_solution_dir = solution_base / assembly_name / family_slug

    # Source files are in verification/foundation-dll/<assembly>/<slug>/
    family_verification_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug
    src_dir = family_verification_root / "src"
    patch_dir = family_verification_root / "src" / "patch"
    tests_dir = family_verification_root / "managed_test" / "tests"
    benchmarks_dir = family_verification_root / "managed_test" / "benchmarks"

    # Reference: src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj
    ref_csproj = repo_root / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
    src_ref_relative = _relative_from(family_solution_dir / "src", ref_csproj)

    # Source csproj path (used for tests/benchmarks ProjectReference)
    src_csproj = family_solution_dir / "src" / f"{project_base}.csproj"
    tests_src_ref = _relative_from(family_solution_dir / "tests", src_csproj)
    benchmarks_src_ref = _relative_from(family_solution_dir / "benchmarks", src_csproj)

    # Compute glob paths for source files
    src_glob = _compile_glob(family_solution_dir / "src", src_dir, "*.cs")
    patch_glob = _compile_glob(family_solution_dir / "patch", patch_dir, "*.cs")
    tests_glob = _compile_glob(family_solution_dir / "tests", tests_dir, "*.cs")
    benchmarks_glob = _compile_glob(family_solution_dir / "benchmarks", benchmarks_dir, "*.cs")

    paths: list[Path] = []

    # 1. src project (Library) — references Chaos.TestFramework.Sdk
    src_path = family_solution_dir / "src" / f"{project_base}.csproj"
    _write_text(src_path, _src_project_template(project_base, src_ref_relative, src_glob))
    paths.append(src_path)

    # 2. patch project (Library, references Chaos.TestFramework.Sdk for attributes)
    patch_path = family_solution_dir / "patch" / f"{project_base}.Patch.csproj"
    _write_text(patch_path, _patch_project_template(project_base, patch_glob, src_ref_relative))
    paths.append(patch_path)

    # 3. tests project (Exe + xunit) — references src project
    tests_path = family_solution_dir / "tests" / f"{project_base}.Tests.csproj"
    _write_text(tests_path, _tests_project_template(project_base, tests_src_ref, tests_glob))
    paths.append(tests_path)

    # 4. benchmarks project (Exe) — references src project
    benchmarks_path = family_solution_dir / "benchmarks" / f"{project_base}.Benchmarks.csproj"
    _write_text(benchmarks_path, _benchmarks_project_template(project_base, benchmarks_src_ref, benchmarks_glob))
    paths.append(benchmarks_path)

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": [p.relative_to(repo_root).as_posix() for p in paths],
    }
