namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Tracks which methods in each assembly are called from other assemblies.
/// These methods must be exported as extern "C" symbols so that the calling
/// assembly can link to them.
///
/// Usage:
///   var registry = new AssemblyExportRegistry();
///   registry.RegisterCall("AssemblyA", "AssemblyB/SomeType::Method");
///   registry.RegisterCall("AssemblyA", "AssemblyB/OtherType::Field_get");
///   var exports = registry.GetExportsForAssembly("AssemblyB");
///   // → ["AssemblyB/SomeType::Method", "AssemblyB/OtherType::Field_get"]
/// </summary>
internal sealed class AssemblyExportRegistry
{
    /// <summary>
    /// callerAssemblyName → set of callee SubjectIds (cross-assembly calls)
    /// </summary>
    private readonly Dictionary<string, HashSet<string>> _inboundCallsByAssembly =
        new(StringComparer.Ordinal);

    /// <summary>
    /// calleeAssemblyName → set of callee SubjectIds (exports needed)
    /// </summary>
    private readonly Dictionary<string, HashSet<string>> _exportsByAssembly =
        new(StringComparer.Ordinal);

    /// <summary>
    /// Register a cross-assembly call from callerAssembly to a method
    /// in another assembly. The callee's assembly must export this method.
    /// </summary>
    public void RegisterCall(string callerAssemblyName, string calleeSubjectId)
    {
        var calleeAssemblyName = ExtractAssemblyName(calleeSubjectId);
        if (string.Equals(calleeAssemblyName, callerAssemblyName, StringComparison.Ordinal))
            return; // Same-assembly call, no export needed

        // Track inbound calls for the caller
        if (!_inboundCallsByAssembly.TryGetValue(callerAssemblyName, out var inboundSet))
        {
            inboundSet = [];
            _inboundCallsByAssembly[callerAssemblyName] = inboundSet;
        }
        inboundSet.Add(calleeSubjectId);

        // Track exports needed for the callee assembly
        if (!_exportsByAssembly.TryGetValue(calleeAssemblyName, out var exportSet))
        {
            exportSet = [];
            _exportsByAssembly[calleeAssemblyName] = exportSet;
        }
        exportSet.Add(calleeSubjectId);
    }

    /// <summary>
    /// Get all SubjectIds that the given assembly must export (because
    /// other assemblies call them).
    /// </summary>
    public IReadOnlySet<string> GetExportsForAssembly(string assemblyName)
    {
        return _exportsByAssembly.TryGetValue(assemblyName, out var exports)
            ? exports
            : new HashSet<string>(StringComparer.Ordinal);
    }

    /// <summary>
    /// Get all SubjectIds from other assemblies that the given assembly calls.
    /// </summary>
    public IReadOnlySet<string> GetImportsForAssembly(string assemblyName)
    {
        return _inboundCallsByAssembly.TryGetValue(assemblyName, out var imports)
            ? imports
            : new HashSet<string>(StringComparer.Ordinal);
    }

    /// <summary>
    /// Returns true if any cross-assembly calls were registered.
    /// </summary>
    public bool HasCrossAssemblyCalls => _exportsByAssembly.Count > 0;

    /// <summary>
    /// Returns the set of assemblies that have export requirements.
    /// </summary>
    public IEnumerable<string> AssembliesWithExports => _exportsByAssembly.Keys;

    /// <summary>
    /// Returns the set of assemblies that make cross-assembly calls.
    /// </summary>
    public IEnumerable<string> AssembliesWithImports => _inboundCallsByAssembly.Keys;

    /// <summary>
    /// Extract the assembly name from a SubjectId (format: "AssemblyName/...").
    /// </summary>
    private static string ExtractAssemblyName(string subjectId)
    {
        if (string.IsNullOrEmpty(subjectId))
            return string.Empty;

        var slashIndex = subjectId.IndexOf('/');
        return slashIndex > 0 ? subjectId[..slashIndex] : subjectId;
    }
}
