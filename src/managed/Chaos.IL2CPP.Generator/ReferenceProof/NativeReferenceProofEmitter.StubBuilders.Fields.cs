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

    private static bool TryBuildAssemblyBoundConstructorFieldSetterStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateConstructorShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var storedFieldSubjectId = GetRequiredOperandString(instructions[4]);
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonConstructorFieldSetterDescriptor(fieldBindingIndex);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConstructorFieldSetterStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundFieldBackedStringReturnStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateFieldBackedStringInstanceMethodShape(method, instructions);
        }
        catch
        {
            return false;
        }

        if (!string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal))
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var loadedFieldSubjectId = GetRequiredOperandString(instructions[2]);
        var messagePrefixLiteral = GetRequiredOperandString(instructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(method, instructions);
        var concatIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(method, instructions));
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBackedStringReturnDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(messagePrefixLiteral),
            Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ToCppStringLiteral(messageSuffixLiteral),
            Encoding.UTF8.GetByteCount(messageSuffixLiteral));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonFieldBackedStringReturnStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundFieldArgumentStringReturnStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 1)
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateFieldBackedStringInstanceMethodShape(method, instructions);
        }
        catch
        {
            return false;
        }

        if (instructions.Count != 7 ||
            !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 1)
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var loadedFieldSubjectId = GetRequiredOperandString(instructions[2]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(method, instructions);
        var concatIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(method, instructions));
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldArgumentStringReturnDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(messageSuffixLiteral),
            Encoding.UTF8.GetByteCount(messageSuffixLiteral));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonFieldArgumentStringReturnStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundFieldGetterStringReturnStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateFieldGetterShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var loadedFieldSubjectId = GetRequiredOperandString(instructions[1]);
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldGetterStringReturnDescriptor(fieldBindingIndex);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonFieldGetterStringReturnStubTemplateRelativePath),
            model);
        return true;
    }



    private static string? BuildPreferredAssemblyDispatchSubjectId(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        return BuildPreferredAssemblyDispatchSubjectId(
            loweringPlan,
            loweringPlan.TranslationUnitMethodSubjectIds ?? []);
    }



    private static string? BuildPreferredAssemblyDispatchSubjectId(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        IReadOnlyList<string> methodSubjectIds)
    {
        if (!string.Equals(loweringPlan.RuntimeExecutionKind, "assembly-bound-native-reference-skeleton", StringComparison.Ordinal))
        {
            return null;
        }

        if (!string.IsNullOrWhiteSpace(loweringPlan.EntrySubjectId))
        {
            return loweringPlan.EntrySubjectId;
        }

        var proofEntrySubjectId = methodSubjectIds.FirstOrDefault(subjectId =>
            subjectId.Contains("ProofEntry::Run", StringComparison.Ordinal) ||
            subjectId.Contains("Entry::Run", StringComparison.Ordinal));
        if (!string.IsNullOrWhiteSpace(proofEntrySubjectId))
        {
            return proofEntrySubjectId;
        }
        return methodSubjectIds.FirstOrDefault(subjectId => subjectId.Contains("::Main:", StringComparison.Ordinal))
            ?? methodSubjectIds.FirstOrDefault(subjectId =>
                !subjectId.Contains("::.cctor:", StringComparison.Ordinal) &&
                !subjectId.Contains("::.ctor:", StringComparison.Ordinal) &&
                !subjectId.Contains("::<", StringComparison.Ordinal))
            ?? methodSubjectIds.FirstOrDefault();
    }

}
