using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorKernelFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (RuntimeSkeletonVectorKernelCore.TryResolveBySubjectId(buildContext.SubjectId) is null)
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonVectorKernelCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("vector-kernel-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorKernelCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundVectorKernelCore(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static bool TryBuildAssemblyBoundVectorKernelCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        _ = metadataRegistration;

        if (!RuntimeSkeletonVectorKernelCore.TryCreate(subjectId, out var plan))
        {
            return false;
        }

        if (string.Equals(plan.SemanticId, "vector-capability-query", StringComparison.Ordinal) ||
            string.Equals(plan.SemanticId, "vector-fixed-shift", StringComparison.Ordinal) ||
            string.Equals(plan.SemanticId, "vector-generic-create", StringComparison.Ordinal) ||
            (string.Equals(plan.SemanticId, "vector-fixed-equality", StringComparison.Ordinal) &&
             !string.IsNullOrWhiteSpace(plan.CapabilityOperationId)) ||
            (string.Equals(plan.SemanticId, "vector-fixed-arithmetic", StringComparison.Ordinal) &&
             !string.IsNullOrWhiteSpace(plan.CapabilityOperationId)))
        {
            if (plan.FixedVectorWidthBytes is null ||
                plan.ScalarResolutionKind is null)
            {
                return false;
            }

            var capabilityModel = new ScriptObject
            {
                ["stub_name"] = stubName,
                ["contract_id"] = plan.ContractId,
                ["kernel_semantic_id"] = plan.SemanticId,
                ["kernel_backend_kind"] = plan.BackendKind.ToString(),
                ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
                ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId)),
                ["scalar_resolution_kind"] = plan.ScalarResolutionKind.Value.ToString(),
                ["scalar_type_token"] = string.IsNullOrWhiteSpace(plan.ScalarTypeSubjectId)
                    ? "0u"
                    : CreateTypeTokenLiteral(metadataRegistration, plan.ScalarTypeSubjectId),
                ["scalar_generic_arg_index"] = plan.ScalarGenericArgumentIndex ?? -1,
                ["fixed_vector_width_bytes"] = plan.FixedVectorWidthBytes.Value,
                ["return_cpp_type"] = plan.CarrierSchema.ReturnCppType,
                ["capability_operation_id"] = plan.CapabilityOperationId ?? string.Empty,
                ["capability_arg_count"] = plan.CarrierSchema.ArgumentCppTypes.Count,
                ["capability_arg0_cpp_type"] = plan.CarrierSchema.ArgumentCppTypes.Count > 0 ? plan.CarrierSchema.ArgumentCppTypes[0] : string.Empty,
                ["capability_arg1_cpp_type"] = plan.CarrierSchema.ArgumentCppTypes.Count > 1 ? plan.CarrierSchema.ArgumentCppTypes[1] : string.Empty,
                ["generic_create_argument_count"] = plan.CarrierSchema.ArgumentCppTypes.Count,
                ["generic_create_arg0_cpp_type"] = plan.CarrierSchema.ArgumentCppTypes.Count > 0 ? plan.CarrierSchema.ArgumentCppTypes[0] : string.Empty,
                ["generic_create_arg1_cpp_type"] = plan.CarrierSchema.ArgumentCppTypes.Count > 1 ? plan.CarrierSchema.ArgumentCppTypes[1] : string.Empty,
                ["shift_count_value_expression"] = plan.ShiftCountValueExpression,
                ["scalar_cpp_type_expression"] = plan.ScalarCppTypeExpression ?? string.Empty,
            };

            if (string.Equals(plan.SemanticId, "vector-fixed-shift", StringComparison.Ordinal))
            {
                if (plan.ShiftTraitKind == RuntimeSkeletonVectorShiftTraitKind.None ||
                    string.IsNullOrWhiteSpace(plan.ShiftOperationId))
                {
                    return false;
                }

                capabilityModel["capability_operation_id"] = plan.ShiftOperationId;
                capabilityModel["shift_trait_kind"] = plan.ShiftTraitKind.ToString();
            }

            stub = ScribanTemplateRenderer.RenderTemplate(
                ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonVectorCapabilityQueryStubTemplateRelativePath),
                capabilityModel);
            return true;
        }

        if (string.Equals(plan.SemanticId, "vector-copy", StringComparison.Ordinal))
        {
            if (string.IsNullOrWhiteSpace(plan.DestinationArgCppType) ||
                string.IsNullOrWhiteSpace(plan.DestinationDataExpression) ||
                string.IsNullOrWhiteSpace(plan.DestinationLengthExpression) ||
                string.IsNullOrWhiteSpace(plan.StartIndexExpression))
            {
                return false;
            }

            var vectorCopyModel = new ScriptObject
            {
                ["stub_name"] = stubName,
                ["contract_id"] = plan.ContractId,
                ["kernel_semantic_id"] = plan.SemanticId,
                ["kernel_backend_kind"] = plan.BackendKind.ToString(),
                ["this_field_declaration"] = plan.CarrierSchema.ThisFieldDeclaration,
                ["arg_field_declarations"] = plan.CarrierSchema.ArgFieldDeclarations,
                ["destination_arg_cpp_type"] = plan.DestinationArgCppType,
                ["destination_data_expression"] = plan.DestinationDataExpression,
                ["destination_length_expression"] = plan.DestinationLengthExpression,
                ["start_index_expression"] = plan.StartIndexExpression,
                ["returns_bool"] = plan.ReturnsBool,
                ["helper_name"] = RuntimeSkeletonVectorKernelCore.TryResolveBySubjectId(subjectId)?.HelperPrefix ?? string.Empty,
            };

            stub = ScribanTemplateRenderer.RenderTemplate(
                ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonVectorCopyKernelStubTemplateRelativePath),
                vectorCopyModel);
            return true;
        }

        var kernelModel = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = plan.ContractId,
            ["kernel_semantic_id"] = plan.SemanticId,
            ["kernel_backend_kind"] = plan.BackendKind.ToString(),
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = "0u",
            ["this_field_declaration"] = plan.CarrierSchema.ThisFieldDeclaration,
            ["arg_field_declarations"] = plan.CarrierSchema.ArgFieldDeclarations,
            ["arg_validation_statements"] = plan.CarrierSchema.ArgValidationStatements,
            ["return_field_declaration"] = plan.CarrierSchema.ReturnFieldDeclaration,
            ["helper_call_expression"] = plan.HelperCallExpression,
        };

        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonValueTypeKernelStubTemplateRelativePath),
            kernelModel);
        return true;
    }

}
