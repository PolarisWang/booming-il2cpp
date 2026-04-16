using System.Globalization;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeAotLoweringPlanner
{
    private const string OverflowExceptionTypeSubjectId = "System.Private.CoreLib/System.OverflowException";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    private IReadOnlyDictionary<string, AotCoreIrMethodArtifact> _methodsBySubjectId =
        new Dictionary<string, AotCoreIrMethodArtifact>(StringComparer.Ordinal);

    private IReadOnlyDictionary<string, string?> _referenceTypeBaseSubjectIds =
        new Dictionary<string, string?>(StringComparer.Ordinal);

    private IReadOnlyDictionary<string, HashSet<string>> _referenceTypeImplementedInterfaceSubjectIds =
        new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);

    private readonly record struct InvocationTarget(
        string TargetSymbol,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
        AotCoreIrAbiSlotArtifact ReturnAbi);

    private sealed record CatchOnlyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact ExceptionRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record FinallyHandlerShape(
        AotCoreIrExceptionRegionArtifact ExceptionRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> Instructions);

    private sealed record FilterAndFinallyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact FilterRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> FilterInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<FinallyHandlerShape> FinallyHandlers,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    public NativeAotTemplateModel Create(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod)
    {
        ArgumentNullException.ThrowIfNull(loweringPlan);
        ArgumentNullException.ThrowIfNull(aotCoreIr);
        ArgumentNullException.ThrowIfNull(entryMethod);

        ValidateEntryMethod(entryMethod);
        if (!string.Equals(entryMethod.NativeSymbol, loweringPlan.EntrySymbol, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot entry symbol '{loweringPlan.EntrySymbol}' does not match aot-core-ir symbol '{entryMethod.NativeSymbol}'");
        }

        _methodsBySubjectId = aotCoreIr.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        _referenceTypeBaseSubjectIds = CollectReferenceTypeBaseSubjectIds(aotCoreIr);
        _referenceTypeImplementedInterfaceSubjectIds = CollectReferenceTypeImplementedInterfaceSubjectIds(aotCoreIr);

        var reachableMethods = CollectReachableMethods(aotCoreIr, entryMethod);
        var objectModelBuilder = new StringBuilder();
        EmitRuntimePrelude(objectModelBuilder);
        EmitObjectModelDeclarations(objectModelBuilder, reachableMethods);

        var methodDeclarations = reachableMethods
            .Select(FormatMethodDeclaration)
            .ToArray();
        var methods = reachableMethods
            .Select(method => new NativeAotMethodTemplateModel
            {
                SubjectId = method.SubjectId,
                MethodSource = BuildMethodSource(method),
            })
            .ToArray();

        return new NativeAotTemplateModel
        {
            Includes =
            [
                "<array>",
                "<cstddef>",
                "<cstdint>",
                "<cstdlib>",
                "<cstring>",
                "<limits>",
            ],
            ObjectModelCode = objectModelBuilder.ToString().TrimEnd(),
            MethodDeclarations = methodDeclarations,
            Methods = methods,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            EntrySymbol = loweringPlan.EntrySymbol,
            EntryNativeSymbol = entryMethod.NativeSymbol,
            NativeEntryFunctionName = loweringPlan.NativeEntryFunctionName,
        };
    }

    private string BuildMethodSource(AotCoreIrMethodArtifact method)
    {
        var builder = new StringBuilder();
        if (method.GenericContext is not null)
        {
            builder.AppendLine(FormatGenericContextComment(method.GenericContext));
        }

        EmitManagedMethod(builder, method);
        return builder.ToString().TrimEnd();
    }

    private static void EmitRuntimePrelude(StringBuilder builder)
    {
        builder.AppendLine("static_assert(sizeof(std::intptr_t) == sizeof(std::uint64_t), \"native-aot ABI lowering requires 64-bit intptr_t\");");
        builder.AppendLine();
        builder.AppendLine("struct chaos_managed_exception");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t object_value = 0;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("template <typename TAction>");
        builder.AppendLine("struct chaos_finally_scope_guard");
        builder.AppendLine("{");
        builder.AppendLine("    explicit chaos_finally_scope_guard(TAction init_action)");
        builder.AppendLine("        : action(init_action)");
        builder.AppendLine("    {");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    chaos_finally_scope_guard(const chaos_finally_scope_guard&) = delete;");
        builder.AppendLine("    chaos_finally_scope_guard& operator=(const chaos_finally_scope_guard&) = delete;");
        builder.AppendLine();
        builder.AppendLine("    ~chaos_finally_scope_guard()");
        builder.AppendLine("    {");
        builder.AppendLine("        if (active)");
        builder.AppendLine("        {");
        builder.AppendLine("            action();");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    TAction action;");
        builder.AppendLine("    bool active = true;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("template <typename TAction>");
        builder.AppendLine("chaos_finally_scope_guard<TAction> chaos_make_finally_scope_guard(TAction action)");
        builder.AppendLine("{");
        builder.AppendLine("    return chaos_finally_scope_guard<TAction>(action);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_float32(float value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint32_t bits = 0;");
        builder.AppendLine("    std::memcpy(&bits, &value, sizeof(value));");
        builder.AppendLine("    return static_cast<std::intptr_t>(static_cast<std::uint64_t>(bits));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("float chaos_load_float32(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto bits = static_cast<std::uint32_t>(static_cast<std::uint64_t>(value));");
        builder.AppendLine("    float result = 0.0f;");
        builder.AppendLine("    std::memcpy(&result, &bits, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_float64(double value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint64_t bits = 0;");
        builder.AppendLine("    std::memcpy(&bits, &value, sizeof(value));");
        builder.AppendLine("    std::intptr_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &bits, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("double chaos_load_float64(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint64_t bits = 0;");
        builder.AppendLine("    std::memcpy(&bits, &value, sizeof(bits));");
        builder.AppendLine("    double result = 0.0;");
        builder.AppendLine("    std::memcpy(&result, &bits, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_int64(std::int64_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int64_t chaos_load_int64(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::int64_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_uint64(std::uint64_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::uint64_t chaos_load_uint64(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint64_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_wrap_add(std::int32_t left, std::int32_t right) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    return static_cast<std::int32_t>(");
        builder.AppendLine("        static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_wrap_sub(std::int32_t left, std::int32_t right) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    return static_cast<std::int32_t>(");
        builder.AppendLine("        static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_wrap_mul(std::int32_t left, std::int32_t right) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    return static_cast<std::int32_t>(");
        builder.AppendLine("        static_cast<std::uint32_t>(left) * static_cast<std::uint32_t>(right));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_div(std::int32_t left, std::int32_t right)");
        builder.AppendLine("{");
        builder.AppendLine("    if (right == 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (left == std::numeric_limits<std::int32_t>::min() && right == -1)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::int32_t>(left / right);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_rem(std::int32_t left, std::int32_t right)");
        builder.AppendLine("{");
        builder.AppendLine("    if (right == 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (left == std::numeric_limits<std::int32_t>::min() && right == -1)");
        builder.AppendLine("    {");
        builder.AppendLine("        return 0;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::int32_t>(left % right);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_shift_left_int32(std::int32_t value, std::int32_t amount) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto shift = static_cast<std::uint32_t>(amount) & 31U;");
        builder.AppendLine("    return static_cast<std::int32_t>(static_cast<std::uint32_t>(value) << shift);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_shift_right_int32(std::int32_t value, std::int32_t amount) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto shift = static_cast<std::uint32_t>(amount) & 31U;");
        builder.AppendLine("    if (shift == 0U)");
        builder.AppendLine("    {");
        builder.AppendLine("        return value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto bits = static_cast<std::uint32_t>(value);");
        builder.AppendLine("    if (value >= 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        return static_cast<std::int32_t>(bits >> shift);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto fill = std::numeric_limits<std::uint32_t>::max() << (32U - shift);");
        builder.AppendLine("    return static_cast<std::int32_t>((bits >> shift) | fill);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_shift_right_un_int32(std::int32_t value, std::int32_t amount) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto shift = static_cast<std::uint32_t>(amount) & 31U;");
        builder.AppendLine("    return static_cast<std::int32_t>(static_cast<std::uint32_t>(value) >> shift);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_checked_conv_ovf_i1(std::intptr_t value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (value < static_cast<std::intptr_t>(std::numeric_limits<std::int8_t>::min()) ||");
        builder.AppendLine("        value > static_cast<std::intptr_t>(std::numeric_limits<std::int8_t>::max()))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::intptr_t>(static_cast<std::int8_t>(value));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_checked_conv_ovf_u1(std::intptr_t value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (value < static_cast<std::intptr_t>(0) ||");
        builder.AppendLine("        value > static_cast<std::intptr_t>(std::numeric_limits<std::uint8_t>::max()))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::intptr_t>(static_cast<std::uint8_t>(value));");
        builder.AppendLine("}");
        builder.AppendLine();
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> CollectReachableMethods(
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod)
    {
        var ordered = new List<AotCoreIrMethodArtifact>();
        var visited = new HashSet<string>(StringComparer.Ordinal);

        void Visit(AotCoreIrMethodArtifact method)
        {
            if (!visited.Add(method.SubjectId))
            {
                return;
            }

            foreach (var instruction in method.Instructions)
            {
                foreach (var reachableMethod in ResolveReachableMethods(instruction))
                {
                    Visit(reachableMethod);
                }
            }

            ordered.Add(method);
        }

        Visit(entryMethod);
        return ordered;
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveReachableMethods(
        AotCoreIrInstructionArtifact instruction)
    {
        if (string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal))
        {
            return instruction.DispatchKindCode == HybridDispatchKind.Virtual
                ? ResolveVirtualDispatchTargets(instruction)
                : ResolveDirectReachableMethods(instruction);
        }

        if (!string.Equals(instruction.Op, "call", StringComparison.Ordinal))
        {
            return [];
        }

        return ResolveDirectReachableMethods(instruction);
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveDirectReachableMethods(
        AotCoreIrInstructionArtifact instruction)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee))
        {
            return [];
        }

        var calleeMethod = TryGetLowerableMethod(instruction.Callee);
        return calleeMethod is null ? [] : [calleeMethod];
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveVirtualDispatchTargets(
        AotCoreIrInstructionArtifact instruction)
    {
        var dispatchSlotMethod = ResolveRequiredDispatchSlotMethod(instruction);
        var slotDeclaringTypeSubjectId = dispatchSlotMethod.Identity.DeclaringTypeSubjectId;
        var slotDeclaringTypeDefinitionSubjectId = GetDeclaringTypeSubjectId(dispatchSlotMethod.Identity.DefinitionSubjectId);
        var slotSignatureSuffix = GetMethodSignatureSuffix(dispatchSlotMethod.SubjectId);

        return _methodsBySubjectId.Values
            .Where(method =>
                !method.IsStatic &&
                CanEmitMethodBody(method) &&
                string.Equals(GetMethodSignatureSuffix(method.SubjectId), slotSignatureSuffix, StringComparison.Ordinal) &&
                IsTypeCompatibleWithSlot(
                    method.Identity.DeclaringTypeSubjectId,
                    GetDeclaringTypeSubjectId(method.Identity.DefinitionSubjectId),
                    slotDeclaringTypeSubjectId,
                    slotDeclaringTypeDefinitionSubjectId))
            .OrderBy(method => method.SubjectId, StringComparer.Ordinal)
            .ToArray();
    }

    private AotCoreIrMethodArtifact ResolveRequiredDispatchSlotMethod(AotCoreIrInstructionArtifact instruction)
    {
        var subjectId = !string.IsNullOrWhiteSpace(instruction.Callee)
            ? instruction.Callee
            : instruction.TargetReference?.SubjectId;
        if (string.IsNullOrWhiteSpace(subjectId) ||
            !_methodsBySubjectId.TryGetValue(subjectId, out var dispatchSlotMethod))
        {
            throw new NotSupportedException(
                $"native-aot lowering does not support unresolved dispatch slot '{instruction.Callee ?? instruction.TargetReference?.SubjectId ?? "<null>"}'.");
        }

        return dispatchSlotMethod;
    }

    private InvocationTarget ResolveDirectInvocationTarget(AotCoreIrInstructionArtifact instruction)
    {
        if (!string.IsNullOrWhiteSpace(instruction.Callee) &&
            TryGetLowerableMethod(instruction.Callee) is { } lowerableMethod)
        {
            return new InvocationTarget(
                lowerableMethod.NativeSymbol,
                GetMethodAbiParameterSlots(lowerableMethod),
                lowerableMethod.ReturnAbi);
        }

        return new InvocationTarget(
            GetRequiredTargetSymbol(instruction),
            CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction)),
            CreateLegacyReturnAbiSlot(instruction.TargetReturnType));
    }

    private AotCoreIrMethodArtifact? TryGetLowerableMethod(string? subjectId)
    {
        if (string.IsNullOrWhiteSpace(subjectId) ||
            !_methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !CanEmitMethodBody(method))
        {
            return null;
        }

        return method;
    }

    private bool IsTypeCompatibleWithSlot(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        var currentTypeSubjectId = candidateTypeSubjectId;
        var currentTypeDefinitionSubjectId = candidateTypeDefinitionSubjectId;
        while (!string.IsNullOrWhiteSpace(currentTypeSubjectId))
        {
            if (MatchesTypeSubjectId(
                    currentTypeSubjectId,
                    currentTypeDefinitionSubjectId,
                    slotDeclaringTypeSubjectId,
                    slotDeclaringTypeDefinitionSubjectId) ||
                ImplementsInterface(
                    currentTypeSubjectId,
                    currentTypeDefinitionSubjectId,
                    slotDeclaringTypeSubjectId,
                    slotDeclaringTypeDefinitionSubjectId))
            {
                return true;
            }

            if (!TryGetBaseTypeSubjectId(
                    currentTypeSubjectId,
                    currentTypeDefinitionSubjectId,
                    out var baseTypeSubjectId) ||
                string.IsNullOrWhiteSpace(baseTypeSubjectId))
            {
                break;
            }

            currentTypeSubjectId = baseTypeSubjectId;
            currentTypeDefinitionSubjectId = baseTypeSubjectId;
        }

        return false;
    }

    private bool ImplementsInterface(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        return ImplementsInterface(candidateTypeSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId) ||
               ImplementsInterface(candidateTypeDefinitionSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId);
    }

    private bool ImplementsInterface(
        string candidateTypeSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        if (string.IsNullOrWhiteSpace(candidateTypeSubjectId) ||
            !_referenceTypeImplementedInterfaceSubjectIds.TryGetValue(candidateTypeSubjectId, out var implementedInterfaceSubjectIds))
        {
            return false;
        }

        return implementedInterfaceSubjectIds.Any(interfaceSubjectId =>
            IsEquivalentTypeSubjectId(interfaceSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId));
    }

    private bool TryGetBaseTypeSubjectId(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        out string? baseTypeSubjectId)
    {
        if (_referenceTypeBaseSubjectIds.TryGetValue(candidateTypeSubjectId, out baseTypeSubjectId))
        {
            return true;
        }

        if (_referenceTypeBaseSubjectIds.TryGetValue(candidateTypeDefinitionSubjectId, out baseTypeSubjectId))
        {
            return true;
        }

        baseTypeSubjectId = null;
        return false;
    }

    private static bool MatchesTypeSubjectId(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        return IsEquivalentTypeSubjectId(candidateTypeSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId) ||
               IsEquivalentTypeSubjectId(candidateTypeDefinitionSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId);
    }

    private static bool IsEquivalentTypeSubjectId(
        string? candidateTypeSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        return !string.IsNullOrWhiteSpace(candidateTypeSubjectId) &&
               (string.Equals(candidateTypeSubjectId, slotDeclaringTypeSubjectId, StringComparison.Ordinal) ||
                string.Equals(candidateTypeSubjectId, slotDeclaringTypeDefinitionSubjectId, StringComparison.Ordinal));
    }

    private static string FormatMethodDeclaration(AotCoreIrMethodArtifact method)
    {
        return $"extern \"C\" {MapAbiSlotReturnType(method.ReturnAbi)} {method.NativeSymbol}({FormatAbiSlotParameterSignature(GetMethodAbiParameterSlots(method))});";
    }

    private void EmitManagedMethod(
        StringBuilder builder,
        AotCoreIrMethodArtifact method)
    {
        ValidateMethod(method);

        var instructions = method.Instructions;
        ValidateInstructions(method, instructions);
        var abiParameterSlots = GetMethodAbiParameterSlots(method);
        var stackCapacity = Math.Max(instructions.Count, 1);
        var nextOffsetsByIlOffset = CreateNextOffsets(instructions);
        var offsets = instructions
            .Select(GetRequiredIlOffset)
            .ToHashSet();

        builder.AppendLine($"// Managed method: {method.SubjectId}");
        builder.AppendLine(
            $"extern \"C\" {MapAbiSlotReturnType(method.ReturnAbi)} {method.NativeSymbol}({FormatAbiSlotParameterSignature(abiParameterSlots)})");
        builder.AppendLine("{");
        builder.AppendLine($"    std::array<std::intptr_t, {Math.Max(abiParameterSlots.Count, 1)}> chaos_args{{}};");
        builder.AppendLine($"    std::array<std::intptr_t, {Math.Max(method.LocalCount, 1)}> chaos_locals{{}};");
        builder.AppendLine($"    std::array<std::intptr_t, {stackCapacity}> chaos_eval_stack{{}};");
        builder.AppendLine("    std::size_t chaos_stack_top = 0;");
        EmitAbiArgumentInitialization(builder, abiParameterSlots);
        builder.AppendLine();

        if (TryCreateCatchOnlyExceptionMethodShape(method, out var catchOnlyShape))
        {
            EmitCatchOnlyExceptionMethodBody(
                builder,
                method,
                catchOnlyShape!,
                nextOffsetsByIlOffset,
                offsets);
            builder.AppendLine("}");
            return;
        }

        if (TryCreateFilterAndFinallyExceptionMethodShape(method, out var filterAndFinallyShape))
        {
            EmitFilterAndFinallyExceptionMethodBody(
                builder,
                method,
                filterAndFinallyShape!,
                nextOffsetsByIlOffset,
                offsets);
            builder.AppendLine("}");
            return;
        }

        var firstOffset = GetRequiredIlOffset(instructions[0]);
        builder.AppendLine($"    goto chaos_ip_{firstOffset};");
        builder.AppendLine();
        EmitInstructionRange(builder, method, instructions, nextOffsetsByIlOffset, offsets);
        builder.AppendLine("}");
    }

    private static void ValidateEntryMethod(AotCoreIrMethodArtifact entryMethod)
    {
        ValidateMethod(entryMethod);

        if (entryMethod.ParameterCount != 0)
        {
            throw new NotSupportedException(
                $"native-aot entry '{entryMethod.SubjectId}' must not take parameters");
        }

        if (!string.Equals(entryMethod.ReturnType, "System.Int32", StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"native-aot entry '{entryMethod.SubjectId}' must return System.Int32");
        }
    }

    private static void ValidateMethod(AotCoreIrMethodArtifact method)
    {
        if (string.IsNullOrWhiteSpace(method.NativeSymbol))
        {
            throw new InvalidOperationException(
                $"native-aot method '{method.SubjectId}' is missing native symbol metadata");
        }

        _ = MapAbiSlotReturnType(method.ReturnAbi);

        if (method.ExceptionRegionCount != 0 &&
            !TryCreateCatchOnlyExceptionMethodShape(method, out _) &&
            !TryCreateFilterAndFinallyExceptionMethodShape(method, out _))
        {
            throw new NotSupportedException(
                $"native-aot method '{method.SubjectId}' does not support current exception region shape");
        }
    }

    private static void ValidateInstructions(
        AotCoreIrMethodArtifact entryMethod,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        if (instructions.Count == 0)
        {
            throw new InvalidOperationException(
                $"native-aot entry '{entryMethod.SubjectId}' does not contain instructions");
        }

        var seenOffsets = new HashSet<int>();
        foreach (var instruction in instructions)
        {
            var offset = GetRequiredIlOffset(instruction);
            if (!seenOffsets.Add(offset))
            {
                throw new InvalidOperationException(
                    $"native-aot entry '{entryMethod.SubjectId}' contains duplicate IL offset {offset}");
            }
        }
    }

    private void EmitInstruction(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        IReadOnlySet<int> offsets)
    {
        var targetReference = instruction.TargetReference;
        if (targetReference?.GenericContext is not null)
        {
            builder.AppendLine($"    {FormatGenericContextComment(targetReference.GenericContext)}");
        }

        switch (instruction.Op)
        {
            case "ldc.i4":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>({FormatInt32Literal(GetRequiredIntOperand(instruction))});");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldc.i8":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_store_int64({FormatInt64Literal(GetRequiredInt64Operand(instruction))});");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldc.r4":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_store_float32({FormatFloat32Literal(GetRequiredSingleOperand(instruction))});");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldc.r8":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64({FormatFloat64Literal(GetRequiredDoubleOperand(instruction))});");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldarg":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_args[{GetRequiredIntOperand(instruction)}];");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldarga":
                EmitLoadArgumentAddress(builder, method, instruction, nextOffset, instruction.Op);
                return;

            case "ldnull":
                builder.AppendLine("    chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(0);");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "dup":
                builder.AppendLine("    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];");
                builder.AppendLine("    chaos_stack_top++;");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "conv.i4":
                builder.AppendLine(
                    "    chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(static_cast<std::int32_t>(chaos_eval_stack[chaos_stack_top - 1]));");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "conv.i1":
                EmitStackTopConversion(builder, "std::int8_t", nextOffset, instruction.Op);
                return;

            case "conv.i2":
                EmitStackTopConversion(builder, "std::int16_t", nextOffset, instruction.Op);
                return;

            case "conv.i8":
                EmitStackTopConversion(builder, "std::int64_t", nextOffset, instruction.Op);
                return;

            case "conv.r4":
                EmitStackTopFloatingPointConversion(builder, "float", "chaos_store_float32", nextOffset, instruction.Op);
                return;

            case "conv.r8":
                EmitStackTopFloatingPointConversion(builder, "double", "chaos_store_float64", nextOffset, instruction.Op);
                return;

            case "conv.u":
                EmitStackTopConversion(builder, "std::uintptr_t", nextOffset, instruction.Op);
                return;

            case "conv.u1":
                EmitStackTopConversion(builder, "std::uint8_t", nextOffset, instruction.Op);
                return;

            case "conv.u2":
                EmitStackTopConversion(builder, "std::uint16_t", nextOffset, instruction.Op);
                return;

            case "conv.ovf.i1":
                EmitCheckedStackTopConversion(
                    builder,
                    "std::int8_t",
                    "std::numeric_limits<std::int8_t>::min()",
                    "std::numeric_limits<std::int8_t>::max()",
                    nextOffset,
                    instruction.Op);
                return;

            case "conv.ovf.u1":
                EmitCheckedStackTopConversion(
                    builder,
                    "std::uint8_t",
                    "0",
                    "std::numeric_limits<std::uint8_t>::max()",
                    nextOffset,
                    instruction.Op);
                return;

            case "cgt.un":
                builder.AppendLine("    {");
                builder.AppendLine(
                    "        const auto chaos_right = static_cast<std::uintptr_t>(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(
                    "        const auto chaos_left = static_cast<std::uintptr_t>(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(
                    "        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(chaos_left > chaos_right ? 1 : 0);");
                builder.AppendLine("    }");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldloc":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_locals[{GetRequiredIntOperand(instruction)}];");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "pop":
                builder.AppendLine("    chaos_stack_top--;");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldloca":
                EmitLoadLocalAddress(builder, instruction, nextOffset, instruction.Op);
                return;

            case "stloc":
                builder.AppendLine(
                    $"    chaos_locals[{GetRequiredIntOperand(instruction)}] = chaos_eval_stack[--chaos_stack_top];");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "add":
                EmitBinaryArithmetic(builder, "chaos_wrap_add", nextOffset, instruction.Op);
                return;

            case "add.ovf":
                EmitOverflowingBinaryArithmetic(builder, instruction, "+", nextOffset, instruction.Op);
                return;

            case "sub":
                EmitBinaryArithmetic(builder, "chaos_wrap_sub", nextOffset, instruction.Op);
                return;

            case "sub.ovf":
                EmitOverflowingBinaryArithmetic(builder, instruction, "-", nextOffset, instruction.Op);
                return;

            case "mul":
                EmitBinaryArithmetic(builder, "chaos_wrap_mul", nextOffset, instruction.Op);
                return;

            case "mul.ovf":
                EmitOverflowingBinaryArithmetic(builder, instruction, "*", nextOffset, instruction.Op);
                return;

            case "div":
                EmitBinaryArithmetic(builder, "chaos_div", nextOffset, instruction.Op);
                return;

            case "rem":
                EmitBinaryArithmetic(builder, "chaos_rem", nextOffset, instruction.Op);
                return;

            case "shl":
                EmitShift(builder, instruction, "chaos_shift_left_int32", nextOffset, instruction.Op);
                return;

            case "shr":
                EmitShift(builder, instruction, "chaos_shift_right_int32", nextOffset, instruction.Op);
                return;

            case "shr.un":
                EmitShift(builder, instruction, "chaos_shift_right_un_int32", nextOffset, instruction.Op);
                return;

            case "not":
                EmitBitwiseNot(builder, instruction, nextOffset, instruction.Op);
                return;

            case "and":
                EmitBinaryBitwise(builder, "&", nextOffset, instruction.Op);
                return;

            case "or":
                EmitBinaryBitwise(builder, "|", nextOffset, instruction.Op);
                return;

            case "xor":
                EmitBinaryBitwise(builder, "^", nextOffset, instruction.Op);
                return;

            case "call":
                EmitDirectCall(builder, instruction, nextOffset, instruction.Op);
                return;

            case "callvirt":
                EmitCallVirt(builder, instruction, nextOffset, instruction.Op);
                return;

            case "newobj":
                EmitNewObject(builder, instruction, nextOffset, instruction.Op);
                return;

            case "throw":
                builder.AppendLine("    throw chaos_managed_exception{chaos_eval_stack[--chaos_stack_top]};");
                return;

            case "newarr":
                EmitNewArray(builder, instruction, nextOffset, instruction.Op);
                return;

            case "ldlen":
                EmitArrayLength(builder, nextOffset, instruction.Op);
                return;

            case "ldelema":
                EmitArrayElementAddress(builder, instruction, nextOffset, instruction.Op);
                return;

            case "castclass":
                EmitCastClass(builder, instruction, nextOffset, instruction.Op);
                return;

            case "isinst":
                EmitIsInst(builder, instruction, nextOffset, instruction.Op);
                return;

            case "ldfld":
            case "ldsfld":
                EmitFieldLoad(builder, instruction, nextOffset, instruction.Op);
                return;

            case "stfld":
            case "stsfld":
                EmitFieldStore(builder, instruction, nextOffset, instruction.Op);
                return;

            case "ldelem":
            case "ldelem.ref":
                EmitArrayLoad(builder, nextOffset, instruction.Op);
                return;

            case "stelem":
            case "stelem.ref":
                EmitArrayStore(builder, nextOffset, instruction.Op);
                return;

            case "ldind.i4":
                EmitLoadIndirectInt32(builder, nextOffset, instruction.Op);
                return;

            case "stind.i4":
                EmitStoreIndirectInt32(builder, nextOffset, instruction.Op);
                return;

            case "box":
                EmitBox(builder, instruction, nextOffset, instruction.Op);
                return;

            case "unbox":
                EmitUnbox(builder, instruction, nextOffset, instruction.Op);
                return;

            case "unbox.any":
                EmitUnboxAny(builder, instruction, nextOffset, instruction.Op);
                return;

            case "initobj":
                EmitInitObj(builder, instruction, nextOffset, instruction.Op);
                return;

            case "ldobj":
                EmitLoadObjectValue(builder, instruction, nextOffset, instruction.Op);
                return;

            case "stobj":
                EmitStoreObjectValue(builder, instruction, nextOffset, instruction.Op);
                return;

            case "br":
                builder.AppendLine($"    goto chaos_ip_{GetRequiredBranchTarget(instruction, offsets)};");
                return;

            case "brtrue":
                EmitTruthBranch(builder, shouldBranchWhenNonZero: true, instruction, nextOffset, offsets);
                return;

            case "leave":
                builder.AppendLine($"    goto chaos_ip_{GetRequiredBranchTarget(instruction, offsets)};");
                return;

            case "beq":
                EmitComparisonBranch(builder, "==", instruction, nextOffset, offsets);
                return;

            case "blt":
                EmitComparisonBranch(builder, "<", instruction, nextOffset, offsets);
                return;

            case "bgt":
                EmitComparisonBranch(builder, ">", instruction, nextOffset, offsets);
                return;

            case "ble":
                EmitComparisonBranch(builder, "<=", instruction, nextOffset, offsets);
                return;

            case "bge":
                EmitComparisonBranch(builder, ">=", instruction, nextOffset, offsets);
                return;

            case "bge.un":
                EmitUnsignedComparisonBranch(builder, ">=", instruction, nextOffset, offsets);
                return;

            case "bne.un":
                EmitComparisonBranch(builder, "!=", instruction, nextOffset, offsets);
                return;

            case "ret":
                EmitMethodReturn(builder, method.ReturnAbi);
                return;

            default:
                throw new NotSupportedException(
                    $"native-aot lowering does not support opcode '{instruction.Op}'");
        }
    }

    private void EmitDirectCall(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var invocationTarget = ResolveDirectInvocationTarget(instruction);
        EmitResolvedInvocation(
            builder,
            invocationTarget.TargetSymbol,
            invocationTarget.ParameterAbis,
            invocationTarget.ReturnAbi,
            nextOffset,
            op,
            enforceInstanceNullCheck: false);
    }

    private void EmitCallVirt(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        switch (instruction.DispatchKindCode ?? HybridDispatchKind.None)
        {
            case HybridDispatchKind.None:
            case HybridDispatchKind.Direct:
                var directTarget = ResolveDirectInvocationTarget(instruction);
                EmitResolvedInvocation(
                    builder,
                    directTarget.TargetSymbol,
                    directTarget.ParameterAbis,
                    directTarget.ReturnAbi,
                    nextOffset,
                    op,
                    enforceInstanceNullCheck: true);
                return;

            case HybridDispatchKind.Virtual:
                EmitVirtualDispatchCall(builder, instruction, nextOffset, op);
                return;

            default:
                throw new NotSupportedException(
                    $"native-aot lowering does not support callvirt dispatch kind '{instruction.DispatchKindCode}'.");
        }
    }

    private static void EmitResolvedInvocation(
        StringBuilder builder,
        string targetSymbol,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis,
        AotCoreIrAbiSlotArtifact returnAbi,
        int? nextOffset,
        string op,
        bool enforceInstanceNullCheck)
    {
        var mappedReturnType = MapAbiSlotReturnType(returnAbi);

        builder.AppendLine("    {");
        for (var parameterIndex = parameterAbis.Count - 1; parameterIndex >= 0; parameterIndex--)
        {
            builder.AppendLine(
                $"        const auto chaos_raw_arg_{parameterIndex} = chaos_eval_stack[--chaos_stack_top];");
            builder.AppendLine(
                $"        const auto chaos_arg_{parameterIndex} = {FormatInboundAbiArgumentExpression(parameterAbis[parameterIndex], $"chaos_raw_arg_{parameterIndex}")};");
        }

        if (enforceInstanceNullCheck && parameterAbis.Count > 0)
        {
            builder.AppendLine("        if (chaos_arg_0 == static_cast<std::intptr_t>(0))");
            builder.AppendLine("        {");
            builder.AppendLine("            std::abort();");
            builder.AppendLine("        }");
        }

        var invocation = $"{targetSymbol}({FormatAbiInvocationArgumentList(parameterAbis)})";
        if (string.Equals(mappedReturnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"        {invocation};");
        }
        else
        {
            builder.AppendLine($"        const auto chaos_result = {invocation};");
            EmitAbiReturnPush(builder, returnAbi, "chaos_result", "        ");
        }

        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private void EmitVirtualDispatchCall(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var dispatchSlotMethod = ResolveRequiredDispatchSlotMethod(instruction);
        var dispatchTargets = ResolveVirtualDispatchTargets(instruction);
        if (dispatchTargets.Count == 0)
        {
            throw new NotSupportedException(
                $"native-aot lowering could not resolve virtual dispatch targets for '{instruction.Callee ?? dispatchSlotMethod.SubjectId}'.");
        }

        var abiParameterSlots = GetMethodAbiParameterSlots(dispatchSlotMethod);
        var mappedReturnType = MapAbiSlotReturnType(dispatchSlotMethod.ReturnAbi);
        var dispatchLabel = $"chaos_callvirt_done_{instruction.IlOffset}";

        builder.AppendLine("    {");
        for (var parameterIndex = abiParameterSlots.Count - 1; parameterIndex >= 0; parameterIndex--)
        {
            builder.AppendLine(
                $"        const auto chaos_raw_arg_{parameterIndex} = chaos_eval_stack[--chaos_stack_top];");
            builder.AppendLine(
                $"        const auto chaos_arg_{parameterIndex} = {FormatInboundAbiArgumentExpression(abiParameterSlots[parameterIndex], $"chaos_raw_arg_{parameterIndex}")};");
        }

        builder.AppendLine("        const auto chaos_instance = chaos_arg_0;");
        builder.AppendLine("        if (chaos_instance == static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_instance);");
        if (!string.Equals(mappedReturnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"        {mappedReturnType} chaos_callvirt_result{{}};");
        }

        builder.AppendLine("        switch (chaos_header->type_id)");
        builder.AppendLine("        {");
        foreach (var dispatchTarget in dispatchTargets)
        {
            builder.AppendLine($"            case {GetNativeTypeIdSymbol(dispatchTarget.Identity.DeclaringTypeSubjectId)}:");
            if (string.Equals(mappedReturnType, "void", StringComparison.Ordinal))
            {
                builder.AppendLine(
                    $"                {dispatchTarget.NativeSymbol}({FormatAbiInvocationArgumentList(abiParameterSlots, "chaos_instance")});");
            }
            else
            {
                builder.AppendLine(
                    $"                chaos_callvirt_result = {dispatchTarget.NativeSymbol}({FormatAbiInvocationArgumentList(abiParameterSlots, "chaos_instance")});");
            }

            builder.AppendLine($"                goto {dispatchLabel};");
        }

        builder.AppendLine("            default:");
        builder.AppendLine("                break;");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        auto chaos_current_type_id = chaos_get_base_type_id(chaos_header->type_id);");
        builder.AppendLine("        while (chaos_current_type_id != static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            switch (chaos_current_type_id)");
        builder.AppendLine("            {");
        foreach (var dispatchTarget in dispatchTargets)
        {
            builder.AppendLine($"                case {GetNativeTypeIdSymbol(dispatchTarget.Identity.DeclaringTypeSubjectId)}:");
            if (string.Equals(mappedReturnType, "void", StringComparison.Ordinal))
            {
                builder.AppendLine(
                    $"                    {dispatchTarget.NativeSymbol}({FormatAbiInvocationArgumentList(abiParameterSlots, "chaos_instance")});");
            }
            else
            {
                builder.AppendLine(
                    $"                    chaos_callvirt_result = {dispatchTarget.NativeSymbol}({FormatAbiInvocationArgumentList(abiParameterSlots, "chaos_instance")});");
            }

            builder.AppendLine($"                    goto {dispatchLabel};");
        }

        builder.AppendLine("                default:");
        builder.AppendLine("                    break;");
        builder.AppendLine("            }");
        builder.AppendLine();
        builder.AppendLine("            chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        std::abort();");
        builder.AppendLine($"{dispatchLabel}:");
        if (!string.Equals(mappedReturnType, "void", StringComparison.Ordinal))
        {
            EmitAbiReturnPush(builder, dispatchSlotMethod.ReturnAbi, "chaos_callvirt_result", "        ");
        }

        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitBinaryArithmetic(
        StringBuilder builder,
        string helperName,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_right = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        const auto chaos_left = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>({helperName}(chaos_left, chaos_right));");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitBinaryBitwise(
        StringBuilder builder,
        string operation,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_right = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        const auto chaos_left = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"        chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64(chaos_left {operation} chaos_right);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitShift(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        string helperName,
        int? nextOffset,
        string op)
    {
        RequireInt32IntegralResultType(instruction);
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_shift = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>({helperName}(chaos_value, chaos_shift));");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitBitwiseNot(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        RequireInt32IntegralResultType(instruction);
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[chaos_stack_top - 1]);");
        builder.AppendLine("        chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(~chaos_value);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitStackTopConversion(
        StringBuilder builder,
        string castType,
        int? nextOffset,
        string op)
    {
        builder.AppendLine(
            $"    chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(static_cast<{castType}>(chaos_eval_stack[chaos_stack_top - 1]));");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitStackTopFloatingPointConversion(
        StringBuilder builder,
        string castType,
        string storeHelperName,
        int? nextOffset,
        string op)
    {
        builder.AppendLine(
            $"    chaos_eval_stack[chaos_stack_top - 1] = {storeHelperName}(static_cast<{castType}>(chaos_eval_stack[chaos_stack_top - 1]));");
        AppendGotoNext(builder, nextOffset, op);
    }

    private void EmitCheckedStackTopConversion(
        StringBuilder builder,
        string castType,
        string minValueExpression,
        string maxValueExpression,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = chaos_eval_stack[chaos_stack_top - 1];");
        builder.AppendLine(
            $"        if (chaos_value < static_cast<std::intptr_t>({minValueExpression}) ||");
        builder.AppendLine(
            $"            chaos_value > static_cast<std::intptr_t>({maxValueExpression}))");
        builder.AppendLine("        {");
        EmitThrowOverflowException(builder, "            ");
        builder.AppendLine("        }");
        builder.AppendLine(
            $"        chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(static_cast<{castType}>(chaos_value));");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private void EmitOverflowingBinaryArithmetic(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        string operation,
        int? nextOffset,
        string op)
    {
        RequireInt32IntegralResultType(instruction);
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_right = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        const auto chaos_left = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"        const auto chaos_wide = static_cast<std::int64_t>(chaos_left) {operation} static_cast<std::int64_t>(chaos_right);");
        builder.AppendLine("        if (chaos_wide < static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min()) ||");
        builder.AppendLine("            chaos_wide > static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()))");
        builder.AppendLine("        {");
        EmitThrowOverflowException(builder, "            ");
        builder.AppendLine("        }");
        builder.AppendLine(
            "        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(static_cast<std::int32_t>(chaos_wide));");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private void EmitThrowOverflowException(
        StringBuilder builder,
        string indentation)
    {
        builder.AppendLine(
            $"{indentation}auto* chaos_exception = new {GetNativeTypeSymbol(OverflowExceptionTypeSubjectId)}{{}};");
        builder.AppendLine(
            $"{indentation}chaos_exception->header.type_id = {GetNativeTypeIdSymbol(OverflowExceptionTypeSubjectId)};");
        builder.AppendLine(
            $"{indentation}throw chaos_managed_exception{{reinterpret_cast<std::intptr_t>(chaos_exception)}};");
    }

    private static void EmitLoadLocalAddress(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        builder.AppendLine(
            $"    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_locals[{GetRequiredIntOperand(instruction)}]) | chaos_managed_pointer_local_slot_tag;");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitLoadArgumentAddress(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var argumentIndex = GetRequiredIntOperand(instruction);
        var argumentAbi = GetRequiredMethodAbiParameterSlot(method, argumentIndex);
        switch (argumentAbi.CarrierKindCode)
        {
            case AotCoreIrAbiCarrierKind.NativeInt:
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_args[{argumentIndex}]) | chaos_managed_pointer_local_slot_tag;");
                break;

            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_args[{argumentIndex}];");
                break;

            default:
                throw new NotSupportedException(
                    $"native-aot lowering does not support ldarga for ABI carrier '{argumentAbi.CarrierKindCode}'.");
        }

        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitNewObject(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot newobj requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine(
            $"    auto* chaos_object = new {GetNativeTypeSymbol(targetReference.SubjectId)}{{}};");
        builder.AppendLine(
            $"    chaos_object->header.type_id = {GetNativeTypeIdSymbol(targetReference.SubjectId)};");
        builder.AppendLine(
            "    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_object);");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitNewArray(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot newarr requires type target reference, got '{targetReference.Kind}'.");
        }

        var elementSubjectId = HasArrayElementReference(targetReference)
            ? targetReference.ArrayElementSubjectId
            : targetReference.SubjectId;
        var elementTypeShape = HasArrayElementReference(targetReference)
            ? targetReference.ArrayElementTypeShape
            : targetReference.TypeShape;

        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_length = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        if (chaos_length < 0)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        auto* chaos_array = new chaos_managed_array{};");
        builder.AppendLine("        chaos_array->header.type_id = chaos_type_id_managed_array;");
        builder.AppendLine(
            $"        chaos_array->element_type_shape = {GetNativeTypeShapeValue(elementTypeShape)};");
        builder.AppendLine(
            $"        chaos_array->element_type_id = {GetRuntimeTypeIdExpression(elementSubjectId, elementTypeShape)};");
        builder.AppendLine("        chaos_array->length = static_cast<std::intptr_t>(chaos_length);");
        builder.AppendLine(
            "        chaos_array->elements = chaos_length == 0 ? nullptr : new std::intptr_t[static_cast<std::size_t>(chaos_length)]{};");
        builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_array);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitArrayLength(
        StringBuilder builder,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        if (chaos_array == nullptr)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_array->length;");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitArrayElementAddress(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot ldelema requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_index = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        if (chaos_array == nullptr)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        if (chaos_index < 0 || static_cast<std::intptr_t>(chaos_index) >= chaos_array->length)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine(
            "        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_array->elements[static_cast<std::size_t>(chaos_index)]);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitCastClass(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot castclass requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = chaos_eval_stack[chaos_stack_top - 1];");
        builder.AppendLine("        if (chaos_value != static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
        if (HasArrayElementReference(targetReference))
        {
            builder.AppendLine("            if (chaos_header->type_id != chaos_type_id_managed_array)");
            builder.AppendLine("            {");
            builder.AppendLine("                std::abort();");
            builder.AppendLine("            }");
            builder.AppendLine("            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
            builder.AppendLine(
                $"            if (!chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_id, {GetNativeTypeShapeValue(targetReference.ArrayElementTypeShape)}, {GetRuntimeTypeIdExpression(targetReference.ArrayElementSubjectId, targetReference.ArrayElementTypeShape)}))");
        }
        else if (targetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
        {
            builder.AppendLine(
                $"            if (!chaos_does_type_implement_interface(chaos_header->type_id, {GetNativeTypeIdSymbol(targetReference.SubjectId)}))");
        }
        else if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
        {
            builder.AppendLine(
                $"            if (!chaos_is_type_compatible(chaos_header->type_id, {GetNativeTypeIdSymbol(targetReference.SubjectId)}))");
        }
        else
        {
            builder.AppendLine(
                $"            if (chaos_header->type_id != {GetNativeTypeIdSymbol(targetReference.SubjectId)})");
        }
        builder.AppendLine("            {");
        builder.AppendLine("                std::abort();");
        builder.AppendLine("            }");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitIsInst(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot isinst requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine("        auto chaos_matches = false;");
        builder.AppendLine("        if (chaos_value != static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
        if (HasArrayElementReference(targetReference))
        {
            builder.AppendLine("            if (chaos_header->type_id == chaos_type_id_managed_array)");
            builder.AppendLine("            {");
            builder.AppendLine("                auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
            builder.AppendLine(
                $"                chaos_matches = chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_id, {GetNativeTypeShapeValue(targetReference.ArrayElementTypeShape)}, {GetRuntimeTypeIdExpression(targetReference.ArrayElementSubjectId, targetReference.ArrayElementTypeShape)});");
            builder.AppendLine("            }");
        }
        else if (targetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
        {
            builder.AppendLine(
                $"            chaos_matches = chaos_does_type_implement_interface(chaos_header->type_id, {GetNativeTypeIdSymbol(targetReference.SubjectId)});");
        }
        else if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
        {
            builder.AppendLine(
                $"            chaos_matches = chaos_is_type_compatible(chaos_header->type_id, {GetNativeTypeIdSymbol(targetReference.SubjectId)});");
        }
        else
        {
            builder.AppendLine(
                $"            chaos_matches = chaos_header->type_id == {GetNativeTypeIdSymbol(targetReference.SubjectId)};");
        }
        builder.AppendLine("        }");
        builder.AppendLine(
            "        chaos_eval_stack[chaos_stack_top++] = chaos_matches ? chaos_value : static_cast<std::intptr_t>(0);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitFieldLoad(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Field)
        {
            throw new NotSupportedException(
                $"native-aot field load requires field target reference, got '{targetReference.Kind}'.");
        }

        if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
        {
            builder.AppendLine(
                $"    chaos_eval_stack[chaos_stack_top++] = {GetNativeStaticFieldSymbol(targetReference.SubjectId)};");
            AppendGotoNext(builder, nextOffset, op);
            return;
        }

        builder.AppendLine("    {");
        if (targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
        {
            var declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(targetReference);
            builder.AppendLine(
                $"        auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(declaringTypeSubjectId)}>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine(
                $"        chaos_eval_stack[chaos_stack_top++] = chaos_value->{GetNativeFieldMemberName(targetReference.SubjectId)};");
        }
        else
        {
            var declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(targetReference);
            builder.AppendLine(
                $"        auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine(
                $"        chaos_eval_stack[chaos_stack_top++] = chaos_object->{GetNativeFieldMemberName(targetReference.SubjectId)};");
        }

        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitFieldStore(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Field)
        {
            throw new NotSupportedException(
                $"native-aot field store requires field target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
        if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.StoreStaticField)
        {
            builder.AppendLine($"        {GetNativeStaticFieldSymbol(targetReference.SubjectId)} = chaos_value;");
        }
        else
        {
            var declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(targetReference);
            if (targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
            {
                builder.AppendLine(
                    $"        auto* chaos_value_owner = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(declaringTypeSubjectId)}>(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(
                    $"        chaos_value_owner->{GetNativeFieldMemberName(targetReference.SubjectId)} = chaos_value;");
            }
            else
            {
                builder.AppendLine(
                    $"        auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(
                    $"        chaos_object->{GetNativeFieldMemberName(targetReference.SubjectId)} = chaos_value;");
            }
        }

        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitArrayLoad(
        StringBuilder builder,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_index = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        if (chaos_array == nullptr)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        if (chaos_index < 0 || static_cast<std::intptr_t>(chaos_index) >= chaos_array->length)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine(
            "        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<std::size_t>(chaos_index)];");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitArrayStore(
        StringBuilder builder,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine("        const auto chaos_index = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        if (chaos_array == nullptr)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        if (chaos_index < 0 || static_cast<std::intptr_t>(chaos_index) >= chaos_array->length)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        if (string.Equals(op, "stelem.ref", StringComparison.Ordinal))
        {
            builder.AppendLine("        if (!chaos_is_array_store_compatible(chaos_array, chaos_value))");
            builder.AppendLine("        {");
            builder.AppendLine("            std::abort();");
            builder.AppendLine("        }");
        }
        builder.AppendLine(
            "        chaos_array->elements[static_cast<std::size_t>(chaos_index)] = chaos_value;");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitLoadIndirectInt32(
        StringBuilder builder,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine("        if (chaos_address == static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine(
            "        const auto chaos_value = static_cast<std::int32_t>(*reinterpret_cast<std::intptr_t*>(chaos_address));");
        builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(chaos_value);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitStoreIndirectInt32(
        StringBuilder builder,
        int? nextOffset,
        string op)
    {
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine("        if (chaos_address == static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        *reinterpret_cast<std::intptr_t*>(chaos_address) = static_cast<std::intptr_t>(chaos_value);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitBox(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot box requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine(
            $"        auto* chaos_boxed = new {GetNativeBoxTypeSymbol(targetReference.SubjectId)}{{}};");
        builder.AppendLine(
            $"        chaos_boxed->header.type_id = {GetNativeBoxTypeIdSymbol(targetReference.SubjectId)};");
        if (RequiresStructuredValueTypePayload(targetReference))
        {
            builder.AppendLine(
                $"        auto* chaos_payload = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(targetReference.SubjectId)}>(chaos_value);");
            builder.AppendLine("        chaos_boxed->value = *chaos_payload;");
        }
        else
        {
            builder.AppendLine("        chaos_boxed->value = chaos_value;");
        }

        builder.AppendLine(
            "        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_boxed);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitUnbox(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot unbox requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        builder.AppendLine(
            $"        auto* chaos_boxed = reinterpret_cast<{GetNativeBoxTypeSymbol(targetReference.SubjectId)}*>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        if (chaos_boxed == nullptr)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_boxed->value);");
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitUnboxAny(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot unbox.any requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        builder.AppendLine(
            $"        auto* chaos_boxed = reinterpret_cast<{GetNativeBoxTypeSymbol(targetReference.SubjectId)}*>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine("        if (chaos_boxed == nullptr)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        if (RequiresStructuredValueTypePayload(targetReference))
        {
            builder.AppendLine(
                $"        auto* chaos_value = new {GetNativeValueTypeSymbol(targetReference.SubjectId)}{{}};");
            builder.AppendLine("        *chaos_value = chaos_boxed->value;");
            builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_value);");
        }
        else
        {
            builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_boxed->value;");
        }
        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitLoadObjectValue(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot ldobj requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        if (RequiresStructuredValueTypePayload(targetReference))
        {
            builder.AppendLine(
                $"        auto* chaos_source = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(targetReference.SubjectId)}>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine(
                $"        auto* chaos_value = new {GetNativeValueTypeSymbol(targetReference.SubjectId)}{{}};");
            builder.AppendLine("        *chaos_value = *chaos_source;");
            builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_value);");
        }
        else
        {
            builder.AppendLine(
                "        auto* chaos_source = chaos_resolve_managed_value_pointer<std::intptr_t>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = *chaos_source;");
        }

        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitStoreObjectValue(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot stobj requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        if (RequiresStructuredValueTypePayload(targetReference))
        {
            builder.AppendLine(
                $"        auto* chaos_source = reinterpret_cast<{GetNativeValueTypeSymbol(targetReference.SubjectId)}*>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine(
                $"        auto* chaos_destination = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(targetReference.SubjectId)}>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine("        if (chaos_source == nullptr)");
            builder.AppendLine("        {");
            builder.AppendLine("            std::abort();");
            builder.AppendLine("        }");
            builder.AppendLine("        *chaos_destination = *chaos_source;");
        }
        else
        {
            builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
            builder.AppendLine(
                "        auto* chaos_destination = chaos_resolve_managed_value_pointer<std::intptr_t>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine("        *chaos_destination = chaos_value;");
        }

        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitInitObj(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetReference = GetRequiredTargetReference(instruction);
        if (targetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException(
                $"native-aot initobj requires type target reference, got '{targetReference.Kind}'.");
        }

        builder.AppendLine("    {");
        if (RequiresStructuredValueTypePayload(targetReference))
        {
            builder.AppendLine(
                $"        auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(targetReference.SubjectId)}>(chaos_eval_stack[--chaos_stack_top]);");
            builder.AppendLine($"        *chaos_value = {GetNativeValueTypeSymbol(targetReference.SubjectId)}{{}};");
        }
        else
        {
            builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
            builder.AppendLine("        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
            builder.AppendLine("        {");
            builder.AppendLine(
                "            auto* chaos_slot = reinterpret_cast<std::intptr_t*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
            builder.AppendLine("            *chaos_slot = static_cast<std::intptr_t>(0);");
            builder.AppendLine("        }");
            builder.AppendLine("        else");
            builder.AppendLine("        {");
            builder.AppendLine("            *reinterpret_cast<std::intptr_t*>(chaos_address) = static_cast<std::intptr_t>(0);");
            builder.AppendLine("        }");
        }

        builder.AppendLine("    }");
        AppendGotoNext(builder, nextOffset, op);
    }

    private static void EmitComparisonBranch(
        StringBuilder builder,
        string comparisonOperator,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        IReadOnlySet<int> offsets)
    {
        if (nextOffset is null)
        {
            throw new InvalidOperationException(
                $"branch opcode '{instruction.Op}' cannot terminate the method");
        }

        var branchTarget = GetRequiredBranchTarget(instruction, offsets);
        var operandType = comparisonOperator is "==" or "!="
            ? "std::intptr_t"
            : "std::int32_t";
        builder.AppendLine("    {");
        builder.AppendLine(
            $"        const auto chaos_right = static_cast<{operandType}>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"        const auto chaos_left = static_cast<{operandType}>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"        if (chaos_left {comparisonOperator} chaos_right)");
        builder.AppendLine("        {");
        builder.AppendLine($"            goto chaos_ip_{branchTarget};");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        builder.AppendLine($"    goto chaos_ip_{nextOffset.Value};");
    }

    private static void EmitUnsignedComparisonBranch(
        StringBuilder builder,
        string comparisonOperator,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        IReadOnlySet<int> offsets)
    {
        if (nextOffset is null)
        {
            throw new InvalidOperationException(
                $"branch opcode '{instruction.Op}' cannot terminate the method");
        }

        var branchTarget = GetRequiredBranchTarget(instruction, offsets);
        builder.AppendLine("    {");
        builder.AppendLine(
            "        const auto chaos_right = static_cast<std::uint32_t>(static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]));");
        builder.AppendLine(
            "        const auto chaos_left = static_cast<std::uint32_t>(static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]));");
        builder.AppendLine(
            $"        if (chaos_left {comparisonOperator} chaos_right)");
        builder.AppendLine("        {");
        builder.AppendLine($"            goto chaos_ip_{branchTarget};");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        builder.AppendLine($"    goto chaos_ip_{nextOffset.Value};");
    }

    private static void EmitTruthBranch(
        StringBuilder builder,
        bool shouldBranchWhenNonZero,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        IReadOnlySet<int> offsets)
    {
        if (nextOffset is null)
        {
            throw new InvalidOperationException(
                $"branch opcode '{instruction.Op}' cannot terminate the method");
        }

        var branchTarget = GetRequiredBranchTarget(instruction, offsets);
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine(
            shouldBranchWhenNonZero
                ? "        if (chaos_condition != static_cast<std::intptr_t>(0))"
                : "        if (chaos_condition == static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine($"            goto chaos_ip_{branchTarget};");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        builder.AppendLine($"    goto chaos_ip_{nextOffset.Value};");
    }

    private static void AppendGotoNext(StringBuilder builder, int? nextOffset, string op)
    {
        if (nextOffset is null)
        {
            throw new InvalidOperationException(
                $"opcode '{op}' cannot be the final instruction in the method");
        }

        builder.AppendLine($"    goto chaos_ip_{nextOffset.Value};");
    }

    private void EmitInstructionRange(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        foreach (var instruction in instructions)
        {
            var offset = GetRequiredIlOffset(instruction);
            builder.AppendLine($"chaos_ip_{offset}:");
            EmitInstruction(builder, method, instruction, nextOffsetsByIlOffset[offset], offsets);
            builder.AppendLine();
        }
    }

    private void EmitCatchOnlyExceptionMethodBody(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        CatchOnlyExceptionMethodShape catchOnlyShape,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        EmitLinearInstructionSequence(builder, catchOnlyShape.PrefixInstructions, "    ");
        if (catchOnlyShape.PrefixInstructions.Count > 0)
        {
            builder.AppendLine();
        }

        builder.AppendLine("    try");
        builder.AppendLine("    {");
        builder.AppendLine($"        goto chaos_ip_{GetRequiredIlOffset(catchOnlyShape.TryInstructions[0])};");
        builder.AppendLine();
        EmitInstructionRange(builder, method, catchOnlyShape.TryInstructions, nextOffsetsByIlOffset, offsets);
        builder.AppendLine("    }");
        builder.AppendLine("    catch (const chaos_managed_exception& chaos_exception)");
        builder.AppendLine("    {");
        builder.AppendLine("        auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_exception.object_value);");
        builder.AppendLine("        if (chaos_header == nullptr)");
        builder.AppendLine("        {");
        builder.AppendLine("            throw;");
        builder.AppendLine("        }");
        builder.AppendLine(
            $"        if (!chaos_is_type_compatible(chaos_header->type_id, {GetNativeTypeIdSymbol(catchOnlyShape.ExceptionRegion.CatchTypeSubjectId!)}))");
        builder.AppendLine("        {");
        builder.AppendLine("            throw;");
        builder.AppendLine("        }");
        builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
        builder.AppendLine($"        goto chaos_ip_{GetRequiredIlOffset(catchOnlyShape.HandlerInstructions[0])};");
        builder.AppendLine();
        EmitInstructionRange(builder, method, catchOnlyShape.HandlerInstructions, nextOffsetsByIlOffset, offsets);
        builder.AppendLine("    }");
        builder.AppendLine();
        EmitInstructionRange(builder, method, catchOnlyShape.TailInstructions, nextOffsetsByIlOffset, offsets);
    }

    private void EmitFilterAndFinallyExceptionMethodBody(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        FilterAndFinallyExceptionMethodShape filterAndFinallyShape,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        EmitLinearInstructionSequence(builder, filterAndFinallyShape.PrefixInstructions, "    ");
        if (filterAndFinallyShape.PrefixInstructions.Count > 0)
        {
            builder.AppendLine();
        }

        EmitNestedFinallyScopes(
            builder,
            method,
            filterAndFinallyShape,
            0,
            nextOffsetsByIlOffset,
            offsets);

        builder.AppendLine();
        EmitInstructionRange(builder, method, filterAndFinallyShape.TailInstructions, nextOffsetsByIlOffset, offsets);
    }

    private void EmitNestedFinallyScopes(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        FilterAndFinallyExceptionMethodShape filterAndFinallyShape,
        int finallyIndex,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        if (finallyIndex >= filterAndFinallyShape.FinallyHandlers.Count)
        {
            EmitFilterAndFinallyTryCatchCore(
                builder,
                method,
                filterAndFinallyShape,
                nextOffsetsByIlOffset,
                offsets);
            return;
        }

        builder.AppendLine("    {");
        builder.AppendLine($"        auto chaos_finally_guard_{finallyIndex} = chaos_make_finally_scope_guard([&]()");
        builder.AppendLine("        {");
        EmitLinearInstructionSequence(
            builder,
            filterAndFinallyShape.FinallyHandlers[finallyIndex].Instructions
                .Take(filterAndFinallyShape.FinallyHandlers[finallyIndex].Instructions.Count - 1)
                .ToArray(),
            "            ");
        builder.AppendLine("        });");
        EmitNestedFinallyScopes(
            builder,
            method,
            filterAndFinallyShape,
            finallyIndex + 1,
            nextOffsetsByIlOffset,
            offsets);
        builder.AppendLine("    }");
    }

    private void EmitFilterAndFinallyTryCatchCore(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        FilterAndFinallyExceptionMethodShape filterAndFinallyShape,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        builder.AppendLine("        try");
        builder.AppendLine("        {");
        builder.AppendLine($"            goto chaos_ip_{GetRequiredIlOffset(filterAndFinallyShape.TryInstructions[0])};");
        builder.AppendLine();
        EmitInstructionRange(builder, method, filterAndFinallyShape.TryInstructions, nextOffsetsByIlOffset, offsets);
        builder.AppendLine("        }");
        builder.AppendLine("        catch (const chaos_managed_exception& chaos_exception)");
        builder.AppendLine("        {");
        builder.AppendLine("            chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
        builder.AppendLine($"            goto chaos_ip_{GetRequiredIlOffset(filterAndFinallyShape.FilterInstructions[0])};");
        builder.AppendLine();
        EmitFilterInstructionRange(
            builder,
            method,
            filterAndFinallyShape,
            nextOffsetsByIlOffset,
            offsets);
        builder.AppendLine("        }");
    }

    private void EmitFilterInstructionRange(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        FilterAndFinallyExceptionMethodShape filterAndFinallyShape,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        var handlerStartOffset = GetRequiredIlOffset(filterAndFinallyShape.HandlerInstructions[0]);
        foreach (var instruction in filterAndFinallyShape.FilterInstructions)
        {
            var offset = GetRequiredIlOffset(instruction);
            builder.AppendLine($"chaos_ip_{offset}:");
            if (string.Equals(instruction.Op, "endfilter", StringComparison.Ordinal))
            {
                builder.AppendLine("    if (chaos_eval_stack[--chaos_stack_top] == static_cast<std::intptr_t>(0))");
                builder.AppendLine("    {");
                builder.AppendLine("        throw;");
                builder.AppendLine("    }");
                builder.AppendLine("    chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
                builder.AppendLine($"    goto chaos_ip_{handlerStartOffset};");
            }
            else
            {
                EmitInstruction(builder, method, instruction, nextOffsetsByIlOffset[offset], offsets);
            }

            builder.AppendLine();
        }

        EmitInstructionRange(
            builder,
            method,
            filterAndFinallyShape.HandlerInstructions,
            nextOffsetsByIlOffset,
            offsets);
    }

    private static void EmitLinearInstructionSequence(
        StringBuilder builder,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        string indentation)
    {
        foreach (var instruction in instructions)
        {
            EmitLinearInstruction(builder, instruction, indentation);
        }
    }

    private static void EmitLinearInstruction(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        string indentation)
    {
        switch (instruction.Op)
        {
            case "ldc.i4":
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>({FormatInt32Literal(GetRequiredIntOperand(instruction))});");
                return;

            case "ldc.i8":
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_int64({FormatInt64Literal(GetRequiredInt64Operand(instruction))});");
                return;

            case "ldloc":
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_locals[{GetRequiredIntOperand(instruction)}];");
                return;

            case "stloc":
                builder.AppendLine(
                    $"{indentation}chaos_locals[{GetRequiredIntOperand(instruction)}] = chaos_eval_stack[--chaos_stack_top];");
                return;

            case "ldnull":
                builder.AppendLine($"{indentation}chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(0);");
                return;

            case "cgt.un":
                builder.AppendLine($"{indentation}{{");
                builder.AppendLine(
                    $"{indentation}    const auto chaos_right = static_cast<std::uintptr_t>(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(
                    $"{indentation}    const auto chaos_left = static_cast<std::uintptr_t>(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(
                    $"{indentation}    chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(chaos_left > chaos_right ? 1 : 0);");
                builder.AppendLine($"{indentation}}}");
                return;

            case "add":
                EmitLinearBinaryArithmetic(builder, indentation, "chaos_wrap_add");
                return;

            case "sub":
                EmitLinearBinaryArithmetic(builder, indentation, "chaos_wrap_sub");
                return;

            case "mul":
                EmitLinearBinaryArithmetic(builder, indentation, "chaos_wrap_mul");
                return;

            case "div":
                EmitLinearBinaryArithmetic(builder, indentation, "chaos_div");
                return;

            case "rem":
                EmitLinearBinaryArithmetic(builder, indentation, "chaos_rem");
                return;

            case "shl":
                EmitLinearShift(builder, instruction, indentation, "chaos_shift_left_int32");
                return;

            case "shr":
                EmitLinearShift(builder, instruction, indentation, "chaos_shift_right_int32");
                return;

            case "shr.un":
                EmitLinearShift(builder, instruction, indentation, "chaos_shift_right_un_int32");
                return;

            case "not":
                EmitLinearBitwiseNot(builder, instruction, indentation);
                return;

            case "and":
                EmitLinearBinaryBitwise(builder, indentation, "&");
                return;

            case "or":
                EmitLinearBinaryBitwise(builder, indentation, "|");
                return;

            case "xor":
                EmitLinearBinaryBitwise(builder, indentation, "^");
                return;

            default:
                throw new NotSupportedException(
                    $"native-aot structured EH linear lowering does not support opcode '{instruction.Op}'.");
        }
    }

    private static void EmitLinearBinaryArithmetic(
        StringBuilder builder,
        string indentation,
        string helperName)
    {
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine(
            $"{indentation}    const auto chaos_right = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"{indentation}    const auto chaos_left = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"{indentation}    chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>({helperName}(chaos_left, chaos_right));");
        builder.AppendLine($"{indentation}}}");
    }

    private static void EmitLinearBinaryBitwise(
        StringBuilder builder,
        string indentation,
        string operation)
    {
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine(
            $"{indentation}    const auto chaos_right = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"{indentation}    const auto chaos_left = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"{indentation}    chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64(chaos_left {operation} chaos_right);");
        builder.AppendLine($"{indentation}}}");
    }

    private static void EmitLinearShift(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        string indentation,
        string helperName)
    {
        RequireInt32IntegralResultType(instruction);
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine(
            $"{indentation}    const auto chaos_shift = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"{indentation}    const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(
            $"{indentation}    chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>({helperName}(chaos_value, chaos_shift));");
        builder.AppendLine($"{indentation}}}");
    }

    private static void EmitLinearBitwiseNot(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        string indentation)
    {
        RequireInt32IntegralResultType(instruction);
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine(
            $"{indentation}    const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[chaos_stack_top - 1]);");
        builder.AppendLine(
            $"{indentation}    chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(~chaos_value);");
        builder.AppendLine($"{indentation}}}");
    }

    private static IReadOnlyDictionary<int, int?> CreateNextOffsets(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        var nextOffsetsByIlOffset = new Dictionary<int, int?>(instructions.Count);
        for (var index = 0; index < instructions.Count; index++)
        {
            var offset = GetRequiredIlOffset(instructions[index]);
            nextOffsetsByIlOffset[offset] = index + 1 < instructions.Count
                ? GetRequiredIlOffset(instructions[index + 1])
                : null;
        }

        return nextOffsetsByIlOffset;
    }

    private static bool TryCreateCatchOnlyExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out CatchOnlyExceptionMethodShape? catchOnlyShape)
    {
        catchOnlyShape = null;
        if (method.ExceptionRegions.Count != 1)
        {
            return false;
        }

        var exceptionRegion = method.ExceptionRegions[0];
        if (exceptionRegion.HandlingKindCode != AotCoreIrExceptionRegionKind.Catch ||
            exceptionRegion.FilterOffset is not null ||
            string.IsNullOrWhiteSpace(exceptionRegion.CatchTypeSubjectId))
        {
            return false;
        }

        var orderedInstructions = method.Instructions
            .OrderBy(GetRequiredIlOffset)
            .ToArray();
        if (orderedInstructions.Length == 0)
        {
            return false;
        }

        var tryEndOffset = checked(exceptionRegion.TryOffset + exceptionRegion.TryLength);
        var handlerEndOffset = checked(exceptionRegion.HandlerOffset + exceptionRegion.HandlerLength);
        if (tryEndOffset != exceptionRegion.HandlerOffset)
        {
            return false;
        }

        var prefixInstructions = orderedInstructions
            .Where(instruction => GetRequiredIlOffset(instruction) < exceptionRegion.TryOffset)
            .ToArray();
        var tryInstructions = orderedInstructions
            .Where(instruction =>
            {
                var offset = GetRequiredIlOffset(instruction);
                return offset >= exceptionRegion.TryOffset && offset < tryEndOffset;
            })
            .ToArray();
        var handlerInstructions = orderedInstructions
            .Where(instruction =>
            {
                var offset = GetRequiredIlOffset(instruction);
                return offset >= exceptionRegion.HandlerOffset && offset < handlerEndOffset;
            })
            .ToArray();
        var tailInstructions = orderedInstructions
            .Where(instruction => GetRequiredIlOffset(instruction) >= handlerEndOffset)
            .ToArray();
        if (tryInstructions.Length == 0 ||
            handlerInstructions.Length < 2 ||
            tailInstructions.Length == 0 ||
            orderedInstructions.Length != prefixInstructions.Length + tryInstructions.Length + handlerInstructions.Length + tailInstructions.Length)
        {
            return false;
        }

        if (!string.Equals(handlerInstructions[0].Op, "pop", StringComparison.Ordinal) ||
            !string.Equals(tryInstructions[^1].Op, "leave", StringComparison.Ordinal) ||
            !string.Equals(handlerInstructions[^1].Op, "leave", StringComparison.Ordinal))
        {
            return false;
        }

        var leaveTarget = GetRequiredIntOperand(tryInstructions[^1]);
        if (leaveTarget != GetRequiredIntOperand(handlerInstructions[^1]) ||
            leaveTarget != GetRequiredIlOffset(tailInstructions[0]))
        {
            return false;
        }

        if (prefixInstructions.Any(instruction => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) ||
            tryInstructions[..^1].Any(instruction => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) ||
            handlerInstructions[1..^1].Any(instruction => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) ||
            tailInstructions.Any(instruction => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)))
        {
            return false;
        }

        catchOnlyShape = new CatchOnlyExceptionMethodShape(
            exceptionRegion,
            prefixInstructions,
            tryInstructions,
            handlerInstructions,
            tailInstructions);
        return true;
    }

    private static bool TryCreateFilterAndFinallyExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out FilterAndFinallyExceptionMethodShape? filterAndFinallyShape)
    {
        filterAndFinallyShape = null;
        if (method.ExceptionRegions.Count < 2)
        {
            return false;
        }

        var filterRegions = method.ExceptionRegions
            .Where(region => region.HandlingKindCode == AotCoreIrExceptionRegionKind.Filter)
            .ToArray();
        var finallyRegions = method.ExceptionRegions
            .Where(region => region.HandlingKindCode == AotCoreIrExceptionRegionKind.Finally)
            .OrderBy(region => region.TryLength)
            .ToArray();
        if (filterRegions.Length != 1 ||
            finallyRegions.Length == 0 ||
            method.ExceptionRegions.Any(region =>
                region.HandlingKindCode != AotCoreIrExceptionRegionKind.Filter &&
                region.HandlingKindCode != AotCoreIrExceptionRegionKind.Finally))
        {
            return false;
        }

        var filterRegion = filterRegions[0];
        if (filterRegion.FilterOffset is null)
        {
            return false;
        }

        var orderedInstructions = method.Instructions
            .OrderBy(GetRequiredIlOffset)
            .ToArray();
        if (orderedInstructions.Length == 0)
        {
            return false;
        }

        var rootTryOffset = filterRegion.TryOffset;
        var filterOffset = filterRegion.FilterOffset.Value;
        var filterHandlerEnd = checked(filterRegion.HandlerOffset + filterRegion.HandlerLength);
        if (checked(filterRegion.TryOffset + filterRegion.TryLength) != filterOffset)
        {
            return false;
        }

        var nextSegmentOffset = filterHandlerEnd;
        foreach (var finallyRegion in finallyRegions)
        {
            if (finallyRegion.TryOffset != rootTryOffset ||
                checked(rootTryOffset + finallyRegion.TryLength) != finallyRegion.HandlerOffset ||
                finallyRegion.HandlerOffset != nextSegmentOffset ||
                finallyRegion.FilterOffset is not null ||
                !string.IsNullOrWhiteSpace(finallyRegion.CatchTypeSubjectId))
            {
                return false;
            }

            nextSegmentOffset = checked(finallyRegion.HandlerOffset + finallyRegion.HandlerLength);
        }

        var prefixInstructions = orderedInstructions
            .Where(instruction => GetRequiredIlOffset(instruction) < rootTryOffset)
            .ToArray();
        var tryInstructions = orderedInstructions
            .Where(instruction =>
            {
                var offset = GetRequiredIlOffset(instruction);
                return offset >= rootTryOffset && offset < filterOffset;
            })
            .ToArray();
        var filterInstructions = orderedInstructions
            .Where(instruction =>
            {
                var offset = GetRequiredIlOffset(instruction);
                return offset >= filterOffset && offset < filterRegion.HandlerOffset;
            })
            .ToArray();
        var handlerInstructions = orderedInstructions
            .Where(instruction =>
            {
                var offset = GetRequiredIlOffset(instruction);
                return offset >= filterRegion.HandlerOffset && offset < filterHandlerEnd;
            })
            .ToArray();
        var finallyHandlersInnerToOuter = finallyRegions
            .Select(finallyRegion =>
            {
                var handlerEnd = checked(finallyRegion.HandlerOffset + finallyRegion.HandlerLength);
                var instructions = orderedInstructions
                    .Where(instruction =>
                    {
                        var offset = GetRequiredIlOffset(instruction);
                        return offset >= finallyRegion.HandlerOffset && offset < handlerEnd;
                    })
                    .ToArray();
                return new FinallyHandlerShape(finallyRegion, instructions);
            })
            .ToArray();
        var tailInstructions = orderedInstructions
            .Where(instruction => GetRequiredIlOffset(instruction) >= nextSegmentOffset)
            .ToArray();
        var accountedInstructionCount = prefixInstructions.Length +
                                       tryInstructions.Length +
                                       filterInstructions.Length +
                                       handlerInstructions.Length +
                                       finallyHandlersInnerToOuter.Sum(shape => shape.Instructions.Count) +
                                       tailInstructions.Length;
        if (accountedInstructionCount != orderedInstructions.Length ||
            tryInstructions.Length == 0 ||
            filterInstructions.Length == 0 ||
            handlerInstructions.Length < 2 ||
            tailInstructions.Length == 0)
        {
            return false;
        }

        if (!string.Equals(filterInstructions[^1].Op, "endfilter", StringComparison.Ordinal) ||
            !string.Equals(handlerInstructions[^1].Op, "leave", StringComparison.Ordinal) ||
            finallyHandlersInnerToOuter.Any(shape =>
                shape.Instructions.Count == 0 ||
                !string.Equals(shape.Instructions[^1].Op, "endfinally", StringComparison.Ordinal)))
        {
            return false;
        }

        if (GetRequiredIntOperand(handlerInstructions[^1]) != GetRequiredIlOffset(tailInstructions[0]))
        {
            return false;
        }

        if (prefixInstructions.Any(instruction => !IsStructuredEhLinearInstructionSupported(instruction.Op)) ||
            finallyHandlersInnerToOuter
                .SelectMany(shape => shape.Instructions.Take(shape.Instructions.Count - 1))
                .Any(instruction => !IsStructuredEhLinearInstructionSupported(instruction.Op)))
        {
            return false;
        }

        filterAndFinallyShape = new FilterAndFinallyExceptionMethodShape(
            filterRegion,
            prefixInstructions,
            tryInstructions,
            filterInstructions,
            handlerInstructions,
            finallyHandlersInnerToOuter.Reverse().ToArray(),
            tailInstructions);
        return true;
    }

    private static bool IsUnsupportedStructuredExceptionControlFlow(string op)
    {
        return string.Equals(op, "br", StringComparison.Ordinal) ||
               string.Equals(op, "blt", StringComparison.Ordinal) ||
               string.Equals(op, "bne.un", StringComparison.Ordinal) ||
               string.Equals(op, "leave", StringComparison.Ordinal) ||
               string.Equals(op, "ret", StringComparison.Ordinal);
    }

    private static bool IsStructuredEhLinearInstructionSupported(string op)
    {
        return op is "ldc.i4" or
               "ldloc" or
               "stloc" or
               "ldnull" or
               "cgt.un" or
               "add" or
               "sub" or
               "mul" or
               "div" or
               "rem";
    }

    private static int GetRequiredBranchTarget(
        AotCoreIrInstructionArtifact instruction,
        IReadOnlySet<int> offsets)
    {
        var targetOffset = GetRequiredIntOperand(instruction);
        if (!offsets.Contains(targetOffset))
        {
            throw new InvalidOperationException(
                $"opcode '{instruction.Op}' targets missing IL offset {targetOffset}");
        }

        return targetOffset;
    }

    private static int GetRequiredIlOffset(AotCoreIrInstructionArtifact instruction)
    {
        return instruction.IlOffset;
    }

    private static int GetRequiredIntOperand(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.Operand is int value)
        {
            return value;
        }

        if (instruction.Operand is JsonElement element &&
            element.ValueKind == JsonValueKind.Number &&
            element.TryGetInt32(out var jsonValue))
        {
            return jsonValue;
        }

        throw new InvalidOperationException(
            $"opcode '{instruction.Op}' requires an Int32 operand for native-aot lowering");
    }

    private static void RequireInt32IntegralResultType(AotCoreIrInstructionArtifact instruction)
    {
        if (string.Equals(instruction.ResultType, "System.Int32", StringComparison.Ordinal) ||
            string.Equals(instruction.ResultType, "System.UInt32", StringComparison.Ordinal))
        {
            return;
        }

        throw new NotSupportedException(
            $"native-aot lowering does not support opcode '{instruction.Op}' for result type '{instruction.ResultType ?? "<null>"}'.");
    }

    private static long GetRequiredInt64Operand(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.Operand is long value)
        {
            return value;
        }

        if (instruction.Operand is int intValue)
        {
            return intValue;
        }

        if (instruction.Operand is JsonElement element &&
            element.ValueKind == JsonValueKind.Number &&
            element.TryGetInt64(out var jsonValue))
        {
            return jsonValue;
        }

        throw new InvalidOperationException(
            $"opcode '{instruction.Op}' requires an Int64 operand for native-aot lowering");
    }

    private static float GetRequiredSingleOperand(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.Operand is float floatValue)
        {
            return floatValue;
        }

        if (instruction.Operand is double doubleValue)
        {
            return checked((float)doubleValue);
        }

        if (instruction.Operand is JsonElement element &&
            element.ValueKind == JsonValueKind.Number &&
            element.TryGetSingle(out var jsonValue))
        {
            return jsonValue;
        }

        throw new InvalidOperationException(
            $"opcode '{instruction.Op}' requires a Single operand for native-aot lowering");
    }

    private static double GetRequiredDoubleOperand(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.Operand is double doubleValue)
        {
            return doubleValue;
        }

        if (instruction.Operand is float floatValue)
        {
            return floatValue;
        }

        if (instruction.Operand is JsonElement element &&
            element.ValueKind == JsonValueKind.Number &&
            element.TryGetDouble(out var jsonValue))
        {
            return jsonValue;
        }

        throw new InvalidOperationException(
            $"opcode '{instruction.Op}' requires a Double operand for native-aot lowering");
    }

    private static string GetRequiredTargetSymbol(AotCoreIrInstructionArtifact instruction)
    {
        if (!string.IsNullOrWhiteSpace(instruction.TargetSymbol))
        {
            return instruction.TargetSymbol;
        }

        throw new NotSupportedException(
            $"native-aot lowering does not support unresolved call target '{instruction.Callee ?? "<null>"}'");
    }

    private static int GetRequiredTargetParameterCount(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.TargetParameterCount is int parameterCount && parameterCount >= 0)
        {
            return parameterCount;
        }

        throw new NotSupportedException(
            $"native-aot lowering does not support call target '{instruction.TargetSymbol ?? instruction.Callee ?? "<null>"}' without parameter metadata");
    }

    private static AotCoreIrReferenceArtifact GetRequiredTargetReference(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.TargetReference is not null)
        {
            return instruction.TargetReference;
        }

        throw new NotSupportedException(
            $"native-aot lowering does not support opcode '{instruction.Op}' without target reference metadata");
    }

    private static void EmitMethodReturn(StringBuilder builder, AotCoreIrAbiSlotArtifact returnAbi)
    {
        switch (returnAbi.CarrierKindCode)
        {
            case AotCoreIrAbiCarrierKind.Void:
                builder.AppendLine("    return;");
                return;

            case AotCoreIrAbiCarrierKind.Int32:
                builder.AppendLine("    return static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.Int8:
                builder.AppendLine("    return static_cast<std::int8_t>(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.UInt8:
                builder.AppendLine("    return static_cast<std::uint8_t>(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.Int16:
                builder.AppendLine("    return static_cast<std::int16_t>(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.UInt16:
                builder.AppendLine("    return static_cast<std::uint16_t>(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.Float32:
                builder.AppendLine("    return chaos_load_float32(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.Float64:
                builder.AppendLine("    return chaos_load_float64(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.Int64:
                builder.AppendLine("    return chaos_load_int64(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.UInt64:
                builder.AppendLine("    return chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
                return;

            case AotCoreIrAbiCarrierKind.NativeInt:
                builder.AppendLine("    return chaos_eval_stack[--chaos_stack_top];");
                return;

            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                builder.AppendLine(
                    $"    return *chaos_resolve_managed_value_pointer<{GetRequiredAbiValueTypeSymbol(returnAbi)}>(chaos_eval_stack[--chaos_stack_top]);");
                return;

            default:
                throw new NotSupportedException(
                    $"native-aot lowering does not support ABI return carrier '{returnAbi.CarrierKindCode}'.");
        }
    }

    private static void EmitAbiReturnPush(
        StringBuilder builder,
        AotCoreIrAbiSlotArtifact returnAbi,
        string resultExpression,
        string indentation)
    {
        switch (returnAbi.CarrierKindCode)
        {
            case AotCoreIrAbiCarrierKind.Int32:
            case AotCoreIrAbiCarrierKind.Int8:
            case AotCoreIrAbiCarrierKind.UInt8:
            case AotCoreIrAbiCarrierKind.Int16:
            case AotCoreIrAbiCarrierKind.UInt16:
            case AotCoreIrAbiCarrierKind.NativeInt:
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>({resultExpression});");
                return;

            case AotCoreIrAbiCarrierKind.Float32:
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_float32({resultExpression});");
                return;

            case AotCoreIrAbiCarrierKind.Float64:
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_float64({resultExpression});");
                return;

            case AotCoreIrAbiCarrierKind.Int64:
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_int64({resultExpression});");
                return;

            case AotCoreIrAbiCarrierKind.UInt64:
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64({resultExpression});");
                return;

            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                builder.AppendLine(
                    $"{indentation}auto* chaos_result_storage = new {GetRequiredAbiValueTypeSymbol(returnAbi)}{{}};");
                builder.AppendLine($"{indentation}*chaos_result_storage = {resultExpression};");
                builder.AppendLine(
                    $"{indentation}chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_result_storage);");
                return;

            default:
                throw new NotSupportedException(
                    $"native-aot lowering does not support pushing ABI return carrier '{returnAbi.CarrierKindCode}'.");
        }
    }

    private static bool CanEmitMethodBody(AotCoreIrMethodArtifact method)
    {
        return method.Instructions.Count > 0;
    }

    private static IReadOnlyList<AotCoreIrAbiSlotArtifact> GetMethodAbiParameterSlots(AotCoreIrMethodArtifact method)
    {
        ArgumentNullException.ThrowIfNull(method);
        if (method.IsStatic)
        {
            return method.ParameterAbis;
        }

        var parameterSlots = new List<AotCoreIrAbiSlotArtifact>(checked(method.ParameterAbis.Count + 1))
        {
            CreateNativeIntAbiSlot(method.Identity.DeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
        };
        parameterSlots.AddRange(method.ParameterAbis);
        return parameterSlots;
    }

    private static AotCoreIrAbiSlotArtifact GetRequiredMethodAbiParameterSlot(
        AotCoreIrMethodArtifact method,
        int argumentIndex)
    {
        var abiParameterSlots = GetMethodAbiParameterSlots(method);
        if (argumentIndex < 0 || argumentIndex >= abiParameterSlots.Count)
        {
            throw new InvalidOperationException(
                $"native-aot lowering could not resolve argument slot {argumentIndex} for '{method.SubjectId}'.");
        }

        return abiParameterSlots[argumentIndex];
    }

    private static IReadOnlyList<AotCoreIrAbiSlotArtifact> CreateLegacyAbiParameterSlots(int parameterCount)
    {
        if (parameterCount < 0)
        {
            throw new NotSupportedException("native-aot lowering requires a non-negative parameter count.");
        }

        if (parameterCount == 0)
        {
            return [];
        }

        return Enumerable.Range(0, parameterCount)
            .Select(_ => CreateNativeIntAbiSlot())
            .ToArray();
    }

    private static AotCoreIrAbiSlotArtifact CreateLegacyReturnAbiSlot(string? returnType)
    {
        return returnType switch
        {
            "System.Void" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
                TypeShape = default,
            },
            "System.Int32" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.SByte" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int8,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.Byte" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.Int16" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int16,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.UInt16" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.Single" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Float32,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.Double" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.Int64" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            "System.UInt64" => new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            },
            _ when !string.IsNullOrWhiteSpace(returnType) => CreateNativeIntAbiSlot(),
            _ => throw new NotSupportedException(
                $"native-aot lowering does not support unresolved legacy return type '{returnType ?? "<null>"}'."),
        };
    }

    private static AotCoreIrAbiSlotArtifact CreateNativeIntAbiSlot(
        string? typeSubjectId = null,
        AotCoreIrTypeShapeKind typeShape = default)
    {
        return new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
            TypeSubjectId = typeSubjectId,
            TypeShape = typeShape,
        };
    }

    private static string MapAbiSlotReturnType(AotCoreIrAbiSlotArtifact abiSlot)
    {
        return abiSlot.CarrierKindCode switch
        {
            AotCoreIrAbiCarrierKind.Void => "void",
            AotCoreIrAbiCarrierKind.Int32 => "std::int32_t",
            AotCoreIrAbiCarrierKind.Int8 => "std::int8_t",
            AotCoreIrAbiCarrierKind.UInt8 => "std::uint8_t",
            AotCoreIrAbiCarrierKind.Int16 => "std::int16_t",
            AotCoreIrAbiCarrierKind.UInt16 => "std::uint16_t",
            AotCoreIrAbiCarrierKind.Float32 => "float",
            AotCoreIrAbiCarrierKind.Float64 => "double",
            AotCoreIrAbiCarrierKind.Int64 => "std::int64_t",
            AotCoreIrAbiCarrierKind.UInt64 => "std::uint64_t",
            AotCoreIrAbiCarrierKind.NativeInt => "std::intptr_t",
            AotCoreIrAbiCarrierKind.ValueTypeByValue => GetRequiredAbiValueTypeSymbol(abiSlot),
            _ => throw new NotSupportedException(
                $"native-aot lowering does not support ABI return carrier '{abiSlot.CarrierKindCode}'."),
        };
    }

    private static string FormatAbiSlotParameterSignature(IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots)
    {
        ArgumentNullException.ThrowIfNull(abiSlots);
        if (abiSlots.Count == 0)
        {
            return "void";
        }

        return string.Join(
            ", ",
            abiSlots.Select((slot, index) => $"{MapAbiSlotParameterType(slot)} chaos_arg_{index}"));
    }

    private static void EmitAbiArgumentInitialization(
        StringBuilder builder,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots)
    {
        for (var parameterIndex = 0; parameterIndex < abiSlots.Count; parameterIndex++)
        {
            var abiSlot = abiSlots[parameterIndex];
            switch (abiSlot.CarrierKindCode)
            {
                case AotCoreIrAbiCarrierKind.Int32:
                case AotCoreIrAbiCarrierKind.Int8:
                case AotCoreIrAbiCarrierKind.UInt8:
                case AotCoreIrAbiCarrierKind.Int16:
                case AotCoreIrAbiCarrierKind.UInt16:
                case AotCoreIrAbiCarrierKind.NativeInt:
                    builder.AppendLine(
                        $"    chaos_args[{parameterIndex}] = static_cast<std::intptr_t>(chaos_arg_{parameterIndex});");
                    break;

                case AotCoreIrAbiCarrierKind.Float32:
                    builder.AppendLine(
                        $"    chaos_args[{parameterIndex}] = chaos_store_float32(chaos_arg_{parameterIndex});");
                    break;

                case AotCoreIrAbiCarrierKind.Float64:
                    builder.AppendLine(
                        $"    chaos_args[{parameterIndex}] = chaos_store_float64(chaos_arg_{parameterIndex});");
                    break;

                case AotCoreIrAbiCarrierKind.Int64:
                    builder.AppendLine(
                        $"    chaos_args[{parameterIndex}] = chaos_store_int64(chaos_arg_{parameterIndex});");
                    break;

                case AotCoreIrAbiCarrierKind.UInt64:
                    builder.AppendLine(
                        $"    chaos_args[{parameterIndex}] = chaos_store_uint64(chaos_arg_{parameterIndex});");
                    break;

                case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                    builder.AppendLine($"    auto chaos_abi_param_{parameterIndex} = chaos_arg_{parameterIndex};");
                    builder.AppendLine(
                        $"    chaos_args[{parameterIndex}] = reinterpret_cast<std::intptr_t>(&chaos_abi_param_{parameterIndex});");
                    break;

                default:
                    throw new NotSupportedException(
                        $"native-aot lowering does not support ABI parameter carrier '{abiSlot.CarrierKindCode}'.");
            }
        }
    }

    private static string FormatAbiInvocationArgumentList(
        IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots,
        string? firstArgumentOverride = null)
    {
        if (abiSlots.Count == 0)
        {
            return string.Empty;
        }

        return string.Join(
            ", ",
            abiSlots.Select((slot, index) =>
            {
                var sourceName = index == 0 && !string.IsNullOrWhiteSpace(firstArgumentOverride)
                    ? firstArgumentOverride!
                    : $"chaos_arg_{index}";
                return FormatAbiArgumentExpression(slot, sourceName);
            }));
    }

    private static string FormatAbiArgumentExpression(
        AotCoreIrAbiSlotArtifact abiSlot,
        string sourceName)
    {
        return abiSlot.CarrierKindCode switch
        {
            AotCoreIrAbiCarrierKind.Int32 => $"static_cast<std::int32_t>({sourceName})",
            AotCoreIrAbiCarrierKind.Int8 => $"static_cast<std::int8_t>({sourceName})",
            AotCoreIrAbiCarrierKind.UInt8 => $"static_cast<std::uint8_t>({sourceName})",
            AotCoreIrAbiCarrierKind.Int16 => $"static_cast<std::int16_t>({sourceName})",
            AotCoreIrAbiCarrierKind.UInt16 => $"static_cast<std::uint16_t>({sourceName})",
            AotCoreIrAbiCarrierKind.Float32 => $"chaos_load_float32({sourceName})",
            AotCoreIrAbiCarrierKind.Float64 => $"chaos_load_float64({sourceName})",
            AotCoreIrAbiCarrierKind.Int64 => $"chaos_load_int64({sourceName})",
            AotCoreIrAbiCarrierKind.UInt64 => $"chaos_load_uint64({sourceName})",
            AotCoreIrAbiCarrierKind.NativeInt => sourceName,
            AotCoreIrAbiCarrierKind.ValueTypeByValue =>
                $"*chaos_resolve_managed_value_pointer<{GetRequiredAbiValueTypeSymbol(abiSlot)}>({sourceName})",
            _ => throw new NotSupportedException(
                $"native-aot lowering does not support ABI argument carrier '{abiSlot.CarrierKindCode}'."),
        };
    }

    private static string FormatInboundAbiArgumentExpression(
        AotCoreIrAbiSlotArtifact abiSlot,
        string sourceName)
    {
        return abiSlot.CarrierKindCode switch
        {
            AotCoreIrAbiCarrierKind.NativeInt => $"chaos_normalize_native_int_argument({sourceName})",
            _ => sourceName,
        };
    }

    private static string MapAbiSlotParameterType(AotCoreIrAbiSlotArtifact abiSlot)
    {
        return abiSlot.CarrierKindCode switch
        {
            AotCoreIrAbiCarrierKind.Int32 => "std::int32_t",
            AotCoreIrAbiCarrierKind.Int8 => "std::int8_t",
            AotCoreIrAbiCarrierKind.UInt8 => "std::uint8_t",
            AotCoreIrAbiCarrierKind.Int16 => "std::int16_t",
            AotCoreIrAbiCarrierKind.UInt16 => "std::uint16_t",
            AotCoreIrAbiCarrierKind.Float32 => "float",
            AotCoreIrAbiCarrierKind.Float64 => "double",
            AotCoreIrAbiCarrierKind.Int64 => "std::int64_t",
            AotCoreIrAbiCarrierKind.UInt64 => "std::uint64_t",
            AotCoreIrAbiCarrierKind.NativeInt => "std::intptr_t",
            AotCoreIrAbiCarrierKind.ValueTypeByValue => GetRequiredAbiValueTypeSymbol(abiSlot),
            _ => throw new NotSupportedException(
                $"native-aot lowering does not support ABI parameter carrier '{abiSlot.CarrierKindCode}'."),
        };
    }

    private static string GetRequiredAbiValueTypeSymbol(AotCoreIrAbiSlotArtifact abiSlot)
    {
        if (abiSlot.CarrierKindCode != AotCoreIrAbiCarrierKind.ValueTypeByValue ||
            string.IsNullOrWhiteSpace(abiSlot.TypeSubjectId))
        {
            throw new NotSupportedException(
                $"native-aot lowering requires a value-type ABI slot with subject metadata, got '{abiSlot.CarrierKindCode}'.");
        }

        return GetNativeValueTypeSymbol(abiSlot.TypeSubjectId);
    }

    private static string FormatInt32Literal(int value)
    {
        return value == int.MinValue
            ? "std::numeric_limits<std::int32_t>::min()"
            : value.ToString(CultureInfo.InvariantCulture);
    }

    private static string FormatInt64Literal(long value)
    {
        return value == long.MinValue
            ? "std::numeric_limits<std::int64_t>::min()"
            : value.ToString(CultureInfo.InvariantCulture) + "LL";
    }

    private static string FormatFloat32Literal(float value)
    {
        return value.ToString("R", CultureInfo.InvariantCulture) + "f";
    }

    private static string FormatFloat64Literal(double value)
    {
        return value.ToString("R", CultureInfo.InvariantCulture);
    }

    private static void RequireStringField(string? value, string fieldName)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            throw new InvalidOperationException(
                $"native-aot lowering plan requires non-empty field '{fieldName}'");
        }
    }

    private static T LoadRequiredJson<T>(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException($"required native-aot lowering plan is missing: {path}", path);
        }

        var value = JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions);
        if (value is null)
        {
            throw new InvalidOperationException($"failed to deserialize native-aot lowering plan: {path}");
        }

        return value;
    }

    private void EmitObjectModelDeclarations(
        StringBuilder builder,
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var referenceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var referenceTypeBaseSubjectIds = new Dictionary<string, string?>(StringComparer.Ordinal);
        var interfaceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var referenceTypeImplementedInterfaceSubjectIds = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
        var valueTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var instanceFieldSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var staticFieldSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var boxedTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        void TrackInterfaceType(string subjectId)
        {
            interfaceTypeSubjectIds.Add(subjectId);
        }

        void TrackReferenceType(
            string subjectId,
            string? baseTypeSubjectId,
            IReadOnlyList<string>? implementedInterfaceSubjectIds = null)
        {
            if (string.IsNullOrWhiteSpace(baseTypeSubjectId) &&
                _referenceTypeBaseSubjectIds.TryGetValue(subjectId, out var resolvedBaseTypeSubjectId) &&
                !string.IsNullOrWhiteSpace(resolvedBaseTypeSubjectId))
            {
                baseTypeSubjectId = resolvedBaseTypeSubjectId;
            }

            if (implementedInterfaceSubjectIds is null &&
                _referenceTypeImplementedInterfaceSubjectIds.TryGetValue(subjectId, out var resolvedImplementedInterfaceSubjectIds) &&
                resolvedImplementedInterfaceSubjectIds.Count > 0)
            {
                implementedInterfaceSubjectIds = resolvedImplementedInterfaceSubjectIds
                    .OrderBy(value => value, StringComparer.Ordinal)
                    .ToArray();
            }

            referenceTypeSubjectIds.Add(subjectId);
            if (!referenceTypeBaseSubjectIds.ContainsKey(subjectId))
            {
                referenceTypeBaseSubjectIds[subjectId] = null;
            }

            if (!referenceTypeImplementedInterfaceSubjectIds.ContainsKey(subjectId))
            {
                referenceTypeImplementedInterfaceSubjectIds[subjectId] = new HashSet<string>(StringComparer.Ordinal);
            }

            if (!string.IsNullOrWhiteSpace(baseTypeSubjectId))
            {
                referenceTypeBaseSubjectIds[subjectId] = baseTypeSubjectId;
                referenceTypeSubjectIds.Add(baseTypeSubjectId);
                if (!referenceTypeBaseSubjectIds.ContainsKey(baseTypeSubjectId))
                {
                    referenceTypeBaseSubjectIds[baseTypeSubjectId] = null;
                }

                if (!referenceTypeImplementedInterfaceSubjectIds.ContainsKey(baseTypeSubjectId))
                {
                    referenceTypeImplementedInterfaceSubjectIds[baseTypeSubjectId] = new HashSet<string>(StringComparer.Ordinal);
                }
            }

            if (implementedInterfaceSubjectIds is null)
            {
                return;
            }

            foreach (var interfaceSubjectId in implementedInterfaceSubjectIds)
            {
                referenceTypeImplementedInterfaceSubjectIds[subjectId].Add(interfaceSubjectId);
                TrackInterfaceType(interfaceSubjectId);
            }
        }

        void TrackCarrierType(
            string subjectId,
            AotCoreIrTypeShapeKind typeShape,
            string? baseTypeSubjectId = null,
            IReadOnlyList<string>? implementedInterfaceSubjectIds = null)
        {
            if (string.IsNullOrWhiteSpace(subjectId))
            {
                return;
            }

            switch (typeShape)
            {
                case AotCoreIrTypeShapeKind.ValueType:
                    valueTypeSubjectIds.Add(subjectId);
                    break;

                case AotCoreIrTypeShapeKind.InterfaceType:
                    TrackInterfaceType(subjectId);
                    break;

                case AotCoreIrTypeShapeKind.ReferenceType:
                    TrackReferenceType(subjectId, baseTypeSubjectId, implementedInterfaceSubjectIds);
                    break;
            }
        }

        void TrackAbiSlotCarrier(AotCoreIrAbiSlotArtifact abiSlot)
        {
            if (abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue &&
                !string.IsNullOrWhiteSpace(abiSlot.TypeSubjectId))
            {
                valueTypeSubjectIds.Add(abiSlot.TypeSubjectId!);
            }
        }

        builder.AppendLine("struct chaos_object_header");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t type_id = 0;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("constexpr std::intptr_t chaos_type_id_managed_array = 1;");
        builder.AppendLine();
        builder.AppendLine("struct chaos_managed_array");
        builder.AppendLine("{");
        builder.AppendLine("    chaos_object_header header{};");
        builder.AppendLine("    std::uint8_t element_type_shape = 0;");
        builder.AppendLine("    std::intptr_t element_type_id = 0;");
        builder.AppendLine("    std::intptr_t length = 0;");
        builder.AppendLine("    std::intptr_t* elements = nullptr;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("constexpr std::uint8_t chaos_type_shape_reference = 1;");
        builder.AppendLine("constexpr std::uint8_t chaos_type_shape_value = 2;");
        builder.AppendLine("constexpr std::uint8_t chaos_type_shape_interface = 3;");
        builder.AppendLine();
        builder.AppendLine("constexpr std::intptr_t chaos_managed_pointer_local_slot_tag = 1;");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_normalize_native_int_argument(std::intptr_t chaos_value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    if ((chaos_value & chaos_managed_pointer_local_slot_tag) == 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    auto* chaos_slot = reinterpret_cast<std::intptr_t*>(static_cast<std::uintptr_t>(chaos_value & ~chaos_managed_pointer_local_slot_tag));");
        builder.AppendLine("    return *chaos_slot;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("template <typename TValue>");
        builder.AppendLine("TValue* chaos_resolve_managed_value_pointer(std::intptr_t chaos_managed_pointer)");
        builder.AppendLine("{");
        builder.AppendLine("    if ((chaos_managed_pointer & chaos_managed_pointer_local_slot_tag) != 0)");
        builder.AppendLine("    {");
        builder.AppendLine(
            "        auto* chaos_slot = reinterpret_cast<std::intptr_t*>(static_cast<std::uintptr_t>(chaos_managed_pointer & ~chaos_managed_pointer_local_slot_tag));");
        builder.AppendLine("        if (*chaos_slot == static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            *chaos_slot = reinterpret_cast<std::intptr_t>(new TValue{});");
        builder.AppendLine("        }");
        builder.AppendLine("        return reinterpret_cast<TValue*>(*chaos_slot);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return reinterpret_cast<TValue*>(chaos_managed_pointer);");
        builder.AppendLine("}");
        builder.AppendLine();

        foreach (var method in reachableMethods)
        {
            TrackAbiSlotCarrier(method.ReturnAbi);
            foreach (var parameterAbi in method.ParameterAbis)
            {
                TrackAbiSlotCarrier(parameterAbi);
            }

            foreach (var exceptionRegion in method.ExceptionRegions)
            {
                if (exceptionRegion.HandlingKindCode == AotCoreIrExceptionRegionKind.Catch &&
                    !string.IsNullOrWhiteSpace(exceptionRegion.CatchTypeSubjectId))
                {
                    TrackReferenceType(exceptionRegion.CatchTypeSubjectId!, null);
                }
            }

            foreach (var instruction in method.Instructions)
            {
                if (instruction.Op is "add.ovf" or "sub.ovf" or "mul.ovf" or "conv.ovf.i1" or "conv.ovf.u1")
                {
                    TrackReferenceType(OverflowExceptionTypeSubjectId, null);
                }

                var targetReference = instruction.TargetReference;
                if (targetReference is null)
                {
                    continue;
                }

                if (targetReference.Kind == AotCoreIrReferenceKind.Type &&
                    instruction.RuntimeServiceKind is AotCoreIrRuntimeServiceKind.NewObject or AotCoreIrRuntimeServiceKind.NewArray or AotCoreIrRuntimeServiceKind.CastClass or AotCoreIrRuntimeServiceKind.IsInst)
                {
                    if (HasArrayElementReference(targetReference))
                    {
                        TrackCarrierType(
                            targetReference.ArrayElementSubjectId!,
                            targetReference.ArrayElementTypeShape,
                            targetReference.ArrayElementBaseTypeSubjectId,
                            targetReference.ArrayElementImplementedInterfaceSubjectIds);
                    }
                    else
                    {
                        TrackCarrierType(
                            targetReference.SubjectId,
                            targetReference.TypeShape,
                            targetReference.BaseTypeSubjectId,
                            targetReference.ImplementedInterfaceSubjectIds);
                    }

                    continue;
                }

                if (targetReference.Kind == AotCoreIrReferenceKind.Type &&
                    instruction.RuntimeServiceKind is AotCoreIrRuntimeServiceKind.Box or AotCoreIrRuntimeServiceKind.Unbox or AotCoreIrRuntimeServiceKind.UnboxAny)
                {
                    if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
                    {
                        valueTypeSubjectIds.Add(targetReference.SubjectId);
                    }

                    boxedTypeSubjectIds.Add(targetReference.SubjectId);
                    continue;
                }

                if (targetReference.Kind == AotCoreIrReferenceKind.Type &&
                    instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.InitObject)
                {
                    if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
                    {
                        valueTypeSubjectIds.Add(targetReference.SubjectId);
                    }

                    continue;
                }

                if (targetReference.Kind == AotCoreIrReferenceKind.Type &&
                    instruction.Op is "ldobj" or "stobj")
                {
                    if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
                    {
                        valueTypeSubjectIds.Add(targetReference.SubjectId);
                    }

                    continue;
                }

                if (targetReference.Kind != AotCoreIrReferenceKind.Field)
                {
                    continue;
                }

                var declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(targetReference);
                if (targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
                {
                    valueTypeSubjectIds.Add(declaringTypeSubjectId);
                }
                else
                {
                    TrackReferenceType(declaringTypeSubjectId, null);
                }

                if (instruction.RuntimeServiceKind is AotCoreIrRuntimeServiceKind.LoadStaticField or AotCoreIrRuntimeServiceKind.StoreStaticField)
                {
                    staticFieldSubjectIds.Add(targetReference.SubjectId);
                }
                else
                {
                    instanceFieldSubjectIds.Add(targetReference.SubjectId);
                }
            }
        }

        var nextTypeId = 2;
        foreach (var typeSubjectId in referenceTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            builder.AppendLine($"constexpr std::intptr_t {GetNativeTypeIdSymbol(typeSubjectId)} = {nextTypeId};");
            nextTypeId++;
        }

        foreach (var typeSubjectId in interfaceTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            builder.AppendLine($"constexpr std::intptr_t {GetNativeTypeIdSymbol(typeSubjectId)} = {nextTypeId};");
            nextTypeId++;
        }

        foreach (var typeSubjectId in boxedTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            builder.AppendLine($"constexpr std::intptr_t {GetNativeBoxTypeIdSymbol(typeSubjectId)} = {nextTypeId};");
            nextTypeId++;
        }

        if (referenceTypeSubjectIds.Count > 0 || boxedTypeSubjectIds.Count > 0)
        {
            builder.AppendLine();
        }

        builder.AppendLine("std::intptr_t chaos_get_base_type_id(std::intptr_t chaos_type_id) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    switch (chaos_type_id)");
        builder.AppendLine("    {");
        foreach (var typeSubjectId in referenceTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            if (!referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out var baseTypeSubjectId) ||
                string.IsNullOrWhiteSpace(baseTypeSubjectId))
            {
                continue;
            }

            builder.AppendLine($"        case {GetNativeTypeIdSymbol(typeSubjectId)}:");
            builder.AppendLine($"            return {GetNativeTypeIdSymbol(baseTypeSubjectId)};");
        }

        builder.AppendLine("        default:");
        builder.AppendLine("            return static_cast<std::intptr_t>(0);");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("bool chaos_is_type_compatible(std::intptr_t chaos_actual_type_id, std::intptr_t chaos_target_type_id) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    auto chaos_current_type_id = chaos_actual_type_id;");
        builder.AppendLine("    while (chaos_current_type_id != static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        if (chaos_current_type_id == chaos_target_type_id)");
        builder.AppendLine("        {");
        builder.AppendLine("            return true;");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return false;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("bool chaos_type_implements_interface(std::intptr_t chaos_actual_type_id, std::intptr_t chaos_target_interface_type_id) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    switch (chaos_actual_type_id)");
        builder.AppendLine("    {");
        foreach (var typeSubjectId in referenceTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            builder.AppendLine($"        case {GetNativeTypeIdSymbol(typeSubjectId)}:");
            if (referenceTypeImplementedInterfaceSubjectIds.TryGetValue(typeSubjectId, out var implementedInterfaces) &&
                implementedInterfaces.Count > 0)
            {
                var interfaceChecks = string.Join(
                    " || ",
                    implementedInterfaces
                        .OrderBy(value => value, StringComparer.Ordinal)
                        .Select(interfaceSubjectId =>
                            $"chaos_target_interface_type_id == {GetNativeTypeIdSymbol(interfaceSubjectId)}"));
                builder.AppendLine($"            return {interfaceChecks};");
            }
            else
            {
                builder.AppendLine("            return false;");
            }
        }

        builder.AppendLine("        default:");
        builder.AppendLine("            return false;");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("bool chaos_does_type_implement_interface(std::intptr_t chaos_actual_type_id, std::intptr_t chaos_target_interface_type_id) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    auto chaos_current_type_id = chaos_actual_type_id;");
        builder.AppendLine("    while (chaos_current_type_id != static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        if (chaos_type_implements_interface(chaos_current_type_id, chaos_target_interface_type_id))");
        builder.AppendLine("        {");
        builder.AppendLine("            return true;");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return false;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("bool chaos_is_array_type_compatible(");
        builder.AppendLine("    std::uint8_t chaos_actual_element_shape,");
        builder.AppendLine("    std::intptr_t chaos_actual_element_type_id,");
        builder.AppendLine("    std::uint8_t chaos_target_element_shape,");
        builder.AppendLine("    std::intptr_t chaos_target_element_type_id) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_actual_element_shape == chaos_type_shape_reference)");
        builder.AppendLine("    {");
        builder.AppendLine("        if (chaos_target_element_shape == chaos_type_shape_reference)");
        builder.AppendLine("        {");
        builder.AppendLine("            return chaos_is_type_compatible(chaos_actual_element_type_id, chaos_target_element_type_id);");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        if (chaos_target_element_shape == chaos_type_shape_interface)");
        builder.AppendLine("        {");
        builder.AppendLine("            return chaos_does_type_implement_interface(chaos_actual_element_type_id, chaos_target_element_type_id);");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        return false;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return chaos_actual_element_shape == chaos_target_element_shape");
        builder.AppendLine("        && chaos_actual_element_type_id == chaos_target_element_type_id;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, std::intptr_t chaos_value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_array == nullptr)");
        builder.AppendLine("    {");
        builder.AppendLine("        return false;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_value == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return true;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
        builder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_interface)");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_does_type_implement_interface(chaos_header->type_id, chaos_array->element_type_id);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_reference)");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_is_type_compatible(chaos_header->type_id, chaos_array->element_type_id);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return false;");
        builder.AppendLine("}");
        builder.AppendLine();

        foreach (var typeSubjectId in GetReferenceTypeEmissionOrder(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
        {
            if (referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out var baseTypeSubjectId) &&
                !string.IsNullOrWhiteSpace(baseTypeSubjectId) &&
                referenceTypeSubjectIds.Contains(baseTypeSubjectId))
            {
                builder.AppendLine($"struct {GetNativeTypeSymbol(typeSubjectId)} : public {GetNativeTypeSymbol(baseTypeSubjectId)}");
            }
            else
            {
                builder.AppendLine($"struct {GetNativeTypeSymbol(typeSubjectId)}");
            }

            builder.AppendLine("{");
            if (!referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out var resolvedBaseTypeSubjectId) ||
                string.IsNullOrWhiteSpace(resolvedBaseTypeSubjectId) ||
                !referenceTypeSubjectIds.Contains(resolvedBaseTypeSubjectId))
            {
                builder.AppendLine("    chaos_object_header header{};");
            }

            var instanceFields = instanceFieldSubjectIds
                .Where(fieldSubjectId => string.Equals(GetDeclaringTypeSubjectId(fieldSubjectId), typeSubjectId, StringComparison.Ordinal))
                .OrderBy(fieldSubjectId => fieldSubjectId, StringComparer.Ordinal)
                .ToList();

            if (instanceFields.Count == 0)
            {
                builder.AppendLine("};");
                builder.AppendLine();
                continue;
            }

            foreach (var fieldSubjectId in instanceFields)
            {
                builder.AppendLine($"    std::intptr_t {GetNativeFieldMemberName(fieldSubjectId)} = 0;");
            }

            builder.AppendLine("};");
            builder.AppendLine();
        }

        foreach (var typeSubjectId in valueTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            builder.AppendLine($"struct {GetNativeValueTypeSymbol(typeSubjectId)}");
            builder.AppendLine("{");

            var instanceFields = instanceFieldSubjectIds
                .Where(fieldSubjectId => string.Equals(GetDeclaringTypeSubjectId(fieldSubjectId), typeSubjectId, StringComparison.Ordinal))
                .OrderBy(fieldSubjectId => fieldSubjectId, StringComparer.Ordinal)
                .ToList();

            if (instanceFields.Count == 0)
            {
                builder.AppendLine("};");
                builder.AppendLine();
                continue;
            }

            foreach (var fieldSubjectId in instanceFields)
            {
                builder.AppendLine($"    std::intptr_t {GetNativeFieldMemberName(fieldSubjectId)} = 0;");
            }

            builder.AppendLine("};");
            builder.AppendLine();
        }

        foreach (var typeSubjectId in boxedTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            builder.AppendLine($"struct {GetNativeBoxTypeSymbol(typeSubjectId)}");
            builder.AppendLine("{");
            builder.AppendLine("    chaos_object_header header{};");
            if (IsStructuredValueTypeSubjectId(typeSubjectId))
            {
                builder.AppendLine($"    {GetNativeValueTypeSymbol(typeSubjectId)} value{{}};");
            }
            else
            {
                builder.AppendLine("    std::intptr_t value = 0;");
            }

            builder.AppendLine("};");
            builder.AppendLine();
        }

        foreach (var fieldSubjectId in staticFieldSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            builder.AppendLine($"std::intptr_t {GetNativeStaticFieldSymbol(fieldSubjectId)} = 0;");
        }

        if (staticFieldSubjectIds.Count > 0)
        {
            builder.AppendLine();
        }
    }

    private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(
        IReadOnlySet<string> referenceTypeSubjectIds,
        IReadOnlyDictionary<string, string?> referenceTypeBaseSubjectIds)
    {
        var ordered = new List<string>();
        var visited = new HashSet<string>(StringComparer.Ordinal);
        var visiting = new HashSet<string>(StringComparer.Ordinal);

        void Visit(string typeSubjectId)
        {
            if (visited.Contains(typeSubjectId))
            {
                return;
            }

            if (!visiting.Add(typeSubjectId))
            {
                throw new InvalidOperationException(
                    $"reference type inheritance cycle detected for '{typeSubjectId}'.");
            }

            if (referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out var baseTypeSubjectId) &&
                !string.IsNullOrWhiteSpace(baseTypeSubjectId) &&
                referenceTypeSubjectIds.Contains(baseTypeSubjectId))
            {
                Visit(baseTypeSubjectId);
            }

            visiting.Remove(typeSubjectId);
            visited.Add(typeSubjectId);
            ordered.Add(typeSubjectId);
        }

        foreach (var typeSubjectId in referenceTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
        {
            Visit(typeSubjectId);
        }

        return ordered;
    }

    private static IReadOnlyDictionary<string, string?> CollectReferenceTypeBaseSubjectIds(
        AotCoreIrArtifact aotCoreIr)
    {
        var baseTypeSubjectIds = new Dictionary<string, string?>(StringComparer.Ordinal);

        foreach (var method in aotCoreIr.Methods)
        {
            foreach (var instruction in method.Instructions)
            {
                var targetReference = instruction.TargetReference;
                if (targetReference is null)
                {
                    continue;
                }

                if (targetReference.Kind == AotCoreIrReferenceKind.Type &&
                    targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
                {
                    baseTypeSubjectIds[targetReference.SubjectId] = targetReference.BaseTypeSubjectId;
                }

                if (!string.IsNullOrWhiteSpace(targetReference.ArrayElementSubjectId) &&
                    targetReference.ArrayElementTypeShape == AotCoreIrTypeShapeKind.ReferenceType)
                {
                    baseTypeSubjectIds[targetReference.ArrayElementSubjectId] = targetReference.ArrayElementBaseTypeSubjectId;
                }
            }
        }

        return baseTypeSubjectIds;
    }

    private static IReadOnlyDictionary<string, HashSet<string>> CollectReferenceTypeImplementedInterfaceSubjectIds(
        AotCoreIrArtifact aotCoreIr)
    {
        var implementedInterfaceSubjectIds = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);

        static void TrackImplementedInterfaces(
            IDictionary<string, HashSet<string>> implementedInterfaceSubjectIds,
            string? typeSubjectId,
            IReadOnlyList<string>? interfaceSubjectIds)
        {
            if (string.IsNullOrWhiteSpace(typeSubjectId) ||
                interfaceSubjectIds is null ||
                interfaceSubjectIds.Count == 0)
            {
                return;
            }

            if (!implementedInterfaceSubjectIds.TryGetValue(typeSubjectId, out var trackedInterfaces))
            {
                trackedInterfaces = new HashSet<string>(StringComparer.Ordinal);
                implementedInterfaceSubjectIds[typeSubjectId] = trackedInterfaces;
            }

            foreach (var interfaceSubjectId in interfaceSubjectIds)
            {
                if (!string.IsNullOrWhiteSpace(interfaceSubjectId))
                {
                    trackedInterfaces.Add(interfaceSubjectId);
                }
            }
        }

        foreach (var method in aotCoreIr.Methods)
        {
            foreach (var instruction in method.Instructions)
            {
                var targetReference = instruction.TargetReference;
                if (targetReference is null)
                {
                    continue;
                }

                if (targetReference.Kind == AotCoreIrReferenceKind.Type &&
                    targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
                {
                    TrackImplementedInterfaces(
                        implementedInterfaceSubjectIds,
                        targetReference.SubjectId,
                        targetReference.ImplementedInterfaceSubjectIds);
                }

                if (!string.IsNullOrWhiteSpace(targetReference.ArrayElementSubjectId) &&
                    targetReference.ArrayElementTypeShape == AotCoreIrTypeShapeKind.ReferenceType)
                {
                    TrackImplementedInterfaces(
                        implementedInterfaceSubjectIds,
                        targetReference.ArrayElementSubjectId,
                        targetReference.ArrayElementImplementedInterfaceSubjectIds);
                }
            }
        }

        return implementedInterfaceSubjectIds;
    }

    private static string GetDeclaringTypeSubjectId(string fieldSubjectId)
    {
        var separatorIndex = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException(
                $"field subject '{fieldSubjectId}' is missing declaring type information");
        }

        return fieldSubjectId[..separatorIndex];
    }

    private static string GetMethodSignatureSuffix(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0 || separatorIndex + 2 >= subjectId.Length)
        {
            throw new InvalidOperationException(
                $"method subject '{subjectId}' is missing method signature information");
        }

        return subjectId[(separatorIndex + 2)..];
    }

    private static string GetRequiredDeclaringTypeSubjectId(AotCoreIrReferenceArtifact targetReference)
    {
        if (!string.IsNullOrWhiteSpace(targetReference.DeclaringTypeSubjectId))
        {
            return targetReference.DeclaringTypeSubjectId;
        }

        return GetDeclaringTypeSubjectId(targetReference.SubjectId);
    }

    private static string GetNativeTypeSymbol(string subjectId)
    {
        return $"chaos_type_{SanitizeSubjectId(subjectId)}";
    }

    private static string GetNativeValueTypeSymbol(string subjectId)
    {
        return $"chaos_valuetype_{SanitizeSubjectId(subjectId)}";
    }

    private static string GetNativeFieldMemberName(string subjectId)
    {
        return $"field_{SanitizeSubjectId(subjectId)}";
    }

    private static string GetNativeStaticFieldSymbol(string subjectId)
    {
        return $"chaos_static_{SanitizeSubjectId(subjectId)}";
    }

    private static string GetNativeBoxTypeSymbol(string subjectId)
    {
        return $"chaos_boxed_type_{SanitizeSubjectId(subjectId)}";
    }

    private static string GetNativeTypeIdSymbol(string subjectId)
    {
        return $"chaos_type_id_{SanitizeSubjectId(subjectId)}";
    }

    private static string GetNativeBoxTypeIdSymbol(string subjectId)
    {
        return $"chaos_boxed_type_id_{SanitizeSubjectId(subjectId)}";
    }

    private static bool HasArrayElementReference(AotCoreIrReferenceArtifact targetReference)
    {
        return !string.IsNullOrWhiteSpace(targetReference.ArrayElementSubjectId);
    }

    private static string GetRuntimeTypeIdExpression(
        string? subjectId,
        AotCoreIrTypeShapeKind typeShape)
    {
        if (string.IsNullOrWhiteSpace(subjectId) || typeShape == AotCoreIrTypeShapeKind.ValueType)
        {
            return "static_cast<std::intptr_t>(0)";
        }

        return GetNativeTypeIdSymbol(subjectId);
    }

    private static byte GetNativeTypeShapeValue(AotCoreIrTypeShapeKind typeShape)
    {
        return typeShape switch
        {
            AotCoreIrTypeShapeKind.ReferenceType => 1,
            AotCoreIrTypeShapeKind.ValueType => 2,
            AotCoreIrTypeShapeKind.InterfaceType => 3,
            _ => 0,
        };
    }

    private static bool RequiresStructuredValueTypePayload(AotCoreIrReferenceArtifact targetReference)
    {
        return targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType &&
               IsStructuredValueTypeSubjectId(targetReference.SubjectId);
    }

    private static bool IsStructuredValueTypeSubjectId(string subjectId)
    {
        return !string.Equals(subjectId, "System.Private.CoreLib/System.Int32", StringComparison.Ordinal);
    }

    private static string SanitizeSubjectId(string subjectId)
    {
        var builder = new StringBuilder(subjectId.Length);
        foreach (var character in subjectId)
        {
            builder.Append(char.IsLetterOrDigit(character) ? character : '_');
        }

        if (builder.Length == 0)
        {
            builder.Append("subject");
        }

        if (char.IsDigit(builder[0]))
        {
            builder.Insert(0, '_');
        }

        return builder.ToString();
    }

    private static string FormatGenericContextComment(GenericContextArtifact genericContext)
    {
        ArgumentNullException.ThrowIfNull(genericContext);

        return
            $"// Generic context: definition={genericContext.DefinitionSubjectId}; type={FormatGenericArgumentList(genericContext.TypeArguments)}; method={FormatGenericArgumentList(genericContext.MethodArguments)}";
    }

    private static string FormatGenericArgumentList(IReadOnlyList<string>? arguments)
    {
        if (arguments is null || arguments.Count == 0)
        {
            return "[]";
        }

        return $"[{string.Join(", ", arguments)}]";
    }
}

public sealed record NativeAotTemplateModel
{
    public required IReadOnlyList<string> Includes { get; init; }

    public required string ObjectModelCode { get; init; }

    public required IReadOnlyList<string> MethodDeclarations { get; init; }

    public required IReadOnlyList<NativeAotMethodTemplateModel> Methods { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string EntrySymbol { get; init; }

    public required string EntryNativeSymbol { get; init; }

    public required string NativeEntryFunctionName { get; init; }
}

public sealed record NativeAotMethodTemplateModel
{
    public required string SubjectId { get; init; }

    public required string MethodSource { get; init; }
}
