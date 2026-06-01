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
        var targetType = assembly.GetType(typeFullName);
        if (targetType is null)
            throw new InvalidOperationException(
                $"Type '{typeFullName}' not found in assembly '{assemblyName}'. " +
                $"Available types: {string.Join(", ", assembly.GetTypes().Take(20).Select(t => t.FullName))}");

        var signatures = new List<MethodSignature>();
        var skippedMethods = new List<string>();

        foreach (var method in targetType.GetMethods(
            BindingFlags.Public | BindingFlags.Static |
            BindingFlags.Instance | BindingFlags.DeclaredOnly))
        {
            // Skip property accessors
            if (method.IsSpecialName && (
                    method.Name.StartsWith("get_") ||
                    method.Name.StartsWith("set_") ||
                    method.Name.StartsWith("add_") ||
                    method.Name.StartsWith("remove_")))
                continue;

            // Skip object inherited methods
            if (ObjectMethods.Contains(method.Name))
                continue;

            // Skip generic methods that can't be concretized
            if (method.ContainsGenericParameters)
            {
                skippedMethods.Add(method.Name);
                continue;
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

        return new DllScanResult(assemblyName, typeFullName, signatures, skippedMethods);
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
    IReadOnlyList<string> SkippedMethods
);
