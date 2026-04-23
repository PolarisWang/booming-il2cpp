using System.Globalization;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed partial class LinkerStage
{
    private static ReachableClosure ComputeReachableClosure(SemanticWorldModel semanticWorld)
    {
        if (semanticWorld.FullAssemblyClosure &&
            semanticWorld.Assemblies.Count == 1)
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

        ManagedMethodModel? entryPointMethod = null;
        if (!string.IsNullOrWhiteSpace(semanticWorld.EntryPointSubjectId) &&
            !methodMap.TryGetValue(semanticWorld.EntryPointSubjectId, out entryPointMethod))
        {
            throw new InvalidOperationException(
                $"linker entry point '{semanticWorld.EntryPointSubjectId}' is missing from semantic world");
        }

        var reachableMethodIds = new HashSet<string>(StringComparer.Ordinal);
        var reachableFieldIds = new HashSet<string>(StringComparer.Ordinal);
        var reachablePropertyIds = new HashSet<string>(StringComparer.Ordinal);
        var reachableTypeIds = new HashSet<string>(StringComparer.Ordinal);
        var pendingMethods = new Queue<ManagedMethodModel>();
        if (entryPointMethod is not null)
        {
            pendingMethods.Enqueue(entryPointMethod);
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
        IncludeFullAssemblyClosure(
            semanticWorld,
            methodMap,
            fieldMap,
            propertyMap,
            typeMap,
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
        IncludeIteratorStateMachineClosure(
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
        IncludePolymorphicDispatchImplementationClosure(
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

    private static void IncludeFullAssemblyClosure(
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
        if (!semanticWorld.FullAssemblyClosure)
        {
            return;
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var type in semanticWorld.Types.Where(candidate =>
                     string.Equals(candidate.AssemblyName, semanticWorld.Assembly.Name, StringComparison.Ordinal)))
        {
            reachableTypeIds.Add(type.SubjectId);
        }

        foreach (var field in semanticWorld.Fields.Where(candidate =>
                     string.Equals(candidate.AssemblyName, semanticWorld.Assembly.Name, StringComparison.Ordinal)))
        {
            reachableFieldIds.Add(field.SubjectId);
            reachableTypeIds.Add(field.DeclaringTypeSubjectId);
        }

        foreach (var property in semanticWorld.Properties.Where(candidate =>
                     string.Equals(candidate.AssemblyName, semanticWorld.Assembly.Name, StringComparison.Ordinal)))
        {
            reachablePropertyIds.Add(property.SubjectId);
            reachableTypeIds.Add(property.DeclaringTypeSubjectId);
        }

        foreach (var method in semanticWorld.Methods.Where(candidate =>
                     string.Equals(candidate.AssemblyName, semanticWorld.Assembly.Name, StringComparison.Ordinal) &&
                     !reachableMethodIds.Contains(candidate.SubjectId)))
        {
            pendingMethods.Enqueue(method);
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

    private static void IncludePolymorphicDispatchImplementationClosure(
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
        var methodsByDeclaringType = semanticWorld.Methods
            .GroupBy(method => method.DeclaringTypeSubjectId, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.Ordinal);

        while (true)
        {
            var pendingMethods = new Queue<ManagedMethodModel>();
            var reachableTypeSnapshot = reachableTypeIds.ToArray();
            var reachableMethodSnapshot = semanticWorld.Methods
                .Where(method => reachableMethodIds.Contains(method.SubjectId))
                .ToArray();

            foreach (var method in reachableMethodSnapshot)
            {
                foreach (var instruction in method.Body.Blocks.SelectMany(block => block.Instructions))
                {
                    if (!TryResolvePolymorphicDispatchSlot(
                            methodMap,
                            typeMap,
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
                        if (!typeMap.TryGetValue(candidateTypeSubjectId, out var candidateType) ||
                            candidateType.IsInterface ||
                            !IsCompatiblePolymorphicDispatchTargetType(
                                candidateType,
                                slotType,
                                slotTypeSubjectId!,
                                interfaceDispatch,
                                typeMap))
                        {
                            continue;
                        }

                        var implementationMethod = ResolvePolymorphicDispatchImplementationMethod(
                            methodsByDeclaringType,
                            typeMap,
                            candidateType,
                            slotMethod,
                            slotSignatureSuffix!,
                            interfaceDispatch);
                        if (implementationMethod is null ||
                            reachableMethodIds.Contains(implementationMethod.SubjectId))
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
                    if (!string.IsNullOrWhiteSpace(instruction.Callee) &&
                        TryResolveMethod(methodMap, instruction.Callee, out var directCalleeMethod) &&
                        IsInternalAssembly(semanticWorld, directCalleeMethod.AssemblyName))
                    {
                        pendingMethods.Enqueue(directCalleeMethod);
                        reachableTypeIds.Add(directCalleeMethod.DeclaringTypeSubjectId);
                    }

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
                        case "method" when TryResolveMethod(methodMap, reference.SubjectId, out var referencedMethod):
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

    private static void IncludeIteratorStateMachineClosure(
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
        var requiresIteratorClosure = reachableMethodIds.Any(subjectId =>
            methodCapabilities.TryGetValue(subjectId, out var capabilities) &&
            capabilities.Contains("requires-string-join-enumerable", StringComparer.Ordinal));
        if (!requiresIteratorClosure)
        {
            return;
        }

        var iteratorTypeIds = semanticWorld.Types
            .Where(IsCompilerGeneratedIteratorStateMachine)
            .Select(type => type.SubjectId)
            .ToHashSet(StringComparer.Ordinal);
        if (iteratorTypeIds.Count == 0)
        {
            return;
        }

        foreach (var iteratorTypeId in iteratorTypeIds)
        {
            reachableTypeIds.Add(iteratorTypeId);
        }

        foreach (var field in semanticWorld.Fields.Where(candidate => iteratorTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            reachableFieldIds.Add(field.SubjectId);
        }

        foreach (var property in semanticWorld.Properties.Where(candidate => iteratorTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
        {
            reachablePropertyIds.Add(property.SubjectId);
        }

        var pendingMethods = new Queue<ManagedMethodModel>();
        foreach (var method in semanticWorld.Methods.Where(candidate => iteratorTypeIds.Contains(candidate.DeclaringTypeSubjectId)))
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

}
