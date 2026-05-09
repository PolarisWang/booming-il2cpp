using System.Globalization;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed partial class LinkerStage
{
    private sealed record ReachabilityContext(
        SemanticWorldModel SemanticWorld,
        IReadOnlyDictionary<string, ManagedMethodModel> MethodMap,
        IReadOnlyDictionary<string, ManagedFieldModel> FieldMap,
        IReadOnlyDictionary<string, ManagedPropertyModel> PropertyMap,
        IReadOnlyDictionary<string, ManagedTypeModel> TypeMap,
        IReadOnlyDictionary<string, IReadOnlyList<string>> MethodCapabilities,
        HashSet<string> ReachableMethodIds,
        HashSet<string> ReachableFieldIds,
        HashSet<string> ReachablePropertyIds,
        HashSet<string> ReachableTypeIds);

    private static ReachableClosure ComputeReachableClosure(SemanticWorldModel semanticWorld)
    {
        if (semanticWorld.FullAssemblyClosure)
        {
            return new ReachableClosure(
                semanticWorld.Types,
                semanticWorld.Fields,
                semanticWorld.Properties,
                semanticWorld.Methods);
        }

        var methodMap = semanticWorld.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var fieldMap = semanticWorld.Fields.ToDictionary(field => field.SubjectId, StringComparer.Ordinal);
        var propertyMap = semanticWorld.Properties.ToDictionary(property => property.SubjectId, StringComparer.Ordinal);
        var typeMap = semanticWorld.Types.ToDictionary(type => type.SubjectId, StringComparer.Ordinal);
        var methodCapabilities = semanticWorld.CapabilityBundles.Methods
            .ToDictionary(bundle => bundle.SubjectId, bundle => bundle.Capabilities, StringComparer.Ordinal);

        ManagedMethodModel? entryPointMethod = null;
        if (!string.IsNullOrWhiteSpace(semanticWorld.EntryPointSubjectId) &&
            !methodMap.TryGetValue(semanticWorld.EntryPointSubjectId, out entryPointMethod))
        {
            throw new InvalidOperationException(
                $"linker entry point '{semanticWorld.EntryPointSubjectId}' is missing from semantic world");
        }

        var ctx = new ReachabilityContext(
            semanticWorld, methodMap, fieldMap, propertyMap, typeMap, methodCapabilities,
            [], [], [], []);

        var pendingMethods = new Queue<ManagedMethodModel>();
        if (entryPointMethod is not null)
        {
            pendingMethods.Enqueue(entryPointMethod);
        }

        ExpandReachableMethods(ctx, pendingMethods);
        IncludeFullAssemblyClosure(ctx);
        IncludeThreadStaticFieldClosure(ctx);
        IncludeReflectionQuerySurface(ctx);
        IncludeAsyncStateMachineClosure(ctx);
        IncludeIteratorStateMachineClosure(ctx);
        IncludePreservedClosure(ctx);
        IncludePolymorphicDispatchImplementationClosure(ctx);
        IncludeGenericTypeDefinitions(ctx.ReachableTypeIds, typeMap);

        return new ReachableClosure(
            semanticWorld.Types.Where(type => ctx.ReachableTypeIds.Contains(type.SubjectId)).ToList(),
            semanticWorld.Fields.Where(field => ctx.ReachableFieldIds.Contains(field.SubjectId)).ToList(),
            semanticWorld.Properties.Where(property => ctx.ReachablePropertyIds.Contains(property.SubjectId)).ToList(),
            semanticWorld.Methods.Where(method => ctx.ReachableMethodIds.Contains(method.SubjectId)).ToList());
    }

    private static void IncludeFullAssemblyClosure(ReachabilityContext ctx)
    {
        if (!ctx.SemanticWorld.FullAssemblyClosure)
        {
            return;
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var type in ctx.SemanticWorld.Types.Where(candidate =>
                     string.Equals(candidate.AssemblyName, ctx.SemanticWorld.Assembly.Name, StringComparison.Ordinal)))
        {
            ctx.ReachableTypeIds.Add(type.SubjectId);
        }

        foreach (var field in ctx.SemanticWorld.Fields.Where(candidate =>
                     string.Equals(candidate.AssemblyName, ctx.SemanticWorld.Assembly.Name, StringComparison.Ordinal)))
        {
            ctx.ReachableFieldIds.Add(field.SubjectId);
            ctx.ReachableTypeIds.Add(field.DeclaringTypeSubjectId);
        }

        foreach (var property in ctx.SemanticWorld.Properties.Where(candidate =>
                     string.Equals(candidate.AssemblyName, ctx.SemanticWorld.Assembly.Name, StringComparison.Ordinal)))
        {
            ctx.ReachablePropertyIds.Add(property.SubjectId);
            ctx.ReachableTypeIds.Add(property.DeclaringTypeSubjectId);
        }

        foreach (var method in ctx.SemanticWorld.Methods.Where(candidate =>
                     string.Equals(candidate.AssemblyName, ctx.SemanticWorld.Assembly.Name, StringComparison.Ordinal) &&
                     !ctx.ReachableMethodIds.Contains(candidate.SubjectId)))
        {
            pendingMethods.Enqueue(method);
        }

        ExpandReachableMethods(ctx, pendingMethods);
    }

    private static void IncludePreservedClosure(ReachabilityContext ctx)
    {
        var pendingMethods = new Queue<ManagedMethodModel>();

        foreach (var type in ctx.SemanticWorld.Types.Where(candidate => candidate.IsPreserved))
        {
            ctx.ReachableTypeIds.Add(type.SubjectId);

            foreach (var field in ctx.SemanticWorld.Fields.Where(candidate =>
                         string.Equals(candidate.DeclaringTypeSubjectId, type.SubjectId, StringComparison.Ordinal)))
            {
                ctx.ReachableFieldIds.Add(field.SubjectId);
            }

            foreach (var property in ctx.SemanticWorld.Properties.Where(candidate =>
                         string.Equals(candidate.DeclaringTypeSubjectId, type.SubjectId, StringComparison.Ordinal)))
            {
                ctx.ReachablePropertyIds.Add(property.SubjectId);
            }

            foreach (var method in ctx.SemanticWorld.Methods.Where(candidate =>
                         string.Equals(candidate.DeclaringTypeSubjectId, type.SubjectId, StringComparison.Ordinal) &&
                         !ctx.ReachableMethodIds.Contains(candidate.SubjectId)))
            {
                pendingMethods.Enqueue(method);
            }
        }

        foreach (var field in ctx.SemanticWorld.Fields.Where(candidate => candidate.IsPreserved))
        {
            ctx.ReachableFieldIds.Add(field.SubjectId);
            ctx.ReachableTypeIds.Add(field.DeclaringTypeSubjectId);
        }

        foreach (var property in ctx.SemanticWorld.Properties.Where(candidate => candidate.IsPreserved))
        {
            ctx.ReachablePropertyIds.Add(property.SubjectId);
            ctx.ReachableTypeIds.Add(property.DeclaringTypeSubjectId);
        }

        foreach (var method in ctx.SemanticWorld.Methods.Where(candidate => candidate.IsPreserved))
        {
            ctx.ReachableTypeIds.Add(method.DeclaringTypeSubjectId);
            if (!ctx.ReachableMethodIds.Contains(method.SubjectId))
            {
                pendingMethods.Enqueue(method);
            }
        }

        ExpandReachableMethods(ctx, pendingMethods);
    }

    private static void IncludePolymorphicDispatchImplementationClosure(ReachabilityContext ctx)
    {
        var methodsByDeclaringType = ctx.SemanticWorld.Methods
            .GroupBy(method => method.DeclaringTypeSubjectId, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.Ordinal);

        while (true)
        {
            var pendingMethods = new Queue<ManagedMethodModel>();
            var reachableTypeSnapshot = ctx.ReachableTypeIds.ToArray();
            var reachableMethodSnapshot = ctx.SemanticWorld.Methods
                .Where(method => ctx.ReachableMethodIds.Contains(method.SubjectId))
                .ToArray();

            foreach (var method in reachableMethodSnapshot)
            {
                foreach (var instruction in method.Body.Blocks.SelectMany(block => block.Instructions))
                {
                    if (!TryResolvePolymorphicDispatchSlot(
                            ctx.MethodMap,
                            ctx.TypeMap,
                            instruction,
                            out var slotMethod,
                            out var slotType,
                            out var slotTypeSubjectId,
                            out var slotSignatureSuffix,
                            out var interfaceDispatch))
                    {
                        continue;
                    }

                    foreach (var candidateTypeSubjectId in reachableTypeSnapshot)
                    {
                        if (!ctx.TypeMap.TryGetValue(candidateTypeSubjectId, out var candidateType) ||
                            candidateType.IsInterface ||
                            !IsCompatiblePolymorphicDispatchTargetType(
                                candidateType,
                                slotType,
                                slotTypeSubjectId!,
                                interfaceDispatch,
                                ctx.TypeMap))
                        {
                            continue;
                        }

                        var implementationMethod = ResolvePolymorphicDispatchImplementationMethod(
                            methodsByDeclaringType,
                            ctx.TypeMap,
                            candidateType,
                            slotMethod,
                            slotSignatureSuffix!,
                            interfaceDispatch);
                        if (implementationMethod is null ||
                            ctx.ReachableMethodIds.Contains(implementationMethod.SubjectId))
                        {
                            continue;
                        }

                        pendingMethods.Enqueue(implementationMethod);
                    }
                }
            }

            if (pendingMethods.Count == 0)
            {
                return;
            }

            ExpandReachableMethods(ctx, pendingMethods);
        }
    }

    private static bool TryResolvePolymorphicDispatchSlot(
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap,
        ManagedInstructionModel instruction,
        out ManagedMethodModel? slotMethod,
        out ManagedTypeModel? slotType,
        out string? slotTypeSubjectId,
        out string? slotSignatureSuffix,
        out bool interfaceDispatch)
    {
        slotMethod = null;
        slotType = null;
        slotTypeSubjectId = null;
        slotSignatureSuffix = null;
        interfaceDispatch = false;

        if (!string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal) ||
            string.IsNullOrWhiteSpace(instruction.Callee) ||
            !instruction.Callee.Contains("::", StringComparison.Ordinal))
        {
            return false;
        }

        slotTypeSubjectId = GetDeclaringTypeSubjectId(instruction.Callee);
        slotSignatureSuffix = GetMethodSignatureSuffix(instruction.Callee);

        if (TryResolveMethod(methodMap, instruction.Callee, out var resolvedSlotMethod))
        {
            slotMethod = resolvedSlotMethod;
            if (!typeMap.TryGetValue(resolvedSlotMethod.DeclaringTypeSubjectId, out slotType) ||
                (!slotType.IsInterface && !resolvedSlotMethod.IsVirtual))
            {
                return false;
            }

            interfaceDispatch = slotType.IsInterface;
            return true;
        }

        if (typeMap.TryGetValue(slotTypeSubjectId, out slotType))
        {
            interfaceDispatch = slotType.IsInterface;
            return true;
        }

        interfaceDispatch = true;
        return true;
    }

    private static bool IsCompatiblePolymorphicDispatchTargetType(
        ManagedTypeModel candidateType,
        ManagedTypeModel? slotType,
        string slotTypeSubjectId,
        bool interfaceDispatch,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        if (slotType is not null)
        {
            return IsCompatibleVirtualDispatchTargetType(candidateType, slotType, typeMap);
        }

        return interfaceDispatch && ImplementsInterface(candidateType, slotTypeSubjectId, typeMap);
    }

    private static ManagedMethodModel? ResolvePolymorphicDispatchImplementationMethod(
        IReadOnlyDictionary<string, ManagedMethodModel[]> methodsByDeclaringType,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap,
        ManagedTypeModel candidateType,
        ManagedMethodModel? slotMethod,
        string slotSignatureSuffix,
        bool interfaceDispatch)
    {
        var currentSubjectId = candidateType.SubjectId;
        var currentDefinitionSubjectId = candidateType.DefinitionSubjectId;

        while (!string.IsNullOrWhiteSpace(currentSubjectId))
        {
            if (TryResolveDispatchImplementationMethodOnType(
                    methodsByDeclaringType,
                    currentSubjectId,
                    slotMethod,
                    slotSignatureSuffix,
                    interfaceDispatch,
                    out var implementationMethod))
            {
                return implementationMethod;
            }

            if (!string.IsNullOrWhiteSpace(currentDefinitionSubjectId) &&
                !string.Equals(currentDefinitionSubjectId, currentSubjectId, StringComparison.Ordinal) &&
                TryResolveDispatchImplementationMethodOnType(
                    methodsByDeclaringType,
                    currentDefinitionSubjectId,
                    slotMethod,
                    slotSignatureSuffix,
                    interfaceDispatch,
                    out implementationMethod))
            {
                return implementationMethod;
            }

            var baseTypeSubjectId = TryResolveBaseTypeSubjectId(
                currentSubjectId,
                currentDefinitionSubjectId,
                typeMap);
            if (string.IsNullOrWhiteSpace(baseTypeSubjectId))
            {
                break;
            }

            if (typeMap.TryGetValue(baseTypeSubjectId, out var baseType))
            {
                currentSubjectId = baseType.SubjectId;
                currentDefinitionSubjectId = baseType.DefinitionSubjectId;
            }
            else
            {
                currentSubjectId = baseTypeSubjectId;
                currentDefinitionSubjectId = baseTypeSubjectId;
            }
        }

        return null;
    }

    private static bool TryResolveDispatchImplementationMethodOnType(
        IReadOnlyDictionary<string, ManagedMethodModel[]> methodsByDeclaringType,
        string declaringTypeSubjectId,
        ManagedMethodModel? slotMethod,
        string slotSignatureSuffix,
        bool interfaceDispatch,
        out ManagedMethodModel? implementationMethod)
    {
        implementationMethod = null;
        if (!methodsByDeclaringType.TryGetValue(declaringTypeSubjectId, out var candidateMethods))
        {
            return false;
        }

        implementationMethod = candidateMethods.FirstOrDefault(candidate =>
            MatchesPolymorphicDispatchSlot(candidate, slotMethod, slotSignatureSuffix, interfaceDispatch));
        return implementationMethod is not null;
    }

    private static bool MatchesPolymorphicDispatchSlot(
        ManagedMethodModel candidateMethod,
        ManagedMethodModel? slotMethod,
        string slotSignatureSuffix,
        bool interfaceDispatch)
    {
        return !candidateMethod.IsStatic &&
               (interfaceDispatch || candidateMethod.IsVirtual) &&
               (slotMethod is not null
                   ? MatchesVirtualDispatchSignature(candidateMethod, slotMethod)
                   : string.Equals(
                       GetMethodSignatureSuffix(candidateMethod.SubjectId),
                       slotSignatureSuffix,
                       StringComparison.Ordinal));
    }

    private static void ExpandReachableMethods(
        ReachabilityContext ctx,
        Queue<ManagedMethodModel> pendingMethods)
    {
        while (pendingMethods.Count > 0)
        {
            var method = pendingMethods.Dequeue();
            if (!ctx.ReachableMethodIds.Add(method.SubjectId))
            {
                continue;
            }

            ctx.ReachableTypeIds.Add(method.DeclaringTypeSubjectId);

            foreach (var block in method.Body.Blocks)
            {
                for (var instructionIndex = 0; instructionIndex < block.Instructions.Count; instructionIndex++)
                {
                    var instruction = block.Instructions[instructionIndex];
                    if (!string.IsNullOrWhiteSpace(instruction.Callee) &&
                        TryResolveMethod(ctx.MethodMap, instruction.Callee, out var directCalleeMethod) &&
                        IsInternalAssembly(ctx.SemanticWorld, directCalleeMethod.AssemblyName))
                    {
                        pendingMethods.Enqueue(directCalleeMethod);
                        ctx.ReachableTypeIds.Add(directCalleeMethod.DeclaringTypeSubjectId);
                    }

                    if (string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal) &&
                        !string.IsNullOrWhiteSpace(instruction.Callee))
                    {
                        var resolvedTargetSubjectId = TryResolveDevirtualizedCallTarget(
                            ctx.MethodMap,
                            block.Instructions,
                            instructionIndex,
                            instruction.Callee!);
                        if (!string.IsNullOrWhiteSpace(resolvedTargetSubjectId) &&
                            ctx.MethodMap.TryGetValue(resolvedTargetSubjectId, out var resolvedMethod))
                        {
                            pendingMethods.Enqueue(resolvedMethod);
                            ctx.ReachableTypeIds.Add(resolvedMethod.DeclaringTypeSubjectId);
                        }
                    }

                    var reference = instruction.Reference;
                    if (reference is null ||
                        !IsInternalAssembly(ctx.SemanticWorld, reference.AssemblyName))
                    {
                        continue;
                    }

                    switch (reference.SubjectKind)
                    {
                        case "method" when TryResolveMethod(ctx.MethodMap, reference.SubjectId, out var referencedMethod):
                            pendingMethods.Enqueue(referencedMethod);
                            break;
                        case "field" when ctx.FieldMap.TryGetValue(reference.SubjectId, out var referencedField):
                            ctx.ReachableFieldIds.Add(referencedField.SubjectId);
                            ctx.ReachableTypeIds.Add(referencedField.DeclaringTypeSubjectId);
                            break;
                        case "property" when ctx.PropertyMap.TryGetValue(reference.SubjectId, out var referencedProperty):
                            ctx.ReachablePropertyIds.Add(referencedProperty.SubjectId);
                            ctx.ReachableTypeIds.Add(referencedProperty.DeclaringTypeSubjectId);
                            break;
                        case "type" when ctx.TypeMap.ContainsKey(reference.SubjectId):
                            ctx.ReachableTypeIds.Add(reference.SubjectId);
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

    private static void IncludeThreadStaticFieldClosure(ReachabilityContext ctx)
    {
        var requiresThreadStaticClosure = ctx.ReachableMethodIds.Any(subjectId =>
            ctx.MethodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            capabilities.Contains("requires-thread-static-storage", StringComparer.Ordinal));
        if (!requiresThreadStaticClosure)
        {
            return;
        }

        foreach (var field in ctx.SemanticWorld.Fields.Where(candidate => candidate.IsThreadStatic))
        {
            ctx.ReachableFieldIds.Add(field.SubjectId);
            ctx.ReachableTypeIds.Add(field.DeclaringTypeSubjectId);
        }
    }

    private static void IncludeAsyncStateMachineClosure(ReachabilityContext ctx)
    {
        var requiresAsyncClosure = ctx.ReachableMethodIds.Any(subjectId =>
            ctx.MethodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            (capabilities.Contains("requires-async-state-machine", StringComparer.Ordinal) ||
             capabilities.Contains("requires-task-awaiter", StringComparer.Ordinal)));
        if (!requiresAsyncClosure)
        {
            return;
        }

        var asyncTypeIds = ctx.SemanticWorld.Types
            .Where(IsCompilerGeneratedAsyncStateMachine)
            .Select(type => type.SubjectId)
            .ToHashSet(StringComparer.Ordinal);
        if (asyncTypeIds.Count == 0)
        {
            return;
        }

        foreach (var asyncTypeId in asyncTypeIds)
        {
            ctx.ReachableTypeIds.Add(asyncTypeId);
        }

        foreach (var field in ctx.SemanticWorld.Fields.Where(candidate => asyncTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            ctx.ReachableFieldIds.Add(field.SubjectId);
        }

        foreach (var property in ctx.SemanticWorld.Properties.Where(candidate => asyncTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            ctx.ReachablePropertyIds.Add(property.SubjectId);
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var method in ctx.SemanticWorld.Methods.Where(candidate => asyncTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            if (!ctx.ReachableMethodIds.Contains(method.SubjectId))
            {
                pendingMethods.Enqueue(method);
            }
        }

        ExpandReachableMethods(ctx, pendingMethods);
    }

    private static void IncludeIteratorStateMachineClosure(ReachabilityContext ctx)
    {
        var requiresIteratorClosure = ctx.ReachableMethodIds.Any(subjectId =>
            ctx.MethodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            capabilities.Contains("requires-string-join-enumerable", StringComparer.Ordinal));
        if (!requiresIteratorClosure)
        {
            return;
        }

        var iteratorTypeIds = ctx.SemanticWorld.Types
            .Where(IsCompilerGeneratedIteratorStateMachine)
            .Select(type => type.SubjectId)
            .ToHashSet(StringComparer.Ordinal);
        if (iteratorTypeIds.Count == 0)
        {
            return;
        }

        foreach (var iteratorTypeId in iteratorTypeIds)
        {
            ctx.ReachableTypeIds.Add(iteratorTypeId);
        }

        foreach (var field in ctx.SemanticWorld.Fields.Where(candidate => iteratorTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            ctx.ReachableFieldIds.Add(field.SubjectId);
        }

        foreach (var property in ctx.SemanticWorld.Properties.Where(candidate => iteratorTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            ctx.ReachablePropertyIds.Add(property.SubjectId);
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var method in ctx.SemanticWorld.Methods.Where(candidate => iteratorTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            if (!ctx.ReachableMethodIds.Contains(method.SubjectId))
            {
                pendingMethods.Enqueue(method);
            }
        }

        ExpandReachableMethods(ctx, pendingMethods);
    }

    private static void IncludeReflectionQuerySurface(ReachabilityContext ctx)
    {
        var requiresReflectionQuerySurface = ctx.ReachableMethodIds.Any(subjectId =>
            ctx.MethodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            (capabilities.Contains("requires-closed-type-member-query", StringComparer.Ordinal) ||
             capabilities.Contains("requires-generic-type-definition-query", StringComparer.Ordinal)));
        if (!requiresReflectionQuerySurface)
        {
            return;
        }

        var targetTypeIds = CollectReflectionQueryTargetTypeIds(ctx);
        if (targetTypeIds.Count == 0)
        {
            return;
        }

        foreach (var targetTypeId in targetTypeIds)
        {
            ctx.ReachableTypeIds.Add(targetTypeId);
        }

        foreach (var field in ctx.SemanticWorld.Fields.Where(candidate => targetTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            ctx.ReachableFieldIds.Add(field.SubjectId);
        }

        foreach (var property in ctx.SemanticWorld.Properties.Where(candidate => targetTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            ctx.ReachablePropertyIds.Add(property.SubjectId);
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var method in ctx.SemanticWorld.Methods.Where(candidate => targetTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            if (!ctx.ReachableMethodIds.Contains(method.SubjectId))
            {
                pendingMethods.Enqueue(method);
            }
        }

        ExpandReachableMethods(ctx, pendingMethods);
    }

    private static HashSet<string> CollectReflectionQueryTargetTypeIds(ReachabilityContext ctx)
    {
        var targetTypeIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in ctx.SemanticWorld.Methods)
        {
            if (!ctx.ReachableMethodIds.Contains(method.SubjectId))
            {
                continue;
            }

            foreach (var instruction in method.Body.Blocks.SelectMany(block => block.Instructions))
            {
                var reference = instruction.Reference;
                if (!string.Equals(instruction.Op, "ldtoken", StringComparison.Ordinal) ||
                    reference is null ||
                    !IsInternalAssembly(ctx.SemanticWorld, reference.AssemblyName) ||
                    !string.Equals(reference.SubjectKind, "type", StringComparison.Ordinal) ||
                    !ctx.TypeMap.ContainsKey(reference.SubjectId))
                {
                    continue;
                }

                targetTypeIds.Add(reference.SubjectId);
            }
        }

        return targetTypeIds;
    }

}
