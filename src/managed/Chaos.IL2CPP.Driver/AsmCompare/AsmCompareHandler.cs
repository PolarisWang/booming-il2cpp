using System.Collections.Concurrent;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;
using Chaos.IL2CPP.Pipeline;
namespace Chaos.IL2CPP.Driver;

internal static class AsmCompareHandler
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
        NumberHandling = System.Text.Json.Serialization.JsonNumberHandling.AllowNamedFloatingPointLiterals,
    };

    // Sections that require C++ generation / NativeAotEmitter
    private static readonly HashSet<string> CppRequiredSections = new(StringComparer.OrdinalIgnoreCase)
    {
        "raw-cpp", "raw-aot", "side-by-side", "inline-map", "il-dump", "ir-trace",
    };

    // Sections that require the full closure AotCoreIr
    private static readonly HashSet<string> ClosureRequiredSections = new(StringComparer.OrdinalIgnoreCase)
    {
        "header", "il-dump", "ir-trace", "raw-cpp", "side-by-side", "raw-aot",
        "inline-map", "metrics", "analysis",
    };

    public static int Run(string[] args)
    {
        var config = AsmCompareConfig.Parse(args);
        if (config.IsHelp)
            return 0;

        var tempDir = Path.Combine(Path.GetTempPath(), "chaos-asm-compare-" + Guid.NewGuid().ToString("N"));
        var needsCpp = _needsCppGeneration(config);

        try
        {
            // ── Step 1: Run IL2CPP closure pipeline (once for all methods) ──
            Console.Error.Write("  [1/3] Running IL2CPP pipeline...");
            ChaosTrace.Point("asm-compare.pipeline", "codegen");

            var pipeline = new PipelinePlan();

            // Resolve target assemblies from methodSubjectIds for full AOT codegen.
            // Handle both explicit --method-subject-ids and bare --method (which may
            // also use the assembly-prefixed format).
            var methodIds = config.MethodSubjectIds.Count > 0
                ? config.MethodSubjectIds
                : config.MethodNames;
            var additionalPaths = new HashSet<string>();
            foreach (var subjectId in methodIds)
            {
                var slashIdx = subjectId.IndexOf('/');
                if (slashIdx <= 0) continue;
                try
                {
                    var asm = System.Reflection.Assembly.Load(subjectId[..slashIdx]);
                    additionalPaths.Add(asm.Location);
                }
                catch { /* skip unresolvable — fallback to Subjects DLL only */ }
            }

            var closureResultResult = pipeline.Execute(new ManagedClosureRequest(
                config.AssemblyPath,
                tempDir,
                EntryPointSubjectIdOverride: null,
                AdditionalAssemblyPaths: additionalPaths.Count > 0 ? additionalPaths.ToArray() : null,
                FullAssemblyClosure: true));
            ChaosTrace.Point("asm-compare.pipeline:exit", "codegen");
            if (closureResultResult.IsFailure)
            {
                Console.Error.WriteLine($"Pipeline failed: [{closureResultResult.Error!.Code}] {closureResultResult.Error.Message}");
                return 1;
            }
            var closureResult = closureResultResult.Value!;

            Console.Error.WriteLine($" {closureResult.AotCoreIr.Methods.Count} methods lowered");

            // P0: Pre-build method lookup dictionary (avoid N×3-pass search)
            var methodMap = new Dictionary<string, AotCoreIrMethodArtifact>(StringComparer.OrdinalIgnoreCase);
            foreach (var m in closureResult.AotCoreIr.Methods)
            {
                // Index by short name (last segment after / or ::)
                var shortName = m.SubjectId.Contains("::")
                    ? m.SubjectId.Substring(m.SubjectId.LastIndexOf("::", StringComparison.Ordinal) + 2)
                    : m.SubjectId;
                methodMap[shortName] = m;
                methodMap[m.SubjectId] = m;
                methodMap[m.MethodId] = m;
            }

            // ── Step 2 (P0): Generate C++ only if sections need it ──────────
            NativeAotResult? emitResult = null;
            if (needsCpp)
            {
                Console.Error.Write("  [2/3] Generating C++ (NativeAot)...");
                ChaosTrace.Point("asm-compare.codegen", "codegen");

                WriteArtifacts(tempDir, closureResult);

                var emitter = new NativeAotEmitter();
                emitResult = emitter.Generate(new NativeAotRequest(tempDir, tempDir));

                foreach (var source in emitResult.GeneratedSources)
                {
                    var targetPath = Path.Combine(tempDir, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
                    Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
                    File.WriteAllText(targetPath, source.Contents);
                }

                WriteJson(Path.Combine(tempDir, NativeAotArtifactNames.LoweringPlan), emitResult.LoweringPlan);
                WriteJson(Path.Combine(tempDir, NativeAotArtifactNames.Manifest), emitResult.Manifest);
                WriteJson(Path.Combine(tempDir, NativeAotArtifactNames.CodegenMetrics), emitResult.CodegenMetrics);

                Console.Error.WriteLine($" {emitResult.GeneratedSources.Count} files");
            }
            else
            {
                Console.Error.WriteLine("  [2/3] Skipping C++ generation (not needed for metrics/analysis)");
            }

            // ── Step 2b (P0): Compile .cpp to .obj once (shared across methods) ──
            NativeCompile.CompilationContext? compileCtx = null;
            bool needsNativeAsm = config.HasSection("raw-aot") || config.HasSection("side-by-side");
            if (needsNativeAsm && needsCpp)
            {
                Console.Error.Write("  [2b/3] Compiling AOT native (once)...");
                compileCtx = NativeCompile.CompileOnce(tempDir);
                Console.Error.WriteLine(compileCtx is null
                    ? " skipped"
                    : compileCtx.CompileSuccess
                        ? $" OK ({compileCtx.ObjectSize})"
                        : $" FAIL ({compileCtx.Error})");
            }

            // ── Step 3 (P1): Parallel JIT capture for all methods ──────────
            int totalMethods = config.MethodNames.Count;
            var methodResults = new ConcurrentDictionary<int, Dictionary<string, object?>>();
            var consoleLock = new object();

            System.Threading.Tasks.Parallel.For(0, totalMethods, idx =>
            {
                var methodName = config.MethodNames[idx];

                // Use contract methodSubjectId for JIT capture and AOT lookup (resolves the
                // real target method in the framework assembly instead of the Subjects DLL
                // Subject_N wrapper). Falls back to methodName if not provided.
                var jitMethodName = idx < config.MethodSubjectIds.Count
                    ? config.MethodSubjectIds[idx]
                    : methodName;

                // Find method in closure (O(1) from pre-built dictionary)
                // Prefer real SubjectId (framework method) over Subject_N wrapper.
                var aotMethod = methodMap.TryGetValue(jitMethodName, out var found)
                    ? found
                    : methodMap.TryGetValue(methodName, out var found2)
                        ? found2
                        : FindMethodInClosure(closureResult, methodName);

                lock (consoleLock)
                {
                    Console.Error.Write($"  [3/3-{idx + 1}/{totalMethods}] {methodName}...");
                }

                if (aotMethod is null)
                {
                    lock (consoleLock)
                        Console.Error.WriteLine(" not found in closure");
                    methodResults[idx] = new Dictionary<string, object?>
                    {
                        ["methodName"] = methodName,
                        ["status"] = "not_found",
                        ["error"] = "Method not found in IL2CPP pipeline output",
                    };
                    return;
                }

                // Read C++ source only if C++ generation happened
                string cppSource = "";
                if (needsCpp && emitResult is not null)
                    cppSource = ReadGeneratedCppForMethod(emitResult, aotMethod);

                // Capture JIT asm — use the full methodSubjectId so JitAsmCapture
                // resolves the real target method from the framework assembly.
                var jitResult = JitAsmCapture.Capture(config.AssemblyPath, jitMethodName);

                lock (consoleLock)
                {
                    if (!jitResult.Success)
                        Console.Error.WriteLine($" warning: {jitResult.Error}");
                    else
                        Console.Error.WriteLine($" {jitResult.Size} bytes, {jitResult.Instructions?.Count ?? 0} instr");
                }

                // Native AOT extraction from pre-compiled .obj
                NativeCompile.NativeCompileResult? nativeResult = null;
                if (needsNativeAsm && compileCtx is not null)
                {
                    var symbol = aotMethod.NativeSymbol ?? "";
                    nativeResult = NativeCompile.ExtractFromObj(compileCtx, symbol);
                }

                // Generate report entry — label with the real methodSubjectId
                var entry = config.Format == "json"
                    ? AsmCompareReport.GenerateJsonEntry(
                        config.AssemblyPath, jitMethodName, jitResult, aotMethod, cppSource, nativeResult, config)
                    : new Dictionary<string, object?>
                    {
                        ["methodName"] = jitMethodName,
                        ["status"] = jitResult.Success ? "ok" : "failed",
                    };

                methodResults[idx] = entry;
            });

            // ── Output: restore ordered list ───────────────────────────────
            var orderedResults = new List<Dictionary<string, object?>>(totalMethods);
            for (int i = 0; i < totalMethods; i++)
                orderedResults.Add(methodResults[i]);

            string output;
            if (config.Format == "json")
            {
                output = totalMethods == 1
                    ? JsonSerializer.Serialize(orderedResults[0], JsonOptions)
                    : JsonSerializer.Serialize(orderedResults, JsonOptions);
            }
            else
            {
                var sb = new StringBuilder();
                int ok = 0, fail = 0;
                foreach (var r in orderedResults)
                {
                    var s = r.GetValueOrDefault("status", "")?.ToString();
                    if (s == "ok" || s is "not_found") ok++; else fail++;
                }
                sb.AppendLine($"asm-compare batch: {ok}/{orderedResults.Count} OK, {fail} failed");
                output = sb.ToString();
            }

            if (config.OutputPath is not null)
            {
                Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(config.OutputPath))!);
                File.WriteAllText(config.OutputPath, output);
                Console.Error.WriteLine($"Report written: {config.OutputPath}");
            }
            else
            {
                Console.WriteLine();
                Console.WriteLine(output);
            }

            return 0;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Error: {ex.Message}");
            return 1;
        }
        finally
        {
            if (!config.KeepTemp && Directory.Exists(tempDir))
                Directory.Delete(tempDir, recursive: true);
        }
    }

    // P0: Determine if any requested section requires C++ code generation
    private static bool _needsCppGeneration(AsmCompareConfig config)
    {
        if (config.AllSections)
            return true;
        foreach (var section in config.Sections)
        {
            if (CppRequiredSections.Contains(section))
                return true;
        }
        return false;
    }

    private static AotCoreIrMethodArtifact? FindMethodInClosure(ManagedClosureResult result, string methodName)
    {
        var methods = result.AotCoreIr.Methods;
        if (methods.Count == 0) return null;

        var searchName = methodName.Replace("::", "/").Trim();

        foreach (var method in result.AotCoreIr.Methods)
        {
            if (method.SubjectId.EndsWith(searchName, StringComparison.OrdinalIgnoreCase) ||
                method.SubjectId.EndsWith(methodName.Replace("::", "."), StringComparison.OrdinalIgnoreCase))
                return method;
        }

        foreach (var method in result.AotCoreIr.Methods)
        {
            if (method.SubjectId.Contains(methodName, StringComparison.OrdinalIgnoreCase))
                return method;
        }

        foreach (var method in result.AotCoreIr.Methods)
        {
            if (method.MethodId.EndsWith(methodName, StringComparison.OrdinalIgnoreCase))
                return method;
        }

        return null;
    }

    private static string ReadGeneratedCppForMethod(NativeAotResult emitResult, AotCoreIrMethodArtifact method)
    {
        var sb = new StringBuilder();

        foreach (var source in emitResult.GeneratedSources)
        {
            sb.AppendLine($"// File: {source.RelativePath}");
            sb.AppendLine();

            var content = source.Contents;

            var symbol = method.NativeSymbol;
            if (!string.IsNullOrEmpty(symbol) && !string.IsNullOrEmpty(content) && content.Contains(symbol))
            {
                var extracted = ExtractFunctionContent(content, symbol);
                sb.AppendLine(extracted);
            }
            else
            {
                sb.AppendLine(content);
            }

            sb.AppendLine();
        }

        return sb.ToString();
    }

    private static string ExtractFunctionContent(string content, string symbol)
    {
        var lines = content.Split('\n');
        var result = new StringBuilder();
        bool inFunction = false;
        int braceDepth = 0;

        foreach (var line in lines)
        {
            if (!inFunction)
            {
                if (line.Contains(symbol) && (line.Contains('{') || line.Contains("->")))
                {
                    inFunction = true;
                    braceDepth = line.Count(c => c == '{') - line.Count(c => c == '}');
                }
                result.AppendLine(line.TrimEnd());
            }
            else
            {
                result.AppendLine(line.TrimEnd());
                braceDepth += line.Count(c => c == '{') - line.Count(c => c == '}');
                if (braceDepth <= 0)
                    break;
            }
        }

        return result.ToString();
    }

    private static void WriteArtifacts(string root, ManagedClosureResult result)
    {
        Directory.CreateDirectory(root);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.TypedIlIr), result.TypedIlIr);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.AotCoreIr), result.AotCoreIr);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.AotManifest), result.AotManifest);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.MetadataRegistration), result.MetadataRegistration);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.SupplementalMetadataTemplate), result.SupplementalMetadataTemplate);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.CodeRegistration), result.CodeRegistration);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), result.GenericInstantiationDemandGraph);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.GenericCapabilityMatrix), result.GenericCapabilityMatrix);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.OptimizationFacts), result.OptimizationFacts);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.PreserveDescriptor), result.PreserveDescriptor);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.NativeReferenceLoweringPlan), result.NativeReferenceLoweringPlan);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.NativeAotLoweringPlan), result.NativeAotLoweringPlan);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);
    }

    private static void WriteJson<T>(string path, T value)
    {
        var dir = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);
        File.WriteAllText(path, JsonSerializer.Serialize(value, JsonOptions) + Environment.NewLine);
    }
}
