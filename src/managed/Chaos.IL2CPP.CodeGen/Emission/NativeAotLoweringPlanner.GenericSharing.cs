using System.Collections.Concurrent;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Maps generic method subjectId → canonical method's native symbol for sharing.
    /// When two generic instantiations share a body (e.g. List&lt;string&gt; and List&lt;object&gt;
    /// when T is a reference type), the non-canonical method only emits a stub that
    /// forwards to the canonical method's body, and call sites resolve to the canonical
    /// symbol directly.
    ///
    /// Populated during <see cref="Create"/> via <see cref="BuildGenericSharingCanonicalMap"/>.
    /// </summary>
    private Dictionary<string, string> _genericSharingCanonicalMap =
        new(StringComparer.Ordinal);

    /// <summary>
    /// Set of NativeSymbols for canonical generic method bodies that require
    /// the <c>chaos_generic_context</c> hidden parameter for runtime type
    /// resolution (typeof(T), default(T), etc.).
    ///
    /// Built after <see cref="_genericSharingCanonicalMap"/> is finalized.
    /// Shared canonical bodies receive this parameter and use it instead of
    /// the side-table GenericContextRegistry. Non-canonical stubs also carry
    /// it so they can forward to the canonical body.
    /// </summary>
    private HashSet<string> _sharedContextSymbols = new(StringComparer.Ordinal);

    /// <summary>
    /// Builds the sharing canonical map by grouping generic methods by their open
    /// definition and classifying type arguments as reference type (SHARED) or
    /// value type (SPECIALIZED).
    ///
    /// Rules:
    /// - All type arguments are reference types → SHARED: map to canonical ref-type instantiation
    /// - Any type argument is a value type → SPECIALIZED: no sharing (keep individual body)
    /// - Non-generic methods → no entry
    /// </summary>
    private Dictionary<string, string> BuildGenericSharingCanonicalMap(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId,
        IReadOnlySet<string> valueTypeSubjectIds)
    {
        var map = new Dictionary<string, string>(StringComparer.Ordinal);

        // Group methods by open definition subjectId
        var byOpenDefinition = methodsBySubjectId.Values
            .Where(m => m.OpenDefinitionSubjectId != null)
            .GroupBy(m => m.OpenDefinitionSubjectId, StringComparer.Ordinal);

        foreach (var group in byOpenDefinition)
        {
            var members = group.ToArray();
            if (members.Length < 2)
                continue; // Only one instantiation — no sharing opportunity

            // Separate into reference-type and value-type instantiations
            var refTypeInstantiations = new List<AotCoreIrMethodArtifact>();
            var valueTypeInstantiations = new List<AotCoreIrMethodArtifact>();

            foreach (var member in members)
            {
                if (IsSharedGenericInstantiation(member, valueTypeSubjectIds))
                    refTypeInstantiations.Add(member);
                else
                    valueTypeInstantiations.Add(member);
            }

            // Reference-type instantiations all share one canonical body.
            // Canonical selection is deterministic:
            //   1. Prefer an instantiation where ALL type arguments are System.Object
            //      (the natural __Canon equivalent in CoreCLR).
            //   2. Fall back to stable sort by SubjectId (dictionary order).
            if (refTypeInstantiations.Count >= 2)
            {
                var canonical = PickCanonicalInstantiation(refTypeInstantiations);
                for (int i = 0; i < refTypeInstantiations.Count; i++)
                {
                    if (refTypeInstantiations[i] == canonical)
                        continue;
                    map[refTypeInstantiations[i].SubjectId] = canonical.NativeSymbol;
                }
            }

            // Value-type instantiations remain specialized (no sharing)
        }

        return map;
    }

    /// <summary>
    /// Returns true if the method is a generic instantiation where all type arguments
    /// are reference types (eligible for shared body codegen).
    /// </summary>
    private bool IsSharedGenericInstantiation(
        AotCoreIrMethodArtifact method,
        IReadOnlySet<string> valueTypeSubjectIds)
    {
        var instKey = method.RuntimeGenericContext?.InstantiationKey;
        if (instKey == null)
            return false;

        // Check type arguments — if all are reference types, this is shareable
        if (instKey.TypeArguments != null && instKey.TypeArguments.Count > 0)
        {
            foreach (var typeArg in instKey.TypeArguments)
            {
                // If the type argument is a value type, cannot share
                if (valueTypeSubjectIds.Contains(typeArg))
                    return false;

                // If it's a generic parameter itself (!!0, !0 etc), cannot determine
                // at compile time — conservative: no sharing
                if (typeArg.Contains('!'))
                    return false;
            }
        }

        // Check method arguments too
        if (instKey.MethodArguments != null && instKey.MethodArguments.Count > 0)
        {
            foreach (var methodArg in instKey.MethodArguments)
            {
                if (valueTypeSubjectIds.Contains(methodArg))
                    return false;
                if (methodArg.Contains('!'))
                    return false;
            }
        }

        // All arguments are reference types or there are no arguments
        // → eligible for sharing (but only useful if there are type arguments)
        return instKey.TypeArguments is { Count: > 0 } ||
               instKey.MethodArguments is { Count: > 0 };
    }

    /// <summary>
    /// Picks the canonical instantiation from a group of reference-type instantiations.
    ///
    /// Selection rules (deterministic):
    ///   1. Prefer an instantiation where ALL type arguments are "System.Object"
    ///      (the natural __Canon equivalent — System.Object is the universal base
    ///      for all reference types).
    ///   2. If multiple "all-Object" instantiations exist, pick the one with the
    ///      smallest SubjectId (dictionary order).
    ///   3. If no all-Object instantiation exists, sort all candidates by SubjectId
    ///      and pick the smallest.
    ///
    /// These rules guarantee that the canonical choice is stable across builds
    /// regardless of module processing order.
    /// </summary>
    private static AotCoreIrMethodArtifact PickCanonicalInstantiation(
        List<AotCoreIrMethodArtifact> candidates)
    {
        // Phase 1: look for an instantiation where all type arguments are System.Object.
        var allObjectInstantiations = candidates
            .Where(c => AreAllTypeArgsSystemObject(c))
            .ToList();

        if (allObjectInstantiations.Count > 0)
        {
            // Multiple all-Object instantiations: pick smallest SubjectId.
            return allObjectInstantiations
                .OrderBy(c => c.SubjectId, StringComparer.Ordinal)
                .First();
        }

        // Phase 2: no all-Object instantiation — deterministic sort by SubjectId.
        return candidates
            .OrderBy(c => c.SubjectId, StringComparer.Ordinal)
            .First();
    }

    /// <summary>
    /// Returns true when every type argument in the method's instantiation key
    /// is exactly "System.Object".
    /// </summary>
    private static bool AreAllTypeArgsSystemObject(AotCoreIrMethodArtifact method)
    {
        var instKey = method.RuntimeGenericContext?.InstantiationKey;
        if (instKey == null)
            return false;

        const string systemObject = "System.Object";

        if (instKey.TypeArguments is { Count: > 0 })
        {
            foreach (var typeArg in instKey.TypeArguments)
            {
                if (!string.Equals(typeArg, systemObject, StringComparison.Ordinal))
                    return false;
            }
        }

        if (instKey.MethodArguments is { Count: > 0 })
        {
            foreach (var methodArg in instKey.MethodArguments)
            {
                if (!string.Equals(methodArg, systemObject, StringComparison.Ordinal))
                    return false;
            }
        }

        // At least one type or method argument must exist to be meaningful.
        return (instKey.TypeArguments is { Count: > 0 } ||
                instKey.MethodArguments is { Count: > 0 });
    }
    /// <summary>
    /// Returns true when a method's native symbol corresponds to a shared
    /// generic body (either canonical or stub) that requires a hidden
    /// <c>chaos_generic_context</c> parameter for runtime type resolution.
    ///
    /// Shared methods call other shared methods with the same context pointer,
    /// avoiding side-table lookups through GenericContextRegistry.
    ///
    /// Conservative: ALL methods in the sharing canonical map (both canonical
    /// and non-canonical) receive the hidden parameter. This could be optimized
    /// to only add it when the method body actually uses typeof(T) etc.
    /// </summary>
    private bool RequiresRuntimeGenericContextParameter(AotCoreIrMethodArtifact method)
    {
        // Canonical body: its NativeSymbol is a VALUE in the map.
        // Non-canonical stub: its SubjectId is a KEY in the map.
        return _genericSharingCanonicalMap.ContainsKey(method.SubjectId) ||
               _genericSharingCanonicalMap.ContainsValue(method.NativeSymbol);
    }

    /// <summary>
    /// Returns true when a native symbol corresponds to a shared generic
    /// method that requires the hidden context parameter.
    /// </summary>
    private bool NativeSymbolRequiresRuntimeGenericContext(string nativeSymbol)
    {
        return _genericSharingCanonicalMap.ContainsValue(nativeSymbol);
    }

    /// <summary>
    /// Computes the canonical native symbol for a generic method's instantiation
    /// stub. For shared generics, the stub forwards to the canonical method's body
    /// instead of its own body.
    /// </summary>
    private string ResolveStubTargetNativeSymbol(AotCoreIrMethodArtifact method)
    {
        if (_genericSharingCanonicalMap.TryGetValue(method.SubjectId, out var canonicalSymbol))
            return canonicalSymbol;
        return method.NativeSymbol;
    }

    /// <summary>
    /// Computes the canonical native symbol for a call target.
    /// For shared generic methods, call sites should invoke the canonical body
    /// directly instead of going through the per-instantiation stub.
    /// </summary>
    private string ResolveCallTargetNativeSymbol(AotCoreIrMethodArtifact method)
    {
        if (_genericSharingCanonicalMap.TryGetValue(method.SubjectId, out var canonicalSymbol))
            return canonicalSymbol;
        return method.NativeSymbol;
    }

    /// <summary>
    /// Builds the set of canonical NativeSymbols that need the hidden
    /// <c>chaos_generic_context</c> parameter for runtime type resolution.
    ///
    /// Every canonical method body in the sharing map (every VALUE in
    /// <see cref="_genericSharingCanonicalMap"/>) requires this parameter
    /// because its body must resolve typeof(T) etc. without the side-table.
    ///
    /// Non-canonical shared stubs (KEYS in the map) also receive the
    /// parameter to forward it to the canonical body.
    /// </summary>
    private HashSet<string> BuildSharedContextSymbols()
    {
        var symbols = new HashSet<string>(StringComparer.Ordinal);

        // All values in the map are canonical body NativeSymbols — they
        // need the context parameter for typeof(T) etc.
        foreach (var canonicalSymbol in _genericSharingCanonicalMap.Values)
        {
            symbols.Add(canonicalSymbol);
        }

        return symbols;
    }
}
