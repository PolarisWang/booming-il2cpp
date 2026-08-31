using System.Reflection.PortableExecutable;
using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;
using Chaos.IL2CPP.Pipeline;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Core service for converting .NET assemblies to native C++ source code.
/// Supports arbitrary DLL/exe input (not just ATG test subjects).
///
/// This is the single entry point for IL→C++ translation used by both
/// app-mode (--mode app) and test-mode (--mode test) publish flows.
/// </summary>
internal static class ConvertService
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
        NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
    };

    public sealed record ConversionResult
    {
        public required string AssemblyName { get; init; }
        public string? EntryPointSubjectId { get; init; }
        public required IReadOnlyList<NativeAotGeneratedSource> GeneratedDirs { get; init; }
        public required IReadOnlyList<string> SubjectMethods { get; init; }
        public required string OutputRoot { get; init; }
        public required string SdkRoot { get; init; }
        public int MethodCount { get; init; }
    }

    /// <summary>
    /// Convert a set of assemblies to native C++ source.
    /// </summary>
    /// <param name="assemblyPaths">Paths to the input assemblies (.dll/.exe).</param>
    /// <param name="outputRoot">Directory where codegen output will be written.</param>
    /// <param name="sdkRoot">Directory for the chaos SDK (chaos-config.cmake, libs, headers). Null to skip SDK emit.</param>
    /// <param name="entryPoint">Optional entry point subject ID override. If null, auto-detected.</param>
    /// <param name="assemblyDirs">Additional assembly search directories.</param>
    /// <param name="configTier">Build config tier (check/profile/ship).</param>
    /// <param name="fullClosure">Enable full assembly closure analysis.</param>
    /// <param name="subjectMethods">Optional filter for which methods to translate.</param>
    public static ConversionResult? Convert(
        IReadOnlyList<string> assemblyPaths,
        string outputRoot,
        string? sdkRoot = null,
        string? entryPoint = null,
        IReadOnlyList<string>? assemblyDirs = null,
        string configTier = "check",
        bool fullClosure = false,
        IReadOnlyList<string>? subjectMethods = null)
    {
        if (assemblyPaths.Count == 0)
        {
            Console.Error.WriteLine("Error: no assembly paths specified.");
            return null;
        }

        outputRoot = Path.GetFullPath(outputRoot);
        Directory.CreateDirectory(outputRoot);

        Console.WriteLine($"  Assemblies: {assemblyPaths.Count}");
        foreach (var asm in assemblyPaths)
            Console.WriteLine($"    {asm}");
        Console.WriteLine($"  Output:   {outputRoot}");

        // ── Step 1: Collect managed dependency DLLs ────────────────────────
        var additionalPaths = new List<string>();
        var entryAssemblyNames = new HashSet<string>(
            assemblyPaths.Select(Path.GetFileNameWithoutExtension),
            StringComparer.OrdinalIgnoreCase);

        if (assemblyDirs != null)
        {
            foreach (var dir in assemblyDirs)
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
        }

        // ── Step 2: Run pipeline ───────────────────────────────────────────
        // NOTE: entry-point resolution is delegated to the loader. When entryPoint
        // is null, the loader auto-detects Main from the PE entry-point token
        // (same rule as dotnet); passing a manual override here risks a
        // SubjectId-format mismatch. DetectEntryPoint() exists for cases where a
        // caller explicitly needs the auto-detected ID (e.g. manifest reporting).
        Console.Write("  [1/3] Running IL2CPP pipeline...");
        var pipeline = new PipelinePlan();

        if (assemblyPaths.Count == 1)
        {
            // FullAssemblyClosure is driven ONLY by the caller's explicit flag.  It
            // is NOT forced by the absence of an entryPoint override: auto-detection
            // of Main is orthogonal to closure depth (the loader re-derives the entry
            // from the PE token independently).  Tying closure depth to entry-point
            // null-ness would silently expand codegen scope from the entry method to
            // the whole assembly whenever the caller omitted an override, with no
            // config escape hatch — surprising large translations.
            var request = new ManagedClosureRequest(
                assemblyPaths[0],
                outputRoot,
                EntryPointSubjectIdOverride: entryPoint,
                AdditionalAssemblyPaths: additionalPaths,
                FullAssemblyClosure: fullClosure)
            {
                SubjectMethodIds = subjectMethods?.ToHashSet(StringComparer.Ordinal),
            };

            var closureResult = pipeline.Execute(request);
            if (closureResult.IsFailure)
            {
                Console.Error.WriteLine($"Pipeline failed: [{closureResult.Error!.Code}] {closureResult.Error.Message}");
                return null;
            }
            var result = closureResult.Value!;
            Console.WriteLine($" {result.AotCoreIr.Methods.Count} methods lowered");

            Console.Write("  [2/3] Emitting C++ (NativeAot, direct)...");
            var emitter = new FullAssemblyEmitter();
            NativeAotResult emitResult;
            try
            {
                emitResult = emitter.Emit(result, outputRoot, CodegenMode.Aot,
                    subjectMethods?.ToList(), goldProfilePath: null);
            }
            catch (OutOfMemoryException oom)
            {
                Console.Error.WriteLine($"\n  OUT OF MEMORY during C++ emission: {oom.Message}");
                return null;
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"\n  ERROR during C++ emission: {ex.GetType().FullName}: {ex.Message}");
                return null;
            }
            Console.WriteLine($" done ({emitResult.GeneratedSources.Count} files)");

            // Write closure artifacts
            Console.Write("  [3/3] Writing closure artifacts...");
            WriteArtifacts(outputRoot, result);
            Console.WriteLine(" done");

            // Emit SDK if requested
            if (sdkRoot != null)
            {
                Console.Write("  Emitting chaos-sdk...");
                var repoRoot = ResolveRepoRoot();
                var nativePresetDir = ResolveNativePreset();
                var nativeLibDir = Path.Combine(repoRoot, "artifacts", "presets", nativePresetDir);
                var assemblyName = result.ClosureManifest?.AssemblyName ?? "unknown";
                var sdkEmitter = new SdkEmitter();
                sdkEmitter.EmitSdk(sdkRoot, outputRoot, repoRoot, nativeLibDir, "RelWithDebInfo", assemblyName, configTier);
                Console.WriteLine(" done");
            }

            // Collect subject method IDs from the result
            var subjectIds = new List<string>();
            if (result.AotCoreIr?.Methods != null)
            {
                foreach (var method in result.AotCoreIr.Methods)
                {
                    if (!string.IsNullOrWhiteSpace(method.SubjectId))
                        subjectIds.Add(method.SubjectId);
                }
            }

            var assemblyName2 = result.ClosureManifest?.AssemblyName ?? "unknown";
            return new ConversionResult
            {
                AssemblyName = assemblyName2,
                EntryPointSubjectId = entryPoint,
                GeneratedDirs = emitResult.GeneratedSources,
                SubjectMethods = subjectIds,
                OutputRoot = outputRoot,
                SdkRoot = sdkRoot ?? outputRoot,
                MethodCount = result.AotCoreIr?.Methods.Count ?? 0,
            };
        }
        else
        {
            // Multi-assembly path
            var multiRequest = new MultiAssemblyClosureRequest(
                assemblyPaths.ToList(),
                outputRoot,
                EntryPointSubjectIdOverride: entryPoint,
                AdditionalAssemblyPaths: additionalPaths);

            var resultsResult = pipeline.ExecuteMulti(multiRequest);
            if (resultsResult.IsFailure)
            {
                Console.Error.WriteLine($"Pipeline failed: [{resultsResult.Error!.Code}] {resultsResult.Error.Message}");
                return null;
            }
            var results = resultsResult.Value!;
            Console.WriteLine($" {results.Sum(r => r.AotCoreIr?.Methods.Count ?? 0)} methods across {results.Count} assemblies");

            Console.Write("  [2/3] Emitting C++ (NativeAot, direct)...");
            int totalFiles = 0;
            var emitResults = new List<NativeAotResult>();
            var allSubjectIds = new List<string>();
            foreach (var result in results)
            {
                var asmOutput = result.OutputRootPath;
                var asmEmitter = new FullAssemblyEmitter();
                var asmEmitResult = asmEmitter.Emit(result, asmOutput, CodegenMode.Aot, subjectMethods?.ToList());
                totalFiles += asmEmitResult.GeneratedSources.Count;
                emitResults.Add(asmEmitResult);

                if (result.AotCoreIr?.Methods != null)
                {
                    foreach (var method in result.AotCoreIr.Methods)
                    {
                        if (!string.IsNullOrWhiteSpace(method.SubjectId))
                            allSubjectIds.Add(method.SubjectId);
                    }
                }
            }
            Console.WriteLine($" done ({totalFiles} files across {results.Count} assemblies)");

            Console.Write("  [3/3] Writing closure artifacts...");
            foreach (var result in results)
            {
                var assemblyOutput = result.OutputRootPath;
                Directory.CreateDirectory(assemblyOutput);
                WriteArtifacts(assemblyOutput, result);
            }
            Console.WriteLine(" done");

            // Emit SDK if requested
            if (sdkRoot != null)
            {
                Console.Write("  Emitting chaos-sdk...");
                var repoRoot = ResolveRepoRoot();
                var nativePresetDir = ResolveNativePreset();
                var nativeLibDir = Path.Combine(repoRoot, "artifacts", "presets", nativePresetDir);
                var assemblyName = results.FirstOrDefault()?.ClosureManifest?.AssemblyName ?? "combined";
                var sdkEmitter = new SdkEmitter();
                sdkEmitter.EmitSdk(sdkRoot, outputRoot, repoRoot, nativeLibDir, "RelWithDebInfo", assemblyName, configTier);
                Console.WriteLine(" done");
            }

            return new ConversionResult
            {
                AssemblyName = "combined",
                EntryPointSubjectId = entryPoint,
                GeneratedDirs = emitResults.SelectMany(r => r.GeneratedSources).ToList(),
                SubjectMethods = allSubjectIds,
                OutputRoot = outputRoot,
                SdkRoot = sdkRoot ?? outputRoot,
                MethodCount = results.Sum(r => r.AotCoreIr?.Methods.Count ?? 0),
            };
        }
    }

    /// <summary>
    /// Auto-detect the entry point (Main method) from a .NET assembly.
    /// Uses the .NET PE header's EntryPointToken, same as the dotnet host.
    /// </summary>
    public static string? DetectEntryPoint(string assemblyPath)
    {
        try
        {
            if (!File.Exists(assemblyPath))
                return null;

            // Try to load via reflection to get the entry point
            // (Assembly.LoadFrom may fail on missing deps; fallback to PE metadata scan)
            try
            {
                var asm = System.Runtime.Loader.AssemblyLoadContext.Default
                    .LoadFromAssemblyPath(assemblyPath);
                var entryPoint = asm.EntryPoint;
                if (entryPoint == null)
                    return null;

                var declaringType = entryPoint.DeclaringType;
                if (declaringType == null)
                    return null;

                var typeName = declaringType.FullName ?? declaringType.Name;
                if (string.IsNullOrWhiteSpace(typeName))
                    return null;

                var methodName = entryPoint.Name;
                var returnType = entryPoint.ReturnType?.Name ?? "System.Void";

                var paramParts = new List<string>();
                foreach (var p in entryPoint.GetParameters())
                {
                    var pt = p.ParameterType;
                    paramParts.Add(BuildTypeName(pt));
                }
                var paramStr = string.Join(",", paramParts);

                // SubjectId format: {assembly}/{namespace}.{type}::{method}:{returnType}({params})
                // with full type names (System.Int32, System.String, etc.)
                var assemblyName = asm.GetName().Name ?? "unknown";
                return $"{assemblyName}/{typeName}::{methodName}:{returnType}({paramStr})";

                static string BuildTypeName(Type type)
                {
                    if (type.IsArray)
                    {
                        var elem = BuildTypeName(type.GetElementType()!);
                        return elem + "[]";
                    }
                    if (type.IsGenericType)
                    {
                        var def = type.GetGenericTypeDefinition();
                        var name = def.FullName;
                        var tick = name!.IndexOf('`');
                        var baseName = tick >= 0 ? name[..tick] : name;
                        var args = string.Join(",", type.GetGenericArguments().Select(BuildTypeName));
                        return $"{baseName}<{args}>";
                    }
                    var fullName = type.FullName ?? type.Name;
                    // Normalize common abbreviations to full System names
                    return fullName switch
                    {
                        "System.String" => "System.String",
                        "System.Int32" => "System.Int32",
                        "System.Void" => "System.Void",
                        "System.Boolean" => "System.Boolean",
                        "System.Double" => "System.Double",
                        "System.Single" => "System.Single",
                        "System.Int64" => "System.Int64",
                        _ => fullName,
                    };
                }
            }
            catch
            {
                // Fallback: try PE metadata
                return DetectEntryPointFromPeMetadata(assemblyPath);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"  [convert] Warning: entry point detection failed: {ex.Message}");
            return null;
        }
    }

    private static string? DetectEntryPointFromPeMetadata(string assemblyPath)
    {
        try
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
                return null;

            var entryPointToken = peReader.PEHeaders?.CorHeader?.EntryPointTokenOrRelativeVirtualAddress;
            if (entryPointToken == null || entryPointToken.Value == 0)
                return null;

            // PE-metadata token-only fallback: we cannot resolve the real SubjectId
            // ({assembly}/{ns}.{Type}::{Method}:{Ret}({Params})) from PEReader alone
            // because it does not expose Assembly.FullName or type/method names.
            // Returning a fabricated string like "(entry point token: 0x{token})" would
            // be a pseudo-SubjectId that no caller can match against the canonical format.
            // Instead, return null so callers do not attempt to use a bogus SubjectId.
            // The entry point is still auto-detected by the loader (ManagedClosureRequest)
            // from the PE entry-point token independently.
            Console.WriteLine($"  [convert] warning: entry-point metadata (token 0x{entryPointToken.Value:X8}) "
                              + "cannot be resolved to a SubjectId from PE metadata. "
                              + "The loader will auto-detect the entry point from the token.");
            return null;
        }
        catch
        {
            return null;
        }
    }

    private static void WriteArtifacts(string outputDir, ManagedClosureResult result)
    {
        try
        {
            Directory.CreateDirectory(outputDir);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.AotCoreIr), result.AotCoreIr);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.AotManifest), result.AotManifest);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.MetadataRegistration), result.MetadataRegistration);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.CodeRegistration), result.CodeRegistration);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), result.GenericInstantiationDemandGraph);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.OptimizationFacts), result.OptimizationFacts);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.PreserveDescriptor), result.PreserveDescriptor);
            WriteJson(Path.Combine(outputDir, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);
        }
        catch
        {
            // Non-fatal — artifacts are optional
        }
    }

    private static void WriteJson<T>(string path, T value)
    {
        if (value == null)
            return;
        File.WriteAllText(path, JsonSerializer.Serialize(value, JsonOptions));
    }

    private static string? ResolveRepoRoot()
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
    /// Resolve the native build preset directory (under artifacts/presets/) for the
    /// current runtime platform + architecture.  On unsupported platforms this logs
    /// an explicit warning and falls back to linux-x64-profile so the ABI mismatch
    /// is surfaced rather than silently linking the wrong-arch library.
    /// </summary>
    private static string ResolveNativePreset()
    {
        var os = System.Runtime.InteropServices.RuntimeInformation
            .IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Windows)
            ? "windows"
            : System.Runtime.InteropServices.RuntimeInformation
                .IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Linux)
                ? "linux"
                : System.Runtime.InteropServices.RuntimeInformation
                    .IsOSPlatform(System.Runtime.InteropServices.OSPlatform.OSX)
                    ? "osx"
                    : "unknown";
        var arch = System.Runtime.InteropServices.RuntimeInformation.ProcessArchitecture
            .ToString().ToLowerInvariant();

        // Known preset combinations.  Add new entries as presets are produced.
        var known = new (string Os, string Arch, string Dir)[]
        {
            ("windows", "x64", "windows-x64-reference"),
            ("linux", "x64", "linux-x64-profile"),
            ("linux", "arm64", "linux-arm64-profile"),
            ("osx", "x64", "osx-x64-profile"),
            ("osx", "arm64", "osx-arm64-profile"),
        };

        foreach (var (o, a, d) in known)
        {
            if (string.Equals(o, os, StringComparison.OrdinalIgnoreCase) &&
                string.Equals(a, arch, StringComparison.OrdinalIgnoreCase))
            {
                return d;
            }
        }

        Console.Error.WriteLine($"  [convert] WARNING: no native preset for OS='{os}' arch='{arch}'; "
                                + "falling back to linux-x64-profile (ABI may mismatch).");
        return "linux-x64-profile";
    }
}