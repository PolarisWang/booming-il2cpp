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

    private static bool TryBuildAssemblyBoundStaticStringCharProviderStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonStringCharProviderShape(
                method,
                out var targetMethodSubjectId))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId)),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringCharProviderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProviderPassthroughStub(
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

        if (!TryResolveRuntimeSkeletonStringProviderPassthroughShape(method))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProviderPassthroughStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticObjectCharProviderStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonObjectCharProviderShape(
                method,
                out var targetMethodSubjectId))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId)),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticObjectCharProviderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticBoxedValueTypeStringInstanceCallStub(
        string assemblyName,
        string subjectId,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonBoxedValueTypeStringInstanceCallShape(
                method,
                methodsBySubjectId,
                out var inputCppType,
                out var boxedValueTypeSubjectId,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var inputSize,
                out var argCount))
        {
            return false;
        }

        string boxedValueTypeTokenLiteral;
        string targetMethodTokenLiteral;
        string targetAssemblyNameLiteral;
        if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
        {
            boxedValueTypeTokenLiteral = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId);
            targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            targetAssemblyNameLiteral = ToCppStringLiteral(targetAssemblyName);
        }
        else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                     targetAssemblyName,
                     targetMethodSubjectId,
                     out var externalResolution))
        {
            boxedValueTypeTokenLiteral = FormatCppTokenLiteral(externalResolution.TypeToken);
            targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            targetAssemblyNameLiteral = ToCppStringLiteral(externalResolution.AssemblyName);
        }
        else
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_assembly_name_literal"] = targetAssemblyNameLiteral,
            ["input_cpp_type"] = inputCppType,
            ["boxed_value_type_token"] = boxedValueTypeTokenLiteral,
            ["target_method_token"] = targetMethodTokenLiteral,
            ["input_size"] = inputSize,
            ["argc"] = argCount,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedValueTypeStringInstanceCallStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticBoxedValueTypeInstanceCallStub(
        string assemblyName,
        string subjectId,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
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

        if (!TryResolveRuntimeSkeletonBoxedValueTypeInstanceCallShape(
                method,
                methodsBySubjectId,
                outputManagedType,
                out var inputCppType,
                out var outputCppType,
                out var boxedValueTypeSubjectId,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var inputSize,
                out var argCount))
        {
            return false;
        }

        string boxedValueTypeTokenLiteral;
        string targetMethodTokenLiteral;
        string targetAssemblyNameLiteral;
        if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
        {
            boxedValueTypeTokenLiteral = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId);
            targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            targetAssemblyNameLiteral = ToCppStringLiteral(targetAssemblyName);
        }
        else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                     targetAssemblyName,
                     targetMethodSubjectId,
                     out var externalResolution))
        {
            boxedValueTypeTokenLiteral = FormatCppTokenLiteral(externalResolution.TypeToken);
            targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            targetAssemblyNameLiteral = ToCppStringLiteral(externalResolution.AssemblyName);
        }
        else
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_assembly_name_literal"] = targetAssemblyNameLiteral,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["boxed_value_type_token"] = boxedValueTypeTokenLiteral,
            ["target_method_token"] = targetMethodTokenLiteral,
            ["input_size"] = inputSize,
            ["argc"] = argCount,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedValueTypeInstanceCallStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringCallForwarderStub(
        string assemblyName,
        string subjectId,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
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
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2)
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

        int callIndex;
        if (method.Parameters.Count == 1)
        {
            if (instructions.Count != 3 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 1;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 2;
        }

        var targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[callIndex], method.SubjectId, callIndex);
        if (string.Equals(targetMethodSubjectId, subjectId, StringComparison.Ordinal))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        // Only match System.Convert methods themselves or methods that forward to System.Convert::ToString
        if (!subjectId.Contains("/System.Convert::", StringComparison.Ordinal) &&
            !targetMethodSubjectId.Contains("/System.Convert::", StringComparison.Ordinal))
        {
            return false;
        }

        string targetAssemblyName;
        string targetMethodTokenLiteral;
        var targetMethodAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);
        try
        {
            if (string.Equals(targetMethodAssemblyName, assemblyName, StringComparison.Ordinal))
            {
                targetAssemblyName = assemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(
                    GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            }
            else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                         targetMethodAssemblyName,
                         targetMethodSubjectId,
                         out var externalResolution))
            {
                targetAssemblyName = externalResolution.AssemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            }
            else
            {
                return false;
            }
        }
        catch
        {
            return false;
        }

        var argFieldDeclarations = new List<string>(method.Parameters.Count);
        var argRefs = new List<string>(method.Parameters.Count);
        foreach (var parameter in method.Parameters)
        {
            var argIndex = argFieldDeclarations.Count;
            if (TryResolveRuntimeSkeletonValueTypeByValueInputCppType(parameter.Type, out var cppType, out _))
            {
                argFieldDeclarations.Add($"{cppType} arg{argIndex};");
            }
            else
            {
                argFieldDeclarations.Add($"void* arg{argIndex};");
            }

            argRefs.Add($"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{argIndex}))");
        }

        var paramCount = method.Parameters.Count;
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_assembly_name_literal"] = ToCppStringLiteral(targetAssemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", argFieldDeclarations),
            ["argc"] = paramCount,
            ["arg_refs"] = string.Join(",\n        ", argRefs),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringCallForwarderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticValueTypeCallForwarderStub(
        string assemblyName,
        string subjectId,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
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

        if (!TryResolveRuntimeSkeletonStaticValueTypeCallShape(
                method,
                outputManagedType,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var argFieldDeclarations,
                out var argRefs,
                out var argCount))
        {
            return false;
        }

        string targetMethodTokenLiteral;
        string resolvedTargetAssemblyName;
        try
        {
            if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
            {
                resolvedTargetAssemblyName = assemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(
                    GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            }
            else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                         targetAssemblyName,
                         targetMethodSubjectId,
                         out var externalResolution))
            {
                resolvedTargetAssemblyName = externalResolution.AssemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            }
            else
            {
                return false;
            }
        }
        catch
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
            ["target_assembly_name_literal"] = ToCppStringLiteral(resolvedTargetAssemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", argFieldDeclarations),
            ["argc"] = argCount,
            ["arg_refs"] = string.Join(",\n        ", argRefs),
            ["output_cpp_type"] = outputCppType,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticValueTypeCallForwarderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundInstanceValueTypeCallForwarderStub(
        string assemblyName,
        string subjectId,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
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

        if (!TryResolveRuntimeSkeletonInstanceValueTypeCallShape(
                method,
                outputManagedType,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var hasNullArg,
                out var argFieldDeclarations,
                out var argRefs,
                out var argCount))
        {
            return false;
        }

        string targetMethodTokenLiteral;
        string resolvedTargetAssemblyName;
        try
        {
            if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
            {
                resolvedTargetAssemblyName = assemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(
                    GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            }
            else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                         targetAssemblyName,
                         targetMethodSubjectId,
                         out var externalResolution))
            {
                resolvedTargetAssemblyName = externalResolution.AssemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            }
            else
            {
                return false;
            }
        }
        catch
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
            ["target_assembly_name_literal"] = ToCppStringLiteral(resolvedTargetAssemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", argFieldDeclarations),
            ["argc"] = argCount,
            ["arg_refs"] = string.Join(",\n        ", argRefs),
            ["output_cpp_type"] = outputCppType,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonInstanceValueTypeCallForwarderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticByteReturnForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        return TryBuildAssemblyBoundStaticPrimitiveReturnForwarderStub(
            subjectId,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            stubName,
            "System.Byte",
            out stub);
    }



    private static bool TryBuildAssemblyBoundStaticCharReturnForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        return TryBuildAssemblyBoundStaticPrimitiveReturnForwarderStub(
            subjectId,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            stubName,
            "System.Char",
            out stub);
    }



    private static bool TryBuildAssemblyBoundStaticPrimitiveReturnForwarderStub(
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
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count != 1)
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

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(method.Parameters[0].Type, out var inputCppType))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out var outputCppType))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["target_stub_name"] = targetStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticByteForwarderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringForwarderStub(
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringLiteralAppendStub(
        string subjectId,
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
            ValidateDelegateClosedTargetRelayStaticTailShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var appendedLiteral = GetDelegateClosedTargetRelayTrailingLiteral(method, instructions);
        var concatIcall = NormalizeStringConcatIcall(GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonStaticStringLiteralAppendDescriptor(
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(appendedLiteral),
            Encoding.UTF8.GetByteCount(appendedLiteral));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringLiteralAppendStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringReturnForwarderStub(
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
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringReturnForwarderStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringForwarderConsoleWriteLineStub(
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
        if (instructions.Count != 5 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName) ||
            !methodsBySubjectId.TryGetValue(targetSubjectId, out var targetMethod) ||
            !string.Equals(GetMethodReturnType(targetSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            ValidateSingleArgumentForwarderShape(targetMethod, GetSingleBlockInstructions(targetMethod));
        }
        catch
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[3]) != 0)
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = Encoding.UTF8.GetByteCount(literal),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderConsoleWriteLineStub(
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
        if (instructions.Count != 5 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (string.Equals(forwarderSubjectId, subjectId, StringComparison.Ordinal) ||
            string.Equals(forwarderSubjectId, producerSubjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName) ||
            !methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod) ||
            !string.Equals(GetMethodReturnType(forwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            ValidateSingleArgumentForwarderShape(forwarderMethod, GetSingleBlockInstructions(forwarderMethod));
        }
        catch
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[3]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["producer_stub_name"] = producerStubName,
            ["forwarder_stub_name"] = forwarderStubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerCtorGetterConsoleWriteLineStub(
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
        if (instructions.Count != 6 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var getterSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(getterSubjectId, out var getterMethod))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> getterInstructions;
        try
        {
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            getterInstructions = GetSingleBlockInstructions(getterMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateFieldGetterShape(getterMethod, getterInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var getterTypeSubjectId = GetDeclaringTypeSubjectId(getterSubjectId);
        if (!string.Equals(constructorTypeSubjectId, getterTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(getterInstructions[1]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[4]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerCtorRenderConsoleWriteLineStub(
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
        if (instructions.Count != 6 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var renderSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(renderSubjectId, out var renderMethod) ||
            !string.Equals(GetMethodReturnType(renderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> renderInstructions;
        try
        {
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            renderInstructions = GetSingleBlockInstructions(renderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateFieldBackedStringInstanceMethodShape(renderMethod, renderInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var renderTypeSubjectId = GetDeclaringTypeSubjectId(renderSubjectId);
        if (!string.Equals(constructorTypeSubjectId, renderTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(renderInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[4]) != 0)
        {
            return false;
        }

        var messagePrefixLiteral = GetRequiredOperandString(renderInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(renderMethod, renderInstructions);
        var concatIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(renderMethod, renderInstructions));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["string_concat_triple_icall_literal"] = ToCppStringLiteral(StringConcatTripleIcall),
            ["message_prefix_literal"] = ToCppStringLiteral(messagePrefixLiteral),
            ["message_prefix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ["message_suffix_literal"] = ToCppStringLiteral(messageSuffixLiteral),
            ["message_suffix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub(
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
        if (instructions.Count != 7 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (!methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(forwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> forwarderInstructions;
        try
        {
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            forwarderInstructions = GetSingleBlockInstructions(forwarderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateSingleArgumentForwarderShape(forwarderMethod, forwarderInstructions);
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

        var writeLineTarget = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[5]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["forwarder_stub_name"] = forwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub(
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
        if (instructions.Count != 7 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(forwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> forwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            forwarderInstructions = GetSingleBlockInstructions(forwarderMethod);
            ValidateSingleArgumentForwarderShape(forwarderMethod, forwarderInstructions);
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

        var writeLineTarget = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[5]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["forwarder_stub_name"] = forwarderStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStub(
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
        if (instructions.Count != 8 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(terminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> terminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            terminalForwarderInstructions = GetSingleBlockInstructions(terminalForwarderMethod);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(terminalForwarderMethod, terminalForwarderInstructions);
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

        var writeLineTarget = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[6]) != 0)
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
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
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
        if (instructions.Count != 8 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(terminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> terminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            terminalForwarderInstructions = GetSingleBlockInstructions(terminalForwarderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
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

        var writeLineTarget = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[6]) != 0)
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
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }



    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
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
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
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
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var middleForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
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
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(middleForwarderMethod, middleForwarderInstructions);
            ValidateSingleArgumentForwarderShape(terminalForwarderMethod, terminalForwarderInstructions);
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
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["middle_forwarder_stub_name"] = middleForwarderStubName,
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }

}
