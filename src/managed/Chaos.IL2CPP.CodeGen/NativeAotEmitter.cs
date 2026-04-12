using System.Globalization;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Loader;

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
        var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
        var loweringPlan = LoadRequiredJson<NativeAotLoweringPlanArtifact>(loweringPlanPath);
        var closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
        ValidateLoweringPlan(loweringPlan, closureManifest);

        var entryMethod = LoadEntryMethod(managedClosureRoot, closureManifest, loweringPlan.EntrySubjectId);
        var translationUnit = BuildGeneratedTranslationUnit(loweringPlan, entryMethod);
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
        RequireStringField(closureManifest.InputAssemblyPath, nameof(closureManifest.InputAssemblyPath));

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

    private static ManagedMethodModel LoadEntryMethod(
        string managedClosureRoot,
        ManagedClosureManifestArtifact closureManifest,
        string entrySubjectId)
    {
        var inputAssemblyPath = ResolveInputAssemblyPath(managedClosureRoot, closureManifest.InputAssemblyPath);
        var loader = new LoaderStage();
        var loadedAssembly = loader.Load(new ManagedClosureRequest(
            inputAssemblyPath,
            managedClosureRoot,
            EntryPointSubjectIdOverride: entrySubjectId));

        var entryMethod = loadedAssembly.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, entrySubjectId, StringComparison.Ordinal));
        if (entryMethod is null)
        {
            throw new InvalidOperationException(
                $"managed closure entry method '{entrySubjectId}' is missing from '{inputAssemblyPath}'");
        }

        return entryMethod;
    }

    private static string ResolveInputAssemblyPath(string managedClosureRoot, string manifestInputAssemblyPath)
    {
        if (Path.IsPathRooted(manifestInputAssemblyPath))
        {
            return manifestInputAssemblyPath;
        }

        var candidates = new[]
        {
            Path.GetFullPath(manifestInputAssemblyPath, Environment.CurrentDirectory),
            Path.GetFullPath(manifestInputAssemblyPath, managedClosureRoot),
        };

        foreach (var candidate in candidates)
        {
            if (File.Exists(candidate))
            {
                return candidate;
            }
        }

        return candidates[0];
    }

    private static string BuildGeneratedTranslationUnit(
        NativeAotLoweringPlanArtifact loweringPlan,
        ManagedMethodModel entryMethod)
    {
        ValidateEntryMethod(entryMethod);

        var instructions = FlattenInstructions(entryMethod);
        ValidateInstructions(entryMethod, instructions);

        var localsCount = DetermineLocalsCount(instructions);
        var stackCapacity = Math.Max(instructions.Count, 1);
        var firstOffset = GetRequiredIlOffset(instructions[0]);
        var offsets = instructions
            .Select(GetRequiredIlOffset)
            .ToHashSet();

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
        builder.AppendLine($"// Native AOT entry for {loweringPlan.EntrySubjectId}");
        builder.AppendLine($"// Managed symbol: {loweringPlan.EntrySymbol}");
        builder.AppendLine($"extern \"C\" int {loweringPlan.NativeEntryFunctionName}(void)");
        builder.AppendLine("{");
        builder.AppendLine($"    std::array<std::int32_t, {Math.Max(localsCount, 1)}> chaos_locals{{}};");
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
            EmitInstruction(builder, instruction, nextOffset, offsets);
            builder.AppendLine();
        }

        builder.AppendLine("}");
        return builder.ToString();
    }

    private static IReadOnlyList<ManagedInstructionModel> FlattenInstructions(ManagedMethodModel entryMethod)
    {
        return entryMethod.Body.Blocks
            .SelectMany(block => block.Instructions)
            .ToList();
    }

    private static void ValidateEntryMethod(ManagedMethodModel entryMethod)
    {
        if (!entryMethod.IsStatic)
        {
            throw new NotSupportedException(
                $"native-aot entry '{entryMethod.SubjectId}' must be static");
        }

        if (entryMethod.Parameters.Count != 0)
        {
            throw new NotSupportedException(
                $"native-aot entry '{entryMethod.SubjectId}' must not take parameters");
        }

        if (!string.Equals(entryMethod.ReturnType, "System.Int32", StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"native-aot entry '{entryMethod.SubjectId}' must return System.Int32");
        }

        if (entryMethod.Body.ExceptionRegions.Count != 0)
        {
            throw new NotSupportedException(
                $"native-aot entry '{entryMethod.SubjectId}' does not support exception regions");
        }
    }

    private static void ValidateInstructions(
        ManagedMethodModel entryMethod,
        IReadOnlyList<ManagedInstructionModel> instructions)
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

    private static int DetermineLocalsCount(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        var maxLocalIndex = -1;
        foreach (var instruction in instructions)
        {
            if (!string.Equals(instruction.Op, "ldloc", StringComparison.Ordinal) &&
                !string.Equals(instruction.Op, "stloc", StringComparison.Ordinal))
            {
                continue;
            }

            maxLocalIndex = Math.Max(maxLocalIndex, GetRequiredIntOperand(instruction));
        }

        return maxLocalIndex + 1;
    }

    private static void EmitInstruction(
        StringBuilder builder,
        ManagedInstructionModel instruction,
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

            case "br":
                builder.AppendLine($"    goto chaos_ip_{GetRequiredBranchTarget(instruction, offsets)};");
                return;

            case "blt":
                EmitComparisonBranch(builder, "<", instruction, nextOffset, offsets);
                return;

            case "ret":
                builder.AppendLine("    return chaos_eval_stack[--chaos_stack_top];");
                return;

            default:
                throw new NotSupportedException(
                    $"native-aot lowering does not support opcode '{instruction.Op}'");
        }
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
        ManagedInstructionModel instruction,
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
        ManagedInstructionModel instruction,
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

    private static int GetRequiredIlOffset(ManagedInstructionModel instruction)
    {
        if (instruction.IlOffset is null)
        {
            throw new InvalidOperationException(
                $"native-aot lowering requires IL offsets; opcode '{instruction.Op}' did not provide one");
        }

        return instruction.IlOffset.Value;
    }

    private static int GetRequiredIntOperand(ManagedInstructionModel instruction)
    {
        if (instruction.Operand is int value)
        {
            return value;
        }

        throw new InvalidOperationException(
            $"opcode '{instruction.Op}' requires an Int32 operand for native-aot lowering");
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
