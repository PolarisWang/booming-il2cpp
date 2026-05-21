"""Project-test pipeline adapter — bridges old-pipeline codegen functions into project-test framework.

Uses build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py functions
with verification= pointing to testing/project-test/ so all paths resolve correctly.

Key difference from foundation-dll:
  - No subject entrypoints (Subject_N) — project has a real Program.Main
  - convert-to-cpp with --entry-point so Program.Main is the entry
  - Custom runtime-entry.cpp that calls Program.Main directly (no RunFactAll)
  - Fact verification is stdout comparison, not per-method pass/fail
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

from _pipeline.stage_result import StageResult

# ── Old pipeline path (for importing codegen functions) ──────────────
_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[3]  # D:/agent/booming-il2cpp/
_OLD_BOOTSTRAP = _REPO_ROOT / "build" / "toolchains" / "run"  # for testing.trace resolution
_OLD_PIPELINE = _OLD_BOOTSTRAP / "testing" / "foundation_dll"

# Inject old paths before repo-root testing/ shadowing
if str(_OLD_BOOTSTRAP) not in sys.path:
    sys.path.insert(0, str(_OLD_BOOTSTRAP))
if str(_OLD_PIPELINE) not in sys.path:
    sys.path.insert(0, str(_OLD_PIPELINE))

# Shim testing.trace so old-pipeline modules can import it without
# conflicting with the repo-root testing package.
import types as _types
_trace_shim = _types.ModuleType("testing.trace")
_trace_shim.trace_init = lambda *a, **kw: None
_trace_shim.trace = lambda *a, **kw: None
sys.modules["testing.trace"] = _trace_shim

# Foundation-dll pipeline modules — all parameterized via verification= path
from pipeline_native_aot_runner import (  # type: ignore[import-untyped]
    _build_entry_exe,
    _codegen_patch_undefined_labels,
    _fix_runtime_entry,
    _run_convert_to_cpp,
    _write_sentinel_patchdata,
)
from generate_verification_dispatch import (  # type: ignore[import-untyped]
    generate_verification_dispatch,
)


# ── Helpers ──────────────────────────────────────────────────────────

def _project_base(ctx) -> Path:
    """Return the verification base path (parent of slug directory).

    This matches the pattern used by foundation-dll where verification base
    is the parent of family directories. For project-test:
      verification_base = testing/project-test/
      slug = runtime-self-test
      → paths resolve to testing/project-test/runtime-self-test/
    """
    return ctx.root_dir.parent  # testing/project-test/


def _build_managed_project(ctx) -> Path | None:
    """Build the managed project DLL, return path to the DLL."""
    managed_dir = ctx.managed_dir
    csproj_files = list(managed_dir.glob("*.csproj"))
    if not csproj_files:
        return None

    csproj = csproj_files[0]
    build_out = managed_dir / "bin" / "Release" / "net8.0"
    build_out.mkdir(parents=True, exist_ok=True)

    r = subprocess.run(
        ["dotnet", "build", str(csproj), "-c", "Release",
         "--nologo", "-v", "quiet"],
        capture_output=True, text=True, timeout=300,
    )
    if r.returncode != 0:
        print(f"    [codegen] dotnet build FAILED (exit={r.returncode})")
        for line in (r.stderr or "").splitlines()[-5:]:
            print(f"      {line}")
        return None

    # Find the DLL
    dll = build_out / f"{ctx.load_project_meta().get('entryPoint', 'RuntimeSelfTest')}.dll"
    if not dll.exists():
        # Fallback: scan bin output
        for f in sorted(build_out.glob("*.dll")):
            # Skip system DLLs
            if not f.name.startswith(("System.", "Microsoft.", "mscorlib")):
                dll = f
                break
    if not dll.exists():
        print(f"    [codegen] DLL not found in {build_out}")
        return None

    print(f"    [codegen] Managed build OK: {dll.name}")
    return dll


def _find_entry_point_subject_id(ctx) -> str | None:
    """Find the subject ID for Program::Main from project.json subjects list."""
    meta = ctx.load_project_meta()
    subjects: list[str] = meta.get("subjects", [])
    for s in subjects:
        if "Program::Main" in s or "::Main(" in s:
            return s
    return subjects[0] if subjects else None


def _find_entry_method_index(ctx, project_base: Path) -> int:
    """Parse native-aot.methods.json to find the entry point method index.

    SchemaVersion 2 uses nativeSymbol (not methodSubjectId).
    Falls back to 0 if not found (first method in dispatch table).
    """
    codegen_dir = project_base / ctx.slug / "codegen"
    if not codegen_dir.exists():
        return 0

    for d in sorted(codegen_dir.iterdir()):
        if d.is_dir():
            manifest = d / "native-aot.methods.json"
            if manifest.exists():
                try:
                    data = json.loads(manifest.read_text(encoding="utf-8"))
                    methods: list[dict[str, Any]] = data.get("methods", [])
                    entry_subject = _find_entry_point_subject_id(ctx)
                    if entry_subject:
                        # Convert subject ID to nativeSymbol pattern
                        # subject: "GenericsDelegates/Program::Main(System.String[])"
                        # nativeSymbol: "GenericsDelegates_Program_Main_System_String"
                        # Strategy: extract TypeName_MethodName and match specifically
                        method_match = entry_subject.split("::")[-1] if "::" in entry_subject else ""
                        if method_match:
                            method_name = method_match.split("(")[0]
                            # Extract type name from subject: "GenericsDelegates/Program::Main"
                            # -> type_name = "Program"
                            ns_part = entry_subject.split("::")[0] if "::" in entry_subject else ""
                            type_name = ns_part.split("/")[-1] if "/" in ns_part else ""
                            # Build specific pattern: _TypeName_MethodName_
                            # e.g. "_Program_Main_" to match "GenericsDelegates_Program_Main_System_String"
                            # and NOT "GenericsDelegates_AutoGeneratedProgram_Main_..."
                            if type_name:
                                specific_pattern = f"_{type_name}_{method_name}_"
                            else:
                                specific_pattern = f"_{method_name}_"

                        # Scan all methods to find best match
                        best_idx = None
                        best_score = -1
                        best_ns = ""
                        for m in methods:
                            ns = m.get("nativeSymbol", "")
                            if not method_match:
                                continue

                            score = 0
                            # Prefer exact type+method match over generic _Main_ match
                            if specific_pattern in ns:
                                score = 10
                            elif f"_{method_name}_" in ns:
                                score = 5
                            elif ns.endswith(f"_{method_name}"):
                                score = 3

                            if score > best_score:
                                best_score = score
                                best_idx = m.get("index", 0)
                                best_ns = ns

                        if best_idx is not None:
                            print(f"    [codegen] Entry method '{best_ns}' at index {best_idx}")
                            return best_idx
                    # Fallback: return first method index
                    if methods:
                        return methods[0].get("index", 0)
                except (json.JSONDecodeError, OSError):
                    pass
    return 0


def _write_project_runtime_entry(project_base: Path, slug: str, entry_index: int) -> None:
    """Write a project-specific runtime-entry.cpp that calls the entry point directly."""
    codegen_dir = project_base / slug / "codegen"
    codegen_dir.mkdir(parents=True, exist_ok=True)
    target = codegen_dir / "runtime-entry.cpp"

    source = f"""// Auto-generated runtime-entry.cpp for project-test [{slug}]
