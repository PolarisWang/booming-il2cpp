using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.CodeGen;
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

    public static int Run(string[] args)
    {
        var config = AsmCompareConfig.Parse(args);
        if (config.IsHelp)
            return 0;

        var tempDir = Path.Combine(Path.GetTempPath(), "chaos-asm-compare-" + Guid.NewGuid().ToString("N"));

        try
        {
            // ── Step 1: Run IL2CPP closure pipeline ────────────────────────
            Console.Error.Write("  [1/3] Running IL2CPP pipeline...");
            ChaosTrace.Point("asm-compare.pipeline", "codegen");

            var pipeline = new PipelinePlan();
            var closureResult = pipeline.Execute(new ManagedClosureRequest(
                config.AssemblyPath,
                tempDir,
                EntryPointSubjectIdOverride: null,
                AdditionalAssemblyPaths: null,
                FullAssemblyClosure: true));

            Console.Error.WriteLine($" {closureResult.AotCoreIr.Methods.Count} methods lowered");

            // Find the target method in the closure results
            var aotMethod = FindMethodInClosure(closureResult, config.MethodName);
            if (aotMethod is null)
            {
                Console.Error.WriteLine($"Error: method '{config.MethodName}' not found in IL2CPP pipeline output.");
                Console.Error.WriteLine("Available methods (first 30):");
                var count = 0;
                foreach (var m in closureResult.AotCoreIr.Methods)
                {
                    if (count++ >= 30) break;
                    Console.Error.WriteLine($"  {m.SubjectId}");
                }
                return 1;
            }

            // ── Step 2: Run NativeAotEmitter to generate C++ ──────────────
            Console.Error.Write("  [2/3] Generating C++ (NativeAot)...");
            ChaosTrace.Point("asm-compare.codegen", "codegen");

            WriteArtifacts(tempDir, closureResult);

            var emitter = new NativeAotEmitter();
            var emitResult = emitter.Generate(new NativeAotRequest(tempDir, tempDir));

            foreach (var source in emitResult.GeneratedSources)
            {
                var targetPath = Path.Combine(tempDir, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
                Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
                File.WriteAllText(targetPath, source.Contents);
            }

            // Write codegen metrics/lowering plan too
            WriteJson(Path.Combine(tempDir, NativeAotArtifactNames.LoweringPlan), emitResult.LoweringPlan);
            WriteJson(Path.Combine(tempDir, NativeAotArtifactNames.Manifest), emitResult.Manifest);
            WriteJson(Path.Combine(tempDir, NativeAotArtifactNames.CodegenMetrics), emitResult.CodegenMetrics);

            Console.Error.WriteLine($" {emitResult.GeneratedSources.Count} files");

            // Read generated C++ content for this method
            var cppSource = ReadGeneratedCppForMethod(emitResult, aotMethod);

            // ── Step 3: Capture JIT asm ───────────────────────────────────
            Console.Error.Write("  [3/3] Capturing JIT asm...");
            ChaosTrace.Point("asm-compare.jit_capture", "codegen");

            var jitResult = JitAsmCapture.Capture(config.AssemblyPath, config.MethodName);
            if (!jitResult.Success)
                Console.Error.WriteLine($" warning: {jitResult.Error}");
            else
                Console.Error.WriteLine($" {jitResult.Size} bytes, {jitResult.Instructions?.Count ?? 0} instructions");

            // ── Step 3b: Native AOT compilation (Phase 3) ─────────────────
            NativeCompile.NativeCompileResult? nativeResult = null;
            if (config.HasSection("raw-aot") || config.HasSection("side-by-side"))
            {
                Console.Error.Write("  [3b/4] Compiling AOT native...");
                ChaosTrace.Point("asm-compare.native_compile", "codegen");
                nativeResult = NativeCompile.Compile(tempDir, aotMethod?.NativeSymbol ?? "");
                if (nativeResult.FoundMsvc && nativeResult.CompileSuccess)
                    Console.Error.WriteLine($" OK ({nativeResult.ObjectSize})");
                else if (!nativeResult.FoundMsvc)
                    Console.Error.WriteLine(" MSVC not found");
                else
                    Console.Error.WriteLine(" FAIL (see report)");
            }

            // ── Step 4: Generate report ───────────────────────────────────
            var report = config.Format == "json"
                ? AsmCompareReport.GenerateJson(
                    config.AssemblyPath,
                    config.MethodName,
                    jitResult,
                    aotMethod,
                    cppSource,
                    nativeResult,
                    config)
                : AsmCompareReport.Generate(
                    config.AssemblyPath,
                    config.MethodName,
                    jitResult,
                    aotMethod,
                    cppSource,
                    nativeResult,
                    config);

            if (config.OutputPath is not null)
            {
                Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(config.OutputPath))!);
                File.WriteAllText(config.OutputPath, report);
                Console.Error.WriteLine($"Report written: {config.OutputPath}");
            }
            else
            {
                Console.WriteLine();
                Console.WriteLine("════════════════════════════════════════════════════════════════════════════");
                Console.WriteLine(report);
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

    private static AotCoreIrMethodArtifact? FindMethodInClosure(ManagedClosureResult result, string methodName)
    {
        var searchName = methodName.Replace("::", "/").Trim();

        // Prefer SubjectId suffix match: "ConvertCharSubjects/ConvertCharSubjects::Run"
        foreach (var method in result.AotCoreIr.Methods)
        {
            if (method.SubjectId.EndsWith(searchName, StringComparison.OrdinalIgnoreCase) ||
                method.SubjectId.EndsWith(methodName.Replace("::", "."), StringComparison.OrdinalIgnoreCase))
                return method;
        }

        // Broader: SubjectId contains the method name
        foreach (var method in result.AotCoreIr.Methods)
        {
            if (method.SubjectId.Contains(methodName, StringComparison.OrdinalIgnoreCase))
                return method;
        }

        // Broader: MethodId matches
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

            // Try to extract just the function for this method
            var symbol = method.NativeSymbol;
            if (!string.IsNullOrEmpty(symbol) && content.Contains(symbol))
            {
                var extracted = ExtractFunctionContent(content, symbol);
                sb.AppendLine(extracted);
            }
            else
            {
                // Show entire file if method-specific extraction fails
                sb.AppendLine(content);
            }

            sb.AppendLine();
        }

        return sb.ToString();
    }

    private static string ExtractFunctionContent(string content, string symbol)
    {
        // Find the function definition and extract it
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