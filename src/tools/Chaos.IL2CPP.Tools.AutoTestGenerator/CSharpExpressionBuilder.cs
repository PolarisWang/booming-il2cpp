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
        // System.Array is abstract — default(Array) is null and causes NRE
        // in AOT-generated code.  Use Array.Empty<int>() for a valid instance.
        ["System.Array"] = "System.Array.Empty<int>()",
        // Crypto abstract type factories — .Create() returns concrete instances
        ["System.Security.Cryptography.HashAlgorithm"] = "System.Security.Cryptography.SHA256.Create()",
        ["System.Security.Cryptography.SymmetricAlgorithm"] = "System.Security.Cryptography.Aes.Create()",
        ["System.Security.Cryptography.AsymmetricAlgorithm"] = "System.Security.Cryptography.RSA.Create()",
        ["System.Security.Cryptography.RSA"] = "System.Security.Cryptography.RSA.Create()",
        ["System.Security.Cryptography.SHA256"] = "System.Security.Cryptography.SHA256.Create()",
        // XML abstract type factories
        ["System.Xml.XmlReader"] = "System.Xml.XmlReader.Create(new System.IO.StringReader(\"<root/>\"))",
        ["System.Xml.XmlWriter"] = "System.Xml.XmlWriter.Create(System.IO.Stream.Null)",
        // IO stream/text factories
        ["System.IO.Stream"] = "System.IO.Stream.Null",
        ["System.IO.TextReader"] = "new System.IO.StringReader(\"\")",
        ["System.IO.TextWriter"] = "new System.IO.StringWriter()",
        // Dataflow block 类型 — new + lambda，避免 default(null) 抛 NRE
        ["System.Threading.Tasks.Dataflow.ActionBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.ActionBlock<System.Int32>(_ => { })",
        ["System.Threading.Tasks.Dataflow.BufferBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BufferBlock<System.Int32>()",
        ["System.Threading.Tasks.Dataflow.BroadcastBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BroadcastBlock<System.Int32>(_ => _)",
        ["System.Threading.Tasks.Dataflow.TransformBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.TransformBlock<System.Int32, System.Int32>(x => x)",
        ["System.Threading.Tasks.Dataflow.TransformManyBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.TransformManyBlock<System.Int32, System.Int32>(x => System.Array.Empty<System.Int32>())",
        ["System.Threading.Tasks.Dataflow.BatchBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BatchBlock<System.Int32>(1)",
        ["System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32>(1)",
        ["System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32, System.Int32>(1)",
        ["System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32>()",
        ["System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32, System.Int32>()",
        ["System.Threading.Tasks.Dataflow.WriteOnceBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.WriteOnceBlock<System.Int32>(_ => _)",
        // IO.Pipelines
        ["System.IO.Pipelines.Pipe"] = "new System.IO.Pipelines.Pipe()",
        ["System.IO.Pipelines.PipeReader"] = "System.IO.Pipelines.PipeReader.Create(System.IO.Stream.Null)",
        ["System.IO.Pipelines.PipeWriter"] = "System.IO.Pipelines.PipeWriter.Create(System.IO.Stream.Null)",
        ["System.IO.Pipelines.PipeScheduler"] = "System.IO.Pipelines.PipeScheduler.Inline",
        // System.Formats.Tar
        ["System.Formats.Tar.TarReader"] = "new System.Formats.Tar.TarReader(System.IO.Stream.Null)",
        ["System.Formats.Tar.TarWriter"] = "new System.Formats.Tar.TarWriter(System.IO.Stream.Null)",
        // System.Net.Http
        ["System.Net.Http.HttpClient"] = "new System.Net.Http.HttpClient()",
        ["System.Net.Http.Headers.AuthenticationHeaderValue"] =
            "new System.Net.Http.Headers.AuthenticationHeaderValue(\"Bearer\", \"token\")",
        ["System.Net.Http.Headers.MediaTypeHeaderValue"] =
            "new System.Net.Http.Headers.MediaTypeHeaderValue(\"application/json\")",
        ["System.Net.Http.Headers.MediaTypeWithQualityHeaderValue"] =
            "new System.Net.Http.Headers.MediaTypeWithQualityHeaderValue(\"application/json\")",
        ["System.Net.Http.Headers.ProductHeaderValue"] =
            "new System.Net.Http.Headers.ProductHeaderValue(\"test\", \"1.0\")",
        ["System.Net.Http.Headers.ProductInfoHeaderValue"] =
            "new System.Net.Http.Headers.ProductInfoHeaderValue(\"test\", \"1.0\")",
        ["System.Net.Http.Headers.CacheControlHeaderValue"] =
            "new System.Net.Http.Headers.CacheControlHeaderValue()",
        ["System.Net.Http.Headers.ContentDispositionHeaderValue"] =
            "new System.Net.Http.Headers.ContentDispositionHeaderValue(\"attachment\")",
        ["System.Net.Http.Headers.ContentRangeHeaderValue"] =
            "new System.Net.Http.Headers.ContentRangeHeaderValue(0, 99, 100)",
        ["System.Net.Http.Headers.EntityTagHeaderValue"] =
            "new System.Net.Http.Headers.EntityTagHeaderValue(\"\\\"tag\\\"\")",
        ["System.Net.Http.Headers.NameValueHeaderValue"] =
            "new System.Net.Http.Headers.NameValueHeaderValue(\"name\", \"value\")",
        ["System.Net.Http.Headers.NameValueWithParametersHeaderValue"] =
            "new System.Net.Http.Headers.NameValueWithParametersHeaderValue(\"name\", \"value\")",
        ["System.Net.Http.Headers.RangeConditionHeaderValue"] =
            "new System.Net.Http.Headers.RangeConditionHeaderValue(new System.DateTime(2024, 1, 1))",
        ["System.Net.Http.Headers.RangeHeaderValue"] =
            "new System.Net.Http.Headers.RangeHeaderValue(0, 100)",
        ["System.Net.Http.Headers.RetryConditionHeaderValue"] =
            "new System.Net.Http.Headers.RetryConditionHeaderValue(new System.DateTime(2024, 1, 1))",
        ["System.Net.Http.Headers.StringWithQualityHeaderValue"] =
            "new System.Net.Http.Headers.StringWithQualityHeaderValue(\"en\", 0.5)",
        ["System.Net.Http.Headers.TransferCodingHeaderValue"] =
            "new System.Net.Http.Headers.TransferCodingHeaderValue(\"chunked\")",
        ["System.Net.Http.Headers.TransferCodingWithQualityHeaderValue"] =
            "new System.Net.Http.Headers.TransferCodingWithQualityHeaderValue(\"chunked\", 0.5)",
        ["System.Net.Http.Headers.ViaHeaderValue"] =
            "new System.Net.Http.Headers.ViaHeaderValue(\"1.1\", \"host\")",
        ["System.Net.Http.Headers.WarningHeaderValue"] =
            "new System.Net.Http.Headers.WarningHeaderValue(299, \"-\", \"\\\"warning\\\"\")",
        // System.Text.Json
        ["System.Text.Json.JsonSerializerOptions"] = "new System.Text.Json.JsonSerializerOptions()",
        // System.Diagnostics.DiagnosticSource
        ["System.Diagnostics.ActivitySource"] = "new System.Diagnostics.ActivitySource(\"test\")",
        ["System.Diagnostics.ActivityListener"] = "new System.Diagnostics.ActivityListener()",
        ["System.Diagnostics.DiagnosticListener"] = "new System.Diagnostics.DiagnosticListener(\"test\")",
        ["System.Diagnostics.ActivityTagsCollection"] = "new System.Diagnostics.ActivityTagsCollection()",
        ["System.Diagnostics.Metrics.Meter"] = "new System.Diagnostics.Metrics.Meter(\"test\")",
        ["System.Diagnostics.Metrics.MeterListener"] = "new System.Diagnostics.Metrics.MeterListener()",
        // System.Security.AccessControl
        ["System.Security.AccessControl.RawSecurityDescriptor"] =
            "new System.Security.AccessControl.RawSecurityDescriptor(\"\")",
        ["System.Security.AccessControl.CommonSecurityDescriptor"] =
            "new System.Security.AccessControl.CommonSecurityDescriptor(false, false, \"\")",
        ["System.Security.Policy.Evidence"] = "new System.Security.Policy.Evidence()",
        ["System.Security.Policy.EvidenceBase"] = "new System.Security.Policy.Evidence()",
        // Globalization types — abstract/default(Calendar)! causes NRE
        ["System.Globalization.Calendar"] = "new System.Globalization.GregorianCalendar()",
        ["System.Globalization.CompareInfo"] = "System.Globalization.CultureInfo.InvariantCulture.CompareInfo",
        ["System.Globalization.DateTimeFormatInfo"] = "System.Globalization.CultureInfo.InvariantCulture.DateTimeFormat",
        ["System.Globalization.CultureInfo"] = "System.Globalization.CultureInfo.InvariantCulture",
        ["System.Globalization.EastAsianLunisolarCalendar"] = "new System.Globalization.ChineseLunisolarCalendar()",
        ["System.Globalization.ChineseLunisolarCalendar"] = "new System.Globalization.ChineseLunisolarCalendar()",
    };

    // Types with a static `Shared` property that returns a valid instance.
    // Using Shared instead of default(Type)! avoids NullReferenceException
    // when calling instance methods (e.g. ArrayPool<Int32>.Shared.Rent(0)).
    // The key is matched as a prefix (before `<` for generic types).
    private static readonly Dictionary<string, string> SharedInstanceTypes = new(StringComparer.Ordinal)
    {
        ["System.Buffers.ArrayPool"] = ".Shared",
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
            return $"global::{CSharpSerializer.StripAssemblyQualification(typeFullName)}";

        var csType = CSharpSerializer.MapToCSharpType(typeFullName);

        // Check known factory instances (Encoding.UTF8, string.Empty)
        if (KnownInstances.TryGetValue(csType, out var knownExpr))
            return knownExpr;

        // Check known type factories (collections, special constructors)
        var factoryResult = TryBuildFactoryExpression(csType, csType);
        if (factoryResult is not null)
            return factoryResult;

        // For types with namespace qualification, use global:: prefix.
        var qualified = CSharpSerializer.StripAssemblyQualification(typeFullName);
        if (qualified.Contains('.'))
        {
            // Check SharedInstanceTypes before falling back to default.
            // Include generic type arguments so e.g. ArrayPool<System.Int32>.Shared works.
            var baseName = qualified.Contains('<') ? qualified[..qualified.IndexOf('<')] : qualified;
            if (SharedInstanceTypes.TryGetValue(baseName, out var sharedSuffix))
            {
                var gaPart = qualified.Contains('<') ? qualified[qualified.IndexOf('<')..] : "";
                return $"global::{baseName}{gaPart}{sharedSuffix}";
            }
            return $"default(global::{qualified.Replace('+', '.')})!";
        }

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

        return _serializer.DefaultExpression(csType);
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