// Boots chaos-il2cpp runtime and calls the managed entry point method.
// Fact verification is done by comparing stdout with expected output.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>

#include <chaos/log.h>
#include "codegen_bridge.h"
#include "runtime_abi.h"
#include "runtime_core.h"
#include "exception_helpers.h"
#include "patch_loader.h"
#include "hotpatch_table.h"
#include "runtime_stubs/misc_stubs.h"
#include "runtime_stubs/stub_common.h"
#include "string_table.h"
#include "support.h"
#include "jit_registration.h"

// Extern symbols from native-aot.generated.cpp
extern "C" void* kChaosExternalRuntimeFnTable[];
extern "C" const char* kChaosExternalRuntimeSubjects[];
extern "C" int32_t kChaosExternalRuntimeCount;
extern "C" void ChaosJitRegisterAll();
extern "C" std::int32_t RunNativeAot(std::int32_t);
extern "C" double RunBenchmark(int, int);

// Patch data (from runtime-patchdata.cpp, sentinel if no patches)
extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

// Codegen registration structs
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
extern "C" const int kAotMethodCount;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" void SetUnhandledExceptionHook(void (*fn)(CHAOS_IL2CPP_INTPTR));

// Entry point method index (set during codegen from native-aot.methods.json)
static const int kProjectEntryIndex = {entry_index};

