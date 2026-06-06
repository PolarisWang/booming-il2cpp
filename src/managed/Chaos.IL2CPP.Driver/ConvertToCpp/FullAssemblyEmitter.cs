using System.Collections.Generic;
using System.IO;
using System.Text;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Driver;

internal sealed class FullAssemblyEmitter
{
    public NativeAotResult Emit(
        ManagedClosureResult closureResult,
        string outputRoot,
        CodegenMode mode = CodegenMode.Aot,
        HashSet<string>? subjectMethods = null,
        IReadOnlyDictionary<string, string>? bridgeRedirectMap = null,
        IReadOnlyList<AotCoreIrMethodArtifact>? bridgeCompiledMethods = null)
    {
        ArgumentNullException.ThrowIfNull(closureResult);

        var nativeAotEmitter = new NativeAotEmitter();
        var emitResult = nativeAotEmitter.GenerateFromArtifacts(
            closureResult.NativeAotLoweringPlan,
            closureResult.AotCoreIr,
            closureResult.ClosureManifest,
            closureResult.MetadataRegistration,
            closureResult.SupplementalMetadataTemplate,
            outputRoot,
            mode,
            subjectMethods);

        foreach (var source in emitResult.GeneratedSources)
        {
            var targetPath = Path.Combine(outputRoot, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            if (source.ContentsBuilder is { } builder)
            {
                using var writer = new StreamWriter(targetPath, append: false, Encoding.UTF8);
                foreach (var chunk in builder.GetChunks())
                    writer.Write(chunk.Span);
            }
            else
            {
                File.WriteAllText(targetPath, source.Contents, Encoding.UTF8);
            }
        }

        // Phase 1c: Bridge redirect table with real function bodies from BridgeMethodBodyEmitter
        if (bridgeRedirectMap is { Count: > 0 })
        {
            var redirectPath = Path.Combine(outputRoot, "bridge-redirect.generated.cpp");
            Directory.CreateDirectory(Path.GetDirectoryName(redirectPath)!);
            File.WriteAllText(redirectPath,
                GenerateBridgeRedirectCpp(bridgeRedirectMap, bridgeCompiledMethods), Encoding.UTF8);

            var registerPath = Path.Combine(outputRoot, "chaos_register_bridge_redirects.generated.cpp");
            Directory.CreateDirectory(Path.GetDirectoryName(registerPath)!);
            File.WriteAllText(registerPath, GenerateBridgeRegisterCpp(), Encoding.UTF8);
            Console.Error.WriteLine($"[BRIDGE-AOT] {bridgeRedirectMap.Count} redirects + registration");
        }

        Console.WriteLine($"    emitted {emitResult.GeneratedSources.Count} files -> {outputRoot}");
        Console.WriteLine($"    ({emitResult.CodegenMetrics.TotalMethodCount} methods, " +
            $"{emitResult.CodegenMetrics.StructuredMethodCount} structured, " +
            $"{emitResult.CodegenMetrics.FlatFallbackCount} flat-fallback)");

        return emitResult;
    }

    private static string GenerateBridgeRedirectCpp(IReadOnlyDictionary<string, string> redirectMap,
        IReadOnlyList<AotCoreIrMethodArtifact>? bridgeCompiledMethods = null)
    {
        var sb = new StringBuilder();
        sb.AppendLine("// Auto-generated bridge redirect table — Phase 1c");
        // Build subjectId → compiled method lookup for real body emission
        var compiledLookup = bridgeCompiledMethods?
            .Where(m => !string.IsNullOrEmpty(m.SubjectId))
            .ToDictionary(m => m.SubjectId!, StringComparer.Ordinal)
            ?? new Dictionary<string, AotCoreIrMethodArtifact>();

        sb.AppendLine("#include <cstring>");
        sb.AppendLine("#include <cstdint>");
        sb.AppendLine("#include <cstddef>");
        sb.AppendLine();
        sb.AppendLine("using CHAOS_IL2CPP_INT32 = int32_t;");
        sb.AppendLine("using CHAOS_IL2CPP_UINT32 = uint32_t;");
        sb.AppendLine("using CHAOS_IL2CPP_INT64 = int64_t;");
        sb.AppendLine("using CHAOS_IL2CPP_INTPTR = intptr_t;");
        sb.AppendLine("using CHAOS_IL2CPP_FLOAT32 = float;");
        sb.AppendLine("using CHAOS_IL2CPP_FLOAT64 = double;");
        sb.AppendLine("#define CHAOS_IL2CPP_STRING_ID(s) reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s)");
        sb.AppendLine("extern \"C\" int32_t kChaosExternalRuntimeCount;");
        sb.AppendLine("extern \"C\" void* kChaosExternalRuntimeFnTable[];");
        sb.AppendLine("extern \"C\" const char* kChaosExternalRuntimeSubjects[];");
        sb.AppendLine("extern \"C\" void* ResolveBridge(const char* subjectId) noexcept;");
        sb.AppendLine("#ifdef _MSC_VER");
        sb.AppendLine("#pragma warning(push)");
        sb.AppendLine("#pragma warning(disable: 2362 4702) // goto skipping init");
        sb.AppendLine("#endif");
        sb.AppendLine();

        // Emit real function bodies for bridge-compiled methods
        int mi = 0;
        foreach (var (subjectId, sym) in redirectMap)
        {
            if (compiledLookup.TryGetValue(subjectId, out var method))
            {
                var body = BridgeMethodBodyEmitter.EmitMethodBody(method, mi, redirectMap);
                sb.AppendLine(body);
                sb.AppendLine();
            }
            else
            {
                // Stub fallback (should not happen — all compiled methods should be in the map)
                sb.AppendLine($"extern \"C\" void {sym}() {{ }}");
            }
            mi++;
        }

        sb.AppendLine("extern \"C\" void* ResolveBridge(const char* subjectId) noexcept {");
        sb.AppendLine("    if (subjectId == nullptr) return nullptr;");
        foreach (var (sid, sym) in redirectMap)
        {
            sb.AppendLine($"    if (std::strcmp(subjectId, {EscapeCppString(sid)}) == 0)");
            sb.AppendLine($"        return reinterpret_cast<void*>(&{sym});");
        }
        sb.AppendLine("    return nullptr;");
        sb.AppendLine("}");
        sb.AppendLine();
        sb.AppendLine("extern \"C\" void* (*ChaosBridgeRedirect)(const char*);");
        sb.AppendLine("namespace { struct _BridgeReg { _BridgeReg() noexcept { ChaosBridgeRedirect = &ResolveBridge; } } _breg; }");
        sb.AppendLine();
        return sb.ToString();
    }

    private static string GenerateBridgeRegisterCpp()
    {
        var sb = new StringBuilder();
        sb.AppendLine("// Auto-generated bridge redirect registration — Phase 2");
        sb.AppendLine("// Patches kChaosExternalRuntimeFnTable with compiled function pointers.");
        sb.AppendLine("#include <cstdint>");
        sb.AppendLine();
        sb.AppendLine("extern \"C\" void* kChaosExternalRuntimeFnTable[];");
        sb.AppendLine("extern \"C\" const char* kChaosExternalRuntimeSubjects[];");
        sb.AppendLine("extern \"C\" int32_t kChaosExternalRuntimeCount;");
        sb.AppendLine("extern \"C\" void* ResolveBridge(const char* subjectId) noexcept;");
        sb.AppendLine();
        sb.AppendLine("namespace {");
        sb.AppendLine("    struct BridgeRegistrar {");
        sb.AppendLine("        BridgeRegistrar() noexcept {");
        sb.AppendLine("            for (int32_t i = 0; i < kChaosExternalRuntimeCount; i++) {");
        sb.AppendLine("                const auto* sub = kChaosExternalRuntimeSubjects[i];");
        sb.AppendLine("                if (sub == nullptr) continue;");
        sb.AppendLine("                auto* fn = ResolveBridge(sub);");
        sb.AppendLine("                if (fn != nullptr) kChaosExternalRuntimeFnTable[i] = fn;");
        sb.AppendLine("            }");
        sb.AppendLine("        }");
        sb.AppendLine("    } _reg;");
        sb.AppendLine("}");
        return sb.ToString();
    }

    private static string EscapeCppString(string s)
    {
        var sb = new StringBuilder(s.Length + 2);
        sb.Append('"');
        foreach (char c in s)
        {
            switch (c)
            {
                case '\\': sb.Append("\\\\"); break;
                case '"': sb.Append("\\\""); break;
                case '\n': sb.Append("\\n"); break;
                case '\r': sb.Append("\\r"); break;
                case '\t': sb.Append("\\t"); break;
                default: sb.Append(c); break;
            }
        }
        sb.Append('"');
        return sb.ToString();
    }
}
