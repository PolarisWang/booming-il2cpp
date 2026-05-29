"""HotUpdate stage runners — patch lifecycle and post-patch verification.

Uses entry.exe --hotupdate and entry.exe --hotupdate-and-benchmark (and JIT variants).
"""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult

_REPO_ROOT = Path(__file__).resolve().parents[4]
_DRIVER_DLL = (
    _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"
    / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
)
_IS_LINUX = sys.platform.startswith("linux")

# Module-level flag: ensures chaos_runtime_core.lib is only rebuilt+synced once
# per process lifetime.  The lib itself doesn't change between hotupdate stages;
# only runtime-patchdata.cpp changes (compiled and linked into entry.exe).
_runtime_lib_synced = False


def _find_entry_binary(build_dir: Path) -> Path | None:
    """Locate the built entry binary in a cmake build directory.

    On Windows/MSVC: build_dir/RelWithDebInfo/entry.exe
    On Linux/Ninja:  build_dir/entry            (no config subdir, no .exe)
    """
    if _IS_LINUX:
        candidate = build_dir / "entry"
        return candidate if candidate.exists() else None
    for cfg in ("RelWithDebInfo", "Release", "Debug", ""):
        base = build_dir / cfg if cfg else build_dir
        candidate = base / "entry.exe"
        if candidate.exists():
            return candidate
    # Fallback: search for any entry* binary
    for f in build_dir.rglob("entry*"):
        if f.is_file() and f.name.startswith("entry") and not f.suffix == ".o":
            return f
    return None


def _has_patch_project(ctx: FamilyContext) -> bool:
    """Check whether the family has a managed/patch/ project for hotupdate testing."""
    patch_dir = ctx.family_dir / "managed" / "patch"
    return any(patch_dir.glob("*.csproj"))


def _build_patch_dll(ctx: FamilyContext) -> Path | None:
    """Build the patch DLL from managed/patch/ and return the DLL path."""
    patch_dir = ctx.family_dir / "managed" / "patch"
    csproj_files = list(patch_dir.glob("*.csproj"))
    if not csproj_files:
        print(f"    [hotupdate] No .csproj found in {patch_dir}")
        return None

    csproj = csproj_files[0]
    build_out = patch_dir / "bin" / "Release" / "net8.0"
    build_out.mkdir(parents=True, exist_ok=True)

    r = subprocess.run(
        ["dotnet", "build", str(csproj), "-c", "Release",
         "--nologo", "-v", "minimal"],
        capture_output=True, text=True, timeout=300,
    )
    if r.returncode != 0:
        print(f"    [hotupdate] Patch DLL build FAILED (exit={r.returncode})")
        # Print both stdout and stderr — MSBuild errors can appear in either
        err_lines = (r.stderr or "").splitlines()
        out_lines = (r.stdout or "").splitlines()
        # Filter to error/warning lines for concise output
        for line in (err_lines + out_lines):
            stripped = line.strip()
            if any(kw in stripped for kw in ("error ", "warning ", "CS", "BUILD", "FAILED")):
                print(f"      {stripped}")
        # If no filtered lines, print last few lines from each
        if not any(kw in (r.stderr or "") + (r.stdout or "") for kw in ("error ", "FAILED")):
            for line in (err_lines + out_lines)[-10:]:
                print(f"      {line.strip()}")
        return None

    dll = build_out / f"{csproj.stem}.dll"
    if not dll.exists():
        for f in sorted(build_out.glob("*.dll")):
            name = f.name
            if name.startswith(("System.", "Microsoft.", "mscorlib",
                               "NuGet", "xunit", "testhost")):
                continue
            dll = f
            break
    if not dll.exists():
        print(f"    [hotupdate] Patch DLL not found in {build_out}")
        return None

    print(f"    [hotupdate] Patch DLL built: {dll.name}")
    return dll


