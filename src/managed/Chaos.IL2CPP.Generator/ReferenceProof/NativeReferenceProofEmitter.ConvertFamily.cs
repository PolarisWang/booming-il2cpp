using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Convert::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonConvertRuntimeHelperCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("convert-runtime-helper-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertRuntimeHelperCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        // Phase A: Try ToChar kernel ABI — direct C++ call, no method_invoke bridge
        // Catches ToChar(primitives), ToChar(Object), ToChar(String), etc.
        if (TryBuildAssemblyBoundConvertCharKernelCore(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var kernelStub))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(kernelStub);
        }

        // Phase B: Fall back to ConvertRuntimeHelper (method_invoke bridge)
        if (TryBuildAssemblyBoundConvertRuntimeHelperCore(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var runtimeHelperStub))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(runtimeHelperStub);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    /// <summary>
    /// Try to build a direct-call kernel stub for Convert.ToChar methods.
    /// Reuses the MathKernel template which generates a C++ function call
    /// instead of a method_invoke bridge call.
    /// </summary>
    private static bool TryBuildAssemblyBoundConvertCharKernelCore(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonConvertCharKernelAbi.TryCreate(method, out var abi))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = abi.ContractId,
            ["kernel_function_name"] = abi.KernelFunctionName,
            ["kernel_call_expression"] = abi.KernelCallExpression,
            ["return_managed_type"] = abi.ReturnShape.ManagedType,
            ["return_cpp_type"] = abi.ReturnShape.CppType,
            ["has_return"] = abi.HasReturn,
            ["arg_shapes"] = abi.ArgumentShapes.Select(a => new ScriptObject
            {
                ["cpp_type"] = a.CppType,
                ["name"] = a.Name,
                ["field_declaration"] = a.FieldDeclaration,
            }).ToList(),
        };

        var template = ScribanTemplateRenderer.LoadTemplate(
            NativeReferenceProofCatalog.RuntimeSkeletonMathKernelStubTemplateRelativePath);
        var renderedStub = string.Empty;
        try
        {
            renderedStub = ScribanTemplateRenderer.RenderTemplate(template, model);
        }
        catch
        {
            return false;
        }

        // If the kernel ABI specifies range check code, prepend it.
        if (abi.HasRangeCheck)
        {
            renderedStub = renderedStub.Replace(
                "request->return_value = ",
                $"{abi.RangeCheckCode}\n    request->return_value = ");
        }

        stub = renderedStub;
        return true;
    }

    private static bool TryBuildAssemblyBoundConvertRuntimeHelperCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonConvertRuntimeHelperAbi.TryCreate(method, out var abi))
        {
            return false;
        }

        string targetMethodTokenLiteral;
        try
        {
            targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId));
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = abi.ContractId,
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", abi.ArgumentFieldDeclarations),
            ["argc"] = abi.ArgumentCount,
            ["arg_storage_size"] = abi.ArgumentStorageSize,
            ["arg_refs"] = string.Join(",\n        ", abi.ArgumentReferences),
            ["return_managed_type"] = abi.ReturnManagedType,
            ["return_field_declaration"] = abi.ReturnFieldDeclaration,
            ["return_value_validation_statement"] = abi.ReturnValueValidationStatement,
            ["return_buffer_declaration"] = abi.ReturnBufferDeclaration,
            ["return_value_argument"] = abi.ReturnValueArgument,
            ["return_value_size"] = abi.ReturnValueSizeExpression,
            ["return_assignment_statement"] = abi.ReturnAssignmentStatement,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConvertRuntimeHelperStubTemplateRelativePath),
            model);
        return true;
    }
}
