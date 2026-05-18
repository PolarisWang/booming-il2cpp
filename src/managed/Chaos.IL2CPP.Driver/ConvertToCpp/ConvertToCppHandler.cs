using System.Reflection.PortableExecutable;
using System.Text.Json;
using Chaos.IL2CPP.CodeGen;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;
using Chaos.IL2CPP.Pipeline;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Handles the convert-to-cpp command. Supports multi-assembly translation
/// in Unity IL2CPP style: --assembly A.dll --assembly B.dll --output dir/
///
/// Each assembly gets its own per-assembly generated C++ files plus a
/// combined CMakeLists.txt for full program compilation.
/// </summary>
internal static class ConvertToCppHandler
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
        NumberHandling = System.Text.Json.Serialization.JsonNumberHandling.AllowNamedFloatingPointLiterals,
    };

    public static int Run(string[] args)
    {
        var config = ConvertToCppConfig.Parse(args);
        if (config.AssemblyPaths.Count == 0)
            return 1;

        var outputRoot = Path.GetFullPath(config.OutputDir);
        Directory.CreateDirectory(outputRoot);

        Console.WriteLine($"chaos-il2cpp convert-to-cpp");
        Console.WriteLine($"  Assemblies: {config.AssemblyPaths.Count}");
        foreach (var asm in config.AssemblyPaths)
            Console.WriteLine($"    {asm}");
        Console.WriteLine($"  Output:   {outputRoot}");

        // ── Step 1: Collect managed dependency DLLs ────────────────────────
        var additionalPaths = new List<string>();
        var entryAssemblyNames = new HashSet<string>(config.AssemblyPaths.Select(Path.GetFileNameWithoutExtension), StringComparer.OrdinalIgnoreCase);
        foreach (var dir in config.AssemblyDirs)
        {
            if (Directory.Exists(dir))
            {
                foreach (var dll in Directory.GetFiles(dir, "*.dll"))
                {
                    try
                    {
                        using var peReader = new PEReader(File.OpenRead(dll));
                        if (peReader.HasMetadata)
                        {
                            // Skip assemblies already specified via --assembly
                            var name = Path.GetFileNameWithoutExtension(dll);
                            if (!entryAssemblyNames.Contains(name))
                                additionalPaths.Add(dll);
                        }
                    }
                    catch { }
                }
            }
            else if (File.Exists(dir))
            {
                additionalPaths.Add(dir);
            }
        }

        // ── Step 2: Run pipeline ───────────────────────────────────────────
        Console.Write($"  [1/3] Running IL2CPP pipeline...");
        ChaosTrace.Point("convert-to-cpp.pipeline", "codegen");

        var pipeline = new PipelinePlan();

        if (config.AssemblyPaths.Count == 1)
        {
            // Single assembly full-closure — entry is specified or auto-detected
            var request = new ManagedClosureRequest(
                config.AssemblyPaths[0],
                outputRoot,
                EntryPointSubjectIdOverride: config.EntryPoint,
                AdditionalAssemblyPaths: additionalPaths,
                FullAssemblyClosure: true);

            var closureResult = pipeline.Execute(request);
            Console.WriteLine($" {closureResult.AotCoreIr.Methods.Count} methods lowered");

            Console.Write("  [2/3] Writing closure artifacts...");
            WriteArtifacts(outputRoot, closureResult);
            Console.WriteLine(" done");

            Console.Write("  [3/3] Emitting C++ (NativeAot)...");
            var emitResult = EmitNativeAot(outputRoot);
            // EmitNativeAot writes .cpp to outputRoot/generated/ but CmakeGenerator
            // expects outputRoot/{AssemblyName}/generated/. Move files to match.
            var asmName = emitResult.Manifest.AssemblyName;
            if (!string.IsNullOrEmpty(asmName))
            {
                var asmOutputDir = Path.Combine(outputRoot, asmName);
                foreach (var source in emitResult.GeneratedSources)
                {
                    var relativePath = source.RelativePath.Replace('/', Path.DirectorySeparatorChar);
                    var srcPath = Path.Combine(outputRoot, relativePath);
                    var dstPath = Path.Combine(asmOutputDir, relativePath);
                    Directory.CreateDirectory(Path.GetDirectoryName(dstPath)!);
                    if (File.Exists(srcPath))
                        File.Move(srcPath, dstPath, overwrite: true);
                }
            }
            Console.WriteLine($" {emitResult.GeneratedSources.Count} files");

            // Generate CMakeLists.txt
            var repoRoot = ResolveRepoRoot();
            var nativeLibDir = Path.Combine(repoRoot, "build", "native");
            var cmakeGen = new Chaos.IL2CPP.CodeGen.BuildSystem.CmakeGenerator(repoRoot);
            var singleCmakeContent = cmakeGen.Generate(
                new[] { emitResult }.ToList(),
                nativeLibDir: nativeLibDir,
                extraSources: new List<string> { "runtime-entry.cpp" },
                targetName: "entry");
            File.WriteAllText(Path.Combine(outputRoot, "CMakeLists.txt"), singleCmakeContent);

            var runtimeEntryCpp = GenerateRuntimeEntryCpp(config.EntryPoint is not null ? "RunNativeAot" : null);
            File.WriteAllText(Path.Combine(outputRoot, "runtime-entry.cpp"), runtimeEntryCpp);

            Console.WriteLine($"Convert completed: {outputRoot}");
        }
        else
        {
            // Multi-assembly or explicit entry point — use unified pipeline
            var multiRequest = new MultiAssemblyClosureRequest(
                config.AssemblyPaths.ToList(),
                outputRoot,
                EntryPointSubjectIdOverride: config.EntryPoint,
                AdditionalAssemblyPaths: additionalPaths);

            var results = pipeline.ExecuteMulti(multiRequest);
            Console.WriteLine($" {results.Sum(r => r.AotCoreIr?.Methods.Count ?? 0)} methods across {results.Count} assemblies");

            // Write per-assembly artifacts and emit C++
            Console.Write("  [2/3] Writing closure artifacts...");
            foreach (var result in results)
            {
                var assemblyOutput = result.OutputRootPath;
                Directory.CreateDirectory(assemblyOutput);
                WriteArtifacts(assemblyOutput, result);
            }
            Console.WriteLine(" done");

            // Emit C++ per assembly
            Console.Write("  [3/3] Emitting C++ (NativeAot)...");
            int totalFiles = 0;
            foreach (var result in results)
            {
                var emitResult = EmitNativeAot(result.OutputRootPath);
                totalFiles += emitResult.GeneratedSources.Count;
            }

            // Write combined report
            WriteCombinedReport(outputRoot, config, results);

            // Generate CMakeLists.txt
            var repoRoot = ResolveRepoRoot();
            var nativeLibDir = Path.Combine(repoRoot, "build", "native");
            var cmakeGen = new Chaos.IL2CPP.CodeGen.BuildSystem.CmakeGenerator(repoRoot);
            var assemblyNames = results.Select(r => r.ClosureManifest?.AssemblyName ?? "unknown").ToList();
            var assemblyInfo = assemblyNames.Select(name => new
            {
                Name = name,
                Sources = new[] { $"{name}.cpp" },
            }).ToList();
            var cmakeContent = cmakeGen.Generate(
                results.Select(r => new NativeAotResult
                {
                    OutputRootPath = r.OutputRootPath,
                    LoweringPlan = new NativeAotLoweringPlanArtifact
                    {
                        PlanKind = "generic-managed-entry",
                        AssemblyName = r.ClosureManifest?.AssemblyName ?? "unknown",
                        EntrySubjectId = r.ClosureManifest?.EntrySubjectId ?? "",
                        EntrySymbol = "",
                        EntryMethodToken = "0u",
                        NativeEntryFunctionName = "RunNativeAot",
                        WorkloadAbi = "int(int32)",
                    },
                    Manifest = new NativeAotManifestArtifact
                    {
                        AssemblyName = r.ClosureManifest?.AssemblyName ?? "unknown",
                        EntrySubjectId = r.ClosureManifest?.EntrySubjectId ?? "",
                        ManagedClosureRootPath = outputRoot,
                        PlanArtifactPath = NativeAotArtifactNames.LoweringPlan,
                        GeneratedArtifacts = [],
                    },
                    CodegenMetrics = new NativeCodegenMetricsArtifact
                    {
                        CodegenKind = "native-aot",
                        PlanKind = "generic-managed-entry",
                        GeneratedSourcePaths = [],
                    },
                    GeneratedSources = r.AotCoreIr?.Methods.Select(m => new NativeAotGeneratedSource
                    {
                        RelativePath = m.SubjectId,
                        Contents = "",
                    }).ToList() ?? [],
                }).ToList(),
                nativeLibDir: nativeLibDir,
                extraSources: new List<string> { "runtime-entry.cpp" },
                targetName: "entry");
            File.WriteAllText(Path.Combine(outputRoot, "CMakeLists.txt"), cmakeContent);

            // Generate runtime entry point (simple text, no Scriban dependency)
            var runtimeEntryContent = GenerateRuntimeEntryCpp(config.EntryPoint is not null ? "RunNativeAot" : null);
            File.WriteAllText(Path.Combine(outputRoot, "runtime-entry.cpp"), runtimeEntryContent);

            Console.WriteLine($" {totalFiles} files across {results.Count} assemblies");
            Console.WriteLine($"  CMakeLists.txt — build with: cmake -S {outputRoot} -B {outputRoot}/build");
            Console.WriteLine($"Convert completed: {outputRoot}");
        }

        return 0;
    }

    private static void WriteArtifacts(string root, ManagedClosureResult result)
    {
        Directory.CreateDirectory(root);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.AotCoreIr), result.AotCoreIr);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.AotManifest), result.AotManifest);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.MetadataRegistration), result.MetadataRegistration);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.SupplementalMetadataTemplate), result.SupplementalMetadataTemplate);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.CodeRegistration), result.CodeRegistration);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), result.GenericInstantiationDemandGraph);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.OptimizationFacts), result.OptimizationFacts);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.PreserveDescriptor), result.PreserveDescriptor);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.NativeReferenceLoweringPlan), result.NativeReferenceLoweringPlan);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.NativeAotLoweringPlan), result.NativeAotLoweringPlan);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);
    }

    private static NativeAotResult EmitNativeAot(string outputRoot)
    {
        var request = new NativeAotRequest(outputRoot, outputRoot);
        var emitter = new NativeAotEmitter();
        var emitResult = emitter.Generate(request);

        foreach (var source in emitResult.GeneratedSources)
        {
            var targetPath = Path.Combine(outputRoot, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, source.Contents);
        }

        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.LoweringPlan), emitResult.LoweringPlan);
        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.Manifest), emitResult.Manifest);
        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.CodegenMetrics), emitResult.CodegenMetrics);

        return emitResult;
    }

    private static void WriteCombinedReport(string outputRoot, ConvertToCppConfig config, IReadOnlyList<ManagedClosureResult> results)
    {
        var report = new
        {
            assembly = string.Join(", ", config.AssemblyPaths.Select(Path.GetFileNameWithoutExtension)),
            totalAssemblies = config.AssemblyPaths.Count,
            assemblies = config.AssemblyPaths.Select(p => Path.GetFileNameWithoutExtension(p)).ToList(),
            results = results.Select(r => new
            {
                assembly = r.ClosureManifest?.AssemblyName ?? "unknown",
                methods = r.AotCoreIr?.Methods?.Count ?? 0,
                output = r.OutputRootPath,
            }).ToList(),
            timestamp = DateTime.UtcNow.ToString("o"),
        };
        WriteJson(Path.Combine(outputRoot, "convert-to-cpp-report.json"), report);
    }

    private static void WriteJson<T>(string path, T value)
    {
        var dir = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);
        File.WriteAllText(path, JsonSerializer.Serialize(value, JsonOptions) + Environment.NewLine);
    }

    private static string ResolveRepoRoot()
    {
        // Starting from the assembly output directory (e.g. Chaos.IL2CPP.Driver/bin/Debug/net8.0),
        // walk up to find the repo root (contains src/, build/, etc.)
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir != null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, "src")) &&
                Directory.Exists(Path.Combine(dir.FullName, "build")))
                return dir.FullName;
            dir = dir.Parent;
        }
        // Fallback: use current directory
        return Directory.GetCurrentDirectory();
    }

    private static string GenerateRuntimeEntryCpp(string? entryFunction)
    {
        bool hasEntry = !string.IsNullOrEmpty(entryFunction);
        string header = hasEntry
            ? "// Auto-generated runtime entry point for chaos-il2cpp AOT output."
            : "// Auto-generated runtime entry point for chaos-il2cpp full-assembly AOT output.";

        return $@"{header}
// Supports six modes:
//   (no args)                        - Fact mode: run all methods, print Passed N/M
//   N                                - Legacy single-entry mode (backward compat)
//   --benchmark N [I]                - Benchmark mode: run method N for I iterations, print JSON timing
//   --hotupdate                      - HotUpdate mode: run patch lifecycle, print JSON results
//   --hotupdate-and-benchmark N [I]  - Apply hotpatch then benchmark method N, print combined JSON
//   --patch-bench N [I]              - Apply hotpatch then benchmark method N (skip all-methods loop)
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <chrono>
// chaos/log.h must be early (before runtime_core.h) because runtime_core.h
// → gc_transition.h → thread_state.h uses CHAOS_IL2CPP_LOG_ERROR_M.
#include <chaos/log.h>
#include ""codegen_bridge.h""
#include ""runtime_abi.h""
#include ""runtime_core.h""
#include ""exception_helpers.h""
#include ""patch_loader.h""
#include ""hotpatch_table.h""
#include ""chaos/profile.h""

// kChaosExternalRuntimeFnTable is defined in native-aot.generated.cpp.
extern ""C"" void* kChaosExternalRuntimeFnTable[];
extern ""C"" const char* kChaosExternalRuntimeSubjects[];
extern ""C"" int32_t kChaosExternalRuntimeCount;

extern ""C"" std::int32_t RunNativeAot(std::int32_t);
extern ""C"" std::int32_t RunNativeAotAll();
extern ""C"" std::int32_t RunNativeAotBench(std::int32_t);
extern ""C"" double BenchmarkMethod(int, int);

// Generated by pipeline: defines kPatchData[], kPatchDataSize, kPatchDataHostClassName
extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

// Codegen-emitted registration structs (from native-aot.generated.cpp)
extern ""C"" const CodeRegistrationV0 chaos_codegen_code_registration;
extern ""C"" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern ""C"" const CodegenRegistrationOptionsV0 chaos_codegen_options;

// kAotMethodCount defined in codegen-emitted code (native-aot.generated.cpp)
extern ""C"" const int kAotMethodCount;

// SetExceptionFallback is declared at global scope in exception_helpers.h.
extern ""C"" void SetExceptionFallback(void (*fn)());

enum class RunMode {{ Fact, Benchmark, HotUpdate, HotUpdateAndBenchmark, PatchAndBenchmark }};

// ── Shared helper: apply hotpatch and print diagnostic ─────────────────────
static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchIfAvailable() {{
    if (kPatchDataSize > 0u) {{
        auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
            kPatchData, kPatchDataSize, kPatchDataHostClassName);
        if (patch_ctx == nullptr) {{
            std::fprintf(stderr, ""WARN: ApplyPatchFromMemory returned null (no patches applied)\n"");
        }} else {{
            std::fprintf(stderr, ""INFO: Applied patches to %u methods\n"",
                         static_cast<unsigned>(patch_ctx->method_count));
        }}
        return patch_ctx;
    }}
    return nullptr;
}}

