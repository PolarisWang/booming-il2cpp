using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed class LinkerStage
{
    public string Name => "Linker";

    public LinkedWorldModel Link(SemanticWorldModel semanticWorld)
    {
        var reachableClosure = ComputeReachableClosure(semanticWorld);
        var orderedMethods = reachableClosure.Methods
            .OrderBy(method => string.Equals(method.SubjectId, semanticWorld.EntryPointSubjectId, StringComparison.Ordinal) ? 0 : 1)
            .ThenBy(method => method.MetadataToken)
            .ToList();
        var orderedTypes = OrderTypes(reachableClosure.Types, orderedMethods);
        var orderedFields = reachableClosure.Fields
            .OrderBy(field => field.MetadataToken)
            .ToList();
        var orderedProperties = reachableClosure.Properties
            .OrderBy(property => property.MetadataToken)
            .ToList();
        var dependencies = CollectExternalDependencies(semanticWorld, orderedMethods);
        var canonicalSubjects = FilterCanonicalSubjects(semanticWorld, orderedTypes, orderedFields, orderedProperties, orderedMethods);
        var semanticShapes = FilterSemanticShapes(semanticWorld, orderedTypes, orderedFields, orderedProperties, orderedMethods);
        var capabilityBundles = FilterCapabilityBundles(semanticWorld, orderedMethods);
        var optimizationFacts = BuildOptimizationFacts(semanticWorld, orderedTypes, orderedFields, orderedMethods);

        return new LinkedWorldModel
        {
            InputAssemblyPath = semanticWorld.InputAssemblyPath,
            Assembly = semanticWorld.Assembly,
            EntryPointSubjectId = semanticWorld.EntryPointSubjectId,
            Types = orderedTypes,
            Fields = orderedFields,
            Properties = orderedProperties,
            Methods = orderedMethods,
            Dependencies = dependencies,
            CanonicalSubjects = canonicalSubjects,
            SemanticShapes = semanticShapes,
            CapabilityBundles = capabilityBundles,
            OptimizationFacts = optimizationFacts,
        };
    }

    private static ReachableClosure ComputeReachableClosure(SemanticWorldModel semanticWorld)
    {
        var methodMap = semanticWorld.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var fieldMap = semanticWorld.Fields.ToDictionary(field => field.SubjectId, StringComparer.Ordinal);
        var propertyMap = semanticWorld.Properties.ToDictionary(property => property.SubjectId, StringComparer.Ordinal);
        var typeMap = semanticWorld.Types.ToDictionary(type => type.SubjectId, StringComparer.Ordinal);

        if (!methodMap.TryGetValue(semanticWorld.EntryPointSubjectId, out var entryPointMethod))
        {
            throw new InvalidOperationException(
                $"linker entry point '{semanticWorld.EntryPointSubjectId}' is missing from semantic world");
        }

        var reachableMethodIds = new HashSet<string>(StringComparer.Ordinal);
        var reachableFieldIds = new HashSet<string>(StringComparer.Ordinal);
        var reachablePropertyIds = new HashSet<string>(StringComparer.Ordinal);
        var reachableTypeIds = new HashSet<string>(StringComparer.Ordinal);
        var pendingMethods = new Queue<ManagedMethodModel>();
        pendingMethods.Enqueue(entryPointMethod);

        while (pendingMethods.Count > 0)
        {
            var method = pendingMethods.Dequeue();
            if (!reachableMethodIds.Add(method.SubjectId))
            {
                continue;
            }

            reachableTypeIds.Add(method.DeclaringTypeSubjectId);

            foreach (var block in method.Body.Blocks)
            {
                for (var instructionIndex = 0; instructionIndex < block.Instructions.Count; instructionIndex++)
                {
                    var instruction = block.Instructions[instructionIndex];
                    if (string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal) &&
                        !string.IsNullOrWhiteSpace(instruction.Callee))
                    {
                        var resolvedTargetSubjectId = TryResolveDevirtualizedCallTarget(
                            methodMap,
                            block.Instructions,
                            instructionIndex,
                            instruction.Callee!);
                        if (!string.IsNullOrWhiteSpace(resolvedTargetSubjectId) &&
                            methodMap.TryGetValue(resolvedTargetSubjectId, out var resolvedMethod))
                        {
                            pendingMethods.Enqueue(resolvedMethod);
                            reachableTypeIds.Add(resolvedMethod.DeclaringTypeSubjectId);
                        }
                    }

                    var reference = instruction.Reference;
                    if (reference is null ||
                        !string.Equals(reference.AssemblyName, semanticWorld.Assembly.Name, StringComparison.Ordinal))
                    {
                        continue;
                    }

                    switch (reference.SubjectKind)
                    {
                        case "method" when methodMap.TryGetValue(reference.SubjectId, out var referencedMethod):
                            pendingMethods.Enqueue(referencedMethod);
                            break;
                        case "field" when fieldMap.TryGetValue(reference.SubjectId, out var referencedField):
                            reachableFieldIds.Add(referencedField.SubjectId);
                            reachableTypeIds.Add(referencedField.DeclaringTypeSubjectId);
                            break;
                        case "property" when propertyMap.TryGetValue(reference.SubjectId, out var referencedProperty):
                            reachablePropertyIds.Add(referencedProperty.SubjectId);
                            reachableTypeIds.Add(referencedProperty.DeclaringTypeSubjectId);
                            break;
                        case "type" when typeMap.ContainsKey(reference.SubjectId):
                            reachableTypeIds.Add(reference.SubjectId);
                            break;
                    }
                }
            }
        }

        return new ReachableClosure(
            semanticWorld.Types.Where(type => reachableTypeIds.Contains(type.SubjectId)).ToList(),
            semanticWorld.Fields.Where(field => reachableFieldIds.Contains(field.SubjectId)).ToList(),
            semanticWorld.Properties.Where(property => reachablePropertyIds.Contains(property.SubjectId)).ToList(),
            semanticWorld.Methods.Where(method => reachableMethodIds.Contains(method.SubjectId)).ToList());
    }

    private static OptimizationFactsArtifact BuildOptimizationFacts(
        SemanticWorldModel semanticWorld,
        IReadOnlyList<ManagedTypeModel> orderedTypes,
        IReadOnlyList<ManagedFieldModel> orderedFields,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var methodMap = orderedMethods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var closedWorldSpecializations = orderedTypes
            .Select(type => new ClosedWorldSpecializationFact
            {
                SubjectKind = "type",
                SubjectId = type.SubjectId,
                Reason = "closed-world-type",
            })
            .Concat(
                orderedMethods.Select(method => new ClosedWorldSpecializationFact
                {
                    SubjectKind = "method",
                    SubjectId = method.SubjectId,
                    Reason = string.Equals(method.SubjectId, semanticWorld.EntryPointSubjectId, StringComparison.Ordinal)
                        ? "closed-world-entry"
                        : "closed-world-method",
                }))
            .ToList();

        var dispatchFacts = orderedMethods
            .SelectMany(method => method.Body.Blocks.SelectMany(block =>
                block.Instructions
                    .Select((instruction, index) => new { instruction, index, block.Instructions, method })
                    .Where(candidate =>
                        string.Equals(candidate.instruction.Op, "callvirt", StringComparison.Ordinal) &&
                        !string.IsNullOrEmpty(candidate.instruction.Callee))
                    .Select(candidate =>
                    {
                        var declaredTargetSubjectId = candidate.instruction.Callee!;
                        var resolvedTargetSubjectId = TryResolveDevirtualizedCallTarget(
                            methodMap,
                            candidate.Instructions,
                            candidate.index,
                            declaredTargetSubjectId) ?? declaredTargetSubjectId;
                        var devirtualized = !string.Equals(resolvedTargetSubjectId, declaredTargetSubjectId, StringComparison.Ordinal)
                            || declaredTargetSubjectId.StartsWith($"{semanticWorld.Assembly.Name}/", StringComparison.Ordinal);

                        return new DispatchFact
                        {
                            MethodSubjectId = method.SubjectId,
                            DispatchKind = devirtualized && !string.Equals(resolvedTargetSubjectId, declaredTargetSubjectId, StringComparison.Ordinal)
                                ? "callvirt.devirtualized-direct"
                                : "callvirt",
                            TargetSubjectId = resolvedTargetSubjectId,
                            Devirtualized = devirtualized,
                        };
                    })))
            .ToList();

        var layoutFacts = orderedFields
            .Select(field => new LayoutFact
            {
                SubjectKind = "field",
                SubjectId = field.SubjectId,
                DataKind = ResolveLayoutDataKind(field.FieldType),
                ElementType = TryGetArrayElementType(field.FieldType),
                RequiresBoxing = RequiresBoxing(field.FieldType),
            })
            .Concat(
                orderedMethods.SelectMany(
                    method => method.Parameters
                        .Select(
                            (parameter, index) => new { parameter, index, method })
                        .Where(candidate => candidate.parameter.Type.EndsWith("[]", StringComparison.Ordinal))
                        .Select(candidate => new LayoutFact
                        {
                            SubjectKind = "parameter",
                            SubjectId = ManagedNaming.CreateParameterSubjectId(candidate.method.SubjectId, candidate.index, candidate.parameter.Name),
                            DataKind = "array",
                            ElementType = TryGetArrayElementType(candidate.parameter.Type),
                            RequiresBoxing = false,
                        })))
            .Concat(
                orderedTypes
                    .Where(type => !string.Equals(type.SubjectId, type.DefinitionSubjectId, StringComparison.Ordinal))
                    .Select(type => new LayoutFact
                    {
                        SubjectKind = "type",
                        SubjectId = type.SubjectId,
                        DataKind = "closed-generic-instance",
                        ElementType = type.DefinitionSubjectId,
                        RequiresBoxing = false,
                    }))
            .ToList();

        return new OptimizationFactsArtifact
        {
            ClosedWorldSpecializations = closedWorldSpecializations,
            DispatchFacts = dispatchFacts,
            LayoutFacts = layoutFacts,
            ExceptionFacts = [],
        };
    }

    private static string? TryResolveDevirtualizedCallTarget(
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        IReadOnlyList<ManagedInstructionModel> instructions,
        int instructionIndex,
        string declaredTargetSubjectId)
    {
        if (instructionIndex <= 0) {
            return null;
        }

        var precedingInstruction = instructions[instructionIndex - 1];
        if (!string.Equals(precedingInstruction.Op, "newobj", StringComparison.Ordinal) ||
            string.IsNullOrWhiteSpace(precedingInstruction.Callee)) {
            return null;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(precedingInstruction.Callee);
        var declaredTypeSubjectId = GetDeclaringTypeSubjectId(declaredTargetSubjectId);
        if (string.Equals(constructorTypeSubjectId, declaredTypeSubjectId, StringComparison.Ordinal)) {
            return declaredTargetSubjectId;
        }

        if (!methodMap.TryGetValue(declaredTargetSubjectId, out var declaredMethod)) {
            return null;
        }

        var exactMatch = methodMap.Values.FirstOrDefault(candidate =>
            string.Equals(candidate.DeclaringTypeSubjectId, constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(candidate.Name, declaredMethod.Name, StringComparison.Ordinal) &&
            candidate.Body.Blocks.Any(block => block.Instructions.Count > 0) &&
            candidate.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                declaredMethod.Parameters.Select(parameter => parameter.Type),
                StringComparer.Ordinal))
            ?.SubjectId;
        if (!string.IsNullOrWhiteSpace(exactMatch))
        {
            return exactMatch;
        }

        return methodMap.Values.FirstOrDefault(candidate =>
            string.Equals(candidate.DeclaringTypeSubjectId, constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(candidate.Name, declaredMethod.Name, StringComparison.Ordinal) &&
            candidate.Body.Blocks.Any(block => block.Instructions.Count > 0))
            ?.SubjectId;
    }

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
                    if (reference is null || string.Equals(reference.AssemblyName, semanticWorld.Assembly.Name, StringComparison.Ordinal))
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
        return subjectId switch
        {
            "System.Private.CoreLib/System.Object::.ctor()" => "base-ctor",
            "System.Private.CoreLib/System.Int32" => "boxed-value-type",
            "System.Private.CoreLib/System.String::Concat(System.String,System.String)" => "narrow-concat-path",
            "System.Console/System.Console::WriteLine(System.String)" => "stdout-path",
            "System.Private.CoreLib/System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetField(System.String)" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetProperty(System.String)" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetMethod(System.String)" => "reflection-query",
            "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()" => "reflection-query",
            "System.Private.CoreLib/System.Type::GetGenericTypeDefinition()" => "reflection-query",
            _ => throw new NotSupportedException($"unsupported external dependency in Stage 3 linker: {subjectId}"),
        };
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

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract declaring type from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
    }
}

internal sealed record ReachableClosure(
    IReadOnlyList<ManagedTypeModel> Types,
    IReadOnlyList<ManagedFieldModel> Fields,
    IReadOnlyList<ManagedPropertyModel> Properties,
    IReadOnlyList<ManagedMethodModel> Methods);