def _run_emit_patch_data(dll_path: str, output_path: str,
                         aot_core_ir_path: str | None = None) -> bool:
    """Run chaos-il2cpp emit-patch-data CLI on a patch DLL."""
    cmd = [
        "dotnet", "exec", str(_DRIVER_DLL),
        "emit-patch-data", dll_path, output_path,
    ]
    if aot_core_ir_path:
        cmd += ["--aot-core-ir", aot_core_ir_path]

    r = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
    if r.returncode != 0:
        print(f"    [hotupdate] emit-patch-data FAILED (exit={r.returncode})")
        for line in (r.stderr or "").splitlines()[-5:]:
            print(f"      {line}")
        return False

    last_line = (r.stdout or "").strip().splitlines()[-1] if r.stdout else ""
    print(f"    [hotupdate] {last_line}")
    return True


def _generate_runtime_patchdata_cpp(patchdata_path: Path, output_cpp_path: Path,
                                    host_class_name: str) -> bool:
    """Generate runtime-patchdata.cpp from a .patchdata binary."""
    if not patchdata_path.exists():
        return False

    data = patchdata_path.read_bytes()
    size = len(data)

    hex_lines = []
    for i in range(0, size, 16):
        chunk = data[i:i + 16]
        hex_str = ", ".join(f"0x{b:02x}" for b in chunk)
        hex_lines.append(f"{hex_str},")

    hex_body = "\n".join(hex_lines)
    if hex_body.endswith(","):
        hex_body = hex_body[:-1]

    cpp_content = (
        f'#include <cstdint>\n'
        f'#include <cstddef>\n'
        f'extern const uint8_t kPatchData[] = {{\n'
        f'{hex_body}\n'
        f'}};\n'
        f'extern const size_t kPatchDataSize = {size}u;\n'
        f'extern const char* const kPatchDataHostClassName = "{host_class_name}";\n'
    )

    output_cpp_path.parent.mkdir(parents=True, exist_ok=True)
    output_cpp_path.write_text(cpp_content, encoding="utf-8")
    print(f"    [hotupdate] Wrote {output_cpp_path.name} ({size} bytes patchdata)")
    return True


def _write_sentinel_patchdata(native_dir: Path) -> None:
    """Write a sentinel runtime-patchdata.cpp (size=0) to disable hotpatch."""
    cpp = native_dir / "runtime-patchdata.cpp"
    cpp.write_text(
        '// Sentinel: no .patchdata available (hotpatch dispatch disabled)\n'
        '#include <cstddef>\n'
        '#include <cstdint>\n'
        '\n'
        'extern const char* const kPatchDataHostClassName;\n'
        'extern const char* const kPatchDataHostClassName = "";\n'
        '\n'
        'extern const uint8_t kPatchData[];\n'
        'extern const uint8_t kPatchData[] = { 0x00 };\n'
        '\n'
        'extern const size_t kPatchDataSize;\n'
        'extern const size_t kPatchDataSize = 0u;\n',
        encoding="utf-8",
    )


def _detect_host_class(native_dir: Path) -> str:
    """Detect the AOT host class name from generated source files."""
    for h in native_dir.glob("*Subjects/generated/chaos_generated_module.h"):
        text = h.read_text(encoding="utf-8")
        m = re.search(r'RegisterSubjectModule\s*\(\s*"([^"]+)"', text)
        if m:
            return m.group(1)
    for d in native_dir.iterdir():
        if d.name.endswith("Subjects") and d.is_dir():
            return d.name
    return "ConvertCharSubjects"


