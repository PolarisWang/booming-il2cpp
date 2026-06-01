using System.Reflection;
using System.Text;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

/// <summary>
/// Builds C# expressions for constructing instances of types.
/// Used to generate instance method calls with proper constructor initialization.
/// </summary>
public sealed class CSharpExpressionBuilder
{
    private readonly CSharpSerializer _serializer;

    // Types with well-known static factory instances (abstract or no default ctor)
    private static readonly Dictionary<string, string> KnownInstances = new(StringComparer.Ordinal)
    {
        ["System.Text.Encoding"] = "Encoding.UTF8",
        ["System.String"] = "string.Empty",
    };

    /// <summary>
    /// Describes how to construct an instance of a known type.
    /// </summary>
    private enum FactoryKind { Collection, Dictionary, EnumerableCtor, CustomExpr }

    private sealed record TypeFactory(
        FactoryKind Kind,
        int Arity,                          // number of generic type parameters
        string? CustomExpr = null);         // literal expression (for CustomExpr kind)

    private static readonly Dictionary<string, TypeFactory> KnownTypeFactories = new(StringComparer.Ordinal)
    {
        // Collections with Add(T) — collection initializer works
        ["System.Collections.Generic.List"] =        new(FactoryKind.Collection, 1),
        ["System.Collections.Generic.HashSet"] =      new(FactoryKind.Collection, 1),
        ["System.Collections.Generic.Collection"] =    new(FactoryKind.Collection, 1),

        // Dictionary with Add(K,V) — dictionary initializer works
        ["System.Collections.Generic.Dictionary"] =   new(FactoryKind.Dictionary, 2),

        // Collections with IEnumerable<T> constructor (no Add method)
        ["System.Collections.Generic.LinkedList"] =   new(FactoryKind.EnumerableCtor, 1),
        ["System.Collections.Generic.Stack"] =         new(FactoryKind.EnumerableCtor, 1),
        ["System.Collections.Generic.Queue"] =         new(FactoryKind.EnumerableCtor, 1),

        // Special constructor expressions
        ["System.IO.MemoryStream"] =                   new(FactoryKind.CustomExpr, 0,
            "new MemoryStream(new byte[] { 1, 2, 3 })"),
    };

    public CSharpExpressionBuilder(CSharpSerializer serializer)
    {
        _serializer = serializer;
    }

    /// <summary>
    /// Get the best instance expression for a type.
    /// Static types → type name (static call prefix).
    /// Known constructible types → "new T(...)" or collection-initializer expression.
    /// Known factory instances → e.g. "Encoding.UTF8".
    /// Fallback → "default(TypeName)!".
    /// </summary>
    public string GetInstanceExpression(string typeFullName, bool isStatic)
    {
        if (isStatic)
            return $"global::{typeFullName}";

        var csType = CSharpSerializer.MapToCSharpType(typeFullName);

        // Check known factory instances (Encoding.UTF8, string.Empty)
        if (KnownInstances.TryGetValue(typeFullName, out var knownExpr))
            return knownExpr;

        // Check known type factories (collections, special constructors)
        var factoryResult = TryBuildFactoryExpression(typeFullName, csType);
        if (factoryResult is not null)
            return factoryResult;

        // Try to find a parameterless constructor via runtime reflection
        try
        {
            var type = Type.GetType(typeFullName, throwOnError: false);
            if (type is not null && !type.IsAbstract && !type.IsInterface)
            {
                var ctors = type.GetConstructors(BindingFlags.Public | BindingFlags.Instance);
                if (ctors.Any(c => c.GetParameters().Length == 0))
                    return $"new {csType}()";
            }
        }
        catch
        {
            // Best-effort — fall through to default
        }

        return _serializer.DefaultExpression(typeFullName);
    }

