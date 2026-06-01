using System.Globalization;
using System.Reflection;
using System.Text;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

/// <summary>
/// Converts CLR objects (from AutoFixture or manual creation) into C# expression strings.
/// </summary>
public sealed class CSharpSerializer
{
    private static readonly HashSet<string> PrimitiveTypeNames = new(StringComparer.Ordinal)
    {
        "System.Boolean", "System.Byte", "System.SByte", "System.Int16", "System.UInt16",
        "System.Int32", "System.UInt32", "System.Int64", "System.UInt64",
        "System.Single", "System.Double", "System.Decimal", "System.Char",
    };

    // Types that can't be meaningfully constructed — emit as default(T)!
    private static readonly HashSet<string> UnconstructableTypes = new(StringComparer.Ordinal)
    {
        "System.IntPtr", "System.UIntPtr", "System.TypedReference",
        "System.ArgIterator", "System.RuntimeArgumentHandle",
    };

    /// <summary>
    /// Serialize a value to a C# expression. Returns null if the value cannot be serialized.
    /// </summary>
    public string? Serialize(string typeName, object? value)
    {
        // Tier 1: null
        if (value is null)
            return "null!";

        // Tier 1: primitives
        if (PrimitiveTypeNames.Contains(typeName))
            return SerializePrimitive(typeName, value);

        // Tier 1: enum
        if (typeName is { Length: > 0 } && value.GetType().IsEnum)
            return $"{typeName}.{value}";

        // Tier 1: string
        if (typeName == "System.String" && value is string strVal)
            return LiteralEscape(strVal);

        // Tier 1: Type
        if (typeName == "System.Type" && value is Type t)
            return $"typeof({t.FullName})";

        // Unconstructable — default fallback
        if (UnconstructableTypes.Contains(typeName))
            return $"default({GetShortTypeName(typeName)})!";

        // Tier 2: try constructor-based serialization
        if (TrySerializeByConstructor(typeName, value, out var ctorExpr))
            return ctorExpr;

        // Tier 3: known simple types by name
        if (TrySerializeKnownType(typeName, value, out var knownExpr))
            return knownExpr;

        // Fallback: return null (caller decides: fixture.Create<T>() or default(T)!)
        return null;
    }

    /// <summary>
    /// Generate a fixture.Create&lt;T&gt;() expression for types that can't be statically serialized.
    /// </summary>
    public string FixtureCreateExpression(string typeName)
    {
        // Strip generic arity for C#: "System.Collections.Generic.List`1" -> "List<byte>"
        var csName = ToCSharpTypeName(typeName);
        return $"fixture.Create<{csName}>()";
    }

    /// <summary>
    /// Generate a default(T) expression.
    /// </summary>
    public string DefaultExpression(string typeName)
    {
        var csName = ToCSharpTypeName(typeName);
        // Value types: default(T) is valid. Reference types: needs null-forgiving.
        if (IsValueType(typeName))
            return $"default({csName})";
        return $"default({csName})!";
    }

    private string SerializePrimitive(string typeName, object value)
    {
        return typeName switch
        {
            "System.Boolean" => (bool)value ? "true" : "false",
            "System.Char" => CharEscape((char)value),
            "System.String" => LiteralEscape((string)value),
            "System.Single" => ((float)value) switch
            {
                float.PositiveInfinity => "float.PositiveInfinity",
                float.NegativeInfinity => "float.NegativeInfinity",
                float.NaN => "float.NaN",
                var f => f.ToString("R", CultureInfo.InvariantCulture) + "f"
            },
            "System.Double" => ((double)value) switch
            {
                double.PositiveInfinity => "double.PositiveInfinity",
                double.NegativeInfinity => "double.NegativeInfinity",
                double.NaN => "double.NaN",
                var d => d.ToString("R", CultureInfo.InvariantCulture) + "d"
            },
            "System.Decimal" => ((decimal)value).ToString(CultureInfo.InvariantCulture) + "m",
            _ => Convert.ToString(value, CultureInfo.InvariantCulture) ?? "0"
        };
    }

    private static string LiteralEscape(string s)
    {
        var sb = new StringBuilder();
        sb.Append('"');
        foreach (var c in s)
        {
            switch (c)
            {
                case '"': sb.Append("\\\""); break;
                case '\\': sb.Append("\\\\"); break;
                case '\n': sb.Append("\\n"); break;
                case '\r': sb.Append("\\r"); break;
                case '\t': sb.Append("\\t"); break;
                case '\0': sb.Append("\\0"); break;
                default: sb.Append(c); break;
            }
        }
        sb.Append('"');
        return sb.ToString();
    }