def _ensure_patch_data(ctx: FamilyContext) -> bool:
    """Build patch DLL -> emit-patch-data -> generate runtime-patchdata.cpp -> rebuild entry.exe."""
    native_dir = ctx.native_dir

    # Copy handwritten partial class files to patch dir so CustomEntryMethodN()
    # implementations are visible to the patch DLL compiler.
    # Handwritten files use class "XxxNativeEntry" but the patch variant
    # uses "XxxPatchEntry".
    handwritten_dir = ctx.family_dir / "handwritten"
    patch_dir = ctx.family_dir / "managed" / "patch"
    patch_dir.mkdir(parents=True, exist_ok=True)
    if handwritten_dir.exists():
        for f in sorted(handwritten_dir.glob("*.cs")):
            content = f.read_text(encoding="utf-8")
            if f.name.endswith(".Custom.cs"):
                dest_name = f.name.replace("NativeEntry", "PatchEntry")
                content = content.replace("NativeEntry", "PatchEntry")
            else:
                dest_name = f.name
            dest = patch_dir / dest_name
            dest.write_text(content, encoding="utf-8")

    dll = _build_patch_dll(ctx)
    if dll is None:
        print("  [hotupdate] _ensure_patch_data: patch DLL build failed")
        return False

    patchdata = native_dir / "subjects.patchdata"
    # Convert hyphenated slug to PascalCase (e.g. "buffer-memory" → "BufferMemorySubjects")
    subjects_class = "".join(part.capitalize() for part in ctx.slug.replace("-", "_").split("_"))
    aot_core_ir = native_dir / f"{subjects_class}Subjects" / "generated" / "aot-core-ir.json"
    if not aot_core_ir.exists():
        aot_core_ir = None
        for d in native_dir.iterdir():
            candidate = d / "generated" / "aot-core-ir.json"
            if candidate.exists():
                aot_core_ir = candidate
                break

    dll_str = str(dll)
    pd_str = str(patchdata)
    aot_str = str(aot_core_ir) if aot_core_ir is not None else None
    ok = _run_emit_patch_data(dll_str, pd_str, aot_core_ir_path=aot_str)
    if not ok or not patchdata.exists():
        print("  [hotupdate] _ensure_patch_data: emit-patch-data failed")
        return False

    host_class = _detect_host_class(native_dir)
    _generate_runtime_patchdata_cpp(patchdata, native_dir / "runtime-patchdata.cpp", host_class)

    runtime_core_build = _REPO_ROOT / "build" / "vs2022" / "src" / "native" / "runtime-core"
    global _runtime_lib_synced
    if runtime_core_build.exists() and not _runtime_lib_synced:
        print(f"  [hotupdate] Rebuilding chaos_runtime_core.lib (once per run)...")
        subprocess.run(
            ["cmake", "--build", str(runtime_core_build), "--config", "RelWithDebInfo", "--parallel"],
            capture_output=True, text=True, timeout=300,
        )
        runtime_core_lib = runtime_core_build / "RelWithDebInfo" / "chaos_runtime_core.lib"
        if runtime_core_lib.exists():
            import shutil as _shutil
            # Copy to ALL per-subject SDK lib dirs (codegen/<Subject>/lib/), not
            # just codegen/lib/.  The entry.vcxproj links from per-subject dirs,
            # and cmake dependency tracking recompiles native-aot.generated.cpp
            # when headers like gc_alloc_stubs.h change — the new .obj references
            # symbols from the rebuilt lib (e.g. tls_alloc_fast_count).  Without
            # this copy, the linker finds a stale lib and reports LNK2001.
            codegen_dir = native_dir.parent / "codegen"
            copied = 0
            if codegen_dir.is_dir():
                for sub_dir in sorted(codegen_dir.iterdir()):
                    sdk_lib_dir = sub_dir / "lib"
                    if sdk_lib_dir.is_dir():
                        _shutil.copy2(str(runtime_core_lib),
                                      str(sdk_lib_dir / "chaos_runtime_core.lib"))
                        copied += 1
            # Also copy to codegen/lib/ (fallback location)
            parent_lib_dir = codegen_dir / "lib" if codegen_dir.is_dir() else native_dir.parent / "codegen" / "lib"
            if parent_lib_dir.exists():
                _shutil.copy2(str(runtime_core_lib),
                              str(parent_lib_dir / "chaos_runtime_core.lib"))
                if copied == 0:
                    copied = 1
            print(f"  [hotupdate] Copied chaos_runtime_core.lib to {copied} SDK lib dir(s)")
        _runtime_lib_synced = True

    print(f"  [hotupdate] Rebuilding entry.exe with patchdata...")
    build_dir = native_dir / "build"

    # Ensure microbench.cpp exists — the initial build creates it, but it may
    # have been deleted between stages.  Without it, cmake --build fails with
    # C1083 (Cannot open source file) because the cmake-generated vcxproj
    # still references it from the initial configure.
    microbench_cpp = native_dir / "microbench.cpp"
    if not microbench_cpp.exists():
        microbench_cpp.write_text(
            '// microbench.cpp — Interpreter internal microbenchmarks.\n'
            '// Auto-generated (hotupdate fallback).\n'
            '#include "fast_frame_pool.h"\n'
            '#include <chrono>\n'
            '#include <cstdio>\n'
            '#include <cstdint>\n'
            '\n'
            'using Clock = std::chrono::high_resolution_clock;\n'
            'using chaos::il2cpp::runtime_core::tls_frame_pool;\n'
            'using chaos::il2cpp::runtime_core::FastFramePool;\n'
            'using chaos::il2cpp::runtime_core::FastFrame;\n'
            '\n'
            'extern "C" const int kAotMethodCount;\n'
            'struct BenchmarkResult { double elapsed_ms; int64_t allocated_bytes; };\n'
            'extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);\n'
            '\n'
            'extern "C" void RunMicrobench() {\n'
            '    printf("microbench: no-op (hotupdate fallback)\\n");\n'
            '}\n',
            encoding="utf-8",
        )
        print(f"  [hotupdate] generated fallback microbench.cpp")

    if not build_dir.exists():
        build_dir = native_dir / "build" / "vs2022"
    if not build_dir.exists():
        # Fallback: try to locate CMakeLists.txt and configure
        cmake_lists = native_dir / "CMakeLists.txt"
        if cmake_lists.exists():
            print(f"  [hotupdate] Configuring CMake at {native_dir}...")
            subprocess.run(
                ["cmake", "-S", str(native_dir), "-B", str(build_dir),
                 "-DCMAKE_BUILD_TYPE=RelWithDebInfo"],
                capture_output=True, text=True, timeout=120,
            )
        else:
            print(f"  [hotupdate] No build directory ({build_dir}) and no CMakeLists.txt found")
            return False
    elif not (build_dir / "CMakeCache.txt").exists():
        # Build dir exists but was never configured — delete and reconfigure fresh
        # to avoid stale intermediate-file paths confusing the linker.
        import shutil as _shutil
        _shutil.rmtree(str(build_dir), ignore_errors=True)
        cmake_lists = native_dir / "CMakeLists.txt"
        if cmake_lists.exists():
            print(f"  [hotupdate] Configuring CMake at {native_dir}...")
            subprocess.run(
                ["cmake", "-S", str(native_dir), "-B", str(build_dir),
                 "-DCMAKE_BUILD_TYPE=RelWithDebInfo"],
                capture_output=True, text=True, timeout=120,
            )
    r = subprocess.run(
        ["cmake", "--build", str(build_dir), "--config", "RelWithDebInfo", "--target", "entry", "--parallel"],
        capture_output=True, text=True, timeout=300,
    )
    if r.returncode != 0:
        # MSBuild on Windows often returns non-zero even when the link succeeds —
        # e.g. missing ZERO_CHECK.vcxproj (a cmake housekeeping project) causes
        # MSB3202, but the actual compilation and linking still produce entry.exe.
        # On Linux/Ninja, returncode is reliable — check binary anyway.
        exe_produced = _find_entry_binary(build_dir)
        if exe_produced:
            print(f"  [hotupdate] entry.exe rebuild OK (exit={r.returncode}, exe produced)")
            import shutil as _shutil
            _shutil.copy2(str(exe_produced), str(native_dir / "entry.exe"))
            if _IS_LINUX:
                # On Linux, ensure native_dir/entry.exe is executable
                native_exe = native_dir / "entry.exe"
                native_exe.chmod(native_exe.stat().st_mode | 0o111)
            return True
        print(f"  [hotupdate] entry.exe rebuild FAILED (exit={r.returncode})")
        for line in (r.stderr or "").splitlines()[-15:]:
            print(f"    {line}")
        # If stderr had no error info, try stdout
        if not any(kw in (r.stderr or "") for kw in ("error", "FAILED", "fatal")):
            for line in (r.stdout or "").splitlines()[-10:]:
                print(f"    {line.strip()}")
        return False

    # Copy rebuilt exe back to native_dir so pipeline stages use the new binary
    import shutil as _shutil
    import time as _time
    src_exe = _find_entry_binary(build_dir)
    if src_exe is None:
        print(f"  [hotupdate] entry binary not found in {build_dir} after successful build")
        return False
    dst_exe = native_dir / "entry.exe"
    for _copy_attempt in range(5):
        try:
            if dst_exe.exists():
                dst_exe.unlink()
            _shutil.copy2(str(src_exe), str(dst_exe))
            if _IS_LINUX:
                dst_exe.chmod(dst_exe.stat().st_mode | 0o111)
            break
        except (PermissionError, OSError) as _e:
            if _copy_attempt < 4:
                _time.sleep(1 << _copy_attempt)
            else:
                print(f"  [hotupdate] entry.exe copy FAILED: {_e}")
                return False
    print(f"  [hotupdate] entry.exe rebuild OK")
    return True


