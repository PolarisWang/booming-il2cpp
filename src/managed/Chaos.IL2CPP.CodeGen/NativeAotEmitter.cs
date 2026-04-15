using System.Globalization;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeAotEmitter
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    public NativeAotResult Generate(NativeAotRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var loweringPlanPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.NativeAotLoweringPlan);
        var aotCoreIrPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.AotCoreIr);
        var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
        var loweringPlan = LoadRequiredJson<NativeAotLoweringPlanArtifact>(loweringPlanPath);
        var aotCoreIr = LoadRequiredJson<AotCoreIrArtifact>(aotCoreIrPath);
        var closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
        ValidateLoweringPlan(loweringPlan, closureManifest);

        var entryMethod = LoadEntryMethod(aotCoreIr, loweringPlan.EntrySubjectId);
        var translationUnit = BuildGeneratedTranslationUnit(loweringPlan, aotCoreIr, entryMethod);
        var generatedSource = new NativeAotGeneratedSource
        {
            RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
            Contents = translationUnit,
        };

        var manifest = new NativeAotManifestArtifact
        {
            AssemblyName = loweringPlan.AssemblyName,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            PlanArtifactPath = NativeAotArtifactNames.LoweringPlan,
            GeneratedArtifacts =
            [
                new NativeAotGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                },
            ],
        };

        return new NativeAotResult
        {
            OutputRootPath = request.OutputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
            GeneratedSources = [generatedSource],
        };
    }

    private static void ValidateLoweringPlan(
        NativeAotLoweringPlanArtifact loweringPlan,
        ManagedClosureManifestArtifact closureManifest)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.EntrySubjectId, nameof(loweringPlan.EntrySubjectId));
        RequireStringField(loweringPlan.NativeEntryFunctionName, nameof(loweringPlan.NativeEntryFunctionName));
        RequireStringField(loweringPlan.EntrySymbol, nameof(loweringPlan.EntrySymbol));
        RequireStringField(loweringPlan.EntryMethodToken, nameof(loweringPlan.EntryMethodToken));
        RequireStringField(loweringPlan.WorkloadAbi, nameof(loweringPlan.WorkloadAbi));

        if (!string.Equals(loweringPlan.PlanKind, "generic-managed-entry", StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"unsupported native-aot lowering plan kind '{loweringPlan.PlanKind}'");
        }

        if (!string.Equals(loweringPlan.WorkloadAbi, "int(void)", StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"unsupported native-aot workload ABI '{loweringPlan.WorkloadAbi}'");
        }

        if (!string.Equals(loweringPlan.EntrySubjectId, closureManifest.EntrySubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot plan entry '{loweringPlan.EntrySubjectId}' does not match closure entry '{closureManifest.EntrySubjectId}'");
        }
    }

    private static AotCoreIrMethodArtifact LoadEntryMethod(
        AotCoreIrArtifact aotCoreIr,
        string entrySubjectId)
    {
        var entryMethod = aotCoreIr.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, entrySubjectId, StringComparison.Ordinal));
        if (entryMethod is null)
        {
            throw new InvalidOperationException(
                $"aot-core-ir entry method '{entrySubjectId}' is missing from generated closure artifacts");
        }

        return entryMethod;
    }

    private static string BuildGeneratedTranslationUnit(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod)
    {
        ValidateEntryMethod(entryMethod);
        if (!string.Equals(entryMethod.NativeSymbol, loweringPlan.EntrySymbol, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot entry symbol '{loweringPlan.EntrySymbol}' does not match aot-core-ir symbol '{entryMethod.NativeSymbol}'");
        }

        var reachableMethods = CollectReachableMethods(aotCoreIr, entryMethod);

        var builder = new StringBuilder();
        builder.AppendLine("#include <array>");
        builder.AppendLine("#include <cstddef>");
        builder.AppendLine("#include <cstdint>");
        builder.AppendLine("#include <cstdlib>");
        builder.AppendLine("#include <limits>");
        builder.AppendLine();
        builder.AppendLine("namespace");
        builder.AppendLine("{");
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
        builder.AppendLine("}");
        builder.AppendLine();

        foreach (var method in reachableMethods)
        {
            builder.AppendLine(FormatMethodDeclaration(method));
        }
        builder.AppendLine();

        foreach (var method in reachableMethods)
        {
            EmitManagedMethod(builder, method);
            builder.AppendLine();
        }

        builder.AppendLine($"// Native AOT entry for {loweringPlan.EntrySubjectId}");
        builder.AppendLine($"// Managed symbol: {loweringPlan.EntrySymbol}");
        builder.AppendLine($"extern \"C\" int {loweringPlan.NativeEntryFunctionName}(void)");
        builder.AppendLine("{");
        builder.AppendLine($"    return {entryMethod.NativeSymbol}();");
        builder.AppendLine("}");
        return builder.ToString();
    }

    private static IReadOnlyList<AotCoreIrMethodArtifact> CollectReachableMethods(
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod)
    {
        var methods = aotCoreIr.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
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
                if (!string.Equals(instruction.Op, "call", StringComparison.Ordinal) ||
                    string.IsNullOrWhiteSpace(instruction.Callee) ||
                    !methods.TryGetValue(instruction.Callee, out var calleeMethod))
                {
                    continue;
                }

                Visit(calleeMethod);
            }

            ordered.Add(method);
        }

        Visit(entryMethod);
        return ordered;
    }

    private static string FormatMethodDeclaration(AotCoreIrMethodArtifact method)
    {
        return $"extern \"C\" {MapDirectCallReturnType(method.ReturnType)} {method.NativeSymbol}({FormatMethodParameterSignature(method.ParameterCount)});";
    }

    private static void EmitManagedMethod(
        StringBuilder builder,
        AotCoreIrMethodArtifact method)
    {
        ValidateMethod(method);

        var instructions = method.Instructions;
        ValidateInstructions(method, instructions);

        var stackCapacity = Math.Max(instructions.Count, 1);
        var firstOffset = GetRequiredIlOffset(instructions[0]);
        var offsets = instructions
            .Select(GetRequiredIlOffset)
            .ToHashSet();

        builder.AppendLine($"// Managed method: {method.SubjectId}");
        builder.AppendLine(
            $"extern \"C\" {MapDirectCallReturnType(method.ReturnType)} {method.NativeSymbol}({FormatMethodParameterSignature(method.ParameterCount)})");
        builder.AppendLine("{");
        builder.AppendLine($"    std::array<std::int32_t, {Math.Max(method.ParameterCount, 1)}> chaos_args{{{FormatMethodArgumentInitializer(method.ParameterCount)}}};");
        builder.AppendLine($"    std::array<std::int32_t, {Math.Max(method.LocalCount, 1)}> chaos_locals{{}};");
        builder.AppendLine($"    std::array<std::int32_t, {stackCapacity}> chaos_eval_stack{{}};");
        builder.AppendLine("    std::size_t chaos_stack_top = 0;");
        builder.AppendLine();
        builder.AppendLine($"    goto chaos_ip_{firstOffset};");
        builder.AppendLine();

        for (var index = 0; index < instructions.Count; index++)
        {
            var instruction = instructions[index];
            var offset = GetRequiredIlOffset(instruction);
            int? nextOffset = index + 1 < instructions.Count ? GetRequiredIlOffset(instructions[index + 1]) : null;
            builder.AppendLine($"chaos_ip_{offset}:");
            EmitInstruction(builder, method, instruction, nextOffset, offsets);
            builder.AppendLine();
        }

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
        if (!method.IsStatic)
        {
            throw new NotSupportedException(
                $"native-aot method '{method.SubjectId}' must be static");
        }

        if (string.IsNullOrWhiteSpace(method.NativeSymbol))
        {
            throw new InvalidOperationException(
                $"native-aot method '{method.SubjectId}' is missing native symbol metadata");
        }

        _ = MapDirectCallReturnType(method.ReturnType);

        if (method.ExceptionRegionCount != 0)
        {
            throw new NotSupportedException(
                $"native-aot method '{method.SubjectId}' does not support exception regions");
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

    private static void EmitInstruction(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        IReadOnlySet<int> offsets)
    {
        switch (instruction.Op)
        {
            case "ldc.i4":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = {FormatInt32Literal(GetRequiredIntOperand(instruction))};");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldarg":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_args[{GetRequiredIntOperand(instruction)}];");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "ldloc":
                builder.AppendLine(
                    $"    chaos_eval_stack[chaos_stack_top++] = chaos_locals[{GetRequiredIntOperand(instruction)}];");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "stloc":
                builder.AppendLine(
                    $"    chaos_locals[{GetRequiredIntOperand(instruction)}] = chaos_eval_stack[--chaos_stack_top];");
                AppendGotoNext(builder, nextOffset, instruction.Op);
                return;

            case "add":
                EmitBinaryArithmetic(builder, "chaos_wrap_add", nextOffset, instruction.Op);
                return;

            case "sub":
                EmitBinaryArithmetic(builder, "chaos_wrap_sub", nextOffset, instruction.Op);
                return;

            case "mul":
                EmitBinaryArithmetic(builder, "chaos_wrap_mul", nextOffset, instruction.Op);
                return;

            case "div":
                EmitBinaryArithmetic(builder, "chaos_div", nextOffset, instruction.Op);
                return;

            case "rem":
                EmitBinaryArithmetic(builder, "chaos_rem", nextOffset, instruction.Op);
                return;

            case "call":
                EmitDirectCall(builder, instruction, nextOffset, instruction.Op);
                return;

            case "br":
                builder.AppendLine($"    goto chaos_ip_{GetRequiredBranchTarget(instruction, offsets)};");
                return;

            case "blt":
                EmitComparisonBranch(builder, "<", instruction, nextOffset, offsets);
                return;

            case "ret":
                if (string.Equals(method.ReturnType, "System.Void", StringComparison.Ordinal))
                {
                    builder.AppendLine("    return;");
                }
                else
                {
                    builder.AppendLine("    return chaos_eval_stack[--chaos_stack_top];");
                }
                return;

            default:
                throw new NotSupportedException(
                    $"native-aot lowering does not support opcode '{instruction.Op}'");
        }
    }

    private static void EmitDirectCall(
        StringBuilder builder,
        AotCoreIrInstructionArtifact instruction,
        int? nextOffset,
        string op)
    {
        var targetSymbol = GetRequiredTargetSymbol(instruction);
        var parameterCount = GetRequiredTargetParameterCount(instruction);
        var mappedReturnType = MapDirectCallReturnType(instruction.TargetReturnType);

        builder.AppendLine("    {");
        for (var parameterIndex = parameterCount - 1; parameterIndex >= 0; parameterIndex--)
        {
            builder.AppendLine(
                $"        const auto chaos_arg_{parameterIndex} = chaos_eval_stack[--chaos_stack_top];");
        }

        var invocation = $"{targetSymbol}({FormatDirectCallArgumentList(parameterCount)})";
        if (string.Equals(mappedReturnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"        {invocation};");
        }
        else
        {
            builder.AppendLine($"        const auto chaos_result = {invocation};");
            builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_result;");
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
        builder.AppendLine("        const auto chaos_right = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine("        const auto chaos_left = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine(
            $"        chaos_eval_stack[chaos_stack_top++] = {helperName}(chaos_left, chaos_right);");
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
        builder.AppendLine("    {");
        builder.AppendLine("        const auto chaos_right = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine("        const auto chaos_left = chaos_eval_stack[--chaos_stack_top];");
        builder.AppendLine(
            $"        if (chaos_left {comparisonOperator} chaos_right)");
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

    private static string MapDirectCallReturnType(string? targetReturnType)
    {
        return targetReturnType switch
        {
            "System.Int32" => "std::int32_t",
            "System.Void" => "void",
            _ => throw new NotSupportedException(
                $"native-aot lowering does not support direct-call return type '{targetReturnType ?? "<null>"}'"),
        };
    }

    private static string FormatMethodParameterSignature(int parameterCount)
    {
        if (parameterCount < 0)
        {
            throw new NotSupportedException("native-aot lowering requires a non-negative parameter count.");
        }

        if (parameterCount == 0)
        {
            return "void";
        }

        return string.Join(
            ", ",
            Enumerable.Range(0, parameterCount).Select(index => $"std::int32_t chaos_arg_{index}"));
    }

    private static string FormatMethodArgumentInitializer(int parameterCount)
    {
        if (parameterCount == 0)
        {
            return string.Empty;
        }

        return string.Join(", ", Enumerable.Range(0, parameterCount).Select(index => $"chaos_arg_{index}"));
    }

    private static string FormatDirectCallArgumentList(int parameterCount)
    {
        if (parameterCount == 0)
        {
            return string.Empty;
        }

        return string.Join(", ", Enumerable.Range(0, parameterCount).Select(index => $"chaos_arg_{index}"));
    }

    private static string FormatInt32Literal(int value)
    {
        return value == int.MinValue
            ? "std::numeric_limits<std::int32_t>::min()"
            : value.ToString(CultureInfo.InvariantCulture);
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
}
