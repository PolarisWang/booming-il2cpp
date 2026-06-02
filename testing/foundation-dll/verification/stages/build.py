"""Build stage - AutoTestGenerator -> subjects DLL -> TPG -> entry.exe.

Flow:
  1. Run AutoTestGenerator --all-types --emit-metadata on target DLL
  2. Combine generated .cs files into a single Library project
  3. Build combined project -> unified subjects DLL
  4. Run TPG generate-dll (subjects DLL + metadata -> entry.exe)
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult

_REPO_ROOT = Path(__file__).resolve().parents[4]


# -- Tool paths --------------------------------------------------------------

def _tool_dll(tool_name: str) -> Path:
    return (_REPO_ROOT / "src" / "tools" / tool_name
            / "bin" / "Debug" / "net8.0" / f"{tool_name}.dll")


def _detect_tfm(dll_path: Path) -> str:
    """Detect target framework moniker from the DLL's runtime directory path.

    E.g. ".../shared/Microsoft.NETCore.App/10.0.6/System.Private.CoreLib.dll"
    -> "net10.0".  Falls back to "net8.0".
    """
    path = str(dll_path).replace("\\", "/")
    m = re.search(r"Microsoft\.NETCore\.App/(\d+)\.(\d+)\.", path)
    if m:
        return f"net{m.group(1)}.{m.group(2)}"
    return "net8.0"


def _ensure_tool_built(tool_name: str) -> bool:
    """Build the tool if its DLL doesn't exist."""
    dll = _tool_dll(tool_name)
    if dll.exists():
        return True
    proj = _REPO_ROOT / "src" / "tools" / tool_name / f"{tool_name}.csproj"
    print(f"  [build] Building {tool_name}...")
    result = subprocess.run(
        ["dotnet", "build", str(proj), "-nologo"],
        capture_output=True, text=True, timeout=180)
    if result.returncode != 0:
        print(f"  [build] ERROR: Failed to build {tool_name}: {result.stderr[:300]}")
        return False
    return dll.exists()


def _chaos_sdk_csproj() -> Path:
    """Path to Chaos.TestFramework.Sdk.csproj (used as ProjectReference)."""
    return (_REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk"
            / "Chaos.TestFramework.Sdk.csproj")


