namespace Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

public sealed partial class NativeReferenceProofEmitter
{

    private static bool TryBuildAssemblyBoundStaticBoolForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        var callInstructionIndex = -1;
        if (instructions.Count == 2 &&
            string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            callInstructionIndex = 0;
        }
        else if (instructions.Count == 3 &&
                 string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                 GetRequiredOperandInt(instructions[0]) == 0 &&
                 string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
                 string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            callInstructionIndex = 1;
        }

        if (callInstructionIndex < 0)
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[callInstructionIndex], method.SubjectId, callInstructionIndex);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticIntForwarderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoolIdentityForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-forwarder", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Boolean", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateSingleArgumentForwarderShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoolIdentityForwarderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoolProducerForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 3 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            string.Equals(forwarderSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName))
        {
            return false;
        }

        if (!methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod))
        {
            return false;
        }

        if (!string.Equals(forwarderMethod.MethodRole, "static-forwarder", StringComparison.Ordinal) ||
            !string.Equals(forwarderMethod.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(forwarderMethod.SubjectId), "System.Boolean", StringComparison.Ordinal) ||
            forwarderMethod.Parameters.Count != 1 ||
            !string.Equals(forwarderMethod.Parameters[0].Type, "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> forwarderInstructions;
        try
        {
            forwarderInstructions = GetSingleBlockInstructions(forwarderMethod);
            ValidateSingleArgumentForwarderShape(forwarderMethod, forwarderInstructions);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["producer_stub_name"] = producerStubName,
            ["forwarder_stub_name"] = forwarderStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoolProducerForwarderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticPrimitiveConvertStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if ((!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) &&
             !string.Equals(method.MethodRole, "static-forwarder", StringComparison.Ordinal)) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1)
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonPrimitiveConvertShape(
                method.Parameters[0].Type,
                GetMethodReturnType(method.SubjectId),
                out var inputCppType,
                out var outputCppType,
                out var convertedValueExpression))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!MatchesRuntimeSkeletonPrimitiveConvertInstructionShape(
                instructions,
                method.Parameters[0].Type,
                GetMethodReturnType(method.SubjectId)))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["converted_value_expression"] = convertedValueExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticPrimitiveConvertStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundConvertRuntimeHelperStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundConvertRuntimeHelperCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundStringManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundStringManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundUnsafeManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundUnsafeManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundMemoryExtensionsManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundMemoryExtensionsManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

}