    /// <summary>
    /// Try to build a factory expression from the KnownTypeFactories table.
    /// Handles generic type argument extraction and seed value generation.
    /// </summary>
    private static string? TryBuildFactoryExpression(string typeFullName, string csType)
    {
        // Extract base name (before <) to look up in KnownTypeFactories
        var gaStart = typeFullName.IndexOf('<');
        var baseName = gaStart >= 0 ? typeFullName[..gaStart] : typeFullName;

        // Strip assembly qualifier + namespace prefix for lookup
        // But KnownTypeFactories uses full namespace.QualifiedName format
        if (!KnownTypeFactories.TryGetValue(baseName, out var factory))
            return null;

        // Extract type arguments from the C#-style type name
        var csGaStart = csType.IndexOf('<');
        string[] csTypeArgs;
        if (csGaStart >= 0 && factory.Arity > 0)
        {
            var inner = csType[(csGaStart + 1)..^1]; // strip <>
            csTypeArgs = SplitTopLevelArgs(inner, ',');
        }
        else
        {
            csTypeArgs = Array.Empty<string>();
        }

        // Custom expression (e.g. MemoryStream)
        if (factory.Kind == FactoryKind.CustomExpr && factory.CustomExpr is not null)
            return factory.CustomExpr;

        // Enumerable constructor: new Stack<int>(new[] { 1, 2, 3 })
        if (factory.Kind == FactoryKind.EnumerableCtor && csTypeArgs.Length >= 1)
        {
            var seeds = GenerateSeedLiterals(csTypeArgs[0], 3);
            return $"new {csType}(new[] {{ {string.Join(", ", seeds)} }})";
        }

        // Collection initializer: new List<int> { 1, 2, 3 }
        if (factory.Kind == FactoryKind.Collection && csTypeArgs.Length >= 1)
        {
            var seeds = GenerateSeedLiterals(csTypeArgs[0], 3);
            return $"new {csType} {{ {string.Join(", ", seeds)} }}";
        }

        // Dictionary initializer: new Dictionary<int,int> { { 1, 2 }, { 3, 4 } }
        if (factory.Kind == FactoryKind.Dictionary && csTypeArgs.Length >= 2)
        {
            var keySeeds = GenerateSeedLiterals(csTypeArgs[0], 3);
            var valSeeds = GenerateSeedLiterals(csTypeArgs[1], 3);
            var entries = new List<string>();
            for (int i = 0; i < 3 && i < keySeeds.Length && i < valSeeds.Length; i++)
                entries.Add($"{{ {keySeeds[i]}, {valSeeds[i]} }}");
            return $"new {csType} {{ {string.Join(", ", entries)} }}";
        }

        return null;
    }

    /// <summary>
    /// Generate seed literal values for a given C# type name.
    /// Produces 'count' distinct literals suitable for collection seeding.
    /// </summary>
    private static string[] GenerateSeedLiterals(string csTypeName, int count)
    {
        return csTypeName switch
        {
            "int" or "Int32" => Enumerable.Range(1, count).Select(i => i.ToString()).ToArray(),
            "uint" or "UInt32" => Enumerable.Range(1, count).Select(i => $"{i}u").ToArray(),
            "long" or "Int64" => Enumerable.Range(1, count).Select(i => $"{i}L").ToArray(),
            "ulong" or "UInt64" => Enumerable.Range(1, count).Select(i => $"{i}UL").ToArray(),
            "short" or "Int16" => Enumerable.Range(1, count).Select(i => $"(short){i}").ToArray(),
            "ushort" or "UInt16" => Enumerable.Range(1, count).Select(i => $"(ushort){i}").ToArray(),
            "byte" or "Byte" => Enumerable.Range(1, count).Select(i => $"(byte){i}").ToArray(),
            "sbyte" or "SByte" => Enumerable.Range(1, count).Select(i => $"(sbyte){i}").ToArray(),
            "float" or "Single" => Enumerable.Range(1, count).Select(i => $"{i}f").ToArray(),
            "double" or "Double" => Enumerable.Range(1, count).Select(i => $"{i}.0").ToArray(),
            "decimal" or "Decimal" => Enumerable.Range(1, count).Select(i => $"{i}m").ToArray(),
            "bool" or "Boolean" => new[] { "true", "false", "true" },
            "char" or "Char" => new[] { "'A'", "'B'", "'C'" },
            "string" or "String" => Enumerable.Range(0, count).Select(i => $"\"seed{i}\"").ToArray(),
            _ => Enumerable.Range(0, count).Select(i => $"default({csTypeName})!").ToArray(),
        };
    }

    /// <summary>
    /// Split top-level generic arguments separated by a delimiter.
    /// Respects nested angle brackets: "int, List<int>" → ["int", "List<int>"].
    /// </summary>
    private static string[] SplitTopLevelArgs(string s, char delimiter)
    {
        var result = new List<string>();
        int depth = 0, start = 0;
        for (int i = 0; i < s.Length; i++)
        {
            if (s[i] == '<' || s[i] == '[' || s[i] == '{')
                depth++;
            else if (s[i] == '>' || s[i] == ']' || s[i] == '}')
                depth--;
            else if (s[i] == delimiter && depth == 0)
            {
                result.Add(s[start..i].Trim());
                start = i + 1;
            }
        }
        if (start < s.Length)
            result.Add(s[start..].Trim());
        return result.ToArray();
    }
}
