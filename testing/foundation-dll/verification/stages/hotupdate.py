"""HotUpdate stage runners — patch lifecycle and post-patch verification.

Uses entry.exe --hotupdate and entry.exe --hotupdate-and-benchmark (and JIT variants).
"""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import time
from pathlib import Path
from typing import Any

from orchestration.context import FamilyContext, StageResult

_REPO_ROOT = Path(__file__).resolve().parents[3]
_DRIVER_DLL = (
    _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"
    / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
)


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
         "--nologo", "-v", "quiet"],
        capture_output=True, text=True, timeout=300,
    )
    if r.returncode != 0:
        print(f"    [hotupdate] Patch DLL build FAILED (exit={r.returncode})")
        for line in (r.stderr or "").splitlines()[-5:]:
            print(f"      {line}")
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

    dll = _build_patch_dll(ctx)
    if dll is None:
        print("  [hotupdate] _ensure_patch_data: patch DLL build failed")
        return False

    patchdata = native_dir / "subjects.patchdata"
    aot_core_ir = native_dir / f"{ctx.slug.capitalize()}Subjects" / "generated" / "aot-core-ir.json"
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
    if runtime_core_build.exists():
        print(f"  [hotupdate] Rebuilding chaos_runtime_core.lib...")
        subprocess.run(
            ["cmake", "--build", str(runtime_core_build), "--config", "RelWithDebInfo", "--parallel"],
            capture_output=True, text=True, timeout=300,
        )
        runtime_core_lib = runtime_core_build / "RelWithDebInfo" / "chaos_runtime_core.lib"
        sdk_lib_dir = native_dir.parent / "codegen" / "lib"
        if runtime_core_lib.exists() and sdk_lib_dir.exists():
            import shutil as _shutil
            _shutil.copy2(str(runtime_core_lib), str(sdk_lib_dir / "chaos_runtime_core.lib"))
            print(f"  [hotupdate] Copied chaos_runtime_core.lib to SDK")

    print(f"  [hotupdate] Rebuilding entry.exe with patchdata...")
    build_dir = native_dir / "build"
    if not build_dir.exists():
        build_dir = native_dir / "build" / "vs2022"
    r = subprocess.run(
        ["cmake", "--build", str(build_dir), "--config", "RelWithDebInfo", "--target", "entry"],
        capture_output=True, text=True, timeout=300,
    )
    if r.returncode != 0:
        print(f"  [hotupdate] entry.exe rebuild FAILED (exit={r.returncode})")
        for line in (r.stderr or "").splitlines()[-10:]:
            print(f"    {line}")
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
    """Run entry.exe --hotupdate and parse JSON result."""
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
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                data = json.loads(line)
                passed = data.get("passedMethods", 0)
                failed = data.get("failedMethods", 0)
                total = data.get("totalMethods", 0)
                status = "passed" if failed == 0 else "failed"
                return {
                    "status": status,
                    "passedMethods": passed,
                    "failedMethods": failed,
                    "totalMethods": total,
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
