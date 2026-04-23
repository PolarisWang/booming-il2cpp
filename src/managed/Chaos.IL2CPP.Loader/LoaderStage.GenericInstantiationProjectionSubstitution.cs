using System.Collections.Immutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static ManagedMethodBodyModel ProjectInstantiationMethodBody(
        ManagedMethodBodyModel body,
        IReadOnlyDictionary<string, string> substitutions,
        IReadOnlyDictionary<string, string> subjectSubstitutions)
    {
        return new ManagedMethodBodyModel
        {
            ExceptionRegions = body.ExceptionRegions.Select(region => new ManagedExceptionRegionModel
            {
                HandlingKind = region.HandlingKind,
                TryOffset = region.TryOffset,
                TryLength = region.TryLength,
                HandlerOffset = region.HandlerOffset,
                HandlerLength = region.HandlerLength,
                FilterOffset = region.FilterOffset,
                CatchTypeSubjectId = region.CatchTypeSubjectId is null
                    ? null
                    : ProjectInstantiationText(region.CatchTypeSubjectId, substitutions, subjectSubstitutions),
            }).ToList(),
            Blocks = body.Blocks.Select(block => new ManagedBlockModel
            {
                BlockId = block.BlockId,
                Instructions = block.Instructions.Select(instruction => new ManagedInstructionModel
                {
                    Op = instruction.Op,
                    Operand = ProjectInstantiationOperand(instruction.Operand, substitutions, subjectSubstitutions),
                    IlOffset = instruction.IlOffset,
                    ResultType = instruction.ResultType is null
                        ? null
                        : ProjectInstantiationText(instruction.ResultType, substitutions, subjectSubstitutions),
                    Callee = instruction.Callee is null
                        ? null
                        : ProjectInstantiationText(instruction.Callee, substitutions, subjectSubstitutions),
                    CallSiteSignature = instruction.CallSiteSignature is null
                        ? null
                        : new ManagedCallSiteSignature
                        {
                            KindCode = instruction.CallSiteSignature.KindCode,
                            ReturnType = ProjectInstantiationText(
                                instruction.CallSiteSignature.ReturnType,
                                substitutions,
                                subjectSubstitutions),
                            ParameterTypes = instruction.CallSiteSignature.ParameterTypes
                                .Select(parameterType => ProjectInstantiationText(parameterType, substitutions, subjectSubstitutions))
                                .ToList(),
                        },
                    Reference = instruction.Reference is null
                        ? null
                        : new ManagedInstructionReference
                        {
                            AssemblyName = instruction.Reference.AssemblyName,
                            SubjectKind = instruction.Reference.SubjectKind,
                            SubjectId = ProjectInstantiationText(instruction.Reference.SubjectId, substitutions, subjectSubstitutions),
                        },
                }).ToList(),
            }).ToList(),
        };
    }

    private static object? ProjectInstantiationOperand(
        object? operand,
        IReadOnlyDictionary<string, string> substitutions,
        IReadOnlyDictionary<string, string> subjectSubstitutions)
    {
        return operand switch
        {
            string value => ProjectInstantiationText(value, substitutions, subjectSubstitutions),
            _ => operand,
        };
    }

    private static string ProjectInstantiationText(
        string value,
        IReadOnlyDictionary<string, string> substitutions,
        IReadOnlyDictionary<string, string> subjectSubstitutions)
    {
        if (subjectSubstitutions.TryGetValue(value, out var exactReplacement))
        {
            value = exactReplacement;
        }

        foreach (var (placeholder, replacement) in substitutions.OrderByDescending(pair => pair.Key.Length))
        {
            value = value.Replace(placeholder, replacement, StringComparison.Ordinal);
        }

        return value;
    }

    private static ImmutableDictionary<string, string> CreateSubstitutionMap(
        IReadOnlyList<string> typeArguments,
        IReadOnlyList<string> methodArguments)
    {
        var builder = ImmutableDictionary.CreateBuilder<string, string>(StringComparer.Ordinal);

        for (var index = 0; index < typeArguments.Count; index++)
        {
            builder[$"!{index}"] = typeArguments[index];
        }

        for (var index = 0; index < methodArguments.Count; index++)
        {
            builder[$"!!{index}"] = methodArguments[index];
        }

        return builder.ToImmutable();
    }
}
