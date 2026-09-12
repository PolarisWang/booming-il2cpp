using System.Reflection;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Chaos.IL2CPP.Tools.ApiSurfaceScanner;

/// <summary>
/// Enumerates the public API surface of the <c>System.Reflection</c> namespace
/// from .NET reference assemblies, producing the coverage matrix that drives
/// the reflection-production-readiness roadmap (Phase 0).
///
/// Layer boundary: this tool lives under src/tools/ (ATG layer) and emits only
/// .cs / .json. It never generates C++.
/// </summary>
public static class Program
{
    /// <summary>
    /// Reflection types whose members are enumerated. The denominator is the
    /// public API surface of these types across the target assemblies.
    /// </summary>
    private static readonly string[] TargetNamespaces =
    {
        "System.Reflection",
    };

    /// <summary>
    /// Reflection.Emit and friends are structurally impossible under AOT.
    /// Per design §2.2 they are classified not-applicable and excluded from
    /// the denominator.
    /// </summary>
    private static readonly string[] NotApplicableTypePrefixes =
    {
        "System.Reflection.Emit.",
        "System.Reflection.Metadata.Ecma335.",  // emit-side writer APIs
    };

    /// <summary>
    /// Namespaces that are reflection-adjacent but out of scope for this
    /// roadmap (separate DLLs with their own tracking).
    /// </summary>
    private static readonly string[] OutOfScopeNamespacePrefixes =
    {
        "System.Reflection.Metadata",           // standalone DLL, separately pipelined
        "System.Reflection.TypeExtensions",     // facade
        "System.Reflection.DispatchProxy",      // facade
    };

    public static int Main(string[] args)
    {
        var options = ParseArgs(args);
        if (options is null) return 2;

        var refPackDir = options.RefPackDir;
        if (!Directory.Exists(refPackDir))
        {
            Console.Error.WriteLine($"ref-pack dir not found: {refPackDir}");
            return 2;
        }

        // Discover the reference assemblies that carry the reflection surface.
        var assemblyPaths = DiscoverTargetAssemblies(refPackDir);
        if (assemblyPaths.Count == 0)
        {
            Console.Error.WriteLine($"no target assemblies found under {refPackDir}");
            return 2;
        }

        Console.WriteLine($"[scan] enumerating {assemblyPaths.Count} assemblies:");
        foreach (var p in assemblyPaths) Console.WriteLine($"       {Path.GetFileName(p)}");

        // Resolver: reference assemblies only (no runtime implementation assemblies),
        // which keeps the surface stable and AOT-relevant.
        var resolver = new PathAssemblyResolver(
            Directory.GetFiles(refPackDir, "*.dll"));

        using var mlc = new MetadataLoadContext(resolver);
        var entries = new List<ApiEntry>();
        var seen = new HashSet<string>(StringComparer.Ordinal);

        foreach (var asmPath in assemblyPaths)
        {
            Assembly asm;
            try { asm = mlc.LoadFromAssemblyPath(asmPath); }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"[warn] cannot load {Path.GetFileName(asmPath)}: {ex.Message}");
                continue;
            }

