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
    private static bool TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
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
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 9 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var middleForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(middleForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(terminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> middleForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> terminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            middleForwarderInstructions = GetSingleBlockInstructions(middleForwarderMethod);
            terminalForwarderInstructions = GetSingleBlockInstructions(terminalForwarderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(middleForwarderMethod, middleForwarderInstructions);
            ValidateSingleArgumentForwarderShape(terminalForwarderMethod, terminalForwarderInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[6], method.SubjectId, 6);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[7]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["middle_forwarder_stub_name"] = middleForwarderStubName,
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
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
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 10 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[9].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var firstTerminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        var secondTerminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        var thirdTerminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[6], method.SubjectId, 6);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(firstTerminalForwarderSubjectId, out var firstTerminalForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(secondTerminalForwarderSubjectId, out var secondTerminalForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(thirdTerminalForwarderSubjectId, out var thirdTerminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(firstTerminalForwarderSubjectId, out var firstTerminalForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(secondTerminalForwarderSubjectId, out var secondTerminalForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(thirdTerminalForwarderSubjectId, out var thirdTerminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(firstTerminalForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(secondTerminalForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(thirdTerminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> firstTerminalForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> secondTerminalForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> thirdTerminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            firstTerminalForwarderInstructions = GetSingleBlockInstructions(firstTerminalForwarderMethod);
            secondTerminalForwarderInstructions = GetSingleBlockInstructions(secondTerminalForwarderMethod);
            thirdTerminalForwarderInstructions = GetSingleBlockInstructions(thirdTerminalForwarderMethod);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(firstTerminalForwarderMethod, firstTerminalForwarderInstructions);
            ValidateSingleArgumentForwarderShape(secondTerminalForwarderMethod, secondTerminalForwarderInstructions);
            ValidateSingleArgumentForwarderShape(thirdTerminalForwarderMethod, thirdTerminalForwarderInstructions);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[7], method.SubjectId, 7);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[8]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["first_terminal_forwarder_stub_name"] = firstTerminalForwarderStubName,
            ["second_terminal_forwarder_stub_name"] = secondTerminalForwarderStubName,
            ["third_terminal_forwarder_stub_name"] = thirdTerminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticLiteralStringReturnStub(
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

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = Encoding.UTF8.GetByteCount(literal),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticResourceKeyStringReturnStub(
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

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            instructions.Count != 3 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!targetSubjectId.EndsWith("::GetResourceString:System.String(System.String)", StringComparison.Ordinal))
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = Encoding.UTF8.GetByteCount(literal),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerConsoleWriteLineStub(
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
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 4 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[2]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }

}
