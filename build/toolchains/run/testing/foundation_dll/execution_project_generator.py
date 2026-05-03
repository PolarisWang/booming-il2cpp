from __future__ import annotations

from pathlib import Path


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return str(family_id).replace("/", "-")


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _test_project_template(name: str, project_reference_path: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        "    <IsTestProject>true</IsTestProject>\n"
        f"    <AssemblyName>{name}</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{project_reference_path}\" />\n"
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


def _project_template(name: str, project_reference_path: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{name}</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{project_reference_path}\" />\n"
        "  </ItemGroup>\n"
        "</Project>\n"
    )


def _benchmark_project_template(name: str, project_reference_path: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{name}</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{project_reference_path}\" />\n"
        "  </ItemGroup>\n"
        "  <ItemGroup>\n"
        '    <Compile Remove="ManagedBenchmarkHarness.cs" />\n'
        '    <Compile Remove="BenchmarkManagedBody.cs" />\n'
        "  </ItemGroup>\n"
        "</Project>\n"
    )


def _project_basename(family_id: str) -> str:
    slug = _family_slug(family_id)
    return "".join(part[:1].upper() + part[1:] for part in slug.split("-") if part)


def generate_execution_projects(repo_root: Path, *, assembly_name: str, family_id: str) -> dict[str, object]:
    family_root = repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id)
    project_base = _project_basename(family_id)
    project_reference_path = str(
        Path("..") / ".." / ".." / ".." / ".." / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
    )
    legacy_paths = [
        family_root / "test" / "Tests.csproj",
        family_root / "native" / "Native.csproj",
        family_root / "benchmark" / "Benchmark.csproj",
        family_root / "host" / "Host.csproj",
        family_root / "patch" / "Patch.csproj",
    ]
    paths = [
        family_root / "test" / f"{project_base}.Tests.csproj",
        family_root / "native" / f"{project_base}.Native.csproj",
        family_root / "benchmark" / f"{project_base}.Benchmark.csproj",
        family_root / "host" / f"{project_base}.Host.csproj",
        family_root / "patch" / f"{project_base}.Patch.csproj",
    ]
    for path in legacy_paths:
        if path.is_file():
            path.unlink()
    for path in paths:
        template = _test_project_template if path.parent.name == "test" else _benchmark_project_template if path.parent.name == "benchmark" else _project_template
        _write_text(path, template(path.stem, project_reference_path))
    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": [path.relative_to(repo_root).as_posix() for path in paths],
    }
