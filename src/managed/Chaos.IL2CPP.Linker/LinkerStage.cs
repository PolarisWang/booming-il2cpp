using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed class LinkerStage
{
    public string Name => "Linker";

    public LinkedWorldModel Link(SemanticWorldModel semanticWorld)
    {
        var orderedMethods = semanticWorld.Methods
            .OrderBy(method => string.Equals(method.SubjectId, semanticWorld.EntryPointSubjectId, StringComparison.Ordinal) ? 0 : 1)
            .ThenBy(method => method.MetadataToken)
            .ToList();

        var orderedTypes = OrderTypes(semanticWorld, orderedMethods);
        var dependencies = CollectExternalDependencies(semanticWorld);

        return new LinkedWorldModel
        {
            InputAssemblyPath = semanticWorld.InputAssemblyPath,
            Assembly = semanticWorld.Assembly,
            EntryPointSubjectId = semanticWorld.EntryPointSubjectId,
            Types = orderedTypes,
            Fields = semanticWorld.Fields.ToList(),
            Methods = orderedMethods,
            Dependencies = dependencies,
        };
    }

    private static IReadOnlyList<ManagedTypeModel> OrderTypes(
        SemanticWorldModel semanticWorld,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var typeMap = semanticWorld.Types.ToDictionary(type => type.SubjectId, StringComparer.Ordinal);
        var orderedTypeIds = new List<string>();

        foreach (var method in orderedMethods)
        {
            if (!orderedTypeIds.Contains(method.DeclaringTypeSubjectId, StringComparer.Ordinal))
            {
                orderedTypeIds.Add(method.DeclaringTypeSubjectId);
            }
        }

        foreach (var type in semanticWorld.Types)
        {
            if (!orderedTypeIds.Contains(type.SubjectId, StringComparer.Ordinal))
            {
                orderedTypeIds.Add(type.SubjectId);
            }
        }

        return orderedTypeIds.Select(subjectId => typeMap[subjectId]).ToList();
    }

    private static IReadOnlyList<LinkedDependencyModel> CollectExternalDependencies(SemanticWorldModel semanticWorld)
    {
        var dependencies = new List<LinkedDependencyModel>();
        var seen = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in semanticWorld.Methods.OrderBy(candidate => candidate.MetadataToken))
        {
            foreach (var block in method.Body.Blocks)
            {
                foreach (var instruction in block.Instructions)
                {
                    var reference = instruction.Reference;
                    if (reference is null || string.Equals(reference.AssemblyName, semanticWorld.Assembly.Name, StringComparison.Ordinal))
                    {
                        continue;
                    }

                    var normalizedSubjectId = NormalizeDependencySubjectId(reference.SubjectId);
                    var key = $"{reference.SubjectKind}:{normalizedSubjectId}";
                    if (!seen.Add(key))
                    {
                        continue;
                    }

                    dependencies.Add(new LinkedDependencyModel
                    {
                        AssemblyName = reference.AssemblyName,
                        SubjectKind = reference.SubjectKind,
                        SubjectId = normalizedSubjectId,
                        Reason = ResolveDependencyReason(normalizedSubjectId),
                    });
                }
            }
        }

        return dependencies;
    }

    private static string NormalizeDependencySubjectId(string subjectId)
    {
        return subjectId switch
        {
            "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)"
                => "System.Private.CoreLib/System.String::Concat(System.String,System.String)",
            _ => subjectId,
        };
    }

    private static string ResolveDependencyReason(string subjectId)
    {
        return subjectId switch
        {
            "System.Private.CoreLib/System.Object::.ctor()" => "base-ctor",
            "System.Private.CoreLib/System.String::Concat(System.String,System.String)" => "narrow-concat-path",
            "System.Console/System.Console::WriteLine(System.String)" => "stdout-path",
            _ => throw new NotSupportedException($"unsupported external dependency in Stage 3 linker: {subjectId}"),
        };
    }
}
