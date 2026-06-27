"""Build stage - AutoTestGenerator -> subjects DLL -> TPG -> entry.exe.

Flow:
  1. Run AutoTestGenerator --all-types --emit-metadata on target DLL
  2. Combine generated .cs files into a single Library project
  3. Build combined project -> unified subjects DLL
  4. Run TPG generate-dll (subjects DLL + metadata -> entry.exe)

Fallback (custom-only chunk): when AutoTestGenerator produces no metadata
  but custom .cs files exist in managed/subjects/, compile those directly
  into the subjects DLL, generate metadata from the DLL, and proceed to TPG.
"""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult

from verification.stages.hephaestus_cache import HephaestusCache, compute_input_hash, compute_context_fingerprint

# Ensure testing/ is on sys.path so _pipeline.tool_helpers can be imported
_TESTING = str(Path(__file__).resolve().parents[3])
if _TESTING not in sys.path:
    sys.path.insert(0, _TESTING)

from _pipeline.tool_helpers import tool_dll, ensure_tool_built, detect_tfm

_REPO_ROOT = Path(__file__).resolve().parents[4]

# Platform-aware SDK paths for dependency tracking
import platform as _platform
_is_linux = _platform.system() == "Linux"
_sdk_preset = "linux-x64-profile" if _is_linux else "windows-x64-reference"
_sdk_lib_ext = ".a" if _is_linux else ".lib"
_sdk_config = "RelWithDebInfo"


# -- Profile mode injection ---------------------------------------------------

_PROFILE_MODE_INCLUDE = '#include <profile_stats.h>'

_PROFILE_MODE_FUNC = """
// ── --profile: per-method GC/allocation/code-size profile ───────────
static int RunProfileMode() {
    using namespace chaos::il2cpp::runtime_core;
    const int kCount = kSubjectEntryCount;
    ProfileStoreInit(kCount);
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t heap_before = chaos_gc_get_heap_size();
        GetThreadProfileData().heap_before = heap_before;
        CHAOS_EH_TRY
            ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
        int64_t heap_after = chaos_gc_get_heap_size();
        GetThreadProfileData().heap_after = heap_after;
        FlushThreadProfileData(i);
    }
    ProfileStoreFinalize();
    ProfileEmitJson();
    return 0;
}

"""

_PROFILE_MODE_CLI = '    if (std::strcmp(argv[1], "--profile") == 0) { ret = RunProfileMode(); goto shutdown; }\n'


def _inject_profile_mode(native_dir: Path) -> None:
    """Inject --profile mode into runtime-entry.cpp after TPG generation.

    The TPG generates runtime-entry.cpp without --profile support even though
    the native infrastructure (profile_stats.h, ProfileStoreInit, ProfileEmitJson)
    is compiled in when CHAOS_IL2CPP_CONFIG_TIER=profile. This function patches
    the generated file to add the missing RunProfileMode() function and CLI handler.
    """
    entry_path = native_dir / "runtime-entry.cpp"
    if not entry_path.exists():
        print(f"  [build] [profile-inject] runtime-entry.cpp not found at {entry_path}, skipping")
        return

    content = entry_path.read_text(encoding="utf-8", errors="replace")

    # 1. Inject #include <profile_stats.h> after last #include <chaos/...>
    if '#include <profile_stats.h>' not in content:
        content = content.replace(
            '#include <gc/gc_api.h>',
            '#include <gc/gc_api.h>\n#include <profile_stats.h>')
        print(f"  [build] [profile-inject] added #include <profile_stats.h>")

    # 2. Inject RunProfileMode() before int main()
    if 'RunProfileMode' not in content:
        content = content.replace(
            'int main(int argc, char* argv[]) {',
            _PROFILE_MODE_FUNC + 'int main(int argc, char* argv[]) {')
        print(f"  [build] [profile-inject] added RunProfileMode()")

    # 3. Inject --profile CLI handler before Unknown flag
    if '--profile' not in content:
        content = content.replace(
            'printf("Unknown flag: %s\\n", argv[1]);',
            '    if (std::strcmp(argv[1], "--profile") == 0) { ret = RunProfileMode(); goto shutdown; }\n    printf("Unknown flag: %s\\n", argv[1]);')
        print(f"  [build] [profile-inject] added --profile CLI handler")

    entry_path.write_text(content, encoding="utf-8")

def _chaos_sdk_csproj() -> Path:
    """Path to Chaos.TestFramework.Sdk.csproj."""
    return (_REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk"
            / "Chaos.TestFramework.Sdk.csproj")