// ── Fill unresolved external runtime stubs with safe no-ops ──────────
static void FillExternalRuntimeStubs() {{
    for (int32_t i = 0; i < kChaosExternalRuntimeCount; i++) {{
        if (kChaosExternalRuntimeFnTable[i] != nullptr) continue;
        const char* sub = kChaosExternalRuntimeSubjects[i];
        if (sub == nullptr || sub[0] == '\\0') continue;
        if (std::strstr(sub, "::") == nullptr) continue;

        // Console.WriteLine — extract UTF-8 from managed string, write to stdout.
        // Handles both real string pointers and StringId (tagged hash) values.
        // Subject ID format: System.Console/System.Console::WriteLine:...
        if (std::strstr(sub, "Console::WriteLine:")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(
                +[](CHAOS_IL2CPP_INTPTR str_ptr) -> CHAOS_IL2CPP_INT32 {{
                    const char* utf8 = nullptr;
                    CHAOS_IL2CPP_UINT32 byte_count = 0;
                    if (str_ptr != 0) {{
                        if (chaos_is_string_id(str_ptr)) {{
                            auto id = chaos_extract_string_id(str_ptr);
                            auto sv = chaos::il2cpp::string_table::Resolve(id);
                            if (sv.utf8_data != nullptr) {{
                                utf8 = sv.utf8_data;
                                byte_count = sv.byte_count;
                            }}
                        }} else {{
                            auto* hdr = reinterpret_cast<const StubStringHeader*>(str_ptr);
                            utf8 = stub_string_data(reinterpret_cast<const void*>(str_ptr));
                            byte_count = static_cast<CHAOS_IL2CPP_UINT32>(hdr->byte_count);
                        }}
                    }}
                    if (utf8 != nullptr && byte_count > 0) {{
                        std::fwrite(utf8, 1, byte_count, stdout);
                    }}
                    std::fputc('\\n', stdout);
                    return 0;
                }});
            continue;
        }}

        // Default: return 0 sentinel for all other unresolved methods
        kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(
            +[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 {{ return 0; }});
    }}
}}

// ── Apply hotpatch if patch data is available ───────────────────────
static void ApplyHotpatchIfAvailable() {{
    if (kPatchDataSize > 0u) {{
        auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
            kPatchData, kPatchDataSize, kPatchDataHostClassName);
        if (patch_ctx) {{
            std::fprintf(stderr, "INFO: Applied patches to %u methods\\n",
                         static_cast<unsigned>(patch_ctx->method_count));
        }}
    }}
}}

