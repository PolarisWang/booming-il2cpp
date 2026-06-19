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

}
