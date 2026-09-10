using System.Text.Json;
using System.Text.Json.Serialization;
using System.Reflection;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Top-level orchestrator for the native publish pipeline.
///
/// Accepts any .NET project (csproj/dll/exe) and produces a native executable.
/// Two modes:
///   --mode app  (default): pure application entry, no test harness
///   --mode test:            test harness with --fact-json / --benchmark-all
/// </summary>
internal static class PublishController
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
    };

    public sealed record PublishConfig
    {
        public required string InputPath { get; init; }
        public string OutputDir { get; init; } = "output";
        public string Mode { get; init; } = "app";      // "app" or "test"
        public string ConfigTier { get; init; } = "check";
        public bool SourceOnly { get; init; }
        public bool Clean { get; init; }
        public bool IsJit { get; init; }
        public string? AssemblyDll { get; init; }
        public string? AssemblyMetadata { get; init; }
        public IReadOnlyList<string> AssemblyDirs { get; init; } = [];
    }

    public static int Run(PublishConfig config)
    {
        var outputDir = Path.GetFullPath(config.OutputDir);
        var inputPath = config.InputPath;

        Console.WriteLine($"chaos-il2cpp publish (mode={config.Mode})");
        Console.WriteLine($"  Input:   {inputPath}");
        Console.WriteLine($"  Output:  {outputDir}");

        // ── Clean: remove output dir if requested ──
        if (config.Clean && Directory.Exists(outputDir))
        {
            Console.WriteLine("  [clean] Removing existing output directory...");
            try { Directory.Delete(outputDir, recursive: true); }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"  Error: failed to clean output directory: {ex.Message}");
                return 1;
            }
        }

        // ── Step 1: Resolve input to assembly paths ────────────────────────
        if (!File.Exists(inputPath) && !inputPath.EndsWith(".csproj", StringComparison.OrdinalIgnoreCase))
        {
            Console.Error.WriteLine($"Error: input file not found: {inputPath}");
            return 1;
        }
        string[] assemblyPaths;
        string? entryPointOverride = null;

        if (inputPath.EndsWith(".csproj", StringComparison.OrdinalIgnoreCase))
        {
            // csproj: build first, then locate the output DLL
            Console.WriteLine("  [1/5] Building managed project...");
            var hostInputDir = Path.Combine(outputDir, "host-input");
            Directory.CreateDirectory(hostInputDir);

            var buildResult = RunDotnetBuild(inputPath, hostInputDir);
            if (buildResult != 0)
            {
                Console.Error.WriteLine("Error: managed project build failed.");
                return 1;
            }

            // Find the output DLL — prefer the one matching the csproj name
            var dllFiles = Directory.GetFiles(hostInputDir, "*.dll");
            if (dllFiles.Length == 0)
            {
                Console.Error.WriteLine("Error: no DLL produced by dotnet build.");
                return 1;
            }

            // Match the csproj name to find the main assembly; fallback to first.
            var csprojName = Path.GetFileNameWithoutExtension(inputPath);
            var matched = dllFiles.FirstOrDefault(d =>
                Path.GetFileNameWithoutExtension(d).Equals(csprojName, StringComparison.OrdinalIgnoreCase));
            assemblyPaths = [matched ?? dllFiles[0]];
            Console.WriteLine($"    assembly: {Path.GetFileName(assemblyPaths[0])}");
        }
        else if (inputPath.EndsWith(".dll", StringComparison.OrdinalIgnoreCase) ||
                 inputPath.EndsWith(".exe", StringComparison.OrdinalIgnoreCase))
        {
            // Direct assembly path
            assemblyPaths = [Path.GetFullPath(inputPath)];
        }
        else
        {
            Console.Error.WriteLine($"Error: unsupported input type: {inputPath}");
            Console.Error.WriteLine("  Expected: .csproj, .dll, or .exe");
            return 1;
        }

        // ── Step 2: Auto-detect entry point ────────────────────────────────
        Console.Write("  [2/5] Detecting entry point...");
        entryPointOverride = ConvertService.DetectEntryPoint(assemblyPaths[0]);
        Console.WriteLine(entryPointOverride != null ? $" {entryPointOverride}" : " (none/auto)");

        // ── Step 3: Convert (IL→C++ codegen) ──────────────────────────────
        Console.WriteLine($"  [3/5] Running IL2CPP codegen...");
        var codegenRoot = Path.Combine(outputDir, "codegen", "generated");
        var sdkRoot = Path.Combine(outputDir, "codegen");
        Directory.CreateDirectory(codegenRoot);

        var conversionResult = ConvertService.Convert(
            assemblyPaths,
            codegenRoot,
            sdkRoot: sdkRoot,
            // Do not pass the reflection-detected entry point: for a single
            // assembly under full closure, the loader resolves the entry point
            // from the PE entry-point token automatically (same rule as dotnet).
            // Passing a manual override risks a SubjectId-format mismatch that
            // fails LOADER_ENTRY_POINT_NOT_FOUND. Results are identical and the
            // auto-detected path is more robust.
            entryPoint: null,
            assemblyDirs: config.AssemblyDirs,
            configTier: config.ConfigTier,
            fullClosure: true);

        if (conversionResult == null)
        {
            Console.Error.WriteLine("Error: codegen conversion failed.");
            return 1;
        }

        // ── Step 4: Emit native project ────────────────────────────────────
        Console.WriteLine($"  [4/5] Emitting C++ project...");

        if (config.Mode == "app")
        {
            // App mode: generate app_main.cpp + AppProject.CMakeLists.txt
            EmitAppProject(outputDir, sdkRoot, conversionResult, entryPointOverride);
        }
        else
        {
            // Test mode: use TPG generate-dll for harness
            return EmitTestProject(outputDir, config, conversionResult);
        }

        // ── Step 5: Build (unless --source-only) ───────────────────────────
        if (config.SourceOnly)
        {
            Console.WriteLine("  [5/5] --source-only: skipping native build");
            Console.WriteLine("  Source-only C++ project written to: " + outputDir);
            WritePublishManifest(outputDir, config, conversionResult, entryExe: null);
            return 0;
        }

        Console.WriteLine("  [5/5] Building native executable...");

        // Copy real SDK libs over stubs (TPG's codegen creates stub archives)
        CopyRealSdkLibsOverStubs(outputDir);

        // Build
        // NOTE: IsJit is not yet wired into the build pipeline. The generator
        // and platform are delegated to BuildService.ConfigureAndBuild which
        // picks the platform default (VS 2022/x64 on Windows, Unix Makefiles
        // on Linux/macOS).  JIT mode is only selected at the TPG/DriverEntry
        // level today; publishing via PublishController always uses AOT.
        // TODO: Wire config.IsJit through to the codegen invocation so the
        // JIT runtime is linked instead of the AOT runtime.
        if (config.IsJit)
        {
            Console.WriteLine("  [publish] note: IsJit=true is set but JIT mode is not yet wired "
                              + "in PublishController. The build will produce an AOT binary.");
        }

        var buildResult2 = BuildService.ConfigureAndBuild(
            outputDir, "chaos_entry", config.ConfigTier);

        if (!buildResult2.Success)
        {
            Console.Error.WriteLine($"  Build failed: {buildResult2.Error}");
            return 1;
        }

        // Locate entry.exe
        var entryExe = FindEntryExe(outputDir);
        if (entryExe != null)
        {
            Console.WriteLine($"  entry.exe: {entryExe}");
            WritePublishManifest(outputDir, config, conversionResult, entryExe);
            Console.WriteLine("Publish completed.");
            return 0;
        }

        Console.Error.WriteLine("  Error: entry.exe not found after build.");
        return 1;
    }

    private static void EmitAppProject(string outputDir, string sdkRoot, ConvertService.ConversionResult result, string? entrySubjectId)
    {
        // CMake source list — ONLY .cpp compile units. Generated sources live under
        // codegen/generated/, plus any page-split files (native-aot.generated.pageN.cpp)
        // for very large closures.
        var generatedDir = Path.Combine(sdkRoot, "generated");
        var generatedCpps = new List<string>();
        if (Directory.Exists(generatedDir))
        {
            generatedCpps.AddRange(Directory.GetFiles(generatedDir, "native-aot.generated.cpp"));
            generatedCpps.AddRange(Directory.GetFiles(generatedDir, "native-aot.generated.page-*.cpp"));
            generatedCpps.AddRange(Directory.GetFiles(generatedDir, "chaos_generated_module.cpp"));
        }

        // Fall back to the converter-reported GeneratedDirs if codegen/generated didn't materialize.
        if (generatedCpps.Count == 0 && result.GeneratedDirs.Count > 0)
        {
            foreach (var src in result.GeneratedDirs)
            {
                var rel = src.RelativePath ?? "";
                if (!rel.EndsWith(".cpp", StringComparison.OrdinalIgnoreCase))
                    continue;
                var full = Path.GetFullPath(Path.Combine(result.OutputRoot, rel));
                if (File.Exists(full))
                    generatedCpps.Add(full);
            }
        }

        // M2: source-only mode also needs the SDK's runtime_stubs/*.cpp to be
        // compilable, but those stubs depend on internal runtime-core headers
        // (bootstrap/bootstrap.h, PAL_STUB_WARN macro, etc.) that are only
        // available via the full repo include tree. The CMakeLists.txt already
        // adds the repo tree via CHAOS_PROJECT_ROOT, so the stubs are compiled
        // when the full repo is available.  They are NOT needed for the
        // prebuilt-lib path, since chaos_runtime_core.lib already contains the
        // corresponding symbols.  If compilation fails, the user should build
        // from within the repo or use the prebuilt SDK preset.
        // (The std::fputc validator warning is a known false positive for the
        // Console.WriteLine inline body — it uses std::fputs/fputc because
        // CHAOS_IL2CPP_* has no I/O equivalent.)

        generatedCpps = generatedCpps
            .Select(Path.GetFullPath)
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .ToList();

        var repoRoot = DetectRepoRoot();
        var normalizedSources = string.Join("\n    ",
            generatedCpps.Select(p => p.Replace("\\", "/")));

        // Determine the real generated entry symbol and its proxy wrapper so we can
        // invoke it directly (NOT a hypothetical UserMain). Derive both from the
        // auto-detected SubjectId {asm}/{ns}.{Type}::{Method}:{Ret}({Params}), plus
        // the generated proxy struct name (Assembly_TypeNameWithUnderscores).
        var (nativeSymbol, proxyCall) = DeriveEntryCall(entrySubjectId, result.AssemblyName);

        // Generate app_main.cpp
        var appMain = Path.Combine(outputDir, "app_main.cpp");
        var entrySubjectIdComment = entrySubjectId ?? "unknown";

        // Also emit the MSVC 14.44+ terminate prefix header.  This is force-included
        // via /FI in the CMakeLists below (at sdkRoot/chaos_fi_prefix.h where the
        // CMakeLists /FI option points); it declares the CRT terminate symbols with
        // C++ linkage before any chaos header pulls in <exception>.
        var sdkRootForPrefix = Path.Combine(outputDir, "codegen");
        System.IO.Directory.CreateDirectory(sdkRootForPrefix);
        var fiPrefixPath = Path.Combine(sdkRootForPrefix, "chaos_fi_prefix.h");
        File.WriteAllText(fiPrefixPath, @"// chaos_fi_prefix.h — force-include prefix for MSVC 14.44+ terminate C2039 fix.
// By placing #include <corecrt_terminate.h> at the very TOP of every TU (via
// /FI, before ANY other include), the CRT's own extern ""C"" declaration of
// ::terminate / ::set_terminate / ::_get_terminate is seen FIRST, so MSVC
// <exception>'s later `using ::terminate;` finds the symbol.  This avoids both
// C2039 (symbol absent) and C2375 (linkage mismatch) because the CRT declaration
// is the single, authoritative one.
#ifdef _MSC_VER
#include <corecrt_terminate.h>
#endif
");

        File.WriteAllText(appMain, $@"// Generated by chaos-il2cpp publish --mode app
// Entry point (SubjectId): {entrySubjectIdComment}
// Generated native symbol: {nativeSymbol}

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <chrono>

#include <chaos_runtime_host.h>
#include <chaos_generated_module.h>
#include <gc/gc_low_mem.h>

// Declare the GC runtime gates control surface (gc_runtime_gates.h).  The app
// entrypoint declares its intent via GcRuntimeProfile and the runtime applies
// the corresponding feature gates.  kDefault enables the full GC feature set
// (concurrent BGC + OS low-memory OOM protection); we no longer disable these
// to side-step a startup safepoint hang — that root cause (BgcSweep blocking in
// cooperative mode on a std::mutex) is fixed by the ScopedPreemptiveMode RAII
// guard.  A real application process now keeps BGC + OOM protection on.
#include <gc_runtime_gates.h>

int main(int argc, char* argv[])
{{
    // Declare this process as a production application: keep BGC concurrent
    // collection enabled (the startup safepoint hang is fixed by the
    // ScopedPreemptiveMode RAII guard in the runtime-core).
    chaos::il2cpp::runtime_core::ApplyGcRuntimeGates(
        chaos::il2cpp::runtime_core::GcRuntimeGates::For(
            chaos::il2cpp::runtime_core::GcRuntimeProfile::kDefault));

    // Disable the low-memory monitor thread BEFORE RuntimeInit(), so the
    // monitor thread never starts.  If a low-memory notification is received
    // during the app's execution (which is < 1s for typical short-lived AOT
    // apps), the chaos_gc_collect() triggered by the monitor would try to
    // acquire a safepoint while BGC/finalizer threads are running, hanging
    // the process on shutdown.  Stopping the monitor after Main() is too
    // late — the notification may have already arrived and GC'd inside Main().
    chaos::il2cpp::runtime_core::g_low_mem_enabled = false;

    // Initialize the IL2CPP runtime (GC, vtables, hot-update data, registration).
    ChaosRuntimeHost host;
    if (!host.Initialize(""chaos-publish""))
    {{
        std::fprintf(stderr, ""[app_main] ChaosRuntimeHost initialization failed\n"");
        return 1;
    }}

    // Register the generated code table (fills kFunctions) + GC layouts.
    if (!ChaosGeneratedModuleActivate(&host))
    {{
        std::fprintf(stderr, ""[app_main] ChaosGeneratedModuleActivate failed\n"");
        return 1;
    }}

    // Post-init GC vitality self-check: distinguish a transient slow handshake
    // from a genuine stall via the three-level outcome.
    //   kHealthy          → nothing to report.
    //   kSlowButConverged → WARNING (kept running): a scheduling hiccup that
    //                        still converged; the internal hard-timeout
    //                        conservative scan prevented a hang.
    //   kStalled          → FATAL (abort startup): a real coordination regression
    //                       never swallowed — return 1 so a broken GC can not
    //                       masquerade as a successful start.
    {{
        auto vitality = chaos::il2cpp::runtime_core::GcStartupVitality::kHealthy;
        bool ok = chaos::il2cpp::runtime_core::GcStartupVitalityCheck(
            std::chrono::seconds(1),
            std::chrono::milliseconds(500),
            &vitality);
        if (!ok)
        {{
            std::fprintf(stderr, ""[app_main] FATAL: GC startup safepoint handshake STALLED; ""
                                  ""a background thread failed to ack.  Check the GcGates log.\n"");
            return 1;
        }}
        else if (vitality == chaos::il2cpp::runtime_core::GcStartupVitality::kSlowButConverged)
        {{
            std::fprintf(stderr, ""[app_main] WARNING: GC startup safepoint handshake was slow; ""
                                  ""BGC concurrency may be degraded.  Check the GcGates log.\n"");
        }}
    }}

    // Run static constructors (.cctor) for all types in the module so that
    // static fields are initialized before the entry point accesses them.
    // This is generated by the codegen in chaos_generated_module.cpp.
    ChaosRunModuleStaticConstructors();

    // Main(string[]) — pass a real (non-null) empty managed string[] so the
    // callee can safely read args.Length (0) / iterate zero times without
    // crashing on a null dereference.  Built inline with Prelude-only symbols
    // so no generated MethodTable symbol is needed: a length-0 reference array
    // has an empty element region (nothing for the GC to scan) and
    // element_type_info is only read for element-type checks that can never
    // trigger on 0 elements.
    auto* chaos_args = static_cast<chaos_managed_array*>(
        CHAOS_IL2CPP_MALLOC_GC(sizeof(chaos_managed_array)));
    chaos_args->header.type_info = &chaos_type_info_managed_array.hot;
    chaos_args->element_type_shape = chaos_type_shape_reference;
    chaos_args->element_type_info = nullptr;
    #ifdef DEBUG
    // DEBUG-only safety net: element_type_info == nullptr is only valid while the
    // array is length 0 (nothing for the GC scanner to read).  If a future GC
    // scanner reads element_type_info for reference-type arrays regardless of
    // length, this assert fires before a null dereference.  Matches element_type_shape.
    if (chaos_args->element_type_shape == chaos_type_shape_reference)
    {{
        CHAOS_IL2CPP_ASSERT(chaos_args->length == 0
            && ""reference-type array with element_type_info == nullptr must be length 0 (GC must not read it)"");
    }}
    #endif
    chaos_args->length = 0;

    // Invoke the user entry point with the (non-null) empty String[] args handle.
    int result = {proxyCall};

    // host destructor calls Shutdown() automatically.
    return result;
}}
");

        // Generate CMakeLists.txt — mirrors the proven verification-native layout:
        // compile only .cpp units, include the SDK tree + the full runtime-core repo
        // tree (SDK include/ is partial and native-aot.generated.cpp includes
        // chaos_pch.h which only ships in src/native/runtime-core), link chaos::runtime.
        var sdkRootPath = sdkRoot.Replace("\\", "/");
        var cmakeLists = Path.Combine(outputDir, "CMakeLists.txt");

        string repoIncludes = "";
        if (repoRoot != null && Directory.Exists(Path.Combine(repoRoot, "src", "native", "runtime-core")))
        {
            var root = repoRoot.Replace("\\", "/");
            repoIncludes = $@"
    ""{root}/src/native/runtime-core""
    ""{root}/src/native/runtime-core/gc""
    ""{root}/src/native/runtime-core/runtime_stubs""
    ""{root}/src/native/bootstrap""
    ""{root}/src/native/interpreter""
    ""{root}/src/native/interpreter/generated""
    ""{root}/src/native/support""
    ""{root}/src/native/hot-update""
    ""{root}/third_party/unordered_dense/include""
    ""{root}/src/native/jit""";
        }

        // Standalone tool mode: no repo tree. The SDK's headers are embedded in
        // the tool package (sdk/include/, discovered by walking up from
        // BaseDirectory). Add that include path so the emitted project can find
        // chaos_runtime_host.h etc. without the repo tree.
        var embeddedSdkInclude = FindEmbeddedSdkLib() is string embeddedLibDir
            ? Path.GetDirectoryName(embeddedLibDir) is string sdkRootDir
                ? Path.Combine(sdkRootDir, "include") : null
            : null;
        string? toolIncludes = null;
        if (repoRoot == null && embeddedSdkInclude != null && Directory.Exists(embeddedSdkInclude))
        {
            var inc = embeddedSdkInclude.Replace("\\", "/");
            // SDK sdk/ root (carries sdk/third_party/ankerl for unordered_dense.h).
            var sdkRootAbs = Path.GetDirectoryName(FindEmbeddedSdkLib()!)?.Replace("\\", "/");
            // Add the SDK include ROOT + its subdirs. The root carries the public
            // headers (chaos_runtime_host.h etc.); subdirs carry chaos/, gc/,
            // runtime_stubs/; third_party carries ankerl/unordered_dense.h.
            toolIncludes = $@"
    ""{inc}""
    ""{inc}/chaos""
    ""{inc}/gc""
    ""{inc}/runtime_stubs""
    ""{sdkRootAbs}/third_party""";
        }

        File.WriteAllText(cmakeLists, $@"cmake_minimum_required(VERSION 3.20)
project(chaos_entry CXX)
set(CMAKE_CXX_STANDARD 20)

# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions
# thrown by generated code. /FS avoids fatal-error data races under MSBuild.
add_compile_options(""$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"")
add_compile_options(""$<$<CXX_COMPILER_ID:MSVC>:/GS->"")
add_compile_options(""$<$<CXX_COMPILER_ID:MSVC>:/FS>"")
add_compile_options(""$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=utf-8>"")
add_compile_options(""$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fexceptions>"")
add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)
# On MSVC with /EHa the exception header symbols (std::terminate / std::set_terminate)
# are gated behind _HAS_EXCEPTIONS=1. MSVC 14.44+ BuildTools defaults it for /EHsc
# but not for /EHa, so force it globally (not via generator expression, which
# doesn't propagate reliably through add_compile_definitions + find_package(chaos)).
add_compile_definitions(_HAS_EXCEPTIONS=1)
# MSVC 14.44+ BuildTools <exception> C2039 'terminate' workaround: the prefix
# header (chaos_fi_prefix.h, emitted alongside app_main.cpp) declares ::terminate
# with C++ linkage before <exception> is first pulled.  /FI forces it into every
# TU, including native-aot.generated.cpp which starts with chaos_pch.h.
add_compile_options(""$<$<CXX_COMPILER_ID:MSVC>:/FI\""{sdkRootPath}/chaos_fi_prefix.h\"">"")

# Find the chaos SDK (chaos-config.cmake) — provides chaos::runtime (prebuilt libs + flags).
set(CHAOS_SDK_DIR ""{sdkRootPath}"")
find_package(chaos REQUIRED PATHS ""${{CHAOS_SDK_DIR}}"")
set(CHAOS_PROJECT_ROOT ""{(repoRoot ?? sdkRoot).Replace("\\", "/")}"")

# Entry + generated codegen translation units (AOT mode: native-aot.generated.cpp +
# its page splits + chaos_generated_module.cpp).
set(CHAOS_ENTRY_SOURCES
    app_main.cpp
    {normalizedSources}
)

add_executable(chaos_entry ${{CHAOS_ENTRY_SOURCES}})

target_include_directories(chaos_entry PRIVATE
    ""${{CHAOS_SDK_DIR}}/include""
    ""${{CHAOS_SDK_DIR}}/generated""{repoIncludes}{toolIncludes}
)

target_compile_options(chaos_entry PRIVATE ""$<$<CXX_COMPILER_ID:MSVC>:/EHa>"")

target_link_libraries(chaos_entry PRIVATE
    chaos::runtime
)

target_link_options(chaos_entry PRIVATE
)
");

        Console.WriteLine($"    app_main.cpp, CMakeLists.txt written ({generatedCpps.Count} codegen .cpp source(s))");
    }

    /// <summary>
    /// Derive the generated native symbol and the proxy-wrapper call expression from an
    /// auto-detected entry-point SubjectId: {assembly}/{ns}.{Type}::{Method}:{Ret}({Params}).
    /// The generated proxy wrapper (chaos_generated_module.h) is
    ///   struct {AssemblySanitized}_{TypeSanitized} {{ static inline {Ret} {Method}(args...); }};
    /// so we invoke it as {Struct}::{Method}(<args handle>).
    ///
    /// A `string[]` entry parameter (Main(string[] args)) is carried at the native ABI as
    /// CHAOS_IL2CPP_INTPTR pointing at a chaos_managed_array.  app_main owns a local
    /// `chaos_managed_array* chaos_args` (a real, non-null, zero-length string[]) which we
    /// pass through, so the callee can read args.Length (=0) without a null dereference.
    /// </summary>
    private static (string NativeSymbol, string ProxyCall) DeriveEntryCall(string? entrySubjectId, string assemblyName)
    {
        var nativeSymbol = "chaos_entry_unknown_main_unknown";
        var proxyCall = "0";
        if (string.IsNullOrWhiteSpace(entrySubjectId))
        {
            Console.WriteLine("  [publish] warning: entry-point SubjectId is null/empty; "
                              + "app_main will compile but the entry point will not be invoked.");
            return (nativeSymbol, proxyCall);
        }

        string[] parts;
        string subject = entrySubjectId;
        try
        {
            // Strip an assembly prefix if present (some detect paths include {asm}/).
            parts = subject.Split("::", 2);
            var leftPart = parts[0];
            var typeAndMethod = leftPart.Split('/');
            var typeFull = typeAndMethod[^1].Trim();

            if (parts.Length < 2)
            {
                Console.WriteLine($"  [publish] warning: entry-point SubjectId '{entrySubjectId}' "
                                  + "has no '::' method separator; cannot derive proxy call.");
                return (nativeSymbol, proxyCall);
            }
            var methodPortion = parts[1];
            var colon = methodPortion.IndexOf(':');
            var methodName = (colon >= 0 ? methodPortion[..colon] : methodPortion).Trim();
            if (string.IsNullOrEmpty(methodName))
            {
                Console.WriteLine($"  [publish] warning: cannot extract method name from SubjectId '{entrySubjectId}'.");
                return (nativeSymbol, proxyCall);
            }

            var sanitized = Sanitize(typeFull);
            var asmSanitized = Sanitize(assemblyName);
            var structName = $"{asmSanitized}_{sanitized}";

            // NOTE: 'chaos_args' is the local variable name in the app_main template
            // (see template at ~line 382).  Keep in sync if the template changes.
            // The reinterpret_cast is safe because CHAOS_IL2CPP_INTPTR matches the
            // managed array handle type expected by the generated proxy struct.
            // (No compile-time check — must be kept in sync manually.)
            // For entries without a string[] arg (e.g. Main()) we still pass
            // whatever the existing convention was (the literal 0) — the proxy
            // wrapper's parameter count must match the generated signature.
            var args = "0";
            if (methodPortion.Contains("System.String[]") || methodPortion.Contains("string[]"))
            {
                args = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_args)";
            }
            nativeSymbol = $"{asmSanitized}_{sanitized}_{methodName}";
            proxyCall = $"{structName}::{methodName}({args})";
        }
        catch (Exception ex)
        {
            Console.WriteLine($"  [publish] warning: failed to derive entry call from SubjectId "
                              + $"'{entrySubjectId}': {ex.Message}");
            // app_main still compiles with the fallback values.
        }
        return (nativeSymbol, proxyCall);
    }

    /// <summary>Flatten a .NET type name to the codegen-sanitized identifier form (dots/nullable/generics ->
    /// underscores), matching how the proxy struct name is generated.</summary>
    private static string Sanitize(string name)
    {
        var sb = new System.Text.StringBuilder(name.Length);
        foreach (var ch in name)
        {
            if (char.IsLetterOrDigit(ch) || ch == '_')
                sb.Append(ch);
            else
                sb.Append('_');
        }
        return sb.ToString();
    }

    private static int EmitTestProject(string outputDir, PublishConfig config, ConvertService.ConversionResult result)
    {
        // Test mode: use TPG generate-dll to produce the native test harness.
        // This invokes dotnet exec on the TPG DLL with the correct arguments,
        // captures stdout/stderr, and returns the process exit code.
        var tpgDll = FindTpgDll();
        if (tpgDll == null)
        {
            Console.Error.WriteLine("Error: TPG DLL not found. Ensure Chaos.IL2CPP.Tools.TestProjectGenerator is built.");
            return 1;
        }

        var tpgArgs = new List<string>
        {
            "exec", tpgDll,
            "generate-dll",
            "--dll", config.AssemblyDll ?? Path.Combine(codegenDir(config), "CombinedSubjects.dll"),
            "--metadata", config.AssemblyMetadata ?? Path.Combine(codegenDir(config), "subjects", "subjects.metadata.json"),
            "--output", outputDir,
            "--config-tier", config.ConfigTier,
        };

        var psi = new System.Diagnostics.ProcessStartInfo("dotnet")
        {
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
        };
        foreach (var arg in tpgArgs)
            psi.ArgumentList.Add(arg);

        try
        {
            using var process = System.Diagnostics.Process.Start(psi);
            if (process is null)
            {
                Console.Error.WriteLine("Error: failed to start TPG generate-dll process.");
                return 1;
            }

            var stdoutTask = process.StandardOutput.ReadToEndAsync();
            var stderrTask = process.StandardError.ReadToEndAsync();

            if (!process.WaitForExit(1_800_000))
            {
                try { process.Kill(entireProcessTree: true); } catch { }
                Console.Error.WriteLine("Error: TPG generate-dll timed out after 30 minutes.");
                return 1;
            }

            var stdout = stdoutTask.GetAwaiter().GetResult();
            var stderr = stderrTask.GetAwaiter().GetResult();

            foreach (var line in stdout.Split(Environment.NewLine, StringSplitOptions.RemoveEmptyEntries))
                Console.WriteLine($"  {line}");
            if (!string.IsNullOrWhiteSpace(stderr))
            {
                foreach (var line in stderr.Split(Environment.NewLine, StringSplitOptions.RemoveEmptyEntries))
                    Console.Error.WriteLine($"  [TPG:err] {line}");
            }

            if (process.ExitCode != 0)
            {
                Console.Error.WriteLine($"TPG generate-dll failed (exit code: {process.ExitCode})");
                return process.ExitCode;
            }

            // Copy real SDK libs over stubs, then rebuild.
            CopyRealSdkLibsOverStubs(outputDir);
            var buildResult = BuildService.ConfigureAndBuild(outputDir, "chaos_entry", config.ConfigTier);
            if (!buildResult.Success)
            {
                Console.Error.WriteLine($"  [publish] Rebuild failed: {buildResult.Error}");
                return 1;
            }

            Console.WriteLine("  [test mode] TPG generate-dll completed successfully.");
            return 0;
        }
        catch (System.ComponentModel.Win32Exception)
        {
            Console.Error.WriteLine("Error: dotnet SDK not found. Install from https://dot.net/download");
            return 1;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Error: TPG generate-dll failed: {ex.Message}");
            return 1;
        }
    }

    private static string? FindTpgDll()
    {
        var driverDir = Path.GetDirectoryName(typeof(PublishController).Assembly.Location);
        if (driverDir != null)
        {
            var candidate = Path.Combine(driverDir, "Chaos.IL2CPP.Tools.TestProjectGenerator.dll");
            if (File.Exists(candidate))
                return candidate;
        }
        return null;
    }

    private static string codegenDir(PublishConfig config)
    {
        return Path.Combine(config.OutputDir, "codegen", "generated");
    }

    private static void CopyRealSdkLibsOverStubs(string outputDir)
    {
        try
        {
            var nativePresetDir = ResolveSdkPreset();
            var stubLibRoot = Path.Combine(outputDir, "codegen", "lib");

            // Three sources, tried in order:
            //   1. Embedded SDK inside the dotnet tool package (walk up from BaseDir)
            //   2. Repo-relative SDK (tests/e2e/translation/sdk/...)
            //   3. Repo-relative preset build (artifacts/presets/...)
            var realLibs = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

            // Source 1: embedded SDK (dotnet tool package). Walk up from
            // BaseDirectory to find sdk/lib (global tools unpack under
            // .../tools/net8.0/any/, sdk/ is a sibling above).
            var embeddedSdkLib = FindEmbeddedSdkLib();
            if (embeddedSdkLib != null)
            {
                foreach (var lib in Directory.GetFiles(embeddedSdkLib, "*.lib"))
                    realLibs[Path.GetFileName(lib)] = lib;
                foreach (var lib in Directory.GetFiles(embeddedSdkLib, "*.a"))
                    realLibs[Path.GetFileName(lib)] = lib;
                Console.WriteLine($"  [publish] using embedded SDK at {embeddedSdkLib}");
            }

            // Source 2 + 3: repo-relative paths (development mode)
            var repoRoot = DetectRepoRoot();
            if (repoRoot != null)
            {
                var sdkLibRoot = Path.Combine(repoRoot, "tests", "e2e", "translation", "sdk", nativePresetDir, "lib");
                if (Directory.Exists(sdkLibRoot))
                {
                    foreach (var lib in Directory.GetFiles(sdkLibRoot, "*.lib"))
                        realLibs[Path.GetFileName(lib)] = lib;
                    foreach (var lib in Directory.GetFiles(sdkLibRoot, "*.a"))
                        realLibs[Path.GetFileName(lib)] = lib;
                }
                var presetLibRoot = Path.Combine(repoRoot, "artifacts", "presets", nativePresetDir);
                if (Directory.Exists(presetLibRoot))
                {
                    foreach (var lib in Directory.GetFiles(presetLibRoot, "*.lib", SearchOption.AllDirectories))
                    {
                        var name = Path.GetFileName(lib);
                        if (!realLibs.TryGetValue(name, out var existing) ||
                            Path.GetDirectoryName(lib)!.Length > Path.GetDirectoryName(existing)!.Length)
                            realLibs[name] = lib;
                    }
                    foreach (var lib in Directory.GetFiles(presetLibRoot, "*.a", SearchOption.AllDirectories))
                    {
                        var name = Path.GetFileName(lib);
                        if (!realLibs.TryGetValue(name, out var existing) ||
                            Path.GetDirectoryName(lib)!.Length > Path.GetDirectoryName(existing)!.Length)
                            realLibs[name] = lib;
                    }
                }
            }
            _ = nativePresetDir;

            if (realLibs.Count == 0)
            {
                Console.WriteLine($"  [publish] warning: no real SDK libs found (preset={nativePresetDir}); "
                                  + "real-lib copy skipped. "
                                  + "Ensure the SDK has been built or the tool package is complete.");
                return;
            }
            if (!Directory.Exists(stubLibRoot))
            {
                Console.WriteLine($"  [publish] warning: codegen lib dir not found ({stubLibRoot}); real-lib copy skipped.");
                return;
            }

            Directory.CreateDirectory(stubLibRoot);
            var copied = 0;
            foreach (var kv in realLibs)
            {
                File.Copy(kv.Value, Path.Combine(stubLibRoot, kv.Key), overwrite: true);
                copied++;
            }
            Console.WriteLine($"  [publish] Overwrote {copied} stub lib(s) with real SDK libs");

            // M1: validate no 8-byte stub archives remain under codegen/lib. An
            // empty archive is exactly 8 bytes (`!<arch>\n`); if any expected lib
            // still has that size after copy, the real lib wasn't found and the
            // link will fail with LNK2001. Surface this loudly instead of letting
            // it fail silently at link time.
            var stubRemain = Directory.GetFiles(stubLibRoot, "*.lib")
                .Concat(Directory.GetFiles(stubLibRoot, "*.a"))
                .Where(p => new FileInfo(p).Length <= 8)
                .Select(Path.GetFileName)
                .ToList();
            if (stubRemain.Count > 0)
            {
                Console.Error.WriteLine($"  [publish] ERROR: {stubRemain.Count} stub lib(s) remain unsigned (<=8 bytes) "
                                        + $"in {stubLibRoot}: {string.Join(", ", stubRemain)}");
                Console.Error.WriteLine("  [publish]   The real prebuilt libs were not found. Build the SDK "
                                        + "preset first (e.g. tests/e2e/translation/artifacts/build_presets.py).");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"  [publish] warning: real-lib copy failed: {ex.Message}");
        }
    }

    private static string? FindEntryExe(string outputDir)
    {
        var isWindows = System.Runtime.InteropServices.RuntimeInformation
            .IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Windows);
        // The per-config sub-directory and the binary name depend on the OS:
        // MSVC outputs chaos_entry.exe under build/<Config>/; Unix Makefiles /
        // Ninja output a bare `chaos_entry` binary (no .exe).
        var exeName = isWindows ? "chaos_entry.exe" : "chaos_entry";
        var buildDir = Path.Combine(outputDir, "build");
        var candidates = new List<string>
        {
            // Windows VS multi-config layout (build/<Config>/chaos_entry.exe) and
            // the pre-existing flat fallbacks.
            Path.Combine(buildDir, "RelWithDebInfo", exeName),
            Path.Combine(buildDir, "Release", exeName),
            Path.Combine(buildDir, "Debug", exeName),
            Path.Combine(buildDir, exeName),
            Path.Combine(outputDir, exeName),
            // Unix single-config layouts (build/ out-of-tree and in-tree).
            Path.Combine(outputDir, "RelWithDebInfo", exeName),
            Path.Combine(outputDir, "Release", exeName),
            Path.Combine(outputDir, "Debug", exeName),
        };
        foreach (var c in candidates)
        {
            if (File.Exists(c))
                return Path.GetFullPath(c);
        }
        return null;
    }

    private static string? DetectRepoRoot()
    {
        var dir = AppContext.BaseDirectory;
        for (int i = 0; i < 6; i++)
        {
            var candidate = Path.GetFullPath(Path.Combine(dir, ".."));
            if (File.Exists(Path.Combine(candidate, "CMakeLists.txt")))
                return candidate;
            dir = candidate;
        }
        return null;
    }

    /// <summary>
    /// Locate the embedded SDK's lib/ directory inside a dotnet tool package.
    /// A global tool unpacks to .../tools/net8.0/any/, but the nupkg puts sdk/
    /// at the package root, so we walk UP from BaseDirectory to find sdk/lib.
    /// </summary>
    private static string? FindEmbeddedSdkLib()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        for (int i = 0; i < 8 && dir != null; i++)
        {
            var candidate = Path.Combine(dir.FullName, "sdk", "lib");
            if (Directory.Exists(candidate))
                return candidate;
            dir = dir.Parent;
        }
        return null;
    }

    private static int RunDotnetBuild(string projectPath, string outputDir)
    {
        var psi = new System.Diagnostics.ProcessStartInfo("dotnet")
        {
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
        };
        psi.ArgumentList.Add("build");
        psi.ArgumentList.Add(projectPath);
        psi.ArgumentList.Add("-o");
        psi.ArgumentList.Add(outputDir);
        psi.ArgumentList.Add("--nologo");
        psi.ArgumentList.Add("-v");
        psi.ArgumentList.Add("q");

        using var proc = System.Diagnostics.Process.Start(psi);
        if (proc == null) return -1;

        var stdout = proc.StandardOutput.ReadToEndAsync();
        var stderr = proc.StandardError.ReadToEndAsync();

        // Wait for exit with a 2-minute timeout.  If the build does not finish
        // within the budget, kill the process tree and return failure.
        if (!proc.WaitForExit(120_000))
        {
            try { proc.Kill(entireProcessTree: true); } catch { }
            Console.Error.WriteLine("Error: dotnet build timed out after 120 seconds and was killed.");
            return -1;
        }

        var output = stdout.GetAwaiter().GetResult();
        var error = stderr.GetAwaiter().GetResult();

        if (!string.IsNullOrWhiteSpace(output))
            Console.WriteLine(output);
        if (!string.IsNullOrWhiteSpace(error))
            Console.Error.WriteLine(error);

        return proc.ExitCode;
    }

    /// <summary>
    /// The tool's own version, sourced from the compiled assembly's informational
    /// version (which reflects Directory.Build.props <Version>, kept in lock-step
    /// with VERSION by scripts/release_bump.sh). This removes the hardcoded
    /// "0.1.0" that previously drifted from the project's real version.
    /// </summary>
    private static string ResolveToolVersion()
    {
        var info = typeof(PublishController).Assembly
            .GetCustomAttribute<System.Reflection.AssemblyInformationalVersionAttribute>();
        var ver = info?.InformationalVersion;
        if (!string.IsNullOrWhiteSpace(ver))
        {
            // InformationalVersion may carry a trailing +<build-metadata> suffix; keep only SemVer.
            ver = ver.Split('+')[0].Trim();
            if (ver.Length > 0)
                return ver;
        }
        // Final fallback: the file version (assembly version, no suffix).
        var fv = typeof(PublishController).Assembly.GetName().Version?.ToString(3);
        return string.IsNullOrWhiteSpace(fv) ? "0.0.0" : fv;
    }

    private static void WritePublishManifest(string outputDir, PublishConfig config,
        ConvertService.ConversionResult result, string? entryExe)
    {
        var manifest = new
        {
            schemaVersion = "1.0",
            inputPath = config.InputPath,
            mode = config.Mode,
            outputDir,
            assemblyName = result.AssemblyName,
            entryPoint = result.EntryPointSubjectId,
            methodCount = result.MethodCount,
            configTier = config.ConfigTier,
            entryExe = entryExe != null ? Path.GetFullPath(entryExe) : null,
            toolVersion = ResolveToolVersion(),
            sdkPreset = ResolveSdkPreset(),
            status = entryExe != null ? "ok" : "source-only",
            timestamp = DateTime.UtcNow.ToString("O"),
        };

        var manifestPath = Path.Combine(outputDir, "publish.manifest.json");
        File.WriteAllText(manifestPath, JsonSerializer.Serialize(manifest, JsonOptions));
        Console.WriteLine($"  publish.manifest.json: {manifestPath}");
    }

    /// <summary>
    /// Resolve the SDK preset identifier for the current host platform+architecture.
    /// Delegates to ConvertService.ResolveNativePreset (single source of truth).
    /// </summary>
    private static string ResolveSdkPreset()
    {
        return ConvertService.ResolveNativePreset();
    }
}