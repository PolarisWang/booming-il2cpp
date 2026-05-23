using System.Collections.Concurrent;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Global registry for detecting and resolving C++ symbol conflicts across assemblies.
/// When multiple assemblies define types/methods with the same name, generated symbols
/// collide at link time. This registry detects such conflicts and assigns assembly-specific
/// disambiguation suffixes.
///
/// Usage:
///   var registry = new CrossAssemblySymbolRegistry();
///   registry.RegisterAssembly("AssemblyA", assemblyASymbols);
///   registry.RegisterAssembly("AssemblyB", assemblyBSymbols);
///   registry.ResolveConflicts();
///   string symbol = registry.GetUniqueSymbol("AssemblyA", "SomeClass::Method");
/// </summary>
internal sealed class CrossAssemblySymbolRegistry
{
    private readonly Dictionary<string, HashSet<string>> _assemblySymbols =
        new(StringComparer.Ordinal);

    private readonly Dictionary<string, Dictionary<string, string>> _disambiguatedSymbols =
        new(StringComparer.Ordinal);

    private readonly ConcurrentDictionary<string, string> _conflictBaseToSuffix =
        new(StringComparer.Ordinal);

    private bool _conflictsResolved;

    /// <summary>
    /// Register symbols for an assembly. Should be called for each assembly
    /// before ResolveConflicts.
    /// </summary>
    public void RegisterAssembly(string assemblyName, IEnumerable<string> symbolNames)
    {
        if (!_assemblySymbols.ContainsKey(assemblyName))
        {
            _disambiguatedSymbols[assemblyName] = new Dictionary<string, string>(StringComparer.Ordinal);
        }

        var symbols = _assemblySymbols[assemblyName] = new HashSet<string>(symbolNames, StringComparer.Ordinal);
        _disambiguatedSymbols[assemblyName] = symbols.ToDictionary(
            s => s, s => s, StringComparer.Ordinal);
    }

    /// <summary>
    /// Detect symbol conflicts across all registered assemblies and assign
    /// disambiguation suffixes where needed.
    /// </summary>
    public void ResolveConflicts()
    {
        if (_assemblySymbols.Count < 2)
        {
            _conflictsResolved = true;
            return;
        }

        // Build global symbol → list of (assemblyName) mapping
        var globalSymbolMap = new Dictionary<string, List<string>>(StringComparer.Ordinal);
        foreach (var (assemblyName, symbols) in _assemblySymbols)
        {
            foreach (var symbol in symbols)
            {
                if (!globalSymbolMap.TryGetValue(symbol, out var assemblies))
                {
                    assemblies = [];
                    globalSymbolMap[symbol] = assemblies;
                }
                assemblies.Add(assemblyName);
            }
        }

        // Find conflicts: symbols appearing in more than one assembly
        foreach (var (symbol, assemblies) in globalSymbolMap)
        {
            if (assemblies.Count <= 1)
                continue;

            // Assign disambiguated symbols for each assembly that owns this symbol
            foreach (var assemblyName in assemblies)
            {
                var suffix = ComputeAssemblySuffix(assemblyName);
                var disambiguated = symbol + suffix;
                _disambiguatedSymbols[assemblyName][symbol] = disambiguated;
            }

            // Track that this base symbol has a conflict
            _conflictBaseToSuffix.TryAdd(symbol, assemblies[0]);
        }

        _conflictsResolved = true;
    }

    /// <summary>
    /// Get the unique C++ symbol for the given assembly and base symbol name.
    /// If the symbol has no conflict, returns the base name unchanged.
    /// </summary>
    public string GetUniqueSymbol(string assemblyName, string symbol)
    {
        if (!_conflictsResolved)
        {
            ResolveConflicts();
        }

        if (_disambiguatedSymbols.TryGetValue(assemblyName, out var assemblyMap) &&
            assemblyMap.TryGetValue(symbol, out var disambiguated))
        {
            return disambiguated;
        }

        return symbol;
    }

    /// <summary>
    /// Returns true if the given symbol has a cross-assembly conflict.
    /// </summary>
    public bool HasConflict(string symbol)
    {
        if (!_conflictsResolved)
            ResolveConflicts();
        return _conflictBaseToSuffix.ContainsKey(symbol);
    }

    /// <summary>
    /// Returns all assemblies that registered the given symbol.
    /// </summary>
    public IReadOnlyList<string> GetConflictingAssemblies(string symbol)
    {
        var result = new List<string>();
        foreach (var (assemblyName, symbols) in _assemblySymbols)
        {
            if (symbols.Contains(symbol))
                result.Add(assemblyName);
        }
        return result;
    }

    /// <summary>
    /// Get all conflicting assembly→(original→disambiguated) mappings.
    /// </summary>
    public IReadOnlyDictionary<string, IReadOnlyDictionary<string, string>> GetAllDisambiguations()
    {
        if (!_conflictsResolved)
            ResolveConflicts();

        return _disambiguatedSymbols
            .Where(kvp => kvp.Value.Any(pair => pair.Key != pair.Value))
            .ToDictionary(
                kvp => kvp.Key,
                kvp => (IReadOnlyDictionary<string, string>)kvp.Value
                    .Where(pair => pair.Key != pair.Value)
                    .ToDictionary(p => p.Key, p => p.Value, StringComparer.Ordinal),
                StringComparer.Ordinal);
    }

    /// <summary>
    /// Number of distinct symbols that have cross-assembly conflicts.
    /// </summary>
    public int ConflictCount
    {
        get
        {
            if (!_conflictsResolved)
                ResolveConflicts();
            return _conflictBaseToSuffix.Count;
        }
    }

    /// <summary>
    /// Compute a short, deterministic assembly suffix for symbol disambiguation.
    /// Uses FNV-1a hash of the assembly name, truncated to 24 bits, formatted as _aXXXXXX.
    /// </summary>
    private static string ComputeAssemblySuffix(string assemblyName)
    {
        // FNV-1a hash
        uint hash = 2166136261;
        foreach (var ch in assemblyName)
        {
            hash ^= (uint)ch;
            hash *= 16777619;
        }

        // Truncate to 24 bits and format
        var truncated = hash & 0xFFFFFF;
        return $"_a{truncated:x6}";
    }
}
