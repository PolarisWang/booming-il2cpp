using System.Globalization;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed partial class LinkerStage
{
    private static IReadOnlyList<ManagedTypeModel> OrderTypes(
        IReadOnlyList<ManagedTypeModel> reachableTypes,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var typeMap = reachableTypes.ToDictionary(type => type.SubjectId, StringComparer.Ordinal);
        var orderedTypeIds = new List<string>();

        foreach (var method in orderedMethods)
        {
            if (!orderedTypeIds.Contains(method.DeclaringTypeSubjectId, StringComparer.Ordinal))
            {
                orderedTypeIds.Add(method.DeclaringTypeSubjectId);
            }
        }

        foreach (var type in reachableTypes.OrderBy(candidate => candidate.MetadataToken))
        {
            if (!orderedTypeIds.Contains(type.SubjectId, StringComparer.Ordinal))
            {
                orderedTypeIds.Add(type.SubjectId);
            }
        }

        return orderedTypeIds.Select(subjectId => typeMap[subjectId]).ToList();
    }

    private static IReadOnlyList<LinkedDependencyModel> CollectExternalDependencies(
        SemanticWorldModel semanticWorld,
        IReadOnlyList<ManagedMethodModel> reachableMethods)
    {
        var canonicalSubjectIds = semanticWorld.CanonicalSubjects.Subjects
            .GroupBy(subject => subject.SubjectId, StringComparer.Ordinal)
            .ToDictionary(
                group => group.Key,
                group => group.Last().CanonicalSubjectId,
                StringComparer.Ordinal);
        var dependencies = new List<LinkedDependencyModel>();
        var seen = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in reachableMethods.OrderBy(candidate => candidate.MetadataToken))
        {
            foreach (var block in method.Body.Blocks)
            {
                foreach (var instruction in block.Instructions)
                {
                    var reference = instruction.Reference;
                    if (reference is null || IsInternalAssembly(semanticWorld, reference.AssemblyName))
                    {
                        continue;
                    }

                    var canonicalSubjectId = ResolveCanonicalSubjectId(canonicalSubjectIds, reference.SubjectId);
                    var key = $"{reference.SubjectKind}:{canonicalSubjectId}";
                    if (!seen.Add(key))
                    {
                        continue;
                    }

                    dependencies.Add(new LinkedDependencyModel
                    {
                        AssemblyName = reference.AssemblyName,
                        SubjectKind = reference.SubjectKind,
                        SubjectId = canonicalSubjectId,
                        Reason = ResolveDependencyReason(canonicalSubjectId),
                    });
                }
            }
        }

        return dependencies;
    }

    private static CanonicalSubjectsModel FilterCanonicalSubjects(
        SemanticWorldModel semanticWorld,
        IReadOnlyList<ManagedTypeModel> orderedTypes,
        IReadOnlyList<ManagedFieldModel> orderedFields,
        IReadOnlyList<ManagedPropertyModel> orderedProperties,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var reachableSubjectIds = new HashSet<string>(
            orderedTypes.Select(type => type.SubjectId)
                .Concat(orderedFields.Select(field => field.SubjectId))
                .Concat(orderedProperties.Select(property => property.SubjectId))
                .Concat(orderedMethods.Select(method => method.SubjectId)),
            StringComparer.Ordinal);

        return new CanonicalSubjectsModel
        {
            Subjects = semanticWorld.CanonicalSubjects.Subjects
                .Where(subject => reachableSubjectIds.Contains(subject.SubjectId))
                .ToList(),
        };
    }

    private static SemanticShapesModel FilterSemanticShapes(
        SemanticWorldModel semanticWorld,
        IReadOnlyList<ManagedTypeModel> orderedTypes,
        IReadOnlyList<ManagedFieldModel> orderedFields,
        IReadOnlyList<ManagedPropertyModel> orderedProperties,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var reachableTypeIds = orderedTypes.Select(type => type.SubjectId).ToHashSet(StringComparer.Ordinal);
        var reachableFieldIds = orderedFields.Select(field => field.SubjectId).ToHashSet(StringComparer.Ordinal);
        var reachablePropertyIds = orderedProperties.Select(property => property.SubjectId).ToHashSet(StringComparer.Ordinal);
        var reachableMethodIds = orderedMethods.Select(method => method.SubjectId).ToHashSet(StringComparer.Ordinal);

        return new SemanticShapesModel
        {
            Types = semanticWorld.SemanticShapes.Types
                .Where(shape => reachableTypeIds.Contains(shape.SubjectId))
                .ToList(),
            Fields = semanticWorld.SemanticShapes.Fields
                .Where(shape => reachableFieldIds.Contains(shape.SubjectId))
                .ToList(),
            Properties = semanticWorld.SemanticShapes.Properties
                .Where(shape => reachablePropertyIds.Contains(shape.SubjectId))
                .ToList(),
            Methods = semanticWorld.SemanticShapes.Methods
                .Where(shape => reachableMethodIds.Contains(shape.SubjectId))
                .ToList(),
        };
    }

    private static CapabilityBundlesModel FilterCapabilityBundles(
        SemanticWorldModel semanticWorld,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var reachableMethodIds = orderedMethods.Select(method => method.SubjectId).ToHashSet(StringComparer.Ordinal);
        var methodBundles = semanticWorld.CapabilityBundles.Methods
            .Where(bundle => reachableMethodIds.Contains(bundle.SubjectId))
            .ToList();

        return new CapabilityBundlesModel
        {
            World = new WorldCapabilityBundleModel
            {
                Capabilities = methodBundles
                    .SelectMany(bundle => bundle.Capabilities)
                    .Distinct(StringComparer.Ordinal)
                    .OrderBy(capability => capability, StringComparer.Ordinal)
                    .ToList(),
            },
            Methods = methodBundles,
        };
    }

    private static string ResolveCanonicalSubjectId(
        IReadOnlyDictionary<string, string> canonicalSubjectIds,
        string subjectId)
    {
        return canonicalSubjectIds.TryGetValue(subjectId, out var canonicalSubjectId)
            ? canonicalSubjectId
            : subjectId;
    }

    private static string ResolveDependencyReason(string subjectId)
    {
        if (IsMonitorEnterExitSubjectId(subjectId))
        {
            return "monitor-enter-exit";
        }

        return subjectId switch
        {
            "System.Private.CoreLib/System.Object::.ctor()" => "base-ctor",
            "System.Private.CoreLib/System.Int32" => "boxed-value-type",
            "System.Private.CoreLib/System.String::Concat(System.String,System.String)" => "narrow-concat-path",
            "System.Private.CoreLib/System.InvalidOperationException::.ctor(System.String)" => "managed-exception-construction",
            "System.Console/System.Console::WriteLine(System.String)" => "stdout-path",
            "System.Private.CoreLib/System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetField(System.String)" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetProperty(System.String)" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetMethod(System.String)" => "reflection-query",
            "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetGenericTypeDefinition()" => "reflection-query",
            _ => "external-call",
        };
    }

    private static bool IsMonitorEnterExitSubjectId(string subjectId)
    {
        return subjectId.Contains("System.Threading.Monitor::Enter(", StringComparison.Ordinal) ||
               subjectId.Contains("System.Threading.Monitor::Exit(", StringComparison.Ordinal);
    }

    private static string ResolveLayoutDataKind(string typeName)
    {
        if (typeName.EndsWith("[]", StringComparison.Ordinal))
        {
            return "array";
        }

        return RequiresBoxing(typeName) ? "value" : "reference";
    }

    private static string? TryGetArrayElementType(string typeName)
    {
        return typeName.EndsWith("[]", StringComparison.Ordinal)
            ? typeName[..^2]
            : null;
    }

    private static bool RequiresBoxing(string typeName)
    {
        return typeName switch
        {
            "System.Boolean" => true,
            "System.Byte" => true,
            "System.Char" => true,
            "System.Double" => true,
            "System.Int16" => true,
            "System.Int32" => true,
            "System.Int64" => true,
            "System.Single" => true,
            "System.UInt16" => true,
            "System.UInt32" => true,
            "System.UInt64" => true,
            _ => false,
        };
    }

    private static bool TryResolveMethod(
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        string subjectIdOrDefinitionSubjectId,
        out ManagedMethodModel method)
    {
        if (methodMap.TryGetValue(subjectIdOrDefinitionSubjectId, out var resolvedMethod))
        {
            method = resolvedMethod;
            return true;
        }

        var definitionMatches = methodMap.Values
            .Where(candidate => string.Equals(candidate.DefinitionSubjectId, subjectIdOrDefinitionSubjectId, StringComparison.Ordinal))
            .Distinct()
            .ToArray();
        if (definitionMatches.Length == 1)
        {
            method = definitionMatches[0];
            return true;
        }

        if (!subjectIdOrDefinitionSubjectId.Contains("::", StringComparison.Ordinal))
        {
            method = null!;
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectIdOrDefinitionSubjectId);
        var signatureSuffix = GetMethodSignatureSuffix(subjectIdOrDefinitionSubjectId);
        var signatureMatches = methodMap.Values
            .Where(candidate =>
                string.Equals(candidate.DeclaringTypeSubjectId, declaringTypeSubjectId, StringComparison.Ordinal) &&
                string.Equals(GetMethodSignatureSuffix(candidate.SubjectId), signatureSuffix, StringComparison.Ordinal))
            .Distinct()
            .ToArray();
        if (signatureMatches.Length == 1)
        {
            method = signatureMatches[0];
            return true;
        }

        method = null!;
        return false;
    }

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract declaring type from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
    }

    private static string GetMethodSignatureSuffix(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0 || separatorIndex + 2 >= subjectId.Length)
        {
            throw new InvalidOperationException($"failed to extract method signature from subject id '{subjectId}'");
        }

        return subjectId[(separatorIndex + 2)..];
    }

    private static bool IsInterfaceDispatchTarget(
        IReadOnlyList<ManagedTypeModel> types,
        string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        return types.Any(type =>
            string.Equals(type.SubjectId, declaringTypeSubjectId, StringComparison.Ordinal) &&
            type.IsInterface);
    }

    private static bool IsCompilerGeneratedAsyncStateMachine(ManagedTypeModel type)
    {
        return type.Name.Contains("d__", StringComparison.Ordinal);
    }

    private static bool IsCompilerGeneratedIteratorStateMachine(ManagedTypeModel type)
    {
        if (!type.Name.Contains("d__", StringComparison.Ordinal) ||
            type.ImplementedInterfaceSubjectIds is null ||
            type.ImplementedInterfaceSubjectIds.Count == 0)
        {
            return false;
        }

        var implementsEnumerable = type.ImplementedInterfaceSubjectIds.Any(interfaceSubjectId =>
            interfaceSubjectId.Contains("System.Collections.Generic.IEnumerable<", StringComparison.Ordinal));
        var implementsEnumerator = type.ImplementedInterfaceSubjectIds.Any(interfaceSubjectId =>
            interfaceSubjectId.Contains("System.Collections.Generic.IEnumerator<", StringComparison.Ordinal));
        return implementsEnumerable && implementsEnumerator;
    }

    private static bool IsInternalAssembly(SemanticWorldModel semanticWorld, string assemblyName)
    {
        return semanticWorld.Assemblies.Any(assembly =>
            string.Equals(assembly.Name, assemblyName, StringComparison.Ordinal));
    }

    private static bool IsInternalSubjectId(SemanticWorldModel semanticWorld, string subjectId)
    {
        var separatorIndex = subjectId.IndexOf('/', StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            return false;
        }

        return IsInternalAssembly(semanticWorld, subjectId[..separatorIndex]);
    }
}
