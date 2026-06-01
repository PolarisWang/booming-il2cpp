using System.Linq;
using System.Reflection;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

public sealed class DllScanner
{
    private static readonly HashSet<string> ObjectMethods = new(StringComparer.Ordinal)
    {
        "ToString", "Equals", "GetHashCode", "Finalize", "MemberwiseClone", "GetType"
    };

    public DllScanResult Scan(string dllPath, string typeFullName)
    {
        var assemblyName = Path.GetFileNameWithoutExtension(dllPath);
        var fullPath = Path.GetFullPath(dllPath);

        var probeDirs = GetProbeDirectories(fullPath);
        var resolver = new AssemblyResolver(probeDirs);
        using var mlc = new MetadataLoadContext(resolver, "System.Private.CoreLib");

        var assembly = mlc.LoadFromAssemblyPath(fullPath);

        // Read target framework from assembly metadata via MLC
        var tfm = "";
        try
        {
            var tfaData = assembly.GetCustomAttributesData()
                .FirstOrDefault(a => a.AttributeType.Name == "TargetFrameworkAttribute");
            if (tfaData?.ConstructorArguments is { Count: > 0 } args)
                tfm = TfmFromMoniker(args[0].Value as string ?? "");
        }
        catch { /* best-effort — fall back to net8.0 */ }

        // Fallback: detect from DLL path or SDK
        if (string.IsNullOrEmpty(tfm))
            tfm = DetectTfm(dllPath);
        var targetType = assembly.GetType(typeFullName);
        if (targetType is null)
            throw new InvalidOperationException(
                $"Type '{typeFullName}' not found in assembly '{assemblyName}'. " +
                $"Available types: {string.Join(", ", assembly.GetTypes().Take(20).Select(t => t.FullName))}");

        var signatures = new List<MethodSignature>();
        var skippedMethods = new List<string>();

        foreach (var rawMethod in targetType.GetMethods(
            BindingFlags.Public | BindingFlags.Static |
            BindingFlags.Instance | BindingFlags.DeclaredOnly))
        {
            // Skip property accessors and operator overloads
            if (rawMethod.IsSpecialName && (
                    rawMethod.Name.StartsWith("get_") ||
                    rawMethod.Name.StartsWith("set_") ||
                    rawMethod.Name.StartsWith("add_") ||
                    rawMethod.Name.StartsWith("remove_") ||
                    rawMethod.Name.StartsWith("op_")))
                continue;

            // Skip object inherited methods
            if (ObjectMethods.Contains(rawMethod.Name))
                continue;

            var method = rawMethod;

            // Skip methods with ref struct parameters or return types
            if (HasRefStructParameter(method) || IsRefStructReturn(method))
            {
                skippedMethods.Add($"{method.Name} (ref struct)");
                continue;
            }

            // Try to concretize generic methods
            if (method.ContainsGenericParameters)
            {
                if (TryConcretizeGenericMethod(mlc, method, out var constructed))
                {
                    method = constructed;
                }
                else
                {
                    skippedMethods.Add(method.Name);
                    continue;
                }
            }

            // Skip methods that still have unresolved generic type parameters in their
            // signature (e.g. static interface methods from generic math interfaces)
            if (HasUnresolvedGenericParameters(method))
            {
                skippedMethods.Add($"{method.Name} (unresolved generics)");
                continue;
            }

            // Capture generic type args for concretized generic methods
            IReadOnlyList<string>? genericTypeArgs = null;
            if (method.IsGenericMethod && !method.ContainsGenericParameters)
            {
                genericTypeArgs = method.GetGenericArguments()
                    .Select(a => GetTypeName(a))
                    .ToList();
            }

            var paramList = method.GetParameters();
            var parameters = new List<MethodParameter>();
            var hasRefParam = false;

            foreach (var p in paramList)
            {
                var isOut = p.IsOut;
                var isRef = !isOut && p.ParameterType.IsByRef;
                if (isRef) hasRefParam = true;

                var paramTypeName = GetTypeName(p.ParameterType);
                var isRefStruct = IsRefStructType(p.ParameterType);
                parameters.Add(new MethodParameter(p.Name ?? $"p{parameters.Count}", paramTypeName, isOut, isRef, isRefStruct));
            }

            var returnType = method.ReturnType;
            var isVoid = returnType.FullName == "System.Void";

            signatures.Add(new MethodSignature(
                method.Name,
                targetType.FullName ?? typeFullName,
                GetTypeName(returnType),
                method.IsStatic,
                isVoid,
                hasRefParam,
                parameters,
                genericTypeArgs
            ));
        }

        return new DllScanResult(assemblyName, typeFullName, signatures, skippedMethods, tfm);
    }