// ── main ─────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {{
    // Bootstrap chaos-il2cpp runtime via codegen bridge
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) {{ std::fprintf(stderr, "get_bridge failed\\n"); return -1; }}
    bridge->register_codegen(
        &chaos_codegen_code_registration,
        &chaos_codegen_metadata_registration,
        &chaos_codegen_options);
    bridge->bootstrap_runtime();

    // Initialize runtime state
    auto* abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
    if (abi == nullptr) {{ std::fprintf(stderr, "GetRuntimeAbiV0 failed\\n"); return -1; }}

    RuntimeInitParams init_params = {{}};
    init_params.struct_size = sizeof(init_params);
    init_params.init_flags = 0;
    init_params.host_name_utf8 = "runtime-entry";

    RuntimeConfig config = {{}};
    config.struct_size = sizeof(config);
    config.allocator = nullptr;
    config.deallocator = nullptr;

    RuntimeState* runtime_state = nullptr;
    RuntimeStatus status = abi->runtime_init(&init_params, &config, &runtime_state);
    if (status != CHAOS_RUNTIME_STATUS_OK || runtime_state == nullptr) {{
        std::fprintf(stderr, "runtime_init failed (status=%d)\\n", static_cast<int>(status));
        return -1;
    }}
    chaos::il2cpp::runtime_core::SetCurrentRuntimeState(runtime_state);

    ThreadState* thread_state = nullptr;
    status = abi->thread_attach(runtime_state, &thread_state); if (status != CHAOS_RUNTIME_STATUS_OK || thread_state == nullptr) {{
        std::fprintf(stderr, "thread_attach failed (status=%d)\\n", static_cast<int>(status));
        return -1;
    }}

    // Fill unresolved external runtime stubs
    FillExternalRuntimeStubs();
    ChaosJitRegisterAll();

    // Dispatch based on argv
    if (argc >= 2 && std::strcmp(argv[1], "--benchmark") == 0) {{
        int idx = (argc >= 3) ? std::atoi(argv[2]) : kProjectEntryIndex;
        int iters = (argc >= 4) ? std::atoi(argv[3]) : 100000;
        double elapsed = RunBenchmark(idx, iters);
        std::printf(
            "{{\\"elapsedMilliseconds\\":%.3f,\\"opsPerSecond\\":%.1f,"
            "\\"methodIndex\\":%d,\\"iterations\\":%d}}\\n",
            elapsed,
            (elapsed > 0.0) ? (iters / (elapsed / 1000.0)) : 0.0,
            idx, iters);
        std::fflush(stdout);
        _Exit(0);
    }} else if (argc >= 2 && std::strcmp(argv[1], "--hotupdate") == 0) {{
        ApplyHotpatchIfAvailable();
        RunNativeAot(kProjectEntryIndex);
        std::fflush(stdout);
        _Exit(0);
    }} else {{
        // Fact mode: call entry point directly.
        chaos::il2cpp::common::g_chaos_fail_hook = []() {{ chaos::il2cpp::runtime_core::chaos_raise_exception(0); }};
        RunNativeAot(kProjectEntryIndex);
        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
        std::fflush(stdout);
        _Exit(0);  // avoid GC background threads during CRT cleanup
    }}
}}  // close main
"""

    target.write_text(source, encoding="utf-8")
    print(f"    [codegen] Wrote custom runtime-entry.cpp (entry_index={entry_index})")


def _patch_console_writeline(project_base: Path, slug: str) -> None:
    """Post-process generated native-aot.generated.cpp to replace the no-op
    Console.WriteLine implementation with one that writes to stdout.

    The codegen emits Console.WriteLine as a static no-op function:
      (void)chaos_arg_0;

    We patch it inline to extract UTF-8 data and write to stdout, handling
    both real string pointers and StringId (tagged hash) values.
    """
    codegen_dir = project_base / slug / "codegen"
    for d in sorted(codegen_dir.iterdir()):
        if d.is_dir():
            cpp = d / "generated" / "native-aot.generated.cpp"
            if not cpp.exists():
                continue
            old_sig = "static void chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_"
            text = cpp.read_text(encoding="utf-8")
            if old_sig not in text:
                continue

            # Add required includes for the patched implementation
            # (StubStringHeader, stub_string_data, string_table::Resolve, etc.)
            extra_includes = (
                '#include "runtime_stubs/stub_common.h"\n'
                '#include "string_table.h"\n'
            )
            insert_point = '#include "runtime_vtable.h"\n'
            if extra_includes not in text and insert_point in text:
                text = text.replace(insert_point, insert_point + extra_includes)

            # Replace the no-op body with inline implementation
            old_body = f"""{old_sig}(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{{
	(void)chaos_arg_0;
}}"""
            new_body = f"""{old_sig}(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{{
	if (chaos_arg_0 != 0) {{
		const char* utf8 = nullptr;
		CHAOS_IL2CPP_UINT32 byte_count = 0;
		if (chaos_is_string_id(chaos_arg_0)) {{
			auto sv = chaos::il2cpp::string_table::Resolve(chaos_extract_string_id(chaos_arg_0));
			if (sv.utf8_data != nullptr) {{
				utf8 = sv.utf8_data;
				byte_count = sv.byte_count;
			}}
		}} else {{
			auto* hdr = reinterpret_cast<const ::StubStringHeader*>(chaos_arg_0);
			utf8 = ::stub_string_data(reinterpret_cast<const void*>(chaos_arg_0));
			byte_count = static_cast<CHAOS_IL2CPP_UINT32>(hdr->byte_count);
		}}
		if (utf8 != nullptr && byte_count > 0) {{
			std::fwrite(utf8, 1, byte_count, stdout);
		}}
		std::fputc('\\n', stdout);
		std::fflush(stdout);
	}}
}}"""
            cpp.write_text(text.replace(old_body, new_body), encoding="utf-8")
            # Safety net: fix fputc if CRLF or raw newline crept into the char literal
            fixed = cpp.read_bytes()
            # Replace literal CRLF or bare LF between fputc quotes with \n escape
            import re as _re
            fixed = _re.sub(b"fputc\\(\\s*'\\r?\\n'\\s*,\\s*stdout\\s*\\)", b"fputc('\\n', stdout)", fixed)
            if fixed != cpp.read_bytes():
                cpp.write_bytes(fixed)
                # Verify
                done = cpp.read_bytes()
                if b"fputc('\\n', stdout)" in done:
                    print(f"    [codegen] Fixed fputc in {cpp.name}")
                else:
                    # Emergency raw fix
                    raw = cpp.read_bytes()
                    idx = raw.find(b"fputc")
                    if idx > 0:
                        # Find the opening quote after fputc(
                        paren = raw.find(b"(", idx)
                        q1 = raw.find(b"'", paren)
                        q2 = raw.find(b"'", q1 + 1)
                        if q1 > 0 and q2 > q1:
                            replacement = b"fputc('\\n', stdout)"
                            fixed_line = raw[:q1-6] + replacement + raw[q2+1:]
                            # Find the semicolon
                            semi = raw.find(b";", q2)
                            if semi > q2:
                                before_semi = raw[:semi]
                                after_semi = raw[semi:]
                                fixed_line = before_semi[:q1-6] + replacement + after_semi
                                cpp.write_bytes(fixed_line)
                                print(f"    [codegen] Emergency fixed fputc in {cpp.name}")
            print(f"    [codegen] Patched Console.WriteLine in {cpp.name}")
            return

def _generate_project_dispatch_code(project_base: Path, slug: str) -> bool:
    """Generate verification_dispatch.generated.cpp from codegen manifest."""
    codegen_dir = project_base / slug / "codegen"
    if not codegen_dir.exists():
        return False

    manifest_path = None
    for d in sorted(codegen_dir.iterdir()):
        if d.is_dir():
            candidate = d / "native-aot.methods.json"
            if candidate.exists():
                manifest_path = candidate
                break

    if manifest_path is None:
        print(f"    [codegen] manifest not found in {codegen_dir}")
        return False

    dispatch_output = project_base / slug / "native" / "verification_dispatch.generated.cpp"
    dispatch_output.parent.mkdir(parents=True, exist_ok=True)

    generate_verification_dispatch(str(manifest_path), str(dispatch_output))
    return True


def _write_project_cmakelists(project_base: Path, slug: str) -> None:
    """Write a project-specific CMakeLists.txt that doesn't require *Subjects/ directory names.

    The auto-generated template from pipeline_native_aot_runner._ensure_cmakelists()
    uses *Subjects/generated/*.cpp GLOB pattern which doesn't match real assembly names.
    """
    native_dir = project_base / slug / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
    cmakelists = native_dir / "CMakeLists.txt"

    repo_root_str = str(_REPO_ROOT).replace("\\", "/")
    codegen_rel = str((project_base / slug / "codegen").resolve()).replace("\\", "/")
    native_build = str((_REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference").resolve()).replace("\\", "/")

    content = (
        f'cmake_minimum_required(VERSION 3.20)\n'
        f'project(chaos_entry CXX)\n'
        f'set(CMAKE_CXX_STANDARD 20)\n'
        f'\n'
        f'add_compile_options(/utf-8)\n'
        f'add_compile_definitions(CHAOS_IL2CPP_EH_WIN32_SEH)  # chaos-il2cpp SEH (Windows)\n'
        f'add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)\n'
        f'add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=0)\n'
        f'\n'
        f'# Paths\n'
        f'set(CHAOS_PROJECT_ROOT "{repo_root_str}")\n'
        f'set(CHAOS_CODEGEN_DIR "{codegen_rel}")\n'
        f'set(CHAOS_NATIVE_BUILD "{native_build}")\n'
        f'\n'
        f'# Source files — GLOB matches any assembly directory (not just *Subjects/)\n'
        f'file(GLOB CHAOS_CODEGEN_CPP "${{CHAOS_CODEGEN_DIR}}/*/generated/native-aot.generated.cpp")\n'
        f'file(GLOB CHAOS_NATIVE_STUBS "*.cpp")\n'
        f'list(REMOVE_ITEM CHAOS_NATIVE_STUBS\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-entry.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-patchdata.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/verification_dispatch.generated.cpp"\n'
        f')\n'
        f'set(CHAOS_ENTRY_SOURCES\n'
        f'    "runtime-entry.cpp"\n'
        f'    "runtime-patchdata.cpp"\n'
        f'    "verification_dispatch.generated.cpp"\n'
        f'    ${{CHAOS_NATIVE_STUBS}}\n'
        f'    ${{CHAOS_CODEGEN_CPP}}\n'
        f')\n'
        f'\n'
        f'set(CHAOS_ENTRY_INCLUDES\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/common"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/gc"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/bootstrap"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter/generated"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/codegen"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/support"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/hot-update"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/contracts/native/v0"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/third_party/fmt/include"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/third_party/fmt/include"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/third_party/unordered_dense/include"\n'
        f'    "${{CHAOS_CODEGEN_DIR}}/RuntimeSelfTest/generated"\n'
        f')\n'
        f'\n'
        f'set(CHAOS_LIB_DIRS\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/runtime-core"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/bootstrap"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/common"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/interpreter"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/codegen"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/support"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/hot-update"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/fmt_build"\n'
        f')\n'
        f'\n'
        f'set(CHAOS_RUNTIME_LIBS\n'
        f'    chaos_runtime_core\n'
        f'    chaos_bootstrap\n'
        f'    chaos_common\n'
        f'    chaos_interpreter\n'
        f'    chaos_codegen\n'
        f'    chaos_support\n'
        f'    chaos_hot_update\n'
        f'    chaos_fmt\n'
        f')\n'
        f'\n'
        f'add_executable(entry ${{CHAOS_ENTRY_SOURCES}})\n'
        f'target_include_directories(entry PRIVATE ${{CHAOS_ENTRY_INCLUDES}})\n'
        f'target_link_directories(entry PRIVATE ${{CHAOS_LIB_DIRS}})\n'
        f'target_compile_options(entry PRIVATE /EHa /GS-)\n'
        f'target_link_libraries(entry PRIVATE ${{CHAOS_RUNTIME_LIBS}})\n'
    )
    cmakelists.write_text(content, encoding="utf-8")
    print(f"    [codegen] Wrote project-specific CMakeLists.txt")


def _save_aot_binary(ctx) -> None:
    """Save entry.exe as entry-aot.exe (always overwrite)."""
    native_dir = ctx.native_dir
    entry_exe = native_dir / "entry.exe"
    aot_exe = native_dir / "entry-aot.exe"
    if entry_exe.exists():
        shutil.copy2(str(entry_exe), str(aot_exe))
        print(f"    [codegen] Saved entry.exe -> entry-aot.exe")


def _save_jit_binary(ctx) -> None:
    """Save entry.exe as entry-jit.exe then restore AOT binary."""
    native_dir = ctx.native_dir
    entry_exe = native_dir / "entry.exe"
    jit_exe = native_dir / "entry-jit.exe"
    if entry_exe.exists():
        shutil.copy2(str(entry_exe), str(jit_exe))
        print(f"    [codegen] Saved entry.exe -> entry-jit.exe")
    # Restore AOT binary so subsequent AOT stages get native dispatch
    aot_exe = native_dir / "entry-aot.exe"
    if aot_exe.exists():
        shutil.copy2(str(aot_exe), str(entry_exe))
        print(f"    [codegen] Restored entry-aot.exe -> entry.exe")


# ── Emit-patch-data helpers ──────────────────────────────────────────

_DRIVER_DLL = (
    _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"
    / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
)


def _build_patch_dll(ctx, patch_src_dir: Path | None = None) -> Path | None:
    """Build the patch DLL from patch/src/ and return the DLL path.

    The patch project lives at:
        testing/project-test/<slug>/patch/src/PatchProject.csproj
    """
    if patch_src_dir is None:
        patch_src_dir = ctx.root_dir / "patch" / "src"
    csproj_files = list(patch_src_dir.glob("*.csproj"))
    if not csproj_files:
        print(f"    [hotupdate] No .csproj found in {patch_src_dir}")
        return None

    csproj = csproj_files[0]
    build_out = patch_src_dir / "bin" / "Release" / "net8.0"
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

    # Extract AssemblyName from csproj to find the correct DLL
    csproj_text = csproj.read_text(encoding="utf-8")
    m = re.search(r'<AssemblyName>([^<]+)</AssemblyName>', csproj_text)
    if m:
        dll_name = m.group(1) + ".dll"
    else:
        dll_name = csproj.stem + ".dll"

    dll = build_out / dll_name
    if not dll.exists():
        # Fallback: scan for non-infrastructure DLLs
        for f in sorted(build_out.glob("*.dll")):
            name = f.name
            if name.startswith(("System.", "Microsoft.", "mscorlib",
                               "FsCheck", "FSharp", "Newtonsoft",
                               "NuGet", "xunit", "testhost",
                               "Microsoft.TestPlatform", "Microsoft.VisualStudio")):
                continue
            dll = f
            break
    if not dll.exists():
        print(f"    [hotupdate] Patch DLL not found in {build_out}")
        return None

    print(f"    [hotupdate] Patch DLL built: {dll.name}")
    return dll


def _run_emit_patch_data(
    dll_path: str,
    output_path: str,
    aot_core_ir_path: str | None = None,
) -> bool:
    """Run chaos-il2cpp emit-patch-data CLI on a patch DLL.

    Args:
        dll_path: Path to the patch DLL
        output_path: Output .patchdata path
        aot_core_ir_path: Optional path to aot-core-ir.json for pre-lowered IR
    Returns:
        True if successful, False otherwise
    """
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

    # Print the success message
    last_line = (r.stdout or "").strip().splitlines()[-1] if r.stdout else ""
    print(f"    [hotupdate] {last_line}")
    return True


def _generate_runtime_patchdata_cpp(
    patchdata_path: Path,
    output_cpp_path: Path,
    host_class_name: str,
) -> bool:
    """Generate runtime-patchdata.cpp from a .patchdata binary.

    Embeds the binary data as a C++ byte array with kPatchData, kPatchDataSize,
    and kPatchDataHostClassName symbols.
    """
    if not patchdata_path.exists():
        return False

    data = patchdata_path.read_bytes()
    size = len(data)

    # Format as 16 hex bytes per line
    hex_lines = []
    for i in range(0, size, 16):
        chunk = data[i:i + 16]
        hex_str = ", ".join(f"0x{b:02x}" for b in chunk)
        hex_lines.append(f"{hex_str},")

    hex_body = "\n".join(hex_lines)
    # Remove trailing comma from last line
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


def _write_sentinel_patchdata_for_project(project_base: Path, slug: str) -> None:
    """Write a sentinel runtime-patchdata.cpp (size=0) to disable hotpatch."""
    native_dir = project_base / slug / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
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


# ── Main stage runner ────────────────────────────────────────────────

def run_project_codegen(ctx, stages) -> StageResult:
    """Stage 1: Full codegen pipeline for a complete project.

    Flow:
      1. dotnet build managed project → DLL
      2. chaos-il2cpp convert-to-cpp (with --entry-point for Program.Main)
      3. Patch undefined branch target labels
      4. Generate verification dispatch code (RunBenchmark etc.)
      5. Write project-specific runtime-entry.cpp (calls entry point directly)
      6. Build entry.exe via CMake
      7. Save entry-aot.exe for fact verification
    """
    start = time.perf_counter()
    project_base = _project_base(ctx)

    # Step 1: Build managed project
    print(f"  [codegen] Building managed project...")
    dll_path = _build_managed_project(ctx)
    if dll_path is None:
        return StageResult(
            stage="codegen", status="failed",
            summary="Managed project build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Isolate DLL to clean input directory (avoids metadata pollution)
    clean_input_dir = project_base / ctx.slug / "codegen" / "_input"
    clean_input_dir.mkdir(parents=True, exist_ok=True)
    clean_dll = clean_input_dir / dll_path.name
    shutil.copy2(str(dll_path), str(clean_dll))

    # Step 2: Run convert-to-cpp with entry point
    entry_point_id = _find_entry_point_subject_id(ctx)
    print(f"  [codegen] Running IL2CPP codegen (entry: {entry_point_id})...")
    # NOTE: entry_point_subject_id="" means no --entry-point flag, which is correct
    # for projects. The codegen converts all methods; our custom runtime-entry.cpp
    # dispatches to the entry method via its index in the AOT dispatch table.
    cg_ok = _run_convert_to_cpp(
        ctx.slug,
        str(clean_dll),
        verification=project_base,
        entry_point_subject_id="",  # no entry point override
        codegen_mode=None,
    )
    if not cg_ok:
        return StageResult(
            stage="codegen", status="failed",
            summary="IL2CPP convert-to-cpp failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Step 3: Patch Console.WriteLine (generated as no-op by codegen)
    _patch_console_writeline(project_base, ctx.slug)

    # Step 4: Patch undefined branch target labels
    _codegen_patch_undefined_labels(ctx.slug, verification=project_base)

    # Step 4: Find entry method index from manifest
    entry_index = _find_entry_method_index(ctx, project_base)

    # Step 5: Generate verification dispatch code
    print(f"  [codegen] Generating dispatch code...")
    _generate_project_dispatch_code(project_base, ctx.slug)

    # Step 6: Write project-specific runtime-entry.cpp (must be before _build_entry_exe
    # since _build_entry_exe copies from codegen/ to native/)
    _write_project_runtime_entry(project_base, ctx.slug, entry_index)

    # Step 6b: Write project-specific CMakeLists.txt (avoids *Subjects/ GLOB mismatch)
    _write_project_cmakelists(project_base, ctx.slug)

    # Step 7: Build entry.exe
    print(f"  [codegen] Building entry.exe...")
    build_ok = _build_entry_exe(ctx.slug, verification=project_base)
    if not build_ok:
        return StageResult(
            stage="codegen", status="failed",
            summary="entry.exe build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Step 8: Save AOT binary
    _save_aot_binary(ctx)

    return StageResult(
        stage="codegen", status="passed",
        summary=f"Project codegen OK (entry={entry_point_id})",
        details={
            "entryPoint": entry_point_id,
            "entryIndex": entry_index,
            "dllPath": str(clean_dll),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_project_jit_codegen(ctx, stages) -> StageResult:
    """JIT codegen pipeline — same flow as AOT but with --mode jit.

    Flow:
      1. dotnet build managed project → DLL
      2. chaos-il2cpp convert-to-cpp with codegen_mode="jit"
      3. Patch undefined branch target labels
      4. Generate verification dispatch code
      5. Write project-specific runtime-entry.cpp
      6. Build entry.exe via CMake
      7. Save entry-jit.exe and restore entry-aot.exe → entry.exe
    """
    start = time.perf_counter()
    project_base = _project_base(ctx)

    # Step 1: Build managed project
    print(f"  [jit_codegen] Building managed project...")
    dll_path = _build_managed_project(ctx)
    if dll_path is None:
        return StageResult(
            stage="jit_codegen", status="failed",
            summary="Managed project build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Step 2: Run convert-to-cpp with JIT mode
    clean_input_dir = project_base / ctx.slug / "codegen" / "_jit_input"
    clean_input_dir.mkdir(parents=True, exist_ok=True)
    clean_dll = clean_input_dir / dll_path.name
    shutil.copy2(str(dll_path), str(clean_dll))

    entry_point_id = _find_entry_point_subject_id(ctx)
    print(f"  [jit_codegen] Running IL2CPP codegen (mode=jit, entry={entry_point_id})...")
    cg_ok = _run_convert_to_cpp(
        ctx.slug,
        str(clean_dll),
        verification=project_base,
        entry_point_subject_id="",
        codegen_mode="jit",
    )
    if not cg_ok:
        return StageResult(
            stage="jit_codegen", status="failed",
            summary="IL2CPP convert-to-cpp (jit) failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Step 3: Patch Console.WriteLine
    _patch_console_writeline(project_base, ctx.slug)

    # Step 4: Patch undefined branch target labels
    _codegen_patch_undefined_labels(ctx.slug, verification=project_base)

    # Step 5: Find entry method index from manifest
    entry_index = _find_entry_method_index(ctx, project_base)

    # Step 6: Generate verification dispatch code
    print(f"  [jit_codegen] Generating dispatch code...")
    _generate_project_dispatch_code(project_base, ctx.slug)

    # Step 7: Write project-specific runtime-entry.cpp
    _write_project_runtime_entry(project_base, ctx.slug, entry_index)

    # Step 7b: Write project-specific CMakeLists.txt
    _write_project_cmakelists(project_base, ctx.slug)

    # Step 8: Build entry.exe
    print(f"  [jit_codegen] Building entry.exe (JIT mode)...")
    build_ok = _build_entry_exe(ctx.slug, verification=project_base)
    if not build_ok:
        return StageResult(
            stage="jit_codegen", status="failed",
            summary="entry.exe (jit) build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Step 9: Save entry-jit.exe and restore AOT binary
    _save_jit_binary(ctx)

    # Clean up jit input directory
    if clean_input_dir.exists():
        import shutil as _shutil
        _shutil.rmtree(str(clean_input_dir))

    return StageResult(
        stage="jit_codegen", status="passed",
        summary=f"JIT codegen OK (entry={entry_point_id})",
        details={
            "entryPoint": entry_point_id,
            "entryIndex": entry_index,
            "codegenMode": "jit",
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
