using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCollectionsKernelFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Collections.", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("Stack`1", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("LinkedList`1", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("SortedDictionary`2", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonCollectionsKernelCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("collections-kernel-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCollectionsKernelCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildCollectionsKernelStub(
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

    private static bool TryBuildCollectionsKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonCollectionsKernelAbi.TryCreate(method, out var abi))
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
            ["kContractVersion"] = RuntimeSkeletonCollectionsKernelAbi.ContractVersion,
            ["contract_version_hex"] = RuntimeSkeletonCollectionsKernelAbi.CollectionsKernelContractVersionHex,
            ["version_assertion"] = abi.VersionAssertionExpression,
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", abi.ArgumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = abi.ArgumentValidationStatements,
            ["arg_refs"] = string.Join(",\n        ", abi.ArgumentShapes.Select(shape => shape.ArgReferenceExpression)),
            ["argc"] = abi.ArgumentCount,
            ["arg_storage_size"] = abi.ArgumentStorageSize,
            ["return_managed_type"] = abi.ReturnShape.ManagedType,
            ["return_field_declarations"] = abi.ReturnShape.FieldDeclarations,
            ["return_field_declaration"] = "void* return_value;",
            ["return_value_validation_statement"] = abi.ReturnShape.ValidationStatement,
            ["return_value_declaration"] = abi.ReturnShape.ReturnValueDeclaration,
            ["return_value_argument"] = abi.ReturnShape.ReturnValueArgument,
            ["return_value_size"] = abi.ReturnShape.ReturnValueSizeExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonCollectionsKernelStubTemplateRelativePath),
            model);
        return true;
    }
}