def run_build(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Build stage: AutoTestGenerator -> subjects DLL -> TPG -> entry.exe."""
    start = time.perf_counter()
    print(f"  [build] Chunk: {ctx.slug}  Assembly: {ctx.assembly}")
    print(f"  [build] foundation_dir: {ctx.foundation_dir}")

    # -- 0. Ensure chunk directory structure --
    ctx.managed_dir.mkdir(parents=True, exist_ok=True)
    ctx.native_dir.mkdir(parents=True, exist_ok=True)

    # -- 1. Find target DLL --
    dll_candidates = [
        ctx.foundation_dir / f"{ctx.assembly}.dll",
        Path(os.environ.get("DOTNET_ROOT", "C:/Program Files/dotnet/shared/Microsoft.NETCore.App/8.0.11"))
        / f"{ctx.assembly}.dll",
    ]
    runtime_base = Path(os.environ.get("DOTNET_ROOT", "C:/Program Files/dotnet/shared"))
    for runtime_dir in sorted(runtime_base.rglob(f"**/{ctx.assembly}.dll")):
        dll_candidates.insert(0, runtime_dir)

    target_dll: Path | None = None
    for c in dll_candidates:
        if c.exists():
            target_dll = c
            break

    if target_dll is None:
        return StageResult(
            stage="build", status="error",
            summary=f"DLL not found for {ctx.assembly}",
            duration_ms=int((time.perf_counter() - start) * 1000))

    print(f"  [build] Target DLL: {target_dll}")

    # -- 2. Ensure tools are built --
    if not _ensure_tool_built("Chaos.IL2CPP.Tools.AutoTestGenerator"):
        return StageResult(
            stage="build", status="error",
            summary="AutoTestGenerator build failed",
            duration_ms=int((time.perf_counter() - start) * 1000))

    # -- 3. Read chunk definition from namespace-partition.json --
    partition_path = ctx.namespace_partition_path
    if not partition_path.exists():
        return StageResult(
            stage="build", status="error",
            summary=f"namespace-partition.json not found: {partition_path}",
            duration_ms=int((time.perf_counter() - start) * 1000))

    partition = json.loads(partition_path.read_text(encoding="utf-8"))
    chunks = partition.get("chunks", [])
    chunk_def = next((c for c in chunks if c["slug"] == ctx.slug), None)
    if chunk_def is None:
        return StageResult(
            stage="build", status="error",
            summary=f"Chunk '{ctx.slug}' not found in partition",
            duration_ms=int((time.perf_counter() - start) * 1000))

    method_count = chunk_def.get("methodCount", 0)
    print(f"  [build] Chunk methods: {method_count}")

    # -- 4. Run AutoTestGenerator --
    print(f"  [build] Running AutoTestGenerator...")
    auto_dll = _tool_dll("Chaos.IL2CPP.Tools.AutoTestGenerator")
    metadata_path = ctx.subjects_metadata_path
    auto_output = ctx.foundation_dir / ".autogen" / ctx.slug

    cmd = [
        "dotnet", "exec", str(auto_dll),
        "--dll", str(target_dll),
        "--all-types",
        "--output", str(auto_output),
        "--emit-metadata", str(metadata_path),
        "--chunk-slug", ctx.slug,
    ]

    chunk_namespaces = chunk_def.get("namespaces", [])
    if chunk_namespaces:
        ns_filter = ",".join(chunk_namespaces)
        cmd.extend(["--namespace-filter", ns_filter])
        print(f"  [build] Namespace filter: {ns_filter}")
    if ctx.skip_probe:
        cmd.append("--skip-probe")
        print(f"  [build] Probe phase skipped (--skip-probe)")
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=1200)

    if result.returncode != 0:
        print(f"  [build] AutoTestGenerator FAILED (rc={result.returncode})")
        for line in (result.stderr.splitlines() + result.stdout.splitlines())[-15:]:
            print(f"      {line}")
        return StageResult(
            stage="build", status="error",
            summary=f"AutoTestGenerator failed (rc={result.returncode})",
            duration_ms=int((time.perf_counter() - start) * 1000))

    print(f"  [build] AutoTestGenerator output: {auto_output}")

    if not metadata_path.exists():
        print(f"  [build] WARNING: Metadata not emitted (all probes may have failed)")
        return StageResult(
            stage="build", status="error",
            summary="No subjects metadata generated",
            duration_ms=int((time.perf_counter() - start) * 1000))

    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    total_subjects = metadata.get("totalMethods", 0)
    print(f"  [build] Subjects in metadata: {total_subjects}")

    if total_subjects == 0:
        return StageResult(
            stage="build", status="skipped",
            summary="0 subjects - nothing to build",
            duration_ms=int((time.perf_counter() - start) * 1000))

    # -- 5. Detect TFM from target DLL --
    tfm = _detect_tfm(target_dll)
    print(f"  [build] Target framework: {tfm}")

    # -- 6. Combine generated .cs files into a single Library project --
    print(f"  [build] Creating combined subjects DLL...")
    subjects_dll = ctx.subjects_dll_path
    subjects_dll.parent.mkdir(parents=True, exist_ok=True)

    all_cs_files = list(auto_output.rglob("*.AutoGenerated.cs"))

    if not all_cs_files:
        return StageResult(
            stage="build", status="error",
            summary="No generated .cs files found for combined build",
            duration_ms=int((time.perf_counter() - start) * 1000))

    print(f"  [build]   {len(all_cs_files)} source files to combine")
    combined_dir = ctx.chunk_dir / "managed" / "combined"
    combined_dir.mkdir(parents=True, exist_ok=True)

    combined_cs_path = combined_dir / "CombinedSubjects.cs"
    all_usings: set[str] = set()
    namespace_blocks: list[str] = []
    for cs_file in sorted(all_cs_files):
        text = cs_file.read_text(encoding="utf-8")
        ns_idx = text.find("\nnamespace ")
        if ns_idx < 0:
            # Fallback: whole file if no namespace
            namespace_blocks.append(text)
        else:
            # Extract using directives from the preamble (before namespace)
            preamble = text[:ns_idx]
            for line in preamble.splitlines():
                stripped = line.strip()
                if stripped.startswith("using ") and stripped.endswith(";"):
                    all_usings.add(stripped)
            # Keep from namespace onward
            namespace_blocks.append(text[ns_idx + 1:])

    with open(combined_cs_path, "w", encoding="utf-8") as f:
        f.write("// Auto-generated combined subjects file\n")
        f.write("// Combined from all per-type AutoTestGenerator outputs\n\n")
        for u in sorted(all_usings):
            f.write(u + "\n")
        f.write("\n")
        for block in namespace_blocks:
            f.write(block)
            f.write("\n\n")

    sdk_csproj = _chaos_sdk_csproj()
    combined_csproj = combined_dir / "CombinedSubjects.csproj"
    combined_csproj.write_text(
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <OutputType>Library</OutputType>\n"
        f"    <TargetFramework>{tfm}</TargetFramework>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>\n"
        "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <Compile Include=\"{combined_cs_path.name}\" />\n"
        f"    <ProjectReference Include=\"{sdk_csproj}\" />\n"
        "  </ItemGroup>\n"
        "</Project>\n"
    )

    print(f"  [build] Building combined project ({tfm})...")
    build_result = subprocess.run(
        ["dotnet", "build", str(combined_csproj),
         f"-p:OutDir={subjects_dll.parent}",
         "-p:ImportDirectoryBuildProps=false",
         "--nologo", "-v", "quiet"],
        capture_output=True, text=True, timeout=120)

    if build_result.returncode != 0 or not subjects_dll.exists():
        print(f"  [build] WARNING: Combined build failed for {tfm}")
        for line in (build_result.stderr.splitlines() + build_result.stdout.splitlines())[-20:]:
            print(f"      {line}")
        # Try net8.0 fallback when TFM is not net8.0
        if tfm != "net8.0":
            print(f"  [build] Retrying with net8.0 fallback...")
            combined_csproj.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
                "  <PropertyGroup>\n"
                "    <OutputType>Library</OutputType>\n"
                "    <TargetFramework>net8.0</TargetFramework>\n"
                "    <ImplicitUsings>enable</ImplicitUsings>\n"
                "    <Nullable>enable</Nullable>\n"
                "    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>\n"
                "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
                "  </PropertyGroup>\n"
                "  <ItemGroup>\n"
                f"    <Compile Include=\"{combined_cs_path.name}\" />\n"
                f"    <ProjectReference Include=\"{sdk_csproj}\" />\n"
                "  </ItemGroup>\n"
                "</Project>\n"
            )
            build_result = subprocess.run(
                ["dotnet", "build", str(combined_csproj),
                 f"-p:OutDir={subjects_dll.parent}",
                 "--nologo", "-v", "quiet"],
                capture_output=True, text=True, timeout=120)

        if build_result.returncode != 0 or not subjects_dll.exists():
            print(f"  [build] Combined build FAILED after fallback")
            for line in (build_result.stderr.splitlines() + build_result.stdout.splitlines())[-15:]:
                print(f"      {line}")
            return StageResult(
                stage="build", status="error",
                summary="Combined subjects DLL build failed",
                details={"buildErrors": build_result.stderr[:500]},
                duration_ms=int((time.perf_counter() - start) * 1000))

    print(f"  [build] Subjects DLL: {subjects_dll} ({subjects_dll.stat().st_size} bytes)")

    # -- 7. Run TPG generate-dll --
    if not _ensure_tool_built("Chaos.IL2CPP.Tools.TestProjectGenerator"):
        return StageResult(
            stage="build", status="error",
            summary="TPG build failed",
            duration_ms=int((time.perf_counter() - start) * 1000))

    tpg_dll = _tool_dll("Chaos.IL2CPP.Tools.TestProjectGenerator")
    print(f"  [build] Running TPG generate-dll...")

    tpg_cmd = [
        "dotnet", "exec", str(tpg_dll),
        "generate-dll",
        "--dll", str(subjects_dll),
        "--metadata", str(metadata_path),
        "--output", str(ctx.native_dir),
        "--clean",
    ]
    tpg_result = subprocess.run(tpg_cmd, capture_output=True, text=True, timeout=1800)

    for line in tpg_result.stdout.splitlines():
        print(f"      {line}")

    if tpg_result.returncode != 0:
        print(f"  [build] TPG generate-dll FAILED (rc={tpg_result.returncode})")
        for line in (tpg_result.stderr.splitlines() + tpg_result.stdout.splitlines())[-15:]:
            print(f"      {line}")
        return StageResult(
            stage="build", status="error",
            summary=f"TPG generate-dll failed (rc={tpg_result.returncode})",
            duration_ms=int((time.perf_counter() - start) * 1000))

    entry_exe = ctx.entry_exe_path
    if not entry_exe.exists():
        return StageResult(
            stage="build", status="error",
            summary=f"entry.exe not produced at {entry_exe}",
            duration_ms=int((time.perf_counter() - start) * 1000))

    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [build] entry.exe: {entry_exe}")
    print(f"  [build] Done ({duration_ms}ms)")

    return StageResult(
        stage="build", status="passed",
        summary=f"{total_subjects} subjects -> entry.exe ({duration_ms}ms)",
        details={
            "chunkSlug": ctx.slug,
            "totalSubjects": total_subjects,
            "tfm": tfm,
            "durationMs": duration_ms,
        },
        duration_ms=duration_ms)
