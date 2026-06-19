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

    private static bool TryBuildAssemblyBoundCollectionsManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCollectionsManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundSpanHelpersKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundSpanHelpersKernelCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundValueTypeKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundValueTypeKernelCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundVectorKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundVectorKernelCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundArrayAndMemoryMarshalStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundArrayAndMemoryMarshalCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundHalfManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundHalfManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundFloatingScalarManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundFloatingScalarManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundPrimitiveScalarManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundPrimitiveScalarManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundWideNumericManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundWideNumericManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundCalendarStructManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCalendarStructManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundIdentityStructManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundIdentityStructManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundCompanionManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCompanionManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundDateTimeSemanticEngineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundDateTimeSemanticEngineCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundGlobalizationDateTimeSupportStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundGlobalizationDateTimeSupportCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundNumberManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundNumberManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundDecimalManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundDecimalManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundCharManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCharManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundEnumManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundEnumManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodPointers,
            methodsBySubjectId,
            pageSupportBuilder,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundStaticCheckedPrimitiveConvertStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        string outputManagedType,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out var outputCppType))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonCheckedPrimitiveConvertShape(
                method.SubjectId,
                method.Parameters[0].Type,
                outputManagedType,
                instructions,
                out var inputCppType,
                out var overflowConditionExpression,
                out var throwSubjectId))
        {
            return false;
        }

        if (!methodStubNamesBySubjectId.TryGetValue(throwSubjectId, out var throwStubName))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["overflow_condition_expression"] = overflowConditionExpression,
            ["converted_value_expression"] = $"static_cast<{outputCppType}>(request->value)",
            ["throw_stub_name"] = throwStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticCheckedPrimitiveConvertStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticValueTypeIdentityForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        string outputManagedType,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            !string.Equals(method.Parameters[0].Type, outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonValueTypeByValueInputCppType(outputManagedType, out var outputCppType, out _))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["output_cpp_type"] = outputCppType,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticValueTypeIdentityForwarderStubTemplateRelativePath),
            model);
        return true;
    }

}
