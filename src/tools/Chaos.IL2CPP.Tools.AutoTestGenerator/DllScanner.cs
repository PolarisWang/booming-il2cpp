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
            // Skip property accessors
            if (rawMethod.IsSpecialName && (
                    rawMethod.Name.StartsWith("get_") ||
                    rawMethod.Name.StartsWith("set_") ||
                    rawMethod.Name.StartsWith("add_") ||
                    rawMethod.Name.StartsWith("remove_")))
                continue;

            // Skip object inherited methods
            if (ObjectMethods.Contains(rawMethod.Name))
                continue;

            var method = rawMethod;

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

            var paramList = method.GetParameters();
            var parameters = new List<MethodParameter>();
            var hasRefParam = false;

            foreach (var p in paramList)
            {
                var isOut = p.IsOut;
                var isRef = !isOut && p.ParameterType.IsByRef;
                if (isRef) hasRefParam = true;

                var paramTypeName = GetTypeName(p.ParameterType);
                parameters.Add(new MethodParameter(p.Name ?? $"p{parameters.Count}", paramTypeName, isOut, isRef));
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
                parameters
            ));
        }

        return new DllScanResult(assemblyName, typeFullName, signatures, skippedMethods, tfm);
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
                        concreteTypes[i] = constraints[0];
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
        if (type.IsGenericType)
        {
            var args = string.Join(",", type.GetGenericArguments().Select(GetTypeName));
            var defName = type.GetGenericTypeDefinition().Name;
            var backtick = defName.IndexOf('`');
            if (backtick >= 0) defName = defName[..backtick];
            return $"{defName}<{args}>";
        }

        var fullName = type.FullName;
        if (fullName is not null) return fullName;

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