    private static string CharEscape(char c)
    {
        return c switch
        {
            '\'' => @"'\''",
            '\\' => @"'\\'",
            '\n' => @"'\n'",
            '\r' => @"'\r'",
            '\t' => @"'\t'",
            '\0' => @"'\0'",
            _ => $"'{c}'"
        };
    }

    private bool TrySerializeByConstructor(string typeName, object value, out string? expr)
    {
        expr = null;
        var type = value.GetType();

        // Get public constructors
        var ctors = type.GetConstructors(BindingFlags.Public | BindingFlags.Instance);
        if (ctors.Length == 0) return false;

        // Try to find a constructor whose parameters match the runtime state
        // This is heuristic: best effort with readable constructors
        foreach (var ctor in ctors.OrderBy(c => c.GetParameters().Length))
        {
            var parameters = ctor.GetParameters();
            if (parameters.Length == 0)
            {
                expr = $"new {GetShortTypeName(typeName)}()";
                return true;
            }

            // Skip constructors with > 3 params for readability (unless primitive-only)
            if (parameters.Length > 3 &&
                parameters.Any(p => !PrimitiveTypeNames.Contains(p.ParameterType.FullName ?? "")))
                continue;

            return false; // Can't reliably extract constructor arg values from runtime object
        }

        return false;
    }

    private bool TrySerializeKnownType(string typeName, object value, out string? expr)
    {
        expr = typeName switch
        {
            "System.DateTime" when value is DateTime dt =>
                $"new DateTime({dt.Year}, {dt.Month}, {dt.Day}, {dt.Hour}, {dt.Minute}, {dt.Second}, DateTimeKind.{dt.Kind})",

            "System.DateTimeOffset" when value is DateTimeOffset dto =>
                $"new DateTimeOffset({dto.Year}, {dto.Month}, {dto.Day}, {dto.Hour}, {dto.Minute}, {dto.Second}, TimeSpan.FromTicks({dto.Offset.Ticks}))",

            "System.TimeSpan" when value is TimeSpan ts =>
                $"TimeSpan.FromTicks({ts.Ticks})",

            "System.Guid" when value is Guid g =>
                $"new Guid(\"{g.ToString("D")}\")",

            "System.Uri" when value is Uri u =>
                $"new Uri(\"{u.AbsoluteUri}\")",

            "System.Version" when value is Version v =>
                $"new Version({v.Major}, {v.Minor}, {v.Build}, {v.Revision})",

            "System.Uri" => $"new Uri(\"https://example.com\")",

            _ => null
        };
        return expr is not null;
    }

    /// <summary>
    /// Convert a CLR type name to a valid C# type name for use in generated code.
    /// Handles: Span`1 -> Span, Span<System.Byte> -> Span<byte>, System.Int32 -> int
    /// </summary>
    internal static string ToCSharpTypeName(string typeName)
    {
        // Handle generic "Name<Arg1,Arg2>" or "Name{Arg1,Arg2}" syntax
        int genericStart = -1;
        char genericClose = '\0';

        if (typeName.Contains('<'))
        {
            genericStart = typeName.IndexOf('<');
            genericClose = '>';
        }
        else if (typeName.Contains('{'))
        {
            genericStart = typeName.IndexOf('{');
            genericClose = '}';
        }

        if (genericStart >= 0)
        {
            var genericEnd = typeName.LastIndexOf(genericClose);
            var bareType = typeName[..genericStart];
            var argsPart = genericEnd > genericStart
                ? typeName[(genericStart + 1)..genericEnd]
                : "";

            var args = argsPart.Split(',')
                .Select(a => MapToCSharpType(a.Trim()))
                .ToArray();

            // Strip backtick arity from bare type name
            var bt = bareType.IndexOf('`');
            if (bt >= 0) bareType = bareType[..bt];

            var shortName = GetShortTypeName(bareType);
            return $"{shortName}<{string.Join(", ", args)}>";
        }

        // Handle bare "Span`1" format (no angle brackets, just backtick)
        var backtick = typeName.IndexOf('`');
        if (backtick >= 0)
        {
            var bareName = typeName[..backtick];
            var arityStr = typeName[(backtick + 1)..];
            if (int.TryParse(arityStr, out var arity) && arity > 0)
            {
                var args = string.Join(", ", Enumerable.Repeat("byte", arity));
                return $"{GetShortTypeName(bareName)}<{args}>";
            }
            return GetShortTypeName(bareName);
        }

        return GetShortTypeName(typeName);
    }

