using System.Reflection;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

public sealed class SubjectAssemblyResolver(string[] probePaths) : MetadataAssemblyResolver
{
    private readonly Dictionary<string, Assembly> _cache = new(StringComparer.OrdinalIgnoreCase);

    public override Assembly? Resolve(MetadataLoadContext context, AssemblyName assemblyName)
    {
        var name = assemblyName.Name;
        if (name is null) return null;

        if (_cache.TryGetValue(name, out var cached))
            return cached;

        foreach (var dir in probePaths)
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