def _load_method_count(ctx: FamilyContext) -> int:
    """Load method count from contract.json."""
    contract_path = ctx.contract_path
    if not contract_path.exists():
        return 0
    try:
        import json as _json
        c = _json.loads(contract_path.read_text(encoding="utf-8"))
        mids = c.get("methodSubjectIds", [])
        if mids:
            return len(mids)
        return c.get("methodSubjectCount", 0)
    except Exception:
        return 0


def _run_hotupdate_fact(exe_path: Path) -> dict[str, Any]:
    """Run entry.exe --hotupdate and parse JSON result.

    Returns a dict with status, passedMethods, allSemantic, etc.
    Status is "failed" when allSemantic is false (patch detected no
    semantic change) — this indicates the patch may not have been
    effective or the detection mechanism is broken.
    """
    try:
        r = subprocess.run(
            [str(exe_path), "--hotupdate"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"status": "error", "summary": "timed out (120s)"}
    except OSError as e:
        return {"status": "error", "summary": str(e)}

    output = (r.stdout or "").strip()
    # Scan for JSON object anywhere in output (handles trailing GC stats)
    json_start = output.find("{")
    if json_start >= 0:
        # Find the matching closing brace for the root object
        depth = 0
        json_end = -1
        for i in range(json_start, len(output)):
            if output[i] == '{':
                depth += 1
            elif output[i] == '}':
                depth -= 1
                if depth == 0:
                    json_end = i + 1
                    break
        if json_end > json_start:
            json_str = output[json_start:json_end]
            try:
                data = json.loads(json_str)
                passed = data.get("passedMethods", 0)
                failed = data.get("failedMethods", 0)
                total = data.get("totalMethods", 0)
                all_semantic = data.get("allSemantic", True)
                semantic_changed = data.get("semanticChangedCount", 0)
                all_revert = data.get("allRevert", True)

                status = "passed" if failed == 0 else "failed"

                # R6: allSemantic=false means no method changed pass/fail
                # behavior after patching.  This is a HARD FAILURE: either
                # the patch didn't take effect, or the detection mechanism
                # is broken (e.g. ChaosDispatchMethod returning dispatch
                # status instead of method return value).
                if not all_semantic and passed > 0:
                    status = "failed"
                    print(f"    [hotupdate] FAILED: no semantic change detected "
                          f"(changed={semantic_changed}/{total})")

                # R6: If revert failed, mark as warning
                if not all_revert:
                    print(f"    [hotupdate] WARNING: revert verification failed — "
                          f"cleanup may have issues")

                return {
                    "status": status,
                    "passedMethods": passed,
                    "failedMethods": failed,
                    "totalMethods": total,
                    "allSemantic": all_semantic,
                    "semanticChangedCount": semantic_changed,
                    "allRevert": all_revert,
                    "exitCode": r.returncode,
                }
            except json.JSONDecodeError:
                pass

    return {
        "status": "failed" if r.returncode != 0 else "passed",
        "exitCode": r.returncode,
        "rawOutput": (r.stdout or "")[:500],
    }


def _run_hotupdate_benchmark(
    exe_path: Path, method_index: int, iterations: int = 100000,
) -> dict[str, Any]:
    """Run entry.exe --hotupdate-and-benchmark N and parse JSON timing."""
    try:
        r = subprocess.run(
            [str(exe_path), "--hotupdate-and-benchmark", str(method_index), str(iterations)],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"methodIndex": method_index, "error": "timed out"}

    output = (r.stdout or "").strip()
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                return json.loads(line)
            except json.JSONDecodeError:
                pass
    return {"methodIndex": method_index, "error": f"no JSON: {(r.stdout or '')[:200]}", "exitCode": r.returncode}


def run_hotupdate(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 9: HotUpdate AOT Fact — run entry.exe --hotupdate."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate", status="skipped",
            summary="entry.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not _has_patch_project(ctx):
        return StageResult(
            stage="hotupdate", status="skipped",
            summary="no patch project — hotupdate not applicable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not _ensure_patch_data(ctx):
        return StageResult(
            stage="hotupdate", status="failed",
            summary="patch data build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        print(f"  [hotupdate] Running {exe_path} --hotupdate...")
        result = _run_hotupdate_fact(exe_path)
        status = result.get("status", "failed")
        passed = result.get("passedMethods", 0)
        total = result.get("totalMethods", 0)

        print(f"  [hotupdate] Result: {status} ({passed}/{total})")

        return StageResult(
            stage="hotupdate", status=status,
            summary=f"{status} ({passed}/{total})",
            details=result,
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    finally:
        _write_sentinel_patchdata(ctx.native_dir)


def run_hotupdate_aot_bench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 10: HotUpdate AOT Bench — run entry.exe --hotupdate-and-benchmark for all methods."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="entry.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    method_count = _load_method_count(ctx)
    if method_count == 0:
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not _has_patch_project(ctx):
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="no patch project — hotupdate not applicable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not _ensure_patch_data(ctx):
        return StageResult(
            stage="hotupdate_aot_benchmark", status="failed",
            summary="patch data build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        print(f"  [hotupdate_aot_bench] Running {method_count} methods...")
        results: list[dict[str, Any]] = []
        ok_count = 0
        for i in range(method_count):
            r = _run_hotupdate_benchmark(exe_path, i)
            results.append(r)
            if "error" not in r:
                ok_count += 1

        status = "passed" if ok_count > 0 else "failed"
        print(f"  [hotupdate_aot_bench] Result: {status} ({ok_count}/{method_count})")

        return StageResult(
            stage="hotupdate_aot_benchmark", status=status,
            summary=f"{status} ({ok_count}/{method_count})",
            details={"results": results, "okCount": ok_count, "totalMethods": method_count},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    finally:
        _write_sentinel_patchdata(ctx.native_dir)


def run_hotupdate_jit_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 11: HotUpdate JIT Fact — run entry-jit.exe --hotupdate."""
    start = time.perf_counter()

    exe_path = ctx.entry_jit_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_jit_fact", status="skipped",
            summary="entry-jit.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate_jit_fact] Running {exe_path} --hotupdate...")
    result = _run_hotupdate_fact(exe_path)
    status = result.get("status", "failed")
    passed = result.get("passedMethods", 0)
    total = result.get("totalMethods", 0)

    print(f"  [hotupdate_jit_fact] Result: {status} ({passed}/{total})")

    return StageResult(
        stage="hotupdate_jit_fact", status=status,
        summary=f"{status} ({passed}/{total})",
        details=result,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_hotupdate_jit_bench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 12: HotUpdate JIT Bench — run entry-jit.exe --hotupdate-and-benchmark."""
    start = time.perf_counter()

    exe_path = ctx.entry_jit_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_jit_benchmark", status="skipped",
            summary="entry-jit.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    method_count = _load_method_count(ctx)
    if method_count == 0:
        return StageResult(
            stage="hotupdate_jit_benchmark", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate_jit_bench] Running {method_count} methods...")
    results: list[dict[str, Any]] = []
    ok_count = 0
    for i in range(method_count):
        r = _run_hotupdate_benchmark(exe_path, i)
        results.append(r)
        if "error" not in r:
            ok_count += 1

    status = "passed" if ok_count > 0 else "failed"
    print(f"  [hotupdate_jit_bench] Result: {status} ({ok_count}/{method_count})")

    return StageResult(
        stage="hotupdate_jit_benchmark", status=status,
        summary=f"{status} ({ok_count}/{method_count})",
        details={"results": results, "okCount": ok_count, "totalMethods": method_count},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