    /// <summary>
    /// List all public types in the assembly with their public method counts.
    /// </summary>
    public List<(string FullName, int MethodCount)> ListPublicTypes(string dllPath)
    {
        var fullPath = Path.GetFullPath(dllPath);
        var probeDirs = GetProbeDirectories(fullPath);
        var resolver = new AssemblyResolver(probeDirs);
        using var mlc = new MetadataLoadContext(resolver, "System.Private.CoreLib");
        var assembly = mlc.LoadFromAssemblyPath(fullPath);

        var result = new List<(string, int)>();
        foreach (var t in assembly.GetTypes().OrderBy(t => t.FullName))
        {
            if (!t.IsPublic && !t.IsNestedPublic) continue;
            if (t.IsEnum) continue;

            var count = t.GetMethods(
                BindingFlags.Public | BindingFlags.Static |
                BindingFlags.Instance | BindingFlags.DeclaredOnly)
                .Count(m => m.Name is not ("get_" or "set_" or "add_" or "remove_")
                    && !m.Name.StartsWith("op_")
                    && !m.IsSpecialName);

            if (count > 0)
                result.Add((t.FullName ?? t.Name, count));
        }
        return result;
    }

    /// <summary>
    /// Try to concretize a generic method by substituting common type arguments.
    /// Works within the MetadataLoadContext.
    /// </summary>
    private static bool TryConcretizeGenericMethod(MetadataLoadContext mlc, MethodInfo method, out MethodInfo constructed)
    {
        constructed = null!;
        try
        {
            var genericParams = method.GetGenericArguments();
            if (genericParams.Length == 0) return false;

            var concreteTypes = new Type[genericParams.Length];

            for (int i = 0; i < genericParams.Length; i++)
            {
                var gp = genericParams[i];
                var constraints = gp.GetGenericParameterConstraints();
                var attrs = gp.GenericParameterAttributes;

                if (constraints.Length > 0)
                {
                    var hasValueTypeConstraint = constraints.Any(c =>
                        c.Name is "ValueType" or "Enum" || c.FullName == "System.ValueType" || c.FullName == "System.Enum");

                    if (hasValueTypeConstraint)
                    {
                        concreteTypes[i] = constraints.Any(c => c.FullName == "System.Enum")
                            ? LoadTypeInContext(mlc, "System.DayOfWeek") ?? LoadTypeInContext(mlc, "System.Int32")!
                            : LoadTypeInContext(mlc, "System.Int32")!;
                    }
                    else if (attrs.HasFlag(GenericParameterAttributes.ReferenceTypeConstraint))
                    {
                        concreteTypes[i] = LoadTypeInContext(mlc, "System.String")!;
                    }
                    else
                    {
                        concreteTypes[i] = LoadTypeInContext(mlc, "System.Int32")!;
                    }
                }
                else if (attrs.HasFlag(GenericParameterAttributes.ReferenceTypeConstraint))
                {
                    concreteTypes[i] = LoadTypeInContext(mlc, "System.String")!;
                }
                else
                {
                    concreteTypes[i] = LoadTypeInContext(mlc, "System.Int32")!;
                }

                if (concreteTypes[i] is null) return false;
            }

            constructed = method.MakeGenericMethod(concreteTypes);
            return true;
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// Find a type by full name across all assemblies loaded in the MetadataLoadContext.
    /// </summary>
    private static Type? LoadTypeInContext(MetadataLoadContext mlc, string typeFullName)
    {
        foreach (var asm in mlc.GetAssemblies())
        {
            var type = asm.GetType(typeFullName);
            if (type is not null) return type;
        }
        return null;
    }

    private static string GetTypeName(Type type)
    {
        // Generic parameter (e.g. T, TOther) — use base type constraint or object
        if (type.IsGenericParameter)
        {
            var constraints = type.GetGenericParameterConstraints();
            if (constraints.Length > 0 &&
                constraints[0].FullName is { } cn &&
                cn != "System.ValueType" &&
                cn != "System.Enum")
                return cn;
            return "System.Object";
        }

        if (type.IsGenericType)
        {
            var args = string.Join(",", type.GetGenericArguments().Select(GetTypeName));
            var defName = type.GetGenericTypeDefinition().Name;
            var backtick = defName.IndexOf('`');
            if (backtick >= 0) defName = defName[..backtick];
            return $"{defName}<{args}>";
        }

        var fullName = type.FullName;
        if (fullName is not null)
        {
            // CLR nested type: produce C# parent-qualified name, e.g.
            // "System.Text.StringBuilder+AppendInterpolatedStringHandler"
            // → "StringBuilder.AppendInterpolatedStringHandler"
            if (fullName.Contains('+') && type.DeclaringType is not null)
            {
                var parentCSharp = CSharpSerializer.MapToCSharpType(
                    type.DeclaringType.FullName ?? type.DeclaringType.Name);
                return $"{parentCSharp}.{type.Name}";
            }
            return fullName;
        }

        // MetadataLoadContext sometimes loses FullName for constructed generics.
        // Fall back to Name (e.g. "Span`1") and fill arity with byte.
        var name = type.Name;
        var bt = name.IndexOf('`');
        if (bt < 0) return name;

        var arityStr = name[(bt + 1)..];
        if (int.TryParse(arityStr, out var arity) && arity > 0)
        {
            var args = string.Join(",", Enumerable.Repeat("System.Byte", arity));
            return $"{name[..bt]}<{args}>";
        }
        return name;
    }

    /// <summary>
    /// Check if a method has any ref struct parameters (e.g. ReadOnlySpan&lt;char&gt;).
    /// </summary>
    private static bool HasRefStructParameter(MethodInfo method)
    {
        try
        {
            return method.GetParameters().Any(p => IsRefStructType(p.ParameterType));
        }
        catch { return false; }
    }

    /// <summary>
    /// Check if a method returns a ref struct type.
    /// </summary>
    private static bool IsRefStructReturn(MethodInfo method)
    {
        try { return IsRefStructType(method.ReturnType); }
        catch { return false; }
    }

    /// <summary>
    /// Check if a type is a ref struct (IsByRefLike).
    /// For MLC's limited detection of CLR nested ref structs (e.g.
    /// StringBuilder+AppendInterpolatedStringHandler), falls back to
    /// heuristic: value type whose FullName contains '+' (CLR nested marker).
    /// </summary>
    private static bool IsRefStructType(Type type)
    {
        // Direct IsByRefLike check
        if (type.IsByRefLike) return true;

        // For ByRef-wrapped types (ref/out parameters)
        if (type.IsByRef && type.GetElementType() is { } elementType)
        {
            if (elementType.IsByRefLike) return true;
            // Heuristic: CLR nested struct is likely a ref struct
            if (elementType.IsValueType && elementType.FullName?.Contains('+') == true)
                return true;
            return false;
        }

        // Heuristic: CLR nested value type that isn't a normal struct
        if (type.IsValueType && type.FullName?.Contains('+') == true)
            return true;

        return false;
    }

    /// <summary>
    /// Check if a method still has unresolved generic parameters in its signature
    /// after concretization (e.g. interface-level type params from generic math interfaces
    /// like INumberBase&lt;TOther&gt; where TOther comes from the interface, not the method).
    /// </summary>
    private static bool HasUnresolvedGenericParameters(MethodInfo method)
    {
        try
        {
            if (method.ReturnType.ContainsGenericParameters)
                return true;
            if (method.GetParameters().Any(p => p.ParameterType.ContainsGenericParameters))
                return true;
            return false;
        }
        catch
        {
            return true; // conservative: skip if we can't inspect
        }
    }

    private static string[] GetProbeDirectories(string dllPath)
    {
        var dirs = new List<string>
        {
            Path.GetDirectoryName(dllPath)!
        };

        // Add .NET reference assembly path
        var dotnetRoot = Path.GetDirectoryName(typeof(object).Assembly.Location);
        if (dotnetRoot is not null && !dirs.Contains(dotnetRoot))
            dirs.Add(dotnetRoot);

        // Add self directory for System.Runtime etc.
        var selfDir = Path.GetDirectoryName(typeof(DllScanner).Assembly.Location);
        if (selfDir is not null && !dirs.Contains(selfDir))
            dirs.Add(selfDir);

        return dirs.ToArray();
    }

    private static string TfmFromMoniker(string frameworkName)
    {
        // ".NETCoreApp,Version=v10.0" → "net10.0"
        if (string.IsNullOrEmpty(frameworkName)) return "";
        var parts = frameworkName.Split(',');
        if (parts.Length < 2) return "";
        var namePart = parts[0].Trim();
        var verPart = parts[1].Trim();
        if (!verPart.StartsWith("Version=v", StringComparison.Ordinal)) return "";
        var ver = verPart["Version=v".Length..];
        return namePart switch
        {
            ".NETCoreApp" => $"net{ver}",
            ".NETStandard" => $"netstandard{ver}",
            ".NETFramework" => $"net{ver.Replace(".", "")}",
            _ => ""
        };
    }

    private static string DetectTfm(string dllPath)
    {
        // Try to infer TFM from the directory path:
        // ".../shared/Microsoft.NETCore.App/10.0.6/System.Runtime.dll" → "net10.0"
        var parts = dllPath.Replace('\\', '/').Split('/');
        for (int i = 0; i < parts.Length - 1; i++)
        {
            if (parts[i] == "Microsoft.NETCore.App" && System.Version.TryParse(parts[i + 1], out var ver))
                return $"net{ver.Major}.{ver.Minor}";
        }
        return "net8.0";
    }

    /// <summary>
    /// Resolves assemblies by probing known directories.
    /// </summary>
    private sealed class AssemblyResolver : MetadataAssemblyResolver
    {
        private readonly string[] _probePaths;
        private readonly Dictionary<string, Assembly> _cache = new(StringComparer.OrdinalIgnoreCase);

        public AssemblyResolver(string[] probePaths)
        {
            _probePaths = probePaths;
        }

        public override Assembly? Resolve(MetadataLoadContext context, AssemblyName assemblyName)
        {
            var name = assemblyName.Name;
            if (name is null) return null;

            if (_cache.TryGetValue(name, out var cached))
                return cached;

            foreach (var dir in _probePaths)
            {
                var path = Path.Combine(dir, $"{name}.dll");
                if (File.Exists(path))
                {
                    var asm = context.LoadFromAssemblyPath(path);
                    _cache[name] = asm;
                    return asm;
                }
            }

            // For core assemblies that aren't found, try the runtime directory
            var runtimeDir = Path.GetDirectoryName(typeof(object).Assembly.Location);
            if (runtimeDir is not null)
            {
                var runtimePath = Path.Combine(runtimeDir, $"{name}.dll");
                if (File.Exists(runtimePath))
                {
                    var asm = context.LoadFromAssemblyPath(runtimePath);
                    _cache[name] = asm;
                    return asm;
                }
            }

            return null;
        }
    }
}

public sealed record DllScanResult(
    string AssemblyName,
    string TypeFullName,
    IReadOnlyList<MethodSignature> Methods,
    IReadOnlyList<string> SkippedMethods,
    string TargetFramework
);