def _load_chunk_config(chunk_dir: Path) -> dict[str, Any]:
    """Load chunk.json config, returning {} if missing or corrupt."""
    config_path = chunk_dir / "chunk.json"
    if config_path.exists():
        try:
            return json.loads(config_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            print(f"  [build] WARNING: corrupt chunk.json at {config_path}")
    return {}


def _get_additional_assemblies(chunk_dir: Path) -> list[str]:
    """Get additional assemblies for codegen from chunk config."""
    config = _load_chunk_config(chunk_dir)
    return config.get("additionalAssemblies", [])


def _detect_tfm(dll_path: Path) -> str:
    """Detect target framework moniker from the DLL's runtime directory path.

    E.g. ".../shared/Microsoft.NETCore.App/10.0.6/System.Private.CoreLib.dll"
    -> "net10.0".  Falls back to "net8.0".

    Also handles custom DLL paths like ".../dotnet-foundation/net10.0/runtime/...".
    """
    path = str(dll_path).replace("\\", "/")
    # Primary: standard runtime layout
    m = re.search(r"Microsoft\.NETCore\.App/(\d+)\.(\d+)\.", path)
    if m:
        return f"net{m.group(1)}.{m.group(2)}"
    # Fallback: custom layout like dotnet-foundation/netX.Y/runtime/
    m = re.search(r"/net(\d+)\.(\d+)/runtime/", path)
    if m:
        return f"net{m.group(1)}.{m.group(2)}"
    return "net10.0"


def _custom_subjects_metadata(
    method_info: list[tuple[str, str, str]],  # [(methodName, returnType, typeName)]
    slug: str,
) -> dict:
    """Build metadata.json for custom subject methods.

    Each method gets a metadata entry referencing the wrapper in the
    AutoGenerated namespace (matching TPG generate-dll's expected format).
    The typeName is the full class path within the CombinedSubjects assembly
    (e.g. "AutoGenerated.GcStress.GcStressSubjects").
    """
    methods: list[dict[str, Any]] = []
    benchmark_indices: list[int] = []
    for idx, (asm_component, type_path, method_name, ret_type) in enumerate(method_info):
        # Build SubjectId matching TPG's expected format:
        #   CombinedSubjects/AutoGenerated.{asm}.{type}::{method}:{retType}()
        # Where asm = first namespace component after AutoGenerated., matching the C# wrapper namespace.
        # TPG reconstructs SubjectId for aotSubjectIds as:
        #   CombinedSubjects/AutoGenerated.{SanitizeIdentifier(asm)}.{SanitizeIdentifier(type)}Tests::{genId}:retType()
        # This must match the codegen's SubjectId (derived from the C# class AutoGenerated.{asm}.{type}Tests).
        subject_id = (
            f"CombinedSubjects/AutoGenerated.{asm_component}.{type_path}::{method_name}:{ret_type}()"
        )
        kind = "benchmark"
        benchmark_indices.append(idx)
        methods.append({
            "index": idx,
            "kind": kind,
            "methodSubjectId": subject_id,
        })

    return {
        "schemaVersion": 1,
        "assemblyName": "CombinedSubjects",
        "chunkSlug": slug,
        "totalMethods": len(methods),
        "customEntryIndices": list(range(len(methods))),
        "benchmarkMethodIndices": benchmark_indices,
        "hotupdateMethodIndices": [],
        "methods": methods,
    }


def _generate_metadata_from_subjects_cs(combined_cs_path: Path, slug: str) -> dict | None:
    """Parse CombinedSubjects.cs to extract AutoGenerated wrapper metadata.

    Scans for patterns like:
        namespace AutoGenerated.{asm_component}
        {
            public unsafe partial class {type_path}
            {
                public long|void|int|bool|string|object {method_name}(...)

    Generates metadata.json in the same format as _custom_subjects_metadata.
    """
    if not combined_cs_path.exists():
        return None

    text = combined_cs_path.read_text(encoding="utf-8")

    # Parse namespace + class + methods using regex
    method_info: list[tuple[str, str, str, str]] = []  # [(asm_component, type_path, method_name, ret_type)]

    # Match: namespace AutoGenerated.{asm_component}
    ns_pattern = re.compile(r'namespace AutoGenerated\.(\S+)')
    # Match: public unsafe partial class {type_path}
    class_pattern = re.compile(r'public\s+unsafe\s+partial\s+class\s+(\S+)')
    # Match: public (long|void|int|bool|string|object) {method_name}(
    method_pattern = re.compile(r'public\s+(long|void|int|bool|string|object)\s+(\S+)\s*\(')

    current_asm = ""
    current_class = ""
    for line in text.splitlines():
        ns_match = ns_pattern.search(line)
        if ns_match:
            current_asm = ns_match.group(1)
            continue
        class_match = class_pattern.search(line)
        if class_match:
            current_class = class_match.group(1)
            continue
        method_match = method_pattern.search(line)
        if method_match and current_asm and current_class:
            ret_type_cs = method_match.group(1)
            method_name = method_match.group(2)

            # Map C# return type to metadata return type
            _ret_map = {
                "long": "System.Int64",
                "void": "System.Void",
                "int": "System.Int32",
                "bool": "System.Boolean",
                "string": "System.String",
                "object": "System.Object",
            }
            ret_type = _ret_map.get(ret_type_cs, "System.Int64")
            method_info.append((current_asm, current_class, method_name, ret_type))

    if not method_info:
        print(f"  [build] WARNING: No AutoGenerated methods found in {combined_cs_path}")
        return None

    print(f"  [build] Parsed {len(method_info)} methods from {combined_cs_path.name}")
    return _custom_subjects_metadata(method_info, slug)


def _compile_custom_subjects(
    custom_cs_files: list[Path],
    subjects_dll: Path,
    tfm: str,
    sdk_csproj: Path,
) -> list[tuple[str, str, str, str]] | None:
    """Compile custom subject .cs files into CombinedSubjects.dll.

    Delegates C# wrapper generation to AutoTestGenerator --generate-wrappers,
    then builds the generated CombinedSubjects.csproj into the subjects DLL.

    Returns list of (asmComponent, typePath, methodName, metadataRetType) tuples
    on success, or None on failure.
    """
    atg_dll = tool_dll("Chaos.IL2CPP.Tools.AutoTestGenerator")
    if not atg_dll.exists():
        print("  [build] ATG DLL not found, building...")
        ensure_tool_built("Chaos.IL2CPP.Tools.AutoTestGenerator")
        if not atg_dll.exists():
            print("  [build] ERROR: AutoTestGenerator DLL not found after build")
            return None

    output_dir = subjects_dll.parent.parent / "combined"
    output_dir.mkdir(parents=True, exist_ok=True)

    # Build args for ATG --generate-wrappers
    args = [
        "dotnet", "exec", str(atg_dll),
        "--generate-wrappers",
        "--custom-cs-files", ";".join(str(f) for f in sorted(custom_cs_files)),
        "--output", str(output_dir),
        "--slug", subjects_dll.parent.parent.parent.name,
        "--sdk-csproj", os.path.relpath(sdk_csproj, output_dir),
        "--tfm", tfm,
    ]

    print(f"  [build] Running ATG --generate-wrappers...")
    result = subprocess.run(args, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=60)

    if result.returncode != 0:
        print(f"  [build] ATG --generate-wrappers FAILED (rc={result.returncode})")
        for line in (result.stderr.splitlines() + result.stdout.splitlines())[-15:]:
            print(f"      {line}")
        return None

    # Print stdout lines for visibility
    for line in result.stdout.splitlines():
        print(f"      {line}")

    # Read metadata generated by ATG
    metadata_path = output_dir / "metadata.json"
    if not metadata_path.exists():
        print(f"  [build] ATG --generate-wrappers did not produce metadata.json")
        return None

    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    method_info: list[tuple[str, str, str, str]] = []
    for m in metadata.get("methods", []):
        # Parse methodSubjectId to extract asmComponent, typePath, methodName, retType
        # Format: CombinedSubjects/AutoGenerated.{asm}.{typePath}::{methodName}:{retType}()
        subject_id = m.get("methodSubjectId", "")
        # Split on "::" to separate type path from method
        parts = subject_id.split("::", 1)
        if len(parts) != 2:
            continue
        type_part = parts[0]
        method_part = parts[1]

        # Extract namespace component (after AutoGenerated.)
        type_parts = type_part.split("AutoGenerated.", 1)
        if len(type_parts) != 2:
            continue
        ns_and_cls = type_parts[1]
        # Split on last dot to get asmComponent and typePath
        dot_idx = ns_and_cls.rfind(".")
        asm_component = ns_and_cls[:dot_idx] if dot_idx >= 0 else ns_and_cls
        type_path = ns_and_cls[dot_idx + 1:] if dot_idx >= 0 else ""

        # Extract methodName and retType from method_part
        # Format: "Subject_0:System.Int64()"
        method_parts = method_part.rsplit(":", 1)
        if len(method_parts) != 2:
            continue
        method_name = method_parts[0]
        ret_type = method_parts[1].rstrip(")")

        method_info.append((asm_component, type_path, method_name, ret_type))

    if not method_info:
        print(f"  [build] No methods found in generated metadata")
        return None

    # Build the generated project
    combined_csproj = output_dir / "CombinedSubjects.csproj"
    if not combined_csproj.exists():
        print(f"  [build] CombinedSubjects.csproj not found at {combined_csproj}")
        return None

    print(f"  [build] Building custom subjects project ({tfm})...")
    build_result = subprocess.run(
        ["dotnet", "build", str(combined_csproj),
         f"-p:OutDir={subjects_dll.parent}",
         "-p:ImportDirectoryBuildProps=false",
         "--nologo", "-v", "quiet"],
        capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=120)

    if build_result.returncode != 0 or not subjects_dll.exists():
        print(f"  [build] Custom subjects build FAILED for {tfm}")
        for line in (build_result.stderr.splitlines() + build_result.stdout.splitlines())[-15:]:
            print(f"      {line}")
        if tfm != "net10.0":
            print(f"  [build] Retrying with net10.0 fallback (original TFM={tfm} failed)")
            return _compile_custom_subjects(
                custom_cs_files, subjects_dll, "net10.0", sdk_csproj)
        return None

    return method_info


def _merge_supplemental_coverage(metadata_path: Path, chunk_dir: Path) -> None:
    """Merge supplemental coverage entries into subjects.metadata.json.

    Reads supplemental-coverage.json from the chunk's managed/subjects/
    directory and appends any entries whose methodSubjectId is not already
    present. This provides AOT-compilation coverage for methods the
    AutoTestGenerator cannot probe (delegate Invoke, internal classes, etc.).

    The file is a no-op (no changes made) when supplemental-coverage.json
    does not exist — making this safe for all chunks regardless of whether
    they have supplemental entries.
    """
    supplemental_path = chunk_dir / "managed" / "subjects" / "supplemental-coverage.json"
    if not supplemental_path.exists():
        return

    print(f"  [build] Found supplemental coverage: {supplemental_path.name}")
    supp_data = json.loads(supplemental_path.read_text(encoding="utf-8"))
    supp_entries = supp_data.get("entries", [])
    if not supp_entries:
        return

    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    existing_ids = {m["methodSubjectId"] for m in metadata.get("methods", [])}

    new_entries = []
    for entry in supp_entries:
        sid = entry["methodSubjectId"]
        if sid in existing_ids:
            continue
        new_index = metadata["totalMethods"] + len(new_entries)
        new_entries.append({
            "index": new_index,
            "kind": "aot-coverage",
            "methodSubjectId": sid,
            "generatedMethodId": "",
        })

    if not new_entries:
        print(f"  [build]   All {len(supp_entries)} supplemental entries already in metadata")
        return

    metadata["methods"].extend(new_entries)
    metadata["totalMethods"] += len(new_entries)
    # NOTE: Do NOT add to benchmarkMethodIndices or hotupdateMethodIndices.
    # These aot-coverage entries have no corresponding test/benchmark methods
    # in CombinedSubjects.dll and must not produce dispatch table entries.

    metadata_path.write_text(json.dumps(metadata, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"  [build]   Merged {len(new_entries)}/{len(supp_entries)} supplemental coverage entries "
          f"(total methods now {metadata['totalMethods']})")


def _build_jit_entry(
    tpg_dll: Path,
    subjects_dll: Path,
    metadata_path: Path,
    native_dir: Path,
    native_config: str = "check",
    assembly_dirs: list[str] | None = None,
) -> bool:
    """Build JIT entry-jit.exe via TPG generate-dll --jit.

    Returns True if JIT build succeeded, False otherwise.
    JIT build failure does not block the pipeline.
    """
    # Use a separate output directory so JIT codegen doesn't clobber AOT artifacts
    jit_output = native_dir.parent / "build_jit_output"
    jit_output.mkdir(parents=True, exist_ok=True)

    cmd = [
        "dotnet", "exec", str(tpg_dll),
        "generate-dll",
        "--jit",
        "--dll", str(subjects_dll),
        "--metadata", str(metadata_path),
        "--output", str(jit_output),
        "--config-tier", native_config,
        "--clean",
    ]
    if assembly_dirs:
        for ad in assembly_dirs:
            cmd.extend(["--assembly-dir", ad])
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=7200)
    except subprocess.TimeoutExpired:
        print(f"  [build] JIT entry build TIMEOUT — continuing")
        return False

    for line in result.stdout.splitlines():
        print(f"      [jit] {line}")

    if result.returncode != 0:
        print(f"  [build] JIT entry build FAILED (rc={result.returncode}) — continuing")
        for line in result.stderr.splitlines():
            print(f"      [jit:err] {line}")
        return False

    jit_exe = jit_output / "entry-jit.exe"
    if not jit_exe.exists():
        print(f"  [build] JIT entry-jit.exe not found at {jit_exe} — continuing")
        return False

    shutil.copy2(jit_exe, native_dir / "entry-jit.exe")
    print(f"  [build] JIT entry-jit.exe: {native_dir / 'entry-jit.exe'} ({jit_exe.stat().st_size} bytes)")

    # Copy JIT data file (aot-core-ir.jdata) alongside entry-jit.exe
    # so ChaosJitDataLoad can find it at runtime via relative path.
    jdata_src = jit_output / "codegen" / "generated" / "aot-core-ir.jdata"
    if jdata_src.exists():
        shutil.copy2(jdata_src, native_dir / "aot-core-ir.jdata")
        print(f"  [build] JIT data: {native_dir / 'aot-core-ir.jdata'} ({jdata_src.stat().st_size} bytes)")
    else:
        print(f"  [build] JIT data not found at {jdata_src} — continuing")
    return True


def _build_jit_entry_fast(
    tpg_dll: Path,
    subjects_dll: Path,
    metadata_path: Path,
    native_dir: Path,
    native_config: str = "check",
    assembly_dirs: list[str] | None = None,
) -> bool:
    """Build JIT entry-jit.exe if stale or missing, skip via mtime check otherwise.

    Checks if entry-jit.exe already exists and is newer than the subjects DLL
    and metadata. If so, skips the expensive TPG codegen + cmake build.
    Returns True if JIT entry is available (built or already current).
    """
    jit_exe = native_dir / "entry-jit.exe"
    if jit_exe.exists():
        jit_mtime = jit_exe.stat().st_mtime
        deps = [subjects_dll, metadata_path]
        if all(d.exists() and jit_mtime >= d.stat().st_mtime for d in deps):
            print(f"  [build] [jit] entry-jit.exe is current ({jit_exe.stat().st_size} bytes)")
            return True
        print(f"  [build] [jit] entry-jit.exe exists but stale — rebuilding")
    return _build_jit_entry(tpg_dll, subjects_dll, metadata_path, native_dir,
                            native_config, assembly_dirs)


def run_build(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Build stage: AutoTestGenerator -> subjects DLL -> TPG -> entry.exe."""
    start = time.perf_counter()
    print(f"  [build] Chunk: {ctx.slug}  Assembly: {ctx.assembly}")

    print(f"  [build] foundation_dir: {ctx.foundation_dir}")

    # -- 0. Ensure chunk directory structure --
    ctx.managed_dir.mkdir(parents=True, exist_ok=True)
    ctx.native_dir.mkdir(parents=True, exist_ok=True)

    # -- 1. Find target DLL --
    # Auto-detect DOTNET_ROOT if not set, via dotnet --info (cross-platform)
    if "DOTNET_ROOT" not in os.environ:
        try:
            info = subprocess.run(
                ["dotnet", "--info"], capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=15
            )
            m = re.search(r"Base Path:\s*(.+)", info.stdout)
            if m:
                base = Path(m.group(1).strip())
                if base.is_dir():
                    os.environ["DOTNET_ROOT"] = str(base.parent.parent)
        except (FileNotFoundError, subprocess.TimeoutExpired, OSError):
            pass

    dotnet_root = os.environ.get("DOTNET_ROOT")
    runtime_base = Path(dotnet_root) / "shared" if dotnet_root else None
    dll_candidates = [
        ctx.foundation_dir / f"{ctx.assembly}.dll",
    ]
    if dotnet_root:
        dll_candidates.append(Path(dotnet_root) / f"{ctx.assembly}.dll")
    if runtime_base:
        for runtime_dir in sorted(runtime_base.rglob(f"**/{ctx.assembly}.dll")):
            dll_candidates.insert(0, runtime_dir)
        # Also search reference assembly packs (e.g. System.ObjectModel.dll)
        packs_base = Path(dotnet_root) / "packs"
        if packs_base.exists():
            for pack_dir in sorted(packs_base.rglob(f"**/{ctx.assembly}.dll")):
                dll_candidates.append(pack_dir)
        # Prefer Microsoft.NETCore.App.Ref over NETStandard.Library.Ref
        dll_candidates.sort(key=lambda p: 1 if "NETStandard" in str(p) else 0)
        # Prefer .NET 10 runtime DLLs for correct API surface detection during
        # auto-generated C# subject compilation.  .NET 8 DLLs have a smaller
        # API surface that causes C# compilation errors when the AutoTestGenerator
        # emits code calling net9+/net10+ APIs (TypeDescriptor.RegisterType,
        # Enumerable.RightJoin, Meter.CreateGauge, etc.).
        dll_candidates.sort(key=lambda p: 0 if re.search(r"10\.\d+", str(p)) else 1)

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
    if not ensure_tool_built("Chaos.IL2CPP.Tools.AutoTestGenerator"):
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

    # Use partition's assemblyName for DLL resolution when it differs from
    # ctx.assembly (e.g. System.Collections → System.Collections.NonGeneric).
    # The directory/context assembly name is kept for reporting/organization.
    partition_assembly = partition.get("assemblyName", "")
    dll_assembly = partition_assembly if partition_assembly else ctx.assembly
    if dll_assembly != ctx.assembly:
        print(f"  [build] Partition assembly: {dll_assembly} (ctx.assembly={ctx.assembly})")
        dll_candidates = [
            ctx.foundation_dir / f"{dll_assembly}.dll",
        ]
        if dotnet_root:
            dll_candidates.append(Path(dotnet_root) / f"{dll_assembly}.dll")
        if runtime_base:
            for runtime_dir in sorted(runtime_base.rglob(f"**/{dll_assembly}.dll")):
                dll_candidates.insert(0, runtime_dir)
        target_dll = None
        for c in dll_candidates:
            if c.exists():
                target_dll = c
                break
        if target_dll:
            print(f"  [build] Re-resolved target DLL: {target_dll}")
        else:
            print(f"  [build] WARNING: could not find {dll_assembly}.dll, falling back to {ctx.assembly}")

    # -- 4. Generate subjects metadata (AutoTestGenerator or restore from cache) --
    print(f"  [build] Generating subjects...")
    auto_dll = tool_dll("Chaos.IL2CPP.Tools.AutoTestGenerator")
    metadata_path = ctx.subjects_metadata_path
    auto_output = ctx.foundation_dir / "chunks" / ctx.slug / "managed" / ".autogen"

    # Check if metadata + autogen .cs files already exist from a previous run.
    # If so, skip the expensive AutoTestGenerator probe phase and reuse them.
    existing_cs_files = list(auto_output.rglob("*.AutoGenerated.cs")) if auto_output.is_dir() else []
    if metadata_path.exists() and existing_cs_files:
        print(f"  [build] Reusing cached metadata ({metadata_path.stat().st_size} bytes, "
              f"{len(existing_cs_files)} .cs files)")
    else:
        # Clear stale metadata from any previous custom-subjects fallback run
        if metadata_path.exists():
            metadata_path.unlink()

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
        # Pass capabilities.json if available (enables async methods etc.)
        caps_path = ctx.chunk_dir / "native" / "codegen" / "generated" / "capabilities.json"
        if caps_path.exists():
            cmd.extend(["--capabilities", str(caps_path)])
            print(f"  [build] Capabilities: {caps_path.name}")
        result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=1200)

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
        print(f"  [build] WARNING: Metadata not emitted (no types matched by AutoTestGenerator)")
        total_subjects = 0
    else:
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
        total_subjects = metadata.get("totalMethods", 0)
        print(f"  [build] AutoTestGenerator subjects: {total_subjects}")

        # ── Merge supplemental coverage entries ──
        # For methods the ATG cannot probe (delegate Invoke, internal classes),
        # supplemental-coverage.json provides entries so the AOT codegen still
        # compiles them. This is a no-op for chunks without the file.
        _merge_supplemental_coverage(metadata_path, ctx.chunk_dir)
        # Re-read after potential merge so total_subjects reflects all entries
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
        total_subjects = metadata.get("totalMethods", 0)
        print(f"  [build] Total subjects (after supplemental merge): {total_subjects}")

    _custom_fallback = False
    if total_subjects == 0:
        print(f"  [build] NO subjects generated for {ctx.assembly}/{ctx.slug} — "
              f"assembly may consist entirely of non-probeable types (delegates, events, etc.)")

        # ── Custom-only fallback ──
        # If the chunk has pre-existing custom subject code in managed/combined/,
        # build the subjects DLL from source and generate metadata from it.
        combined_dir = ctx.chunk_dir / "managed" / "combined"
        combined_cs_path = combined_dir / "CombinedSubjects.cs"
        if combined_cs_path.exists():
            print(f"  [build] Custom-only fallback: {combined_cs_path}")

            # 1. Generate or copy metadata.json
            combined_meta = combined_dir / "metadata.json"
            if combined_meta.exists():
                shutil.copy2(combined_meta, metadata_path)
                print(f"  [build] Using existing metadata ({combined_meta})")
            else:
                meta_dict = _generate_metadata_from_subjects_cs(combined_cs_path, ctx.slug)
                if meta_dict is None:
                    return StageResult(
                        stage="build", status="skipped",
                        summary=f"Custom-only fallback: no metadata could be generated from {combined_cs_path.name}",
                        duration_ms=int((time.perf_counter() - start) * 1000))
                metadata_path.write_text(json.dumps(meta_dict, indent=2), encoding="utf-8")
                print(f"  [build] Generated metadata for {meta_dict['totalMethods']} custom subjects")

            # 2. Build subjects DLL from combined/CombinedSubjects.csproj
            tfm = _detect_tfm(target_dll)
            print(f"  [build] Target framework: {tfm}")
            subjects_dll = ctx.subjects_dll_path
            subjects_dll.parent.mkdir(parents=True, exist_ok=True)

            combined_csproj = combined_dir / "CombinedSubjects.csproj"
            if not combined_csproj.exists():
                # Create csproj if not present (e.g. runtime-intrinsics style)
                sdk_csproj = _chaos_sdk_csproj()
                combined_csproj.write_text(
                    "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
                    "  <PropertyGroup>\n"
                    "    <OutputType>Library</OutputType>\n"
                    "    <TargetFrameworks>net8.0;net9.0;net10.0</TargetFrameworks>\n"
                    "    <ImplicitUsings>enable</ImplicitUsings>\n"
                    "    <Nullable>enable</Nullable>\n"
                    "    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>\n"
                    "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
                    "  </PropertyGroup>\n"
                    "  <ItemGroup>\n"
                    f"    <Compile Include=\"{combined_cs_path.name}\" />\n"
                    f"    <ProjectReference Include=\"{os.path.relpath(sdk_csproj, combined_csproj.parent)}\" />\n"
                    "  </ItemGroup>\n"
                    "</Project>\n"
                )

            print(f"  [build] Building custom combined project ({tfm})...")
            build_result = subprocess.run(
                ["dotnet", "build", str(combined_csproj),
                 "-f", tfm,
                 f"-p:OutDir={subjects_dll.parent}",
                 "-p:ImportDirectoryBuildProps=false",
                 "--nologo", "-v", "quiet"],
                capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=120)

            if build_result.returncode != 0 or not subjects_dll.exists():
                print(f"  [build] Custom combined build FAILED for {tfm}")
                for line in (build_result.stderr.splitlines() + build_result.stdout.splitlines())[-15:]:
                    print(f"      {line}")
                return StageResult(
                    stage="build", status="error",
                    summary="Custom combined subjects DLL build failed",
                    details={"buildErrors": build_result.stderr[:500]},
                    duration_ms=int((time.perf_counter() - start) * 1000))

            print(f"  [build] Custom subjects DLL: {subjects_dll} ({subjects_dll.stat().st_size} bytes)")

            # Re-read metadata to get total_subjects count
            metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
            total_subjects = metadata.get("totalMethods", 0)
            print(f"  [build] Custom subjects count: {total_subjects}")

            # If entry.exe already exists from a previous successful build,
            # skip the full TPG build (which may fail for exotic types like
            # hardware intrinsics). The pre-built entry.exe is still current.
            if ctx.entry_exe_path.exists():
                print(f"  [build] Using pre-built entry.exe ({ctx.entry_exe_path.stat().st_size} bytes)")
                _build_jit_entry_fast(tool_dll("Chaos.IL2CPP.Tools.TestProjectGenerator"),
                                      subjects_dll, metadata_path, ctx.native_dir,
                                      ctx.native_config, ctx.assembly_dirs)
                duration_ms = int((time.perf_counter() - start) * 1000)
                return StageResult(
                    stage="build", status="passed",
                    summary=f"[CUSTOM FALLBACK] {total_subjects} subjects -> entry.exe (pre-built, {duration_ms}ms)",
                    details={"chunkSlug": ctx.slug, "totalSubjects": total_subjects,
                             "tfm": tfm, "durationMs": duration_ms, "customFallback": True},
                    duration_ms=duration_ms)

            # Set flag to skip the ATG combine stage below
            _custom_fallback = True
        else:
            return StageResult(
                stage="build", status="skipped",
                summary=f"AutoTestGenerator produced 0 subjects for {ctx.assembly}/{ctx.slug}",
                duration_ms=int((time.perf_counter() - start) * 1000))

    # -- 5. Detect TFM from target DLL --
    if not _custom_fallback:
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
                namespace_blocks.append(text)
            else:
                preamble = text[:ns_idx]
                for line in preamble.splitlines():
                    stripped = line.strip()
                    if stripped.startswith("using ") and stripped.endswith(";"):
                        all_usings.add(stripped)
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

        assembly_name = ctx.assembly
        pkg_refs: list[str] = []
        pkg_block = ""

        combined_csproj.write_text(
            "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
            "  <PropertyGroup>\n"
            "    <OutputType>Library</OutputType>\n"
            "    <TargetFrameworks>net8.0;net9.0;net10.0</TargetFrameworks>\n"
            "    <ImplicitUsings>enable</ImplicitUsings>\n"
            "    <Nullable>enable</Nullable>\n"
            "    <DefineConstants>VERIFY</DefineConstants>\n"
            "    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>\n"
            "    <NoWarn>$(NoWarn);SYSLIB0011;SYSLIB5006</NoWarn>\n"
            "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
            "  </PropertyGroup>\n"
            "  <ItemGroup>\n"
            f"    <Compile Include=\"{combined_cs_path.name}\" />\n"
            f"    <ProjectReference Include=\"{os.path.relpath(sdk_csproj, combined_csproj.parent)}\" />\n"
            f"{pkg_block}"
            "  </ItemGroup>\n"
            "</Project>\n"
        )

        print(f"  [build] Building combined project ({tfm})...")
        build_result = None
        for attempt in range(3):
            if attempt > 0:
                import time as _t
                _t.sleep(2.0 * attempt)
                print(f"  [build] Retry {attempt + 1}/3 for {tfm}...")
            build_result = subprocess.run(
                ["dotnet", "build", str(combined_csproj),
                 "-f", tfm,
                 f"-p:OutDir={subjects_dll.parent}",
                 "-p:ImportDirectoryBuildProps=false",
                 "--nologo", "-v", "quiet"],
                capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=120)
            if build_result.returncode == 0 and subjects_dll.exists():
                break
            # Check if failure is an MSBuild file-lock race — retry if so
            if build_result.returncode != 0 and ("being used by another process" in build_result.stderr
                                                  or "MSB3883" in build_result.stderr):
                print(f"  [build] File-lock race on attempt {attempt + 1}, retrying...")
                continue
            # Other build errors: don't retry
            break

        if build_result.returncode != 0 or not subjects_dll.exists():
            # If the detected TFM is net8.0 and compilation fails (likely because
            # auto-generated CombinedSubjects.cs calls net10+ APIs like
            # TypeDescriptor.RegisterType, Enumerable.RightJoin, etc.), retry with
            # net10.0 which has the full API surface.
            if tfm != "net10.0" and subjects_dll.exists() is False:
                print(f"  [build] Retrying with net10.0 (original TFM={tfm} failed)")
                tfm = "net10.0"
                build_result = subprocess.run(
                    ["dotnet", "build", str(combined_csproj),
                     "-f", tfm,
                     f"-p:OutDir={subjects_dll.parent}",
                     "-p:ImportDirectoryBuildProps=false",
                     "--nologo", "-v", "quiet"],
                    capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=120)
                if build_result.returncode == 0 and subjects_dll.exists():
                    print(f"  [build] net10.0 retry SUCCEEDED")
                else:
                    print(f"  [build] net10.0 retry also FAILED")
            if build_result.returncode != 0 or not subjects_dll.exists():
                print(f"  [build] Combined build FAILED for {tfm}")
                for line in (build_result.stderr.splitlines() + build_result.stdout.splitlines())[-15:]:
                    print(f"      {line}")
                return StageResult(
                    stage="build", status="error",
                    summary="Combined subjects DLL build failed",
                    details={"buildErrors": build_result.stderr[:500]},
                    duration_ms=int((time.perf_counter() - start) * 1000))

        print(f"  [build] Subjects DLL: {subjects_dll} ({subjects_dll.stat().st_size} bytes)")

    # Include runtime stub source files in hash so changes to interop_stubs.cpp
    # etc. invalidate the build cache and trigger a fresh rebuild.
    _runtime_stubs = [
        _REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_stubs" / s
        for s in ("interop_stubs.cpp", "math_stubs.cpp", "vector_stubs.cpp",
                  "misc_stubs.cpp", "array_stubs.cpp", "char_stubs.cpp",
                  "async_stubs.cpp", "exception_stubs.cpp")
    ]
    # Include TPG/build dependencies so changes to templates, emitter, or
    # pipeline scripts invalidate the cache.  Without this, modifying
    # .scriban templates or build.py produces stale entry.exe from cache.
    _tpg_build_deps = [
        _REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.TestProjectGenerator" / "Templates" / s
        for s in ("TestProject.RuntimeEntry.cpp.scriban",
                  "TestProject.CMakeLists.txt.scriban",
                  "TestProject.Dispatch.cpp.scriban")
    ] + [
        _REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.TestProjectGenerator" / "Emission" / "CppProjectEmitter.cs",
        _REPO_ROOT / "testing" / "foundation-dll" / "verification" / "stages" / "build.py",
        _REPO_ROOT / "testing" / "foundation-dll" / "verification" / "stages" / "hephaestus_cache.py",
        # Tool binaries — any rebuild of ATG/TPG invalidates all caches
        tool_dll("Chaos.IL2CPP.Tools.AutoTestGenerator"),
        tool_dll("Chaos.IL2CPP.Tools.TestProjectGenerator"),
        # Runtime library — cmake rebuild invalidates all cached entry.exe
        _REPO_ROOT / "artifacts" / "presets" / _sdk_preset / "src" / "native" / "runtime-core" / _sdk_config / f"chaos_runtime_core{_sdk_lib_ext}",
        _REPO_ROOT / "artifacts" / "presets" / _sdk_preset / "src" / "native" / "bootstrap" / _sdk_config / f"chaos_bootstrap{_sdk_lib_ext}",
    ]

    # -- 7a. Fast-path mtime check: skip TPG + cmake if entry.exe is already up to date --
    if ctx.entry_exe_path.exists():
        exe_mtime = ctx.entry_exe_path.stat().st_mtime
        deps = [
            subjects_dll, metadata_path, target_dll,
            tool_dll("Chaos.IL2CPP.Tools.AutoTestGenerator"),
            tool_dll("Chaos.IL2CPP.Tools.TestProjectGenerator"),
        ] + _runtime_stubs + _tpg_build_deps
        deps = [d for d in deps if d is not None and d.exists()]
        stalest_dep_mtime = max(d.stat().st_mtime for d in deps)
        if exe_mtime >= stalest_dep_mtime:
            exe_size = ctx.entry_exe_path.stat().st_size
            print(f"  [build] [fastpath] entry.exe is current ({exe_size} bytes, mtime={exe_mtime:.0f})")
            duration_ms = int((time.perf_counter() - start) * 1000)
            # Also build JIT entry if stale or missing (non-blocking)
            _build_jit_entry_fast(tool_dll("Chaos.IL2CPP.Tools.TestProjectGenerator"),
                                  subjects_dll, metadata_path, ctx.native_dir,
                                  ctx.native_config, ctx.assembly_dirs)
            return StageResult(
                stage="build", status="passed",
                summary=f"[FASTPATH] {total_subjects} subjects -> entry.exe ({duration_ms}ms)",
                details={"chunkSlug": ctx.slug, "totalSubjects": total_subjects,
                         "tfm": tfm, "durationMs": duration_ms, "fastpath": True},
                duration_ms=duration_ms)

    # -- 7b. Hephaestus cache lookup: skip TPG if unchanged input --
    cache = HephaestusCache(ctx.foundation_dir, verbose=True)
    cache_status = "miss"
    input_hash = compute_input_hash(
        subjects_dll, metadata_path, ctx.assembly,
        additional_dlls=[target_dll] if target_dll else None,
        extra_source_paths=_runtime_stubs,  # content hash only: DLLs + stubs
    )
    # Context fingerprint: fast mtime check on tools/templates (not full SHA-256)
    context_fp = compute_context_fingerprint(_tpg_build_deps)

    cache_key = cache.compute_key(input_hash, ctx.assembly, ctx.slug)
    cache_hit = cache.is_cache_hit(cache_key)
    # Full cache hit: content + tools/templates all match
    if cache_hit and cache.is_context_fresh(cache_key, context_fp):
        print(f"  [build] [hephaestus] FULL CACHE HIT: {cache_key[:48]}...")
        if cache.restore_to(cache_key, ctx.native_dir):
            # Verify the restored entry.exe exists
            if ctx.entry_exe_path.exists():
                exe_size = ctx.entry_exe_path.stat().st_size
                print(f"  [build] [hephaestus] Restored entry.exe ({exe_size} bytes)")
                # Clean stale cmake build dir that may have been restored from
                # cache.  CMakeCache.txt records CMAKE_HOME_DIRECTORY from the
                # original projectDir which may differ from the current path
                # (e.g. different Docker container, clone path changed).  The
                # cache-miss path (below) already does this; the cache-hit path
                # was missing it.
                restored_build_dir = ctx.native_dir / "build"
                if restored_build_dir.exists():
                    import shutil
                    shutil.rmtree(restored_build_dir, ignore_errors=True)
                    print(f"  [build] [hephaestus] Cleaned stale cmake build dir from cache")
                duration_ms = int((time.perf_counter() - start) * 1000)
                # Also build JIT entry if stale or missing (non-blocking)
                _build_jit_entry_fast(tool_dll("Chaos.IL2CPP.Tools.TestProjectGenerator"),
                                      subjects_dll, metadata_path, ctx.native_dir,
                                      ctx.native_config, ctx.assembly_dirs)
                return StageResult(
                    stage="build", status="passed",
                    summary=f"[CACHE HIT] {total_subjects} subjects -> entry.exe ({duration_ms}ms)",
                    details={
                        "chunkSlug": ctx.slug,
                        "totalSubjects": total_subjects,
                        "tfm": tfm,
                        "durationMs": duration_ms,
                        "hephaestus": "cache_hit",
                        "cacheKey": cache_key,
                    },
                    duration_ms=duration_ms)
            else:
                print(f"  [build] [hephaestus] Cached entry.exe missing, falling through to full build")
                cache.invalidate_assembly(ctx.assembly)
                cache_status = "restore_failed_entry_missing"
        else:
            cache_status = "restore_failed"
            print(f"  [build] [hephaestus] Cache restore failed, falling through to full build")
            print(f"  [build] [hephaestus] Cache restore failed, falling through to full build")

    print(f"  [build] [hephaestus] CACHE MISS: performing full build")

    # -- Clean stale cmake build cache --
    # Prevents stale CMakeCache.txt from a previous run with a different source
    # layout (e.g., another chunk) from polluting this build.
    native_build_dir = ctx.native_dir / "build"
    if native_build_dir.exists():
        print(f"  [build] Cleaning stale cmake build dir: {native_build_dir}")
        shutil.rmtree(native_build_dir, ignore_errors=True)

    # -- 8. Run TPG generate-dll --
    if not ensure_tool_built("Chaos.IL2CPP.Tools.TestProjectGenerator"):
        return StageResult(
            stage="build", status="error",
            summary="TPG build failed",
            duration_ms=int((time.perf_counter() - start) * 1000))

    tpg_dll = tool_dll("Chaos.IL2CPP.Tools.TestProjectGenerator")
    print(f"  [build] Running TPG generate-dll...")

    tpg_cmd = [
        "dotnet", "exec", str(tpg_dll),
        "generate-dll",
        "--dll", str(subjects_dll),
        "--metadata", str(metadata_path),
        "--output", str(ctx.native_dir),
        "--config-tier", ctx.native_config,
        "--clean",
    ]

    # Pass assembly dirs from pipeline-config.yaml (populated by chunk_pipeline.py)
    for ad in ctx.assembly_dirs:
        tpg_cmd.extend(['--assembly-dir', ad])
        print(f"  [build] assembly-dir: {ad}")

    # Additional assemblies from chunk config (declared in chunk.json)
    # e.g. crypto DLL for interpreter fallback in security-cryptography chunks.
    config_assemblies = _get_additional_assemblies(ctx.chunk_dir)
    if config_assemblies:
        for aa in config_assemblies:
            aa_path = ctx.foundation_dir / "crypto-refs" / aa
            if aa_path.exists():
                tpg_cmd.extend(['--additional-assembly', str(aa_path)])
                print(f"  [build] additional-assembly: {aa_path}")
            else:
                print(f"  [build] WARNING: additional assembly {aa} not found at {aa_path}")
    else:
        # Fallback: slug-based detection for chunks without chunk.json (e.g. CoreLib)
        crypto_dll = ctx.foundation_dir / "crypto-refs" / "System.Security.Cryptography.dll"
        if crypto_dll.exists() and any(x in ctx.slug for x in ("security-cryptography", "x509")):
            tpg_cmd.extend(['--additional-assembly', str(crypto_dll)])
            print(f"  [build] additional-assembly: {crypto_dll} (slug fallback)")
    tpg_result = subprocess.run(tpg_cmd, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=7200)

    for line in tpg_result.stdout.splitlines():
        print(f"      {line}")

    if tpg_result.returncode != 0:
        print(f"  [build] TPG generate-dll FAILED (rc={tpg_result.returncode})")
        for line in tpg_result.stderr.splitlines():
            try:
                print(f"  [TPG:err] {line}")
            except UnicodeEncodeError:
                print(f"  [TPG:err] {line.encode('ascii', errors='replace').decode('ascii')}")
        for line in tpg_result.stdout.splitlines()[-5:]:
            print(f"  [TPG:out] {line}")

        if tpg_result.returncode != 0:
            return StageResult(
                stage="build", status="error",
                summary=f"TPG generate-dll failed (rc={tpg_result.returncode})",
                duration_ms=int((time.perf_counter() - start) * 1000))

    # -- 8b. Inject --profile mode into runtime-entry.cpp --
    _inject_profile_mode(ctx.native_dir)

    entry_exe = ctx.entry_exe_path
    if not entry_exe.exists():
        return StageResult(
            stage="build", status="error",
            summary=f"entry.exe not produced at {entry_exe}",
            duration_ms=int((time.perf_counter() - start) * 1000))

    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [build] entry.exe: {entry_exe}")

    # -- Store in Hephaestus cache --
    cache.store(
        cache_key, ctx.native_dir,
        assembly=ctx.assembly, chunk_slug=ctx.slug,
        input_hash=input_hash, duration_ms=duration_ms,
        context_fingerprint=context_fp,
    )
    print(f"  [build] [hephaestus] Cached build output ({cache_key[:48]}...)")

    # -- 9. Build JIT entry (non-blocking) --
    jit_built = _build_jit_entry_fast(tpg_dll, subjects_dll, metadata_path, ctx.native_dir, ctx.native_config, ctx.assembly_dirs)

    print(f"  [build] Done ({duration_ms}ms)")

    return StageResult(
        stage="build", status="passed",
        summary=f"{total_subjects} subjects -> entry.exe ({duration_ms}ms)",
        details={
            "chunkSlug": ctx.slug,
            "totalSubjects": total_subjects,
            "tfm": tfm,
            "durationMs": duration_ms,
            "hephaestus": cache_status,
            "cacheKey": cache_key,
            "jitSkipped": not jit_built,
        },
        duration_ms=duration_ms)

