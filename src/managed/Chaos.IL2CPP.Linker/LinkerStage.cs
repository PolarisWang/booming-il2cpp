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
        var preserveDescriptor = BuildPreserveDescriptor(orderedTypes, orderedFields, orderedProperties, orderedMethods);

        return new LinkedWorldModel
        {
            InputAssemblyPath = semanticWorld.InputAssemblyPath,
            Assembly = semanticWorld.Assembly,
            Assemblies = semanticWorld.Assemblies,
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
            PreserveDescriptor = preserveDescriptor,
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

        ExpandReachableMethods(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
            pendingMethods,
            reachableMethodIds,
            reachableFieldIds,
            reachablePropertyIds,
            reachableTypeIds);
        IncludeThreadStaticFieldClosure(
            semanticWorld,
            reachableMethodIds,
            reachableFieldIds,
            reachableTypeIds);
        IncludeReflectionQuerySurface(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
            reachableMethodIds,
            reachableFieldIds,
            reachablePropertyIds,
            reachableTypeIds);
        IncludeAsyncStateMachineClosure(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
            reachableMethodIds,
            reachableFieldIds,
            reachablePropertyIds,
            reachableTypeIds);
        IncludePreservedClosure(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
            reachableMethodIds,
            reachableFieldIds,
            reachablePropertyIds,
            reachableTypeIds);
        IncludeGenericTypeDefinitions(reachableTypeIds, typeMap);

        return new ReachableClosure(
            semanticWorld.Types.Where(type => reachableTypeIds.Contains(type.SubjectId)).ToList(),
            semanticWorld.Fields.Where(field => reachableFieldIds.Contains(field.SubjectId)).ToList(),
            semanticWorld.Properties.Where(property => reachablePropertyIds.Contains(property.SubjectId)).ToList(),
            semanticWorld.Methods.Where(method => reachableMethodIds.Contains(method.SubjectId)).ToList());
    }

    private static void IncludePreservedClosure(
        SemanticWorldModel semanticWorld,
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        IReadOnlyDictionary<string, ManagedFieldModel> fieldMap,
        IReadOnlyDictionary<string, ManagedPropertyModel> propertyMap,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap,
        HashSet<string> reachableMethodIds,
        HashSet<string> reachableFieldIds,
        HashSet<string> reachablePropertyIds,
        HashSet<string> reachableTypeIds)
    {
        var pendingMethods = new Queue<ManagedMethodModel>();

        foreach (var type in semanticWorld.Types.Where(candidate => candidate.IsPreserved))
        {
            reachableTypeIds.Add(type.SubjectId);

            foreach (var field in semanticWorld.Fields.Where(candidate =>
                         string.Equals(candidate.DeclaringTypeSubjectId, type.SubjectId, StringComparison.Ordinal)))
            {
                reachableFieldIds.Add(field.SubjectId);
            }

            foreach (var property in semanticWorld.Properties.Where(candidate =>
                         string.Equals(candidate.DeclaringTypeSubjectId, type.SubjectId, StringComparison.Ordinal)))
            {
                reachablePropertyIds.Add(property.SubjectId);
            }

            foreach (var method in semanticWorld.Methods.Where(candidate =>
                         string.Equals(candidate.DeclaringTypeSubjectId, type.SubjectId, StringComparison.Ordinal) &&
                         !reachableMethodIds.Contains(candidate.SubjectId)))
            {
                pendingMethods.Enqueue(method);
            }
        }

        foreach (var field in semanticWorld.Fields.Where(candidate => candidate.IsPreserved))
        {
            reachableFieldIds.Add(field.SubjectId);
            reachableTypeIds.Add(field.DeclaringTypeSubjectId);
        }

        foreach (var property in semanticWorld.Properties.Where(candidate => candidate.IsPreserved))
        {
            reachablePropertyIds.Add(property.SubjectId);
            reachableTypeIds.Add(property.DeclaringTypeSubjectId);
        }

        foreach (var method in semanticWorld.Methods.Where(candidate => candidate.IsPreserved))
        {
            reachableTypeIds.Add(method.DeclaringTypeSubjectId);
            if (!reachableMethodIds.Contains(method.SubjectId))
            {
                pendingMethods.Enqueue(method);
            }
        }

        ExpandReachableMethods(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
            pendingMethods,
            reachableMethodIds,
            reachableFieldIds,
            reachablePropertyIds,
            reachableTypeIds);
    }

    private static void ExpandReachableMethods(
        SemanticWorldModel semanticWorld,
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        IReadOnlyDictionary<string, ManagedFieldModel> fieldMap,
        IReadOnlyDictionary<string, ManagedPropertyModel> propertyMap,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap,
        Queue<ManagedMethodModel> pendingMethods,
        HashSet<string> reachableMethodIds,
        HashSet<string> reachableFieldIds,
        HashSet<string> reachablePropertyIds,
        HashSet<string> reachableTypeIds)
    {
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
                        !IsInternalAssembly(semanticWorld, reference.AssemblyName))
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
    }

    private static void IncludeGenericTypeDefinitions(
        HashSet<string> reachableTypeIds,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        var pendingTypeIds = new Queue<string>(reachableTypeIds);

        while (pendingTypeIds.Count > 0)
        {
            var subjectId = pendingTypeIds.Dequeue();
            if (!typeMap.TryGetValue(subjectId, out var type) ||
                string.Equals(type.SubjectId, type.DefinitionSubjectId, StringComparison.Ordinal) ||
                !typeMap.ContainsKey(type.DefinitionSubjectId))
            {
                continue;
            }

            if (reachableTypeIds.Add(type.DefinitionSubjectId))
            {
                pendingTypeIds.Enqueue(type.DefinitionSubjectId);
            }
        }
    }

    private static void IncludeThreadStaticFieldClosure(
        SemanticWorldModel semanticWorld,
        IReadOnlySet<string> reachableMethodIds,
        HashSet<string> reachableFieldIds,
        HashSet<string> reachableTypeIds)
    {
        var methodCapabilities = semanticWorld.CapabilityBundles.Methods
            .ToDictionary(bundle => bundle.SubjectId, bundle => bundle.Capabilities, StringComparer.Ordinal);
        var requiresThreadStaticClosure = reachableMethodIds.Any(subjectId =>
            methodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            capabilities.Contains("requires-thread-static-storage", StringComparer.Ordinal));
        if (!requiresThreadStaticClosure)
        {
            return;
        }

        foreach (var field in semanticWorld.Fields.Where(candidate => candidate.IsThreadStatic))
        {
            reachableFieldIds.Add(field.SubjectId);
            reachableTypeIds.Add(field.DeclaringTypeSubjectId);
        }
    }

    private static void IncludeAsyncStateMachineClosure(
        SemanticWorldModel semanticWorld,
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        IReadOnlyDictionary<string, ManagedFieldModel> fieldMap,
        IReadOnlyDictionary<string, ManagedPropertyModel> propertyMap,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap,
        HashSet<string> reachableMethodIds,
        HashSet<string> reachableFieldIds,
        HashSet<string> reachablePropertyIds,
        HashSet<string> reachableTypeIds)
    {
        var methodCapabilities = semanticWorld.CapabilityBundles.Methods
            .ToDictionary(bundle => bundle.SubjectId, bundle => bundle.Capabilities, StringComparer.Ordinal);
        var requiresAsyncClosure = reachableMethodIds.Any(subjectId =>
            methodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            (capabilities.Contains("requires-async-state-machine", StringComparer.Ordinal) ||
             capabilities.Contains("requires-task-awaiter", StringComparer.Ordinal)));
        if (!requiresAsyncClosure)
        {
            return;
        }

        var asyncTypeIds = semanticWorld.Types
            .Where(IsCompilerGeneratedAsyncStateMachine)
            .Select(type => type.SubjectId)
            .ToHashSet(StringComparer.Ordinal);
        if (asyncTypeIds.Count == 0)
        {
            return;
        }

        foreach (var asyncTypeId in asyncTypeIds)
        {
            reachableTypeIds.Add(asyncTypeId);
        }

        foreach (var field in semanticWorld.Fields.Where(candidate => asyncTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            reachableFieldIds.Add(field.SubjectId);
        }

        foreach (var property in semanticWorld.Properties.Where(candidate => asyncTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            reachablePropertyIds.Add(property.SubjectId);
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var method in semanticWorld.Methods.Where(candidate => asyncTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            if (!reachableMethodIds.Contains(method.SubjectId))
            {
                pendingMethods.Enqueue(method);
            }
        }

        ExpandReachableMethods(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
            pendingMethods,
            reachableMethodIds,
            reachableFieldIds,
            reachablePropertyIds,
            reachableTypeIds);
    }

    private static void IncludeReflectionQuerySurface(
        SemanticWorldModel semanticWorld,
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        IReadOnlyDictionary<string, ManagedFieldModel> fieldMap,
        IReadOnlyDictionary<string, ManagedPropertyModel> propertyMap,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap,
        HashSet<string> reachableMethodIds,
        HashSet<string> reachableFieldIds,
        HashSet<string> reachablePropertyIds,
        HashSet<string> reachableTypeIds)
    {
        var methodCapabilities = semanticWorld.CapabilityBundles.Methods
            .ToDictionary(bundle => bundle.SubjectId, bundle => bundle.Capabilities, StringComparer.Ordinal);
        var requiresReflectionQuerySurface = reachableMethodIds.Any(subjectId =>
            methodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            (capabilities.Contains("requires-closed-type-member-query", StringComparer.Ordinal) ||
             capabilities.Contains("requires-generic-type-definition-query", StringComparer.Ordinal)));
        if (!requiresReflectionQuerySurface)
        {
            return;
        }

        var targetTypeIds = CollectReflectionQueryTargetTypeIds(semanticWorld, reachableMethodIds, typeMap);
        if (targetTypeIds.Count == 0)
        {
            return;
        }

        foreach (var targetTypeId in targetTypeIds)
        {
            reachableTypeIds.Add(targetTypeId);
        }

        foreach (var field in semanticWorld.Fields.Where(candidate => targetTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            reachableFieldIds.Add(field.SubjectId);
        }

        foreach (var property in semanticWorld.Properties.Where(candidate => targetTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            reachablePropertyIds.Add(property.SubjectId);
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var method in semanticWorld.Methods.Where(candidate => targetTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            if (!reachableMethodIds.Contains(method.SubjectId))
            {
                pendingMethods.Enqueue(method);
            }
        }

        ExpandReachableMethods(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
            pendingMethods,
            reachableMethodIds,
            reachableFieldIds,
            reachablePropertyIds,
            reachableTypeIds);
    }

    private static HashSet<string> CollectReflectionQueryTargetTypeIds(
        SemanticWorldModel semanticWorld,
        IReadOnlySet<string> reachableMethodIds,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        var targetTypeIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in semanticWorld.Methods)
        {
            if (!reachableMethodIds.Contains(method.SubjectId))
            {
                continue;
            }

            foreach (var instruction in method.Body.Blocks.SelectMany(block => block.Instructions))
            {
                var reference = instruction.Reference;
                if (!string.Equals(instruction.Op, "ldtoken", StringComparison.Ordinal) ||
                    reference is null ||
                    !IsInternalAssembly(semanticWorld, reference.AssemblyName) ||
                    !string.Equals(reference.SubjectKind, "type", StringComparison.Ordinal) ||
                    !typeMap.ContainsKey(reference.SubjectId))
                {
                    continue;
                }

                targetTypeIds.Add(reference.SubjectId);
            }
        }

        return targetTypeIds;
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
                Reason = IsCompilerGeneratedAsyncStateMachine(type)
                    ? "compiler-generated-async-state-machine"
                    : "closed-world-type",
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
                        var interfaceDispatch = IsInterfaceDispatchTarget(semanticWorld.Types, declaredTargetSubjectId);
                        var devirtualized = !string.Equals(resolvedTargetSubjectId, declaredTargetSubjectId, StringComparison.Ordinal)
                            || IsInternalSubjectId(semanticWorld, declaredTargetSubjectId);

                        return new DispatchFact
                        {
                            MethodSubjectId = method.SubjectId,
                            DispatchKind = interfaceDispatch
                                ? "callvirt.interface-runtime-helper"
                                : devirtualized && !string.Equals(resolvedTargetSubjectId, declaredTargetSubjectId, StringComparison.Ordinal)
                                    ? "callvirt.devirtualized-direct"
                                    : "callvirt",
                            TargetSubjectId = resolvedTargetSubjectId,
                            Devirtualized = devirtualized,
                        };
                    })))
            .Concat(BuildDelegateDispatchFacts(orderedMethods))
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
            ExceptionFacts = BuildExceptionFacts(orderedMethods),
        };
    }

    private static PreserveDescriptorArtifact BuildPreserveDescriptor(
        IReadOnlyList<ManagedTypeModel> orderedTypes,
        IReadOnlyList<ManagedFieldModel> orderedFields,
        IReadOnlyList<ManagedPropertyModel> orderedProperties,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var entries = orderedTypes
            .Where(type => type.IsPreserved)
            .Select(type => new PreserveDescriptorEntry
            {
                SubjectKind = "type",
                SubjectId = type.SubjectId,
                Preserve = "all",
                Reason = "preserve-attribute",
            })
            .Concat(
                orderedFields
                    .Where(field => field.IsPreserved)
                    .Select(field => new PreserveDescriptorEntry
                    {
                        SubjectKind = "field",
                        SubjectId = field.SubjectId,
                        Preserve = "members",
                        Reason = "preserve-attribute",
                    }))
            .Concat(
                orderedProperties
                    .Where(property => property.IsPreserved)
                    .Select(property => new PreserveDescriptorEntry
                    {
                        SubjectKind = "property",
                        SubjectId = property.SubjectId,
                        Preserve = "members",
                        Reason = "preserve-attribute",
                    }))
            .Concat(
                orderedMethods
                    .Where(method => method.IsPreserved)
                    .Select(method => new PreserveDescriptorEntry
                    {
                        SubjectKind = "method",
                        SubjectId = method.SubjectId,
                        Preserve = "signature",
                        Reason = "preserve-attribute",
                    }))
            .ToList();

        return new PreserveDescriptorArtifact
        {
            Entries = entries,
        };
    }

    private static IReadOnlyList<DispatchFact> BuildDelegateDispatchFacts(
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        return orderedMethods
            .SelectMany(method => method.Body.Blocks.SelectMany(block =>
                block.Instructions
                    .Where(instruction =>
                        (string.Equals(instruction.Op, "ldftn", StringComparison.Ordinal) ||
                         string.Equals(instruction.Op, "ldvirtftn", StringComparison.Ordinal)) &&
                        !string.IsNullOrWhiteSpace(instruction.Callee))
                    .Select(instruction => new DispatchFact
                    {
                        MethodSubjectId = method.SubjectId,
                        DispatchKind = string.Equals(instruction.Op, "ldftn", StringComparison.Ordinal)
                            ? "delegate.exact-target-direct"
                            : "delegate.runtime-helper-fallback",
                        TargetSubjectId = instruction.Callee!,
                        Devirtualized = string.Equals(instruction.Op, "ldftn", StringComparison.Ordinal),
                    })))
            .ToList();
    }

    private static IReadOnlyList<ExceptionFact> BuildExceptionFacts(
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        return orderedMethods
            .SelectMany(method =>
            {
                _ = HasThrowCatchFinallyShape(method) ? "throw-catch-finally" : null;
                _ = HasNestedExceptionHandlerShape(method) ? "nested-throw-catch-finally" : null;
                _ = HasUtf8StringMarshalShape(method) ? "utf8-string-marshal" : null;
                _ = HasUnmanagedExportShape(method) ? "unmanaged-callers-only-export" : null;
                return method.Body.ExceptionRegions.Select(region => new ExceptionFact
                {
                    MethodSubjectId = method.SubjectId,
                    HandlingKind = region.HandlingKind,
                    CatchTypeSubjectId = region.CatchTypeSubjectId,
                });
            })
            .ToList();
    }

    private static bool HasThrowCatchFinallyShape(ManagedMethodModel method)
    {
        var instructions = method.Body.Blocks.SelectMany(block => block.Instructions);
        return instructions.Any(instruction => string.Equals(instruction.Op, "throw", StringComparison.Ordinal)) &&
               method.Body.ExceptionRegions.Any(region => string.Equals(region.HandlingKind, "catch", StringComparison.Ordinal)) &&
               method.Body.ExceptionRegions.Any(region => string.Equals(region.HandlingKind, "finally", StringComparison.Ordinal));
    }

    private static bool HasNestedExceptionHandlerShape(ManagedMethodModel method)
    {
        var exceptionRegions = method.Body.ExceptionRegions;
        for (var outerIndex = 0; outerIndex < exceptionRegions.Count; outerIndex++)
        {
            var outerRegion = exceptionRegions[outerIndex];
            var outerTryEnd = outerRegion.TryOffset + outerRegion.TryLength;

            for (var innerIndex = 0; innerIndex < exceptionRegions.Count; innerIndex++)
            {
                if (outerIndex == innerIndex)
                {
                    continue;
                }

                var innerRegion = exceptionRegions[innerIndex];
                var innerTryEnd = innerRegion.TryOffset + innerRegion.TryLength;
                if (outerRegion.TryOffset <= innerRegion.TryOffset &&
                    innerTryEnd <= outerTryEnd &&
                    (outerRegion.TryOffset != innerRegion.TryOffset || outerTryEnd != innerTryEnd))
                {
                    return true;
                }
            }
        }

        return false;
    }

    private static bool HasUtf8StringMarshalShape(ManagedMethodModel method)
    {
        return method.Body.Blocks
            .SelectMany(block => block.Instructions)
            .Any(instruction =>
                string.Equals(instruction.Callee, "System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8(System.String)", StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, "System.Runtime.InteropServices.Marshal::PtrToStringUTF8(System.IntPtr)", StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, "System.Runtime.InteropServices.Marshal::FreeCoTaskMem(System.IntPtr)", StringComparison.Ordinal));
    }

    private static bool HasUnmanagedExportShape(ManagedMethodModel method)
    {
        return method.IsUnmanagedCallersOnly;
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

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract declaring type from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
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

internal sealed record ReachableClosure(
    IReadOnlyList<ManagedTypeModel> Types,
    IReadOnlyList<ManagedFieldModel> Fields,
    IReadOnlyList<ManagedPropertyModel> Properties,
    IReadOnlyList<ManagedMethodModel> Methods);
