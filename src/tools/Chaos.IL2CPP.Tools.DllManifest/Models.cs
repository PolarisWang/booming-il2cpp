using System.Reflection;
using System.Text.Json.Serialization;

public sealed record MethodInfoData(
    string MethodSubjectId,
    string Namespace,
    string DeclaringType,
    string MethodName,
    string ReturnType,
    IReadOnlyList<string> Parameters,
    string Visibility,
    bool IsStatic
);

public sealed record NamespaceInfo(int MethodCount);

public sealed record ManifestData(
    int SchemaVersion,
    string AssemblyName,
    string GeneratedAt,
    int TotalMethods,
    IReadOnlyList<MethodInfoData> Methods,
    Dictionary<string, NamespaceInfo> Namespaces
);

/// <summary>
/// Resolves assembly references by probing known directories.
/// </summary>
public sealed class ManifestAssemblyResolver : MetadataAssemblyResolver
{
    private readonly string[] _probePaths;
    private readonly Dictionary<string, Assembly> _cache = new(StringComparer.OrdinalIgnoreCase);

    public ManifestAssemblyResolver(string[] probePaths) => _probePaths = probePaths;

    public override Assembly? Resolve(MetadataLoadContext context, AssemblyName assemblyName)
    {
        var name = assemblyName.Name;
        if (name == null) return null;
        if (_cache.TryGetValue(name, out var cached)) return cached;

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
        return null;
    }
}
