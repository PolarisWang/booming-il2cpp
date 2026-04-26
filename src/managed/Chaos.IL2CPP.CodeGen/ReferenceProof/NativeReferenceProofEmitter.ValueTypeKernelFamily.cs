using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonValueTypeKernelFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (RuntimeSkeletonKernelCore.TryResolveBySubjectId(buildContext.SubjectId) is null)
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonValueTypeKernelCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("valuetype-kernel-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonValueTypeKernelCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundValueTypeKernelCore(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static bool TryBuildAssemblyBoundValueTypeKernelCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        _ = metadataRegistration;

        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!RuntimeSkeletonKernelCore.TryCreate(method, out var plan))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = plan.ContractId,
            ["kernel_semantic_id"] = plan.SemanticId,
            ["kernel_backend_kind"] = plan.BackendKind.ToString(),
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = "0u",
            ["this_field_declaration"] = plan.CarrierSchema.ThisFieldDeclaration,
            ["arg_field_declarations"] = plan.CarrierSchema.ArgFieldDeclarations,
            ["return_field_declaration"] = plan.CarrierSchema.ReturnFieldDeclaration,
            ["helper_call_expression"] = plan.HelperCallExpression,
        };

        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonValueTypeKernelStubTemplate(),
            model);
        return true;
    }
}