            foreach (var type in SafeGetTypes(asm))
            {
                if (!IsTargetType(type)) continue;
                CollectTypeMembers(type, entries, seen);
            }        }

        var matrix = BuildMatrix(entries, options);
        Classifier.Apply(matrix);
        WriteOutputs(matrix, options);

        PrintSummary(matrix);
        return 0;
    }

    // ── CLI ──────────────────────────────────────────────────────────────

    private sealed class Options
    {
        public string RefPackDir { get; init; } = "";
        public string OutDir { get; init; } = "";
        public string TargetFramework { get; init; } = "";
    }

    private static Options? ParseArgs(string[] args)
    {
        string? refPack = null, outDir = null, tfm = "net8.0";
        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--ref-pack" when i + 1 < args.Length: refPack = args[++i]; break;
                case "--out" when i + 1 < args.Length: outDir = args[++i]; break;
                case "--tfm" when i + 1 < args.Length: tfm = args[++i]; break;
                case "-h" or "--help":
                    Console.WriteLine("""
                        Usage: ApiSurfaceScanner --ref-pack <dir> --out <dir> [--tfm net8.0]

                          --ref-pack  Directory of .NET reference assemblies (Microsoft.NETCore.App.Ref/*/ref/<tfm>)
                          --out       Output directory for api-coverage-matrix.json and api-surface-report.md
                          --tfm       Target framework moniker, recorded in the matrix (default net8.0)
                        """);
                    return null;
                default:
                    Console.Error.WriteLine($"unknown arg: {args[i]}");
                    return null;
            }
        }
        if (refPack is null || outDir is null)
        {
            Console.Error.WriteLine("--ref-pack and --out are required (use --help)");
            return null;
        }
        return new Options { RefPackDir = refPack, OutDir = outDir, TargetFramework = tfm };
    }

    // ── Assembly discovery ───────────────────────────────────────────────

    private static List<string> DiscoverTargetAssemblies(string refPackDir)
    {
        // System.Runtime is the primary carrier of reflection API contracts;
        // System.Private.CoreLib is not shipped in the ref pack, so
        // System.Runtime + mscorlib facades provide the authoritative surface.
        string[] preferred = { "System.Runtime.dll", "mscorlib.dll", "System.Reflection.dll", "System.Runtime.Extensions.dll" };

        var all = Directory.GetFiles(refPackDir, "*.dll")
            .GroupBy(p => Path.GetFileName(p), StringComparer.OrdinalIgnoreCase)
            .ToDictionary(g => g.Key, g => g.First(), StringComparer.OrdinalIgnoreCase);
        var result = new List<string>();
        foreach (var name in preferred)
            if (all.TryGetValue(name, out var p)) result.Add(p);

        // Fall back to scanning every assembly if the preferred set is absent.
        if (result.Count == 0) result.AddRange(all.Values);
        return result;
    }

    // ── Type filtering ───────────────────────────────────────────────────

    private static bool IsTargetType(Type type)
    {
        var ns = type.Namespace ?? "";

        // Must be in the reflection namespace tree.
        bool inScope = TargetNamespaces.Any(t =>
            ns.Equals(t, StringComparison.Ordinal) ||
            ns.StartsWith(t + ".", StringComparison.Ordinal));
        if (!inScope) return false;

        // Reflection.Emit & friends: structurally impossible under AOT.
        if (NotApplicableTypePrefixes.Any(p => ns.StartsWith(p, StringComparison.Ordinal))) return false;

        // Metadata/Ecma335 and facades are tracked elsewhere.
        if (OutOfScopeNamespacePrefixes.Any(p =>
                ns.Equals(p, StringComparison.Ordinal) ||
                ns.StartsWith(p + ".", StringComparison.Ordinal))) return false;

        // Only public surface counts toward the denominator.
        return type.IsPublic || type.IsNestedPublic;
    }

    private static bool IsNotApplicableType(Type type)
    {
        var ns = type.Namespace ?? "";
        return NotApplicableTypePrefixes.Any(p => ns.StartsWith(p, StringComparison.Ordinal));
    }

    /// <summary>
    /// Data-carrier types: exceptions and attributes. These are ordinary managed
    /// classes with no reflection-operation semantics of their own. Their members
    /// are handled by the general BCL translation path (see Classifier), so they
    /// are excluded from the reflection-operation denominator.
    ///
    /// A type qualifies when, within the reflection namespace, it is an exception
    /// (derives from Exception) or is annotated with AttributeUsageAttribute.
    /// </summary>
    private static bool IsDataCarrierType(Type type)
    {
        // Exception hierarchy
        if (DerivesFrom(type, "System.Exception")) return true;

        // Attribute classes: AttributeUsage is inherited from System.Attribute,
        // and MetadataLoadContext does not surface inherited attribute data, so
        // walk the base chain to System.Attribute.
        if (DerivesFrom(type, "System.Attribute")) return true;

        return false;
    }

    /// <summary>Walks the base-type chain looking for <paramref name="baseFullName"/>.</summary>
    private static bool DerivesFrom(Type type, string baseFullName)
    {
        try
        {
            for (var t = type.BaseType; t is not null; t = t.BaseType)
                if (t.FullName == baseFullName) return true;
        }
        catch { /* metadata unavailable */ }
        return false;
    }

    private static IEnumerable<Type> SafeGetTypes(Assembly asm)
    {
        try { return asm.GetTypes(); }
        catch (ReflectionTypeLoadException ex)
        {
            Console.Error.WriteLine($"[warn] partial type load in {asm.GetName().Name}: {ex.LoaderExceptions.Length} loader errors");
            return ex.Types.Where(t => t is not null)!;
        }
    }

    // ── Member collection ────────────────────────────────────────────────

    private static void CollectTypeMembers(Type type, List<ApiEntry> entries, HashSet<string> seen)
    {
        const BindingFlags flags = BindingFlags.Public
                                 | BindingFlags.Instance
                                 | BindingFlags.Static
                                 | BindingFlags.DeclaredOnly;

        string displayType = DisplayTypeName(type);

        // Data-carrier types (exceptions / attributes) and pure enums hold no
        // reflection-operation semantics. Record them for completeness but tier
        // them outside the operation denominator.
        string? carrierTier =
            type.IsEnum ? Tier.EnumSurface :
            IsDataCarrierType(type) ? Tier.DataCarrier :
            null;

        if (carrierTier is not null)
        {
            CollectCarrierMembers(type, displayType, entries, seen, carrierTier);
            return;
        }

        // Methods
        foreach (var m in SafeMembers<MethodInfo>(() => type.GetMethods(flags)))
        {
            if (m.IsSpecialName) continue;  // property/event accessors counted via the property/event
            Add(entries, seen, type.Namespace ?? "", displayType, m.Name, "method",
                Signature(m), m.IsStatic, m.IsPublic);
        }

        // Constructors
        foreach (var c in SafeMembers<ConstructorInfo>(() => type.GetConstructors(flags)))
        {
            Add(entries, seen, type.Namespace ?? "", displayType, ".ctor", "constructor",
                CtorSignature(c), false, c.IsPublic);
        }

        // Properties
        foreach (var p in SafeMembers<PropertyInfo>(() => type.GetProperties(flags)))
        {
            Add(entries, seen, type.Namespace ?? "", displayType, p.Name, "property",
                PropertySignature(p), false, true);
        }

        // Events
        foreach (var e in SafeMembers<EventInfo>(() => type.GetEvents(flags)))
        {
            Add(entries, seen, type.Namespace ?? "", displayType, e.Name, "event",
                $"{DisplayTypeName(e.EventHandlerType!)} {e.Name}", false, true);
        }

        // Fields (public only — non-public fields are not part of the contract surface)
        foreach (var f in SafeMembers<FieldInfo>(() => type.GetFields(flags)))
        {
            Add(entries, seen, type.Namespace ?? "", displayType, f.Name, "field",
                $"{DisplayTypeName(f.FieldType)} {f.Name}", f.IsStatic, f.IsPublic);
        }
    }

    private static IEnumerable<T> SafeMembers<T>(Func<T[]> getter)
    {
        try { return getter(); }
        catch { return Array.Empty<T>(); }
    }

    /// <summary>
    /// True when a tier represents an actual reflection <em>operation</em> and
    /// therefore belongs in the denominator.
    /// </summary>
    internal static bool IsOperationTier(string tier) =>
        tier is not (Tier.NotApplicable or Tier.EnumSurface or Tier.DataCarrier);

    /// <summary>
    /// Records members of enum / exception / attribute types under a
    /// non-operation tier so the denominator stays focused on reflection
    /// operations while the full surface remains auditable.
    /// </summary>
    private static void CollectCarrierMembers(Type type, string displayType,
        List<ApiEntry> entries, HashSet<string> seen, string tier)
    {
        const BindingFlags flags = BindingFlags.Public
                                 | BindingFlags.Instance
                                 | BindingFlags.Static
                                 | BindingFlags.DeclaredOnly;

        if (type.IsEnum)
        {
            foreach (var f in SafeMembers<FieldInfo>(() => type.GetFields(BindingFlags.Public | BindingFlags.Static)))
            {
                if (!f.IsLiteral) continue;
                Add(entries, seen, type.Namespace ?? "", displayType, f.Name, "enum",
                    $"{displayType}.{f.Name}", true, true, tier);
            }
            return;
        }

        // Data carriers: record only the public constructors and properties that
        // constitute their data surface — enough for auditability, without
        // flooding the matrix with inherited object members.
        foreach (var c in SafeMembers<ConstructorInfo>(() => type.GetConstructors(flags)))
            Add(entries, seen, type.Namespace ?? "", displayType, ".ctor", "carrier-ctor",
                CtorSignature(c), false, c.IsPublic, tier);

        foreach (var p in SafeMembers<PropertyInfo>(() => type.GetProperties(flags)))
            Add(entries, seen, type.Namespace ?? "", displayType, p.Name, "carrier-property",
                PropertySignature(p), false, true, tier);
    }

    private static void Add(List<ApiEntry> entries, HashSet<string> seen,
        string ns, string typeName, string memberName, string kind,
        string signature, bool isStatic, bool isPublic,
        string tier = Tier.Unclassified)
    {
        // Public contract surface only.
        if (!isPublic) return;

        string key = $"{ns}|{typeName}|{memberName}|{kind}|{signature}";
        if (!seen.Add(key)) return;

        entries.Add(new ApiEntry
        {
            Namespace = ns,
            TypeName = typeName,
            MemberName = memberName,
            MemberKind = kind,
            Signature = signature,
            IsStatic = isStatic,
            IsPublic = isPublic,
            TierValue = tier,
            RealKind = "none",
        });
    }

    // ── Signature formatting ─────────────────────────────────────────────

    private static string Signature(MethodInfo m)
    {
        var ps = string.Join(", ", m.GetParameters().Select(ParameterDisplay));
        return $"{DisplayTypeName(m.ReturnType)} {m.Name}({ps})";
    }

    private static string CtorSignature(ConstructorInfo c)
    {
        var ps = string.Join(", ", c.GetParameters().Select(ParameterDisplay));
        return $".ctor({ps})";
    }

    private static string PropertySignature(PropertyInfo p)
    {
        string idx = "";
        var ip = p.GetIndexParameters();
        if (ip.Length > 0) idx = "[" + string.Join(", ", ip.Select(ParameterDisplay)) + "]";
        return $"{DisplayTypeName(p.PropertyType)} {p.Name}{idx} {{ get; }}";
    }

    private static string ParameterDisplay(ParameterInfo p)
    {
        var t = p.ParameterType;
        if (t.IsByRef) return "ref " + DisplayTypeName(t.GetElementType()!);
        return DisplayTypeName(t);
    }

    private static string DisplayTypeName(Type t)
    {
        if (t is null) return "?";
        if (t.IsGenericType)
        {
            var name = t.Name;
            int tick = name.IndexOf('`');
            if (tick >= 0) name = name[..tick];
            var args = string.Join(", ", t.GetGenericArguments().Select(DisplayTypeName));
            return $"{name}<{args}>";
        }
        if (t.IsArray) return DisplayTypeName(t.GetElementType()!) + "[]";
        if (t == typeof(void)) return "void";
        return t.Name;
    }

    // ── Matrix assembly & output ─────────────────────────────────────────

    private static CoverageMatrix BuildMatrix(List<ApiEntry> entries, Options options)
    {
        var byTier = entries.GroupBy(e => e.TierValue)
                            .ToDictionary(g => g.Key, g => g.Count());
        var byType = entries.Where(e => e.TierValue != Tier.EnumSurface)
                            .GroupBy(e => e.TypeName)
                            .OrderByDescending(g => g.Count())
                            .ToDictionary(g => g.Key, g => g.Count());

        // Denominator excludes not-applicable (design §2.2), pure enum constants
        // and data-carrier types (metadata values / ordinary BCL classes, not
        // reflection operations).
        int denominator = entries.Count(e => IsOperationTier(e.TierValue));

        int enumSurface = entries.Count(e => e.TierValue == Tier.EnumSurface);

        return new CoverageMatrix
        {
            GeneratedAt = DateTimeOffset.UtcNow.ToString("O"),
            Summary = new MatrixSummary
            {
                GeneratedFrom = new List<string> { "Microsoft.NETCore.App.Ref", options.TargetFramework },
                TargetFramework = options.TargetFramework,
                TotalApis = entries.Count,
                ByTier = byTier,
                ByType = byType,
                Denominator = denominator,            },
            Entries = entries.OrderBy(e => e.Namespace, StringComparer.Ordinal)
                             .ThenBy(e => e.TypeName, StringComparer.Ordinal)
                             .ThenBy(e => e.MemberName, StringComparer.Ordinal)
                             .ToList(),
        };
    }

    private static void WriteOutputs(CoverageMatrix matrix, Options options)
    {
        Directory.CreateDirectory(options.OutDir);

        var jsonOpts = new JsonSerializerOptions
        {
            WriteIndented = true,
            DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
            Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping,
        };

        var matrixPath = Path.Combine(options.OutDir, "api-coverage-matrix.json");
        File.WriteAllText(matrixPath, JsonSerializer.Serialize(matrix, jsonOpts), new UTF8Encoding(false));
        Console.WriteLine($"[out] {matrixPath}");

        var reportPath = Path.Combine(options.OutDir, "api-surface-report.md");
        File.WriteAllText(reportPath, BuildReport(matrix, options), new UTF8Encoding(false));
        Console.WriteLine($"[out] {reportPath}");
    }

    private static string BuildReport(CoverageMatrix matrix, Options options)
    {
        var sb = new StringBuilder();
        var s = matrix.Summary;

        sb.AppendLine("# Reflection API Surface Report");
        sb.AppendLine();
        sb.AppendLine($"> Auto-generated by `Chaos.IL2CPP.Tools.ApiSurfaceScanner` at {matrix.GeneratedAt}");
        sb.AppendLine($"> Target framework: `{s.TargetFramework}` ｜ Source: `Microsoft.NETCore.App.Ref`");
        sb.AppendLine();
        sb.AppendLine("## 总量数字（Phase 0 关键产出）");
        sb.AppendLine();
        sb.AppendLine("| 指标 | 值 |");
        sb.AppendLine("|---|---|");
        sb.AppendLine($"| **公开 API 总数** | **{s.TotalApis}** |");
        sb.AppendLine($"| **分母**（排除 not-applicable） | **{s.Denominator}** |");
        foreach (var kv in s.ByTier.OrderByDescending(k => k.Value))
            sb.AppendLine($"| tier = `{kv.Key}` | {kv.Value} |");
        sb.AppendLine();

        sb.AppendLine("## 按类型分布（Top 30）");
        sb.AppendLine();
        sb.AppendLine("| 类型 | API 数 |");
        sb.AppendLine("|---|---|");
        foreach (var kv in s.ByType.Take(30))
            sb.AppendLine($"| `{kv.Key}` | {kv.Value} |");
        sb.AppendLine();

        sb.AppendLine("## 判定口径");
        sb.AppendLine();
        sb.AppendLine("三档判定制（design §3.1），**不允许静默返错值**：");
        sb.AppendLine();
        sb.AppendLine("- `real` — native 有真逻辑（非 residual stub）");
        sb.AppendLine("- `not-supported` — 显式抛 `NotSupportedException`");
        sb.AppendLine("- `not-applicable` — AOT 下无意义（如 Reflection.Emit），不计入分母");
        sb.AppendLine("- `unclassified` — 待判定（Phase 2 的工作面）");
        sb.AppendLine();

        return sb.ToString();
    }

    private static void PrintSummary(CoverageMatrix matrix)
    {
        var s = matrix.Summary;
        Console.WriteLine();
        Console.WriteLine("══════════ Phase 0 总量数字 ══════════");
        Console.WriteLine($"  公开 API 总数 : {s.TotalApis}");
        Console.WriteLine($"  分母(排除N/A) : {s.Denominator}");
        foreach (var kv in s.ByTier.OrderByDescending(k => k.Value))
            Console.WriteLine($"    tier={kv.Key,-16} {kv.Value}");
        Console.WriteLine("══════════════════════════════════════");
    }
}
