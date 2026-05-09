using System.IO;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Loader;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private StaticInitializationSupportModel _staticInitializationSupport = StaticInitializationSupportModel.Empty;

    private sealed record StaticInitializationAction(
        string FieldSubjectId,
        string ConstructedTypeSubjectId,
        string ConstructorSubjectId,
        bool ElideConstructorCall);

    private sealed record StaticInitializationPlan(
        string TypeSubjectId,
        IReadOnlyList<StaticInitializationAction> Actions);

    private sealed record StaticInitializationSupportModel(
        IReadOnlyDictionary<string, StaticInitializationPlan> PlansByTypeSubjectId,
        IReadOnlyDictionary<string, string> DeclaringTypeByStaticFieldSubjectId,
        IReadOnlySet<string> RequiredExternalRuntimeHelperSubjectIds)
    {
        public static readonly StaticInitializationSupportModel Empty = new(
            new Dictionary<string, StaticInitializationPlan>(StringComparer.Ordinal),
            new Dictionary<string, string>(StringComparer.Ordinal),
            new HashSet<string>(StringComparer.Ordinal));
    }

    private StaticInitializationSupportModel BuildStaticInitializationSupportModel(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        ManagedClosureManifestArtifact closureManifest)
    {
        var candidateTypeSubjectIds = new HashSet<string>(reachableMethods.Count, StringComparer.Ordinal);
        var declaringTypeByStaticFieldSubjectId = new Dictionary<string, string>(StringComparer.Ordinal);

        foreach (var method in reachableMethods)
        {
            if (method.IsStatic &&
                !string.IsNullOrEmpty(method.Identity.DeclaringTypeSubjectId))
            {
                candidateTypeSubjectIds.Add(method.Identity.DeclaringTypeSubjectId);
            }

            foreach (var instruction in method.Instructions)
            {
                if (instruction.TargetReference?.Kind != AotCoreIrReferenceKind.Field ||
                    instruction.RuntimeServiceKind is not (
                        AotCoreIrRuntimeServiceKind.LoadStaticField or
                        AotCoreIrRuntimeServiceKind.StoreStaticField))
                {
                    continue;
                }

                var fieldSubjectId = instruction.TargetReference.SubjectId;
                var declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(instruction.TargetReference);
                candidateTypeSubjectIds.Add(declaringTypeSubjectId);
                declaringTypeByStaticFieldSubjectId[fieldSubjectId] = declaringTypeSubjectId;
            }
        }

        if (candidateTypeSubjectIds.Count == 0)
        {
            return StaticInitializationSupportModel.Empty;
        }

        var loadedWorld = new LoaderStage().LoadMultiple(new ManagedClosureRequest(
            closureManifest.InputAssemblyPath,
            Path.GetDirectoryName(closureManifest.InputAssemblyPath) ?? closureManifest.InputAssemblyPath,
            closureManifest.EntrySubjectId,
            GetResolvedAdditionalAssemblyPaths(closureManifest),
            FullAssemblyClosure: closureManifest.FullAssemblyClosure));
        var loadedMethodsBySubjectId = loadedWorld.Methods
            .GroupBy(method => method.SubjectId, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.First(), StringComparer.Ordinal);
        var loadedFieldsBySubjectId = loadedWorld.Fields
            .GroupBy(field => field.SubjectId, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.First(), StringComparer.Ordinal);
        var reachableMethodSubjectIds = reachableMethods
            .Select(method => method.SubjectId)
            .ToHashSet(StringComparer.Ordinal);
        var plansByTypeSubjectId = new Dictionary<string, StaticInitializationPlan>(candidateTypeSubjectIds.Count, StringComparer.Ordinal);
        var requiredExternalRuntimeHelperSubjectIds = new HashSet<string>(candidateTypeSubjectIds.Count, StringComparer.Ordinal);

        foreach (var typeSubjectId in candidateTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            var cctorSubjectId = ManagedNaming.CreateMethodSubjectId(typeSubjectId, ".cctor", "System.Void", []);
            if (!loadedMethodsBySubjectId.TryGetValue(cctorSubjectId, out var cctorMethod))
            {
                continue;
            }

            var plan = CreateStaticInitializationPlan(typeSubjectId, cctorMethod, loadedFieldsBySubjectId);
            if (plan is null)
            {
                continue;
            }

            var normalizedActions = new List<StaticInitializationAction>(plan.Actions.Count);
            var skipPlan = false;
            foreach (var action in plan.Actions)
            {
                if (TryCreateExternalRuntimeHelperDefinition(action.ConstructorSubjectId, out _))
                {
                    requiredExternalRuntimeHelperSubjectIds.Add(action.ConstructorSubjectId);
                    normalizedActions.Add(action);
                    continue;
                }

                if (reachableMethodSubjectIds.Contains(action.ConstructorSubjectId))
                {
                    normalizedActions.Add(action);
                    continue;
                }

                if (loadedMethodsBySubjectId.TryGetValue(action.ConstructorSubjectId, out var constructorMethod) &&
                    CanElideStaticInitializationConstructorInvocation(constructorMethod))
                {
                    normalizedActions.Add(action with { ElideConstructorCall = true });
                    continue;
                }

                // Constructor is not in the emitted method set (e.g., a BCL type
                // constructor like List<string>.ctor()).  Skip this cctor plan
                // entirely — the runtime handles lazy initialization on first access.
                skipPlan = true;
                break;
            }

            if (skipPlan)
                continue;

            plansByTypeSubjectId[typeSubjectId] = plan with { Actions = normalizedActions };
        }

        if (plansByTypeSubjectId.Count == 0)
        {
            return StaticInitializationSupportModel.Empty;
        }

        return new StaticInitializationSupportModel(
            plansByTypeSubjectId,
            declaringTypeByStaticFieldSubjectId,
            requiredExternalRuntimeHelperSubjectIds);
    }

    private static IReadOnlyList<string>? GetResolvedAdditionalAssemblyPaths(ManagedClosureManifestArtifact closureManifest)
    {
        if (closureManifest.ResolvedAssemblies is not { Count: > 0 })
        {
            return closureManifest.AdditionalAssemblyPaths;
        }

        var normalizedInputAssemblyPath = Path.GetFullPath(closureManifest.InputAssemblyPath);
        var resolvedAdditionalAssemblyPaths = closureManifest.ResolvedAssemblies
            .Select(assembly => assembly.Path)
            .Where(path => !string.IsNullOrEmpty(path))
            .Select(path => Path.GetFullPath(path))
            .Where(path => !string.Equals(path, normalizedInputAssemblyPath, StringComparison.OrdinalIgnoreCase))
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .ToList();

        return resolvedAdditionalAssemblyPaths;
    }

    private static StaticInitializationPlan? CreateStaticInitializationPlan(
        string typeSubjectId,
        ManagedMethodModel cctorMethod,
        IReadOnlyDictionary<string, ManagedFieldModel> loadedFieldsBySubjectId)
    {
        if (!cctorMethod.IsStatic)
        {
            throw new NotSupportedException(
                $"native-aot static initializer '{cctorMethod.SubjectId}' must be static.");
        }

        if (cctorMethod.Body.ExceptionRegions.Count > 0)
        {
            throw new NotSupportedException(
                $"native-aot static initializer '{cctorMethod.SubjectId}' does not support exception regions.");
        }

        var instructions = cctorMethod.Body.Blocks
            .SelectMany(block => block.Instructions)
            .Where(instruction => !string.Equals(instruction.Op, "nop", StringComparison.Ordinal))
            .ToArray();
        if (instructions.Length == 0 ||
            (instructions.Length == 1 && string.Equals(instructions[0].Op, "ret", StringComparison.Ordinal)))
        {
            return null;
        }

        var actions = new List<StaticInitializationAction>();
        var index = 0;
        while (index < instructions.Length)
        {
            var instruction = instructions[index];
            if (string.Equals(instruction.Op, "ret", StringComparison.Ordinal))
            {
                if (index != instructions.Length - 1)
                {
                    throw new NotSupportedException(
                        $"native-aot static initializer '{cctorMethod.SubjectId}' contains instructions after ret.");
                }

                break;
            }

            // For generic type cctors, skip unsupported opcodes rather than throwing
            if (typeSubjectId!.Contains("!!") || typeSubjectId.Contains("`") || typeSubjectId.Contains("<!"))
            {
                return null;
            }

            // For any cctor instruction that doesn't fit the simple newobj+stsfld
            // pattern, skip the static init plan. Complex static initialization
            // is handled lazily by the runtime at first type access.
            if (!string.Equals(instruction.Op, "newobj", StringComparison.Ordinal) ||
                string.IsNullOrEmpty(instruction.Callee))
            {
                return null;
            }

            if (index + 1 >= instructions.Length ||
                !string.Equals(instructions[index + 1].Op, "stsfld", StringComparison.Ordinal))
            {
                throw new NotSupportedException(
                    $"native-aot static initializer '{cctorMethod.SubjectId}' requires immediate stsfld after newobj at IL offset {instruction.IlOffset ?? -1}.");
            }

            var storeInstruction = instructions[index + 1];
            var fieldSubjectId = GetManagedFieldSubjectId(storeInstruction);
            if (!loadedFieldsBySubjectId.TryGetValue(fieldSubjectId, out var field) ||
                !field.IsStatic)
            {
                throw new NotSupportedException(
                    $"native-aot static initializer '{cctorMethod.SubjectId}' references unsupported static field '{fieldSubjectId}'.");
            }

            // Normalize generic parameter placeholders before comparison
            var normFieldType = (field.DeclaringTypeSubjectId ?? "").Replace("`1", "<!0>");
            var normCctorType = (typeSubjectId ?? "").Replace("`1", "<!0>");
            if (!string.Equals(normFieldType, normCctorType, StringComparison.Ordinal) &&
                !string.Equals(field.DeclaringTypeSubjectId, typeSubjectId, StringComparison.Ordinal))
            {
                throw new NotSupportedException(
                    $"native-aot static initializer '{cctorMethod.SubjectId}' must initialize fields declared on '{typeSubjectId}', got '{field.DeclaringTypeSubjectId}'.");
            }

            actions.Add(new StaticInitializationAction(
                fieldSubjectId,
                GetMethodDeclaringTypeSubjectId(instruction.Callee),
                instruction.Callee,
                ElideConstructorCall: false));
            index += 2;
        }

        return actions.Count == 0
            ? null
            : new StaticInitializationPlan(typeSubjectId!, actions);
    }

    private static string GetManagedFieldSubjectId(ManagedInstructionModel instruction)
    {
        if (instruction.Reference is not null &&
            !string.IsNullOrWhiteSpace(instruction.Reference.SubjectId))
        {
            return instruction.Reference.SubjectId;
        }

        if (instruction.Operand is string operand &&
            !string.IsNullOrEmpty(operand))
        {
            return operand;
        }

        throw new InvalidOperationException(
            $"native-aot static initializer opcode '{instruction.Op}' is missing field subject metadata.");
    }

    private bool TryGetStaticInitializationPlanForType(
        string typeSubjectId,
        out StaticInitializationPlan? plan)
    {
        return _staticInitializationSupport.PlansByTypeSubjectId.TryGetValue(typeSubjectId, out plan);
    }

    private bool TryGetStaticInitializationPlanForField(
        string fieldSubjectId,
        out StaticInitializationPlan? plan)
    {
        plan = null;
        return _staticInitializationSupport.DeclaringTypeByStaticFieldSubjectId.TryGetValue(fieldSubjectId, out var typeSubjectId) &&
               _staticInitializationSupport.PlansByTypeSubjectId.TryGetValue(typeSubjectId, out plan);
    }

    private static bool CanElideStaticInitializationConstructorInvocation(ManagedMethodModel constructorMethod)
    {
        if (constructorMethod.IsStatic ||
            constructorMethod.Parameters.Count != 0)
        {
            return false;
        }

        var instructions = constructorMethod.Body.Blocks
            .SelectMany(block => block.Instructions)
            .Where(instruction => !string.Equals(instruction.Op, "nop", StringComparison.Ordinal))
            .ToArray();
        if (instructions.Length == 1 &&
            string.Equals(instructions[0].Op, "ret", StringComparison.Ordinal))
        {
            return true;
        }

        return instructions.Length == 3 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               Equals(instructions[0].Operand, 0) &&
               string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Callee, ObjectCtorMethodSubjectId, StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal);
    }
}
