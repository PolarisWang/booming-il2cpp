using System.Globalization;
using System.Reflection;
using System.Text;
using System.Text.Json;

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
    /// Convert a CLR type name to a fully qualified C# type reference (with namespace).
    /// Unlike MapToCSharpType (which strips to short name), this preserves the full path
    /// to avoid ambiguity when the same short name exists in multiple namespaces
    /// (e.g. AssemblyHashAlgorithm in System.Reflection and System.Configuration.Assemblies).
    /// Primitive types (int, string, etc.) are still mapped to their C# keywords.
    /// </summary>
    internal static string ToQualifiedCSharpType(string typeName)
    {
        typeName = StripAssemblyQualification(typeName).Replace('+', '.');

        // Handle pointer suffix '*' separately: strip, process base, re-add.
        // This ensures "System.Void*" → "void*", not "System.Void*".
        var pointerSuffix = "";
        if (typeName.EndsWith("*"))
        {
            pointerSuffix = "*";
            typeName = typeName[..^1].TrimEnd();
        }

        // Map the last segment to C# keyword if applicable, keeping the full path.
        // e.g. "System.Int32" → "int", "System.Configuration.Assemblies.AssemblyHashAlgorithm" stays as-is.
        var lastDot = typeName.LastIndexOf('.');
        if (lastDot >= 0)
        {
            var shortName = typeName[(lastDot + 1)..];
            var ns = typeName[..lastDot];
            var mapped = KeywordMap(shortName);
            // Preserve full path if not a primitive keyword
            return (mapped != shortName ? mapped : typeName) + pointerSuffix;
        }
        return typeName + pointerSuffix;
    }

    /// <summary>
    /// Serialize a value to a C# expression. Returns null if the value cannot be serialized.
    /// </summary>
    public string? Serialize(string typeName, object? value)
    {
        // Tier 1: null — use typed default() so overload resolution has type info.
        // Use fully qualified type name to avoid ambiguity (e.g. AssemblyHashAlgorithm).
        if (value is null)
            return $"default({ToQualifiedCSharpType(typeName)})!";

        // Tier 1: primitives
        if (PrimitiveTypeNames.Contains(typeName))
            return SerializePrimitive(typeName, value);

        // Tier 1: enum — use qualified C# type name (handles CLR+ nested types)
        if (typeName is { Length: > 0 } && value.GetType().IsEnum)
            return $"{ToQualifiedCSharpType(typeName)}.{value}";

        // Tier 1: string
        if (typeName == "System.String" && value is string strVal)
            return LiteralEscape(strVal);

        // Tier 1: Type
        if (typeName == "System.Type" && value is Type t)
            return $"typeof({MapToCSharpType(t.FullName ?? t.Name)})";

        // Unconstructable — default fallback with fully qualified type name
        if (UnconstructableTypes.Contains(typeName))
            return $"default({ToQualifiedCSharpType(typeName)})!";

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
        var csName = MapToCSharpType(typeName);
        return $"fixture.Create<{csName}>()";
    }

    /// <summary>
    /// Generate a default(T) expression with fully qualified type name to avoid ambiguity.
    /// </summary>
    public string DefaultExpression(string typeName)
    {
        var csName = ToQualifiedCSharpType(typeName);
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
        // Handle generic "Name<Arg1,Arg2>" syntax with proper bracket-depth matching,
        // supporting nested generic types like "Dictionary<int,int>.AlternateLookup<int>".
        int depth = 0;
        int genericStart = -1;
        int genericEnd = -1;
        char genericClose = '\0';

        for (int i = 0; i < typeName.Length; i++)
        {
            var c = typeName[i];
            if (c == '<' && depth == 0 && genericStart < 0)
            {
                genericStart = i;
                genericClose = '>';
                depth = 1;
            }
            else if (c == '{' && depth == 0 && genericStart < 0)
            {
                genericStart = i;
                genericClose = '}';
                depth = 1;
            }
            else if (c == genericClose)
            {
                depth--;
                if (depth == 0)
                {
                    // Found matching close bracket at depth 0.
                    // If there's more content after, it's a nested type suffix
                    // (e.g. ".AlternateLookup<int>") — recurse to handle it.
                    genericEnd = i;
                    break;
                }
            }
            else if ((c == '<' || c == '{') && genericStart >= 0)
            {
                depth++;
            }
            else if ((c == '>' || c == '}') && genericStart >= 0)
            {
                depth--;
            }
        }

        if (genericStart >= 0 && genericEnd > genericStart)
        {
            var bareType = typeName[..genericStart];
            var argsPart = typeName[(genericStart + 1)..genericEnd];

            var args = SplitGenericArgs(argsPart)
                .Select(a => ToCSharpTypeName(a.Trim()))
                .ToArray();

            // Strip backtick arity from bare type name
            var bt = bareType.IndexOf('`');
            if (bt >= 0) bareType = bareType[..bt];

            var shortName = GetShortTypeName(bareType);
            var result = $"{shortName}<{string.Join(", ", args)}>";

            // Handle nested type suffix: ">.AlternateLookup<int>" → ".<type>"
            var suffix = typeName[(genericEnd + 1)..];
            if (suffix.Length > 0)
                result += suffix[0] == '.' ? "." + ToCSharpTypeName(suffix[1..]) : suffix;

            return result;
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
        return KeywordMap(fullName);
    }

    internal static string MapToCSharpType(string typeName)
    {
        // Strip assembly qualification that may come from MLC's Type.FullName
        typeName = StripAssemblyQualification(typeName);

        // Handle pointer suffix '*' separately: strip all, process the base type,
        // then re-add.  This ensures "System.Void*" → "void*" (KeywordMap
        // recognizes "Void" but not "Void*"), and "System.Void**" → "void**".
        var pointerSuffix = "";
        while (typeName.EndsWith("*"))
        {
            pointerSuffix += "*";
            typeName = typeName[..^1].TrimEnd();
        }

        // Use ToCSharpTypeName for full generic type name resolution including
        // nested types like "Dictionary<A,B>.Nested<C>" — it has depth-aware bracket matching.
        var result = ToCSharpTypeName(typeName);

        // Convert CLR nested type separator '+' to C# '.' (e.g. "Outer+Inner" → "Outer.Inner").
        // ToCSharpTypeName preserves '+' because it only strips the namespace (last segment after dot),
        // leaving the CLR-format parent+child in the short name.
        return result.Replace('+', '.') + pointerSuffix;
    }

    /// <summary>
    /// Map a CLR type name to its C# keyword equivalent and handle nested type prefix preservation.
    /// Does NOT handle generic args or backtick arity — only keyword mapping + parent prefix.
    /// </summary>
    private static string KeywordMap(string typeName)
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

        // For nested types: if the prefix before last dot is a parent type
        // (no dot in prefix = single segment = type name, not namespace),
        // preserve the parent qualification. E.g. "StringBuilder.AppendInterpolatedStringHandler"
        // → keep "StringBuilder." prefix since it's a type, not a namespace.
        if (lastDot >= 0 && mapped == shortName)
        {
            var prefix = bareTypeName[..lastDot];
            if (!prefix.Contains('.'))
                mapped = $"{prefix}.{mapped}";
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

    /// <summary>
    /// Reconstruct a C# expression string from a JSON-serialized probe value.
    /// Used to generate assertion expected-values from probe output.
    /// </summary>
    public string DeserializeToExpression(string json, string typeName)
    {
        using var doc = JsonDocument.Parse(json);
        var root = doc.RootElement;

        // Strip '&' suffix for ref/out params
        var cleanType = typeName.EndsWith('&') ? typeName[..^1].Trim() : typeName;

        // Types with CLR nested type marker '+' are implementation details (iterators, etc.)
        // that can't be constructed in C# test code. Skip before StripAssemblyQualification
        // since the latter replaces '+' with '.'.
        if (cleanType.Contains('+'))
            return $"/* TODO: nested type */ default({MapToCSharpType(cleanType)})!";

        // Strip assembly qualification from generic type arguments.
        // Type.FullName produces e.g. "Type`1[[System.Int32, System.Private.CoreLib, Version=10.0.0.0, ...]]"
        // which breaks all downstream type-name parsing. Reduce to "Type<System.Int32>".
        cleanType = StripAssemblyQualification(cleanType);

        // ── System.Text.Json.JsonElement: use JsonSerializer.Deserialize for any JSON value ──
        if (cleanType == "System.Text.Json.JsonElement")
        {
            var escapedJson = LiteralEscape(json);
            return $"System.Text.Json.JsonSerializer.Deserialize<System.Text.Json.JsonElement>({escapedJson})";
        }

        // ── System.Text.Json.JsonDocument: same approach — direct cast from int is invalid ──
        if (cleanType == "System.Text.Json.JsonDocument")
        {
            var escapedJson = LiteralEscape(json);
            return $"System.Text.Json.JsonSerializer.Deserialize<System.Text.Json.JsonDocument>({escapedJson})";
        }

        // ── Null ──
        if (root.ValueKind == JsonValueKind.Null)
            return $"default({MapToCSharpType(cleanType)})!";

        // ── String type ──
        if (cleanType == "System.String")
            return LiteralEscape(root.GetString() ?? "");

        // ── Char type: JSON stores char as a string ──
        if (cleanType == "System.Char")
        {
            var s = root.GetString() ?? "\0";
            if (s.Length == 0) s = "\0";
            return CharEscape(s[0]);
        }

        // ── Boolean ──
        if (cleanType == "System.Boolean")
            return root.GetBoolean() ? "true" : "false";

        // ── byte[] ──
        if (cleanType == "System.Byte[]")
        {
            if (root.ValueKind == JsonValueKind.String)
            {
                // System.Text.Json serializes byte[] as Base64 string
                var base64 = root.GetString() ?? "";
                var decoded = Convert.FromBase64String(base64);
                if (decoded.Length == 0)
                    return "Array.Empty<byte>()";
                return $"new byte[]{{{string.Join(",", decoded.Select(b => b.ToString()))}}}";
            }
            if (root.ValueKind != JsonValueKind.Array)
                return $"default(byte[])!";
            var items = root.EnumerateArray().Select(e => e.GetByte().ToString()).ToArray();
            return $"new byte[]{{{string.Join(",", items)}}}";
        }

        // ── Numeric types ──
        if (root.ValueKind == JsonValueKind.Number)
        {
            try
            {
                return cleanType switch
                {
                    "System.Byte" => $"(byte){root.GetByte()}",
                    "System.SByte" => $"(sbyte)({root.GetSByte()})",
                    "System.Int16" => $"(short)({root.GetInt16()})",
                    "System.Half" => $"({MapToCSharpType(cleanType)})({root.GetDouble().ToString("R", CultureInfo.InvariantCulture)})",
                    "System.UInt16" => $"(ushort){root.GetUInt16()}",
                    "System.Int32" => root.GetInt32().ToString(),
                    "System.UInt32" => root.GetUInt32().ToString(),
                    "System.Int64" => root.GetInt64().ToString(),
                    "System.UInt64" => root.GetUInt64().ToString(),
                    "System.Single" => root.GetSingle().ToString("R", CultureInfo.InvariantCulture) + "f",
                    "System.Double" => root.GetDouble().ToString("R", CultureInfo.InvariantCulture) + "d",
                    "System.Decimal" => root.GetDecimal().ToString(CultureInfo.InvariantCulture) + "m",
                    _ => TryEnumExpression(cleanType, root)
                };
            }
            catch (Exception ex)
            {
                // JSON number overflow or format mismatch — emit as fallback cast
                return $"/* deserialize error: {ex.Message} */ ({cleanType})({root.GetRawText()})";
            }
        }

        // ── Array types (non-byte) ──
        if (root.ValueKind == JsonValueKind.Array)
        {
            var csType = MapToCSharpType(cleanType);

            // Only use collection initializer for types known to support it
            // (parameterless ctor + Add method). ReadOnlyCollection<T> does NOT
            // support collection initializer — fall back to default for everything
            // except well-known collection types.
            if (!IsCollectionType(cleanType))
                return $"default({csType})!";

            var elementType = ExtractElementType(cleanType);
            if (elementType is null)
                return $"default({csType})!";

            var elementExprs = root.EnumerateArray()
                .Select(e => DeserializeToExpression(e.GetRawText(), elementType ?? "System.Object"))
                .ToArray();
            return $"new {csType} {{{string.Join(", ", elementExprs)}}}";
        }

        // ── Object / fallback ──
        return $"/* TODO: manual */ default({MapToCSharpType(cleanType)})!";
    }

    /// <summary>
    /// Try to produce an enum expression: (EnumType)underlyingValue
    /// </summary>
    private static string TryEnumExpression(string cleanType, JsonElement root)
    {
        // Use the full CLR type name (e.g. "System.Buffers.OperationStatus") directly
        // for enum types, rather than the mapped C# short name which loses namespace info.
        if (cleanType.Contains('.') && !PrimitiveTypeNames.Contains(cleanType))
        {
            return $"({cleanType}){root.GetRawText()}";
        }
        return $"/* unknown type */ ({MapToCSharpType(cleanType)}){root.GetRawText()}";
    }

    /// <summary>
    /// Check if a type supports collection-initializer syntax (parameterless ctor + Add(T)).
    /// Only returns true for well-known BCL collection types that support { } initializer.
    /// ReadOnlyCollection&lt;T&gt;, Stack&lt;T&gt;, Queue&lt;T&gt;, LinkedList&lt;T&gt; all do NOT.
    /// </summary>
    private static bool IsCollectionType(string typeName)
    {
        var gaStart = typeName.IndexOf('<');
        var baseName = gaStart >= 0 ? typeName[..gaStart] : typeName;
        baseName = StripAssemblyQualification(baseName);

        return baseName switch
        {
            "System.Collections.Generic.List" => true,
            "System.Collections.Generic.HashSet" => true,
            "System.Collections.Generic.Collection" => true,
            "System.Collections.Generic.Dictionary" => true,
            _ => false
        };
    }

    /// <summary>
    /// Extract element type from an array type name like "System.Int32[]" → "System.Int32"
    /// </summary>
    private static string? ExtractElementType(string typeName)
    {
        var bracketIdx = typeName.IndexOf('[');
        return bracketIdx >= 0 ? typeName[..bracketIdx] : null;
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

    internal static List<string> SplitGenericArgs(string argsPart)
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

    /// <summary>
    /// Strip assembly qualification from generic type arguments.
    /// "Type`1[[System.Int32, System.Private.CoreLib, Version=10.0.0.0, Culture=neutral, PublicKeyToken=...]]"
    /// → "Type&lt;System.Int32&gt;"
    /// Also converts CLR '+' nested type separator to '.'.
    /// </summary>
    internal static string StripAssemblyQualification(string typeName)
    {
        // Unescape JSON Unicode escapes that Type.FullName writes
        var name = typeName.Replace("\\u002B", "+").Replace("\\u0060", "`");

        // Strip trailing & (ByRef marker) — it interferes with ^1 bracket stripping below.
        if (name.EndsWith('&'))
            name = name[..^1];

        var backtickIdx = name.IndexOf('`');
        if (backtickIdx < 0)
        {
            // No backtick: check for <...>-style assembly qualification embedded in
            // generic args, e.g. "ArraySegment<System.Byte, System.Private.CoreLib,
            // Version=10.0.0.0, Culture=neutral, PublicKeyToken=...>".
            // This happens when MLC's Type.FullName uses angle-bracket qualification
            // instead of the CLR standard backtick+[[...]] format.
            var gaStart = name.IndexOf('<');
            if (gaStart >= 0)
            {
                var gaEnd = name.LastIndexOf('>');
                if (gaEnd > gaStart)
                {
                    var basePart = name[..gaStart].Replace('+', '.');
                    var argsPart = name[(gaStart + 1)..gaEnd];
                    var args = SplitGenericArgs(argsPart);
                    // Assembly metadata tokens always contain '=' (Version=, Culture=, etc.)
                    var hasAssemblyInfo = args.Any(a => a.Contains('='));
                    if (hasAssemblyInfo)
                    {
                        var cleanArgs = args
                            .Select(a => a.Trim())
                            .Where(a => !a.Contains('='))
                            .Select(a => StripAssemblyQualification(a))
                            .ToArray();
                        return $"{basePart}<{string.Join(", ", cleanArgs)}>";
                    }
                }
            }
            return name.Replace('+', '.');
        }

        // Extract base name (strip arity)
        var baseName = name[..backtickIdx].Replace('+', '.');

        // Find generic arguments in bracket-enclosed form.
        // Type.FullName uses: Type`N[[arg1details],[arg2details]] (double-bracket for assembly-qualified)
        // or:                  Type`N[arg1,arg2] (single-bracket for simple names)
        var bracketStart = name.IndexOf('[');
        if (bracketStart < 0)
            return baseName;

        if (bracketStart + 1 < name.Length && name[bracketStart + 1] == '[')
        {
            // Double-bracket: [[Arg1, Assembly, ...], [Arg2, Assembly, ...]]
            var inner = name[(bracketStart + 1)..^1]; // strip outer []
            var args = SplitTopLevelBracketArgs(inner);
            var cleanArgs = args
                .Select(a => StripSingleAssemblyArg(a))
                .Select(a => StripAssemblyQualification(a))
                .ToArray();
            return $"{baseName}<{string.Join(", ", cleanArgs)}>";
        }
        else
        {
            // Single-bracket: [simple, args]
            var inner = name[(bracketStart + 1)..^1];
            var args = SplitGenericArgs(inner)
                .Select(a => StripAssemblyQualification(a.Trim()))
                .ToArray();
            return $"{baseName}<{string.Join(", ", args)}>";
        }
    }

    /// <summary>
    /// Split top-level bracket-enclosed generic arguments:
    /// "[A, ver, ...], [B, ver, ...]" → ["A, ver, ...", "B, ver, ..."]
    /// </summary>
    private static List<string> SplitTopLevelBracketArgs(string s)
    {
        var result = new List<string>();
        int depth = 0, start = 0;
        for (int i = 0; i < s.Length; i++)
        {
            if (s[i] == '[') { if (depth == 0) start = i + 1; depth++; }
            else if (s[i] == ']') { depth--; if (depth == 0) result.Add(s[start..i]); }
        }
        return result;
    }

    /// <summary>
    /// Given a single assembly-qualified generic arg like
    /// "System.Int32, System.Private.CoreLib, Version=10.0.0.0, ..."
    /// return just "System.Int32".
    /// Handles re-entrant generics: "Type`2[[...],[...]], Assembly, ..." → "Type`2[[...],[...]]"
    /// </summary>
    private static string StripSingleAssemblyArg(string arg)
    {
        arg = arg.Trim();
        int depth = 0;
        for (int i = 0; i < arg.Length; i++)
        {
            if (arg[i] == '[') depth++;
            else if (arg[i] == ']') depth--;
            else if (arg[i] == ',' && depth == 0)
                return arg[..i].Trim();
        }
        return arg;
    }
}
