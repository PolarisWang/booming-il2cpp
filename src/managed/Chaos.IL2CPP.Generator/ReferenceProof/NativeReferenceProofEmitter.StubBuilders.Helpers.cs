using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{

    private static bool IsConsoleWriteLineStringTarget(string? callee)
    {
        return string.Equals(callee, ConsoleWriteLineStringIcall, StringComparison.Ordinal) ||
               string.Equals(callee, ConsoleWriteLineStringMethodSubjectId, StringComparison.Ordinal);
    }



    private static string NormalizeStringConcatIcall(string? callee)
    {
        if (string.Equals(callee, StringConcatPairIcall, StringComparison.Ordinal) ||
            string.Equals(callee, StringConcatPairMethodSubjectId, StringComparison.Ordinal))
        {
            return StringConcatPairIcall;
        }

        if (string.Equals(callee, StringConcatTripleIcall, StringComparison.Ordinal) ||
            string.Equals(callee, StringConcatTripleMethodSubjectId, StringComparison.Ordinal))
        {
            return StringConcatTripleIcall;
        }

        return callee ?? StringConcatPairIcall;
    }



    private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(TypedIlMethodArtifact method)
    {
        if (method.Blocks.Count != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects a single block in '{method.SubjectId}', but found {method.Blocks.Count}");
        }

        return method.Blocks[0].Instructions;
    }



    private static bool MatchesLegacyStaticForwarderCtorGetterEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 7 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal);
    }



    private static bool IsPInvokeDllImportMinimalEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 5 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal);
    }



    private static string GetRequiredInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to carry a callee");
        }

        return instruction.Callee;
    }



    private static string GetRequiredOperandString(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            string value => value,
            JsonElement element when element.ValueKind == JsonValueKind.String => element.GetString()
                ?? throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
            _ => throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
        };
    }



    private static int GetRequiredOperandInt(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value,
            long value => checked((int)value),
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetInt32(),
            _ => throw new InvalidOperationException($"expected integer operand for '{instruction.Op}'"),
        };
    }



    private static int GetRequiredPromotedInt64ConstantAsInt(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int constantInstructionIndex,
        string subjectId)
    {
        RequireInstructionOp(instructions[constantInstructionIndex], "ldc.i4", subjectId, constantInstructionIndex);
        RequireInstructionOp(instructions[constantInstructionIndex + 1], "conv.i8", subjectId, constantInstructionIndex + 1);
        return GetRequiredOperandInt(instructions[constantInstructionIndex]);
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



    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        string subjectId,
        string description)
    {
        if (GetMetadataRegistrationLookup(registrations).TryGetRegistration(registrationKind, subjectId, out var registration))
        {
            return registration;
        }

        throw new InvalidOperationException(
            $"missing required '{registrationKind}' metadata registration for {description}");
    }



    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        Func<MetadataRegistrationEntry, bool> predicate,
        string description)
    {
        var registration = GetMetadataRegistrationsForKind(registrations, registrationKind)
            .FirstOrDefault(predicate);
        if (registration is null)
        {
            throw new InvalidOperationException(
                $"missing required '{registrationKind}' metadata registration for {description}");
        }

        return registration;
    }



    private static string GetRequiredRegistrationName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing name metadata");
    }



    private static string GetRequiredRegistrationDisplayName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.DisplayName))
        {
            return registration.DisplayName;
        }

        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing displayName metadata");
    }



    private static bool IsCompilerGeneratedMemberName(string? name)
    {
        return string.IsNullOrWhiteSpace(name) || name.StartsWith("<", StringComparison.Ordinal);
    }



    private static bool IsSpecialMethodName(string? name)
    {
        if (string.IsNullOrWhiteSpace(name))
        {
            return true;
        }

        return name.StartsWith(".", StringComparison.Ordinal) ||
               name.StartsWith("<", StringComparison.Ordinal) ||
               name.StartsWith("get_", StringComparison.Ordinal) ||
               name.StartsWith("set_", StringComparison.Ordinal) ||
               name.StartsWith("add_", StringComparison.Ordinal) ||
               name.StartsWith("remove_", StringComparison.Ordinal);
    }



    private static uint GetRequiredMetadataToken(
        MetadataRegistrationArtifact metadataRegistration,
        string registrationKind,
        string subjectId)
    {
        return GetMetadataRegistrationLookup(metadataRegistration.Registrations)
            .GetRequiredToken(registrationKind, subjectId);
    }



    private static string CreateTypeTokenLiteral(
        MetadataRegistrationArtifact metadataRegistration,
        string subjectId)
    {
        var hasConcreteRegistration = GetMetadataRegistrationLookup(metadataRegistration.Registrations)
            .HasConcreteType(subjectId);
        return hasConcreteRegistration
            ? FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", subjectId))
            : FormatCppTokenLiteral(CreatePseudoTypeToken(subjectId));
    }



    private static uint CreatePseudoTypeToken(string subjectId)
    {
        const uint typeTokenPrefix = 0x02000000u;
        uint hash = 2166136261u;

        foreach (var current in subjectId)
        {
            hash ^= current;
            hash *= 16777619u;
        }

        var rowIndex = hash & 0x00FFFFFFu;
        if (rowIndex == 0u)
        {
            rowIndex = 1u;
        }

        return typeTokenPrefix | rowIndex;
    }

}
