using System.Reflection;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

public sealed class MetadataReader
{
    private static readonly (string, SubjectKind)[] AttributeMappings =
    [
        ("MethodSubjectId", SubjectKind.Fact),
        ("BenchmarkSubjectId", SubjectKind.Benchmark),
        ("HotUpdateSubjectId", SubjectKind.HotUpdate),
    ];

    public List<SubjectModel> ReadAssemblies(IReadOnlyList<string> assemblyPaths)
    {
        var probeDirs = assemblyPaths
            .Select(Path.GetDirectoryName)
            .OfType<string>()
            .Distinct()
            .ToList();

        // Add .NET reference assembly path for System.Private.CoreLib resolution
        var dotnetRoot = Path.GetDirectoryName(typeof(object).Assembly.Location);
        if (dotnetRoot is not null)
            probeDirs.Add(dotnetRoot);

        var resolver = new SubjectAssemblyResolver([.. probeDirs]);
        using var mlc = new MetadataLoadContext(resolver, "System.Private.CoreLib");

        var subjects = new List<SubjectModel>();
        foreach (var path in assemblyPaths)
        {
            var asm = mlc.LoadFromAssemblyPath(path);
            ScanAssembly(asm, subjects);
        }

        return subjects;
    }

    private static void ScanAssembly(Assembly assembly, List<SubjectModel> subjects)
    {
        var asmName = assembly.GetName().Name ?? "";

        foreach (var type in assembly.GetTypes())
        {
            foreach (var method in type.GetMethods(
                BindingFlags.Public | BindingFlags.Static |
                BindingFlags.Instance | BindingFlags.DeclaredOnly))
            {
                foreach (var (attrName, kind) in AttributeMappings)
                {
                    if (!HasCustomAttribute(method, attrName))
                        continue;

                    var paramList = method.GetParameters();
                    var subjectId = ExtractSubjectId(method, attrName);

                    subjects.Add(new SubjectModel
                    {
                        Kind = kind,
                        AssemblyName = asmName,
                        FullTypeName = type.FullName ?? type.Name,
                        MethodName = method.Name,
                        IsStatic = method.IsStatic,
                        ReturnTypeFullName = method.ReturnType.FullName ?? "",
                        Parameters = [.. paramList.Select(p => new ParameterModel
                        {
                            TypeFullName = p.ParameterType.FullName ?? "",
                            Name = p.Name ?? ""
                        })],
                        SubjectId = subjectId,
                    });
                }
            }
        }
    }

    private static bool HasCustomAttribute(MethodInfo method, string attributeName)
    {
        foreach (var attr in method.GetCustomAttributesData())
        {
            var name = attr.AttributeType.Name;
            if (name == attributeName || name == $"{attributeName}Attribute")
                return true;
        }
        return false;
    }

    private static string ExtractSubjectId(MethodInfo method, string attributeName)
    {
        foreach (var attr in method.GetCustomAttributesData())
        {
            var name = attr.AttributeType.Name;
            if (name != attributeName && name != $"{attributeName}Attribute")
                continue;

            if (attr.ConstructorArguments.Count > 0)
            {
                var arg = attr.ConstructorArguments[0];
                if (arg.Value is string s)
                    return s;
            }
        }
        return "";
    }
}
