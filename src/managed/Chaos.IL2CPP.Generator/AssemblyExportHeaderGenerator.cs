using System.Text;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Generates per-assembly C++ export header files that declare only the types
/// and methods consumed by other assemblies, eliminating the need for each
/// translation unit to include all types from all 200+ assemblies in a closure.
///
/// The generated header provides:
///   1. Forward declarations for types referenced cross-assembly
///   2. Extern "C" declarations for methods called cross-assembly
///
/// Assembly A consuming a method from Assembly B:
///   → A #includes "chaos_System_Collections_exports.h" (minimal, not ALL of B)
///   → Only types/methods that A actually calls from B are declared
/// </summary>
internal sealed class AssemblyExportHeaderGenerator
{
    private readonly AssemblyExportRegistry _exportRegistry;
    private readonly CrossAssemblySymbolRegistry _symbolRegistry;
    private readonly string _assemblyName;

    // SubjectId format: "AssemblyName/Namespace.Type::Method"
    // Type SubjectId format: "AssemblyName/Namespace.Type"
    private const char SubjectIdSeparator = '/';
    private const char MethodSeparator = ':';

    public AssemblyExportHeaderGenerator(
        AssemblyExportRegistry exportRegistry,
        CrossAssemblySymbolRegistry symbolRegistry,
        string assemblyName)
    {
        _exportRegistry = exportRegistry;
        _symbolRegistry = symbolRegistry;
        _assemblyName = assemblyName;
    }

    /// <summary>
    /// Generate the C++ export header content for the given assembly.
    /// Returns empty string when the assembly has no cross-assembly consumers.
    /// </summary>
    public string GenerateExportHeader()
    {
        var exports = _exportRegistry.GetExportsForAssembly(_assemblyName);
        if (exports == null || exports.Count == 0)
            return string.Empty;

        var typeDeclarations = new HashSet<string>(StringComparer.Ordinal);
        var methodDeclarations = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);

        foreach (var subjectId in exports)
        {
            // Extract type name from subjectId: "Assembly/Namespace.Type::Method" → "Namespace.Type"
            var typeName = ExtractTypeName(subjectId);
            if (typeName != null)
                typeDeclarations.Add(typeName);

            // Check if symbol has cross-assembly conflict (needs disambiguation suffix)
            var symbol = _symbolRegistry.HasConflict(subjectId)
                ? _symbolRegistry.GetUniqueSymbol(_assemblyName, SanitizeSubjectId(subjectId))
                : "chaos_external_runtime_" + SanitizeSubjectId(subjectId);

            // Append parameter count to differentiate overloaded extern "C" functions.
            // C++ does not allow overloading extern "C" functions with different parameter
            // lists, so we must generate unique symbol names.
            var paramCount = CountMethodParameters(subjectId);
            if (paramCount >= 0)
                symbol += "_" + paramCount;

            if (!methodDeclarations.TryGetValue(typeName ?? "", out var symbols))
            {
                symbols = new HashSet<string>(StringComparer.Ordinal);
                methodDeclarations[typeName ?? ""] = symbols;
            }
            symbols.Add(symbol);
        }

        var sb = new StringBuilder();
        sb.AppendLine("// ── Auto-generated cross-assembly export header ──");
        sb.AppendLine($"// Assembly '{_assemblyName}' exports these declarations");
        sb.AppendLine("// for consumption by other assemblies in the closure.");
        sb.AppendLine("#pragma once");
        sb.AppendLine();

        // Type forward declarations
        if (typeDeclarations.Count > 0)
        {
            sb.AppendLine("// ── Type forward declarations ──");
            foreach (var typeName in typeDeclarations.OrderBy(t => t))
            {
                var sanitizedTypeName = SanitizeSubjectId(typeName);
                sb.AppendLine($"struct chaos_type_{sanitizedTypeName};");
                sb.AppendLine($"extern uint64_t chaos_type_id_{sanitizedTypeName};");
            }
            sb.AppendLine();
        }

        // Extern "C" method declarations
        if (methodDeclarations.Count > 0)
        {
            sb.AppendLine("// ── Extern \"C\" method declarations ──");
            sb.AppendLine("#if defined(__cplusplus)");
            sb.AppendLine("extern \"C\" {");
            sb.AppendLine("#endif");
            foreach (var (_, symbols) in methodDeclarations.OrderBy(kv => kv.Key))
            {
                foreach (var symbol in symbols.OrderBy(s => s))
                {
                    sb.AppendLine($"CHAOS_IL2CPP_INTPTR {symbol}();");
                }
            }
            sb.AppendLine("#if defined(__cplusplus)");
            sb.AppendLine("}");
            sb.AppendLine("#endif");
        }

        return sb.ToString();
    }

    /// <summary>
    /// Extract the type portion from a SubjectId.
    /// "System.Private.CoreLib/System.String::Format" → "System.String"
    /// "System.Private.CoreLib/System.Collections.Generic.List`1" → "System.Collections.Generic.List`1"
    /// </summary>
    private static string? ExtractTypeName(string subjectId)
    {
        if (string.IsNullOrEmpty(subjectId))
            return null;

        var slashIndex = subjectId.IndexOf(SubjectIdSeparator);
        if (slashIndex < 0 || slashIndex >= subjectId.Length - 1)
            return null;

        var afterAssembly = subjectId[(slashIndex + 1)..];

        // Strip method part after "::"
        var methodSepIndex = afterAssembly.IndexOf("::", StringComparison.Ordinal);
        return methodSepIndex >= 0 ? afterAssembly[..methodSepIndex] : afterAssembly;
    }

    /// <summary>
    /// Sanitize a subjectId for use as a C++ identifier.
    /// Replaces all non-alphanumeric characters with underscores.
    /// </summary>
    private static string SanitizeSubjectId(string subjectId)
    {
        if (string.IsNullOrEmpty(subjectId))
            return "empty";

        var sb = new StringBuilder(subjectId.Length);
        foreach (var ch in subjectId)
        {
            sb.Append(char.IsLetterOrDigit(ch) ? ch : '_');
        }
        return sb.ToString();
    }

    /// <summary>
    /// Count the number of parameters in a subjectId method signature.
    /// SubjectId format: "Assembly/Namespace.Type::MethodName:ReturnType(Param1,Param2,...)"
    /// Returns -1 if the signature cannot be parsed.
    /// </summary>
    private static int CountMethodParameters(string subjectId)
    {
        if (string.IsNullOrEmpty(subjectId))
            return -1;
        // Find the opening paren in the signature part (after "::Method:")
        var methodSep = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (methodSep < 0) return -1;
        var afterReturnType = subjectId.IndexOf(':', methodSep + 2);
        if (afterReturnType < 0) return -1;
        var parenOpen = subjectId.IndexOf('(', afterReturnType + 1);
        if (parenOpen < 0) return -1;
        var parenClose = subjectId.IndexOf(')', parenOpen + 1);
        if (parenClose < 0) return -1;
        // Extract the parameter list
        var paramList = subjectId.Substring(parenOpen + 1, parenClose - parenOpen - 1);
        if (string.IsNullOrEmpty(paramList))
            return 0;
        // Count commas + 1
        int count = 1;
        foreach (var ch in paramList)
        {
            if (ch == ',') count++;
        }
        return count;
    }
}