// ── Fill remaining null external runtime table entries with safe stubs ──
// After ChaosResolveExternalRuntimeFnTable() runs during bootstrap, entries
// that could NOT be resolved remain nullptr.  This function fills them with
// generic no-op stubs so the generated code does not crash on null pointers.
// The stub signatures are inferred from the subject ID strings.
static void FillExternalRuntimeStubs() {{
    for (int32_t i = 0; i < kChaosExternalRuntimeCount; i++) {{
        if (kChaosExternalRuntimeFnTable[i] != nullptr) continue;
        const char* sub = kChaosExternalRuntimeSubjects[i];
        if (sub == nullptr || sub[0] == '\0') continue;

        // Types (no ""::"" — just a type name like System.Int32) are not callable.
        if (std::strstr(sub, ""::"") == nullptr) continue;

        // Known managed GC methods — wire real runtime implementations.
        if (std::strstr(sub, ""System.GC::Collect:"")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](){{ ChaosGcCollect(-1); }});
            continue;
        }}
        if (std::strstr(sub, ""System.GC::WaitForPendingFinalizers:"")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](){{ ChaosGcCollect(-1); }});
            continue;
        }}
        if (std::strstr(sub, ""System.GC::GetGeneration:"")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR obj) -> CHAOS_IL2CPP_INT32 {{ return ChaosGcGetGeneration(obj); }});
            continue;
        }}

        // Parse return type from subject ID pattern:
        //   ""Namespace.Type::Method:ReturnType(Params)""
        if (std::strstr(sub, "":System.Void("")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](){{}});
        }} else if (std::strstr(sub, "":System.Int32("")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 {{ return 0; }});
        }} else if (std::strstr(sub, "":System.Int64("")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT64 {{ return 0; }});
        }} else if (std::strstr(sub, "":System.Boolean("")) {{
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 {{ return 0; }});
        }} else {{
            // Unknown return type — safest default is void(void) to at least
            // not corrupt the stack on callee-saved registers.
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](){{}});
        }}
    }}
}}