    internal static string GetShortTypeName(string fullName)
    {
        // For namespaced type names, extract the short name
        // But keep generic type names intact for angle-bracket parsing
        return MapToCSharpType(fullName);
    }

    internal static string MapToCSharpType(string typeName)
    {
        // Handle generic type names — extract short name for the base type
        var genericStart = typeName.IndexOf('<');
        var bareTypeName = genericStart >= 0 ? typeName[..genericStart] : typeName;

        // Strip dot prefix from fully-qualified names
        var lastDot = bareTypeName.LastIndexOf('.');
        var shortName = lastDot >= 0 ? bareTypeName[(lastDot + 1)..] : bareTypeName;

        var mapped = shortName switch
        {
            "Boolean" => "bool",
            "Byte" => "byte",
            "SByte" => "sbyte",
            "Int16" => "short",
            "UInt16" => "ushort",
            "Int32" => "int",
            "UInt32" => "uint",
            "Int64" => "long",
            "UInt64" => "ulong",
            "Single" => "float",
            "Double" => "double",
            "Decimal" => "decimal",
            "Char" => "char",
            "String" => "string",
            "Object" => "object",
            "Void" => "void",
            _ => shortName
        };

        if (genericStart >= 0)
        {
            var argsPart = typeName[(genericStart + 1)..^1]; // strip <>
            var args = argsPart.Split(',').Select(a => MapToCSharpType(a.Trim()));
            return $"{mapped}<{string.Join(", ", args)}>";
        }

        return mapped;
    }

    internal static bool IsValueType(string typeName)
    {
        // Extract just the type name, stripping namespace and generic args
        var genericStart = typeName.IndexOf('<');
        var bareTypeName = genericStart >= 0 ? typeName[..genericStart] : typeName;
        var lastDot = bareTypeName.LastIndexOf('.');
        var shortName = lastDot >= 0 ? bareTypeName[(lastDot + 1)..] : bareTypeName;

        return shortName switch
        {
            "Boolean" or "Byte" or "SByte" or "Int16" or "UInt16" or
            "Int32" or "UInt32" or "Int64" or "UInt64" or
            "Single" or "Double" or "Decimal" or "Char" or
            "DateTime" or "TimeSpan" or "Guid" or "DateTimeOffset" or
            "IntPtr" or "UIntPtr" or "Span" or "ReadOnlySpan" or
            "Nullable" => true,
            _ => false
        };
    }

    internal static string ComputeChecksum(string typeName, object? value)
    {
        if (value is null) return "0";
        return typeName switch
        {
            "System.Int32" => ((int)value).ToString(),
            "System.Int64" => ((long)value).ToString(),
            "System.Int16" => ((short)value).ToString(),
            "System.Byte" => ((byte)value).ToString(),
            "System.SByte" => ((sbyte)value).ToString(),
            "System.UInt32" => ((uint)value).ToString(),
            "System.UInt64" => ((ulong)value).ToString(),
            "System.UInt16" => ((ushort)value).ToString(),
            "System.Boolean" => ((bool)value) ? "1" : "0",
            "System.Char" => ((int)(char)value).ToString(),
            "System.Single" => ((int)(float)value).ToString(),
            "System.Double" => ((int)(double)value).ToString(),
            "System.Decimal" => ((int)(decimal)value).ToString(),
            "System.String" => ((string)value).Length.ToString(),
            _ => value.GetHashCode().ToString()
        };
    }

    private static List<string> SplitGenericArgs(string argsPart)
    {
        var result = new List<string>();
        var depth = 0;
        var start = 0;
        for (int i = 0; i < argsPart.Length; i++)
        {
            switch (argsPart[i])
            {
                case '<':
                case '[':
                    depth++;
                    break;
                case '>':
                case ']':
                    depth--;
                    break;
                case ',' when depth == 0:
                    result.Add(argsPart[start..i].Trim());
                    start = i + 1;
                    break;
            }
        }
        if (start < argsPart.Length)
            result.Add(argsPart[start..].Trim());
        return result;
    }
}
