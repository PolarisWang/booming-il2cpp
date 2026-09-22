// ShapeRegistryIndex — the authoritative "does the AOT have a real native body
// for this method?" answer, derived from the codegen shape registry itself.
//
// Why this exists
// ---------------
// TestEmitter.HasKnownNativeImpl() decides whether an external-assembly method
// gets the AOT-STUB-GAP `return 42L` body or a real call.  It used to consult
// only Classifier.KnownNativeImpls — a hand-maintained list of (Type.Member)
// keys.  That list covers the reflection surface and nothing else: measured
// 2026-09-22, it held 358 literal keys, none of them json/xml.  The result was
// that RuntimeHelperShapeRegistry could back a method with a fully implemented
// native symbol and the ATG would still emit a stub for it — the implementation
// was unreachable from the fact layer.  On the xml chunk that showed up as 185
// stubGap subjects whose native symbols were all present and real.
//
// The registry is already the single source of truth for this question: it is
// what ExportManifest() serializes and what the C++ shape dispatch is generated
// from.  Reading it directly removes the drift instead of adding another list
// to keep in sync.
//
// Matching
// --------
// Keys are (TypeDisplayName, MethodName).  The *type* is matched by exact
// display name or by bare name (the ATG sees `System.Xml.XmlWriter` while a
// registration may use the bare `XmlWriter`), and accessor names are
// normalized the same way the legacy list did (`get_X` in the contract vs the
// bare `X` the probe records).
//
// This index deliberately does NOT include paramTypes in the key: the callers
// ask a per-method question, and the legacy list was equally coarse.  Overload
// precision is a separate concern — see ExportManifest()'s comment on why the
// *manifest* carries signatures even though this matcher does not.
using System.Collections.Generic;
using Chaos.IL2CPP.Generator;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

/// <summary>
/// Index over <c>NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault()</c>
/// answering "is there a registered AOT shape for (type, method)?".
/// </summary>
public static class ShapeRegistryIndex
{
    // (typeDisplayName, methodName) — both the full and the bare type name are
    // inserted for every entry so either spelling matches.
    private static readonly HashSet<string> _keys = new(StringComparer.Ordinal);
    private static readonly object _gate = new();
    private static bool _built;

    /// <summary>Number of distinct (type, method) pairs indexed. Diagnostics only.</summary>
    public static int KeyCount
    {
        get { EnsureBuilt(); return _keys.Count; }
    }

    /// <summary>
    /// True when the AOT shape registry registers a shape for
    /// <paramref name="typeDisplayName"/> / <paramref name="methodName"/>.
    /// </summary>
    public static bool HasShape(string? typeDisplayName, string? methodName)
    {
        if (string.IsNullOrEmpty(typeDisplayName) || string.IsNullOrEmpty(methodName))
            return false;

        EnsureBuilt();

        lock (_gate)
        {
            if (_keys.Contains(Compose(typeDisplayName, methodName))) return true;

            // Accessor normalization: the registry is written from the contract
            // viewpoint ("get_Name"); the ATG records the bare member name.
            if (methodName.StartsWith("get_", StringComparison.Ordinal) &&
                _keys.Contains(Compose(typeDisplayName, methodName[4..]))) return true;
            if (methodName.StartsWith("set_", StringComparison.Ordinal) &&
                _keys.Contains(Compose(typeDisplayName, methodName[4..]))) return true;

            // Bare declaring-type name: the ATG's DeclaringTypeFullName is a
            // dotted full name; registrations may use either spelling.
            var lastDot = typeDisplayName.LastIndexOf('.');
            if (lastDot >= 0)
            {
                var bare = typeDisplayName[(lastDot + 1)..];
                if (_keys.Contains(Compose(bare, methodName))) return true;
                if (methodName.StartsWith("get_", StringComparison.Ordinal) &&
                    _keys.Contains(Compose(bare, methodName[4..]))) return true;
                if (methodName.StartsWith("set_", StringComparison.Ordinal) &&
                    _keys.Contains(Compose(bare, methodName[4..]))) return true;
            }

            return false;
        }
    }

    private static string Compose(string type, string method) => type + "::" + method;

    private static void EnsureBuilt()
    {
        if (_built) return;
        lock (_gate)
        {
            if (_built) return;
            Build();
            _built = true;
        }
    }

    private static void Build()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();

        // Exact registrations.
        foreach (var entry in registry.Entries)
            Add(entry.TypeDisplayName, entry.MethodName);

        // Pattern-based registrations.  These are matched by type *prefix* at
        // codegen time, so index the prefix itself; a caller asking about a
        // concrete type is answered by the exact entries above.
        foreach (var (typePrefix, methodName) in registry.GenericPatternKeys)
            Add(typePrefix, methodName);

        foreach (var (typePrefix, methodName) in registry.InlinePatternKeys)
            Add(typePrefix, methodName);
    }

    private static void Add(string? typeDisplayName, string? methodName)
    {
        if (string.IsNullOrEmpty(typeDisplayName) || string.IsNullOrEmpty(methodName)) return;

        _keys.Add(Compose(typeDisplayName, methodName));

        var lastDot = typeDisplayName.LastIndexOf('.');
        if (lastDot >= 0)
            _keys.Add(Compose(typeDisplayName[(lastDot + 1)..], methodName));
    }
}