int main(int argc, char** argv) {{
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) return -1;
    bridge->register_codegen(
        &chaos_codegen_code_registration,
        &chaos_codegen_metadata_registration,
        &chaos_codegen_options);
    bridge->bootstrap_runtime();

    // Set TLS to non-null sentinels so RaiseManagedException gets past its
    // first guard (runtime == nullptr / thread == nullptr check).
    // RuntimeState and ThreadState are opaque types (only forward-declared
    // in runtime_abi.h), so sentinels are safe — the exception path never
    // dereferences these pointers when type resolution fails.
    chaos::il2cpp::runtime_core::SetCurrentRuntimeState(
        reinterpret_cast<RuntimeState*>(static_cast<uintptr_t>(1)));
    chaos::il2cpp::runtime_core::SetCurrentThreadState(
        reinterpret_cast<ThreadState*>(static_cast<uintptr_t>(1)));

    // Fill unresolved external runtime table entries with safe stubs.
    FillExternalRuntimeStubs();

    RunMode mode = RunMode::Fact;
    int entry_index = 0;
    int iterations = 10000;

    // Simple argv parsing (self-contained, no getopt dependency)
    if (argc >= 3 && std::strcmp(argv[1], ""--benchmark"") == 0) {{
        mode = RunMode::Benchmark;
        entry_index = std::atoi(argv[2]);
        if (argc >= 4) iterations = std::atoi(argv[3]);
    }} else if (argc >= 2 && std::strcmp(argv[1], ""--hotupdate"") == 0) {{
        mode = RunMode::HotUpdate;
    }} else if (argc >= 3 && std::strcmp(argv[1], ""--hotupdate-and-benchmark"") == 0) {{
        mode = RunMode::HotUpdateAndBenchmark;
        entry_index = std::atoi(argv[2]);
        if (argc >= 4) iterations = std::atoi(argv[3]);
    }} else if (argc >= 3 && std::strcmp(argv[1], ""--patch-bench"") == 0) {{
        mode = RunMode::PatchAndBenchmark;
        entry_index = std::atoi(argv[2]);
        if (argc >= 4) iterations = std::atoi(argv[3]);
    }} else if (argc >= 2) {{
        entry_index = std::atoi(argv[1]);  // backward compat: numeric entry index
    }}

    switch (mode) {{
    case RunMode::Fact: {{
        // NOTE: Uses try/catch instead of setjmp/longjmp because the generated
        // code throws chaos_managed_exception (C++ exception) for unresolved
        // external calls. setjmp/longjmp cannot catch C++ exceptions and mixing
        // both with /EHa corrupts the /GS stack cookie (0xC0000409).
        int result = 0;
        for (int i = 0; i < kAotMethodCount; i++) {{
            bool caught = false;
            chaos::il2cpp::common::g_chaos_fail_hook = []() {{ throw chaos_managed_exception{{}}; }};
            try {{
                RunNativeAot(i);
            }} catch (const chaos_managed_exception&) {{
                caught = true;
            }} catch (...) {{
                caught = true;
            }}
            if (caught) result |= (1 << i);
        }}
        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
        int failed_count = 0;
        int tmp = result;
        while (tmp) {{ failed_count += tmp & 1; tmp >>= 1; }}
        int passed_count = kAotMethodCount - failed_count;
        printf(""Passed: %d/%d\n"", passed_count, kAotMethodCount);
        std::fflush(stdout);
        return result;
    }}
    case RunMode::Benchmark: {{
        double elapsed_ms = -1.0;
        chaos::il2cpp::common::g_chaos_fail_hook = []() {{ throw chaos_managed_exception{{}}; }};
        try {{
            elapsed_ms = BenchmarkMethod(entry_index, iterations);
        }} catch (...) {{
            elapsed_ms = -1.0;
        }}
        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
        if (elapsed_ms < 0.0) {{
            printf(""{{\""error\"":\""invalid method index %d\""}}\n"", entry_index);
            std::fflush(stdout);
            return -1;
        }}
        double ops_per_sec = iterations / (elapsed_ms / 1000.0);
        printf(""{{\""elapsedMilliseconds\"":%.3f,\""calibratedMs\"":%.3f,\""calOverheadMs\"":0.000,""
               ""\""opsPerSecond\"":%.1f,\""iterations\"":%d,\""methodIndex\"":%d}}\n"",
               elapsed_ms, elapsed_ms, ops_per_sec, iterations, entry_index);
        std::fflush(stdout);
        return 0;
    }}
    case RunMode::HotUpdate: {{
        int result = 0;
        for (int i = 0; i < kAotMethodCount; i++) {{
            chaos::il2cpp::common::g_chaos_fail_hook = []() {{ throw chaos_managed_exception{{}}; }};
            try {{
                RunNativeAot(i);
            }} catch (...) {{
                result |= (1 << i);
            }}
        }}
        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
        int failed_count = 0;
        int tmp2 = result;
        while (tmp2) {{ failed_count += tmp2 & 1; tmp2 >>= 1; }}
        int passed_count = kAotMethodCount - failed_count;
        printf(""{{\""passedMethods\"":%d,\""failedMethods\"":%d,\""totalMethods\"":%d}}\n"",
               passed_count, failed_count, kAotMethodCount);
        std::fflush(stdout);
        return result;
    }}
    case RunMode::HotUpdateAndBenchmark: {{
        int hot_result = 0;
        for (int i = 0; i < kAotMethodCount; i++) {{
            chaos::il2cpp::common::g_chaos_fail_hook = []() {{ throw chaos_managed_exception{{}}; }};
            try {{
                RunNativeAot(i);
            }} catch (...) {{
                hot_result |= (1 << i);
            }}
        }}
        chaos::il2cpp::common::g_chaos_fail_hook = []() {{ throw chaos_managed_exception{{}}; }};
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < iterations; i++) {{
            try {{
                RunNativeAot(entry_index);
            }} catch (...) {{
                // Skip iteration on exception
            }}
        }}
        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
        double ns_per_op = (elapsed_ms * 1e6) / iterations;
        printf(""{{\""postPatchNsPerOp\"":%.1f,\""elapsedMilliseconds\"":%.3f,\""iterations\"":%d,\""methodIndex\"":%d,\""hotResult\"":%d}}\n"",
               ns_per_op, elapsed_ms, iterations, entry_index, hot_result);
        std::fflush(stdout);
        return 0;
    }}
    case RunMode::PatchAndBenchmark: {{
        chaos::il2cpp::common::g_chaos_fail_hook = []() {{ throw chaos_managed_exception{{}}; }};
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < iterations; i++) {{
            try {{
                RunNativeAot(entry_index);
            }} catch (...) {{
                // Skip iteration on exception
            }}
        }}
        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
        double ns_per_op = (elapsed_ms * 1e6) / iterations;
        printf(""{{\""postPatchNsPerOp\"":%.1f,\""elapsedMilliseconds\"":%.3f,\""iterations\"":%d,\""methodIndex\"":%d}}\n"",
               ns_per_op, elapsed_ms, iterations, entry_index);
        std::fflush(stdout);
        return 0;
    }}
    }}
}}";
    }
}
