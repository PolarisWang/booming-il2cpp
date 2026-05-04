using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    [RuntimeSkeletonFamilyPlugin("threading-sync-kernel", CodeGenPluginProductLine.NativeReference, 425, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingSyncKernelFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        // Match Monitor/SpinLock/Lock/WaitHandle. Return NoMatch on failure
        // so the managed-invoke handler (priority 430) gets a fallback chance.
        if (!buildContext.SubjectId.Contains("/System.Threading.Monitor::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Threading.SpinLock::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Threading.Lock::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Threading.WaitHandle::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Threading.EventWaitHandle::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonThreadingSyncKernelCore(buildContext);
        // NoMatch so managed-invoke fallback (priority 430) tries these methods
        // when kernel doesn't support the shape (e.g. value-type parameters).
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingSyncKernelCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildThreadingSyncKernelStub(
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

    private static bool TryBuildThreadingSyncKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonThreadingSyncKernelAbi.TryCreate(method, out var abi))
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
            ["kContractVersion"] = RuntimeSkeletonThreadingSyncKernelAbi.ContractVersion,
            ["contract_version_hex"] = RuntimeSkeletonThreadingSyncKernelAbi.ContractVersionHex,
            ["version_assertion"] = abi.VersionAssertionExpression,
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["this_field_declaration"] = abi.ThisFieldDeclaration,
            ["arg_field_declarations"] = string.Join("\n    ", abi.ArgumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = abi.ArgumentValidationStatements,
            ["arg_refs"] = string.Join(",\n        ", abi.ArgumentShapes.Select(shape => shape.ArgReferenceExpression)),
            ["argc"] = abi.ArgumentCount,
            ["arg_storage_size"] = abi.ArgumentStorageSize,
            ["return_managed_type"] = abi.ReturnShape.ManagedType,
            ["return_field_declarations"] = abi.ReturnShape.FieldDeclarations,
            ["return_value_validation_statement"] = abi.ReturnShape.ValidationStatement,
            ["return_value_declaration"] = abi.ReturnShape.ReturnValueDeclaration,
            ["return_value_argument"] = abi.ReturnShape.ReturnValueArgument,
            ["return_value_size"] = abi.ReturnShape.ReturnValueSizeExpression,
            ["helper_call_expression"] = abi.HelperCallExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonThreadingSyncKernelStubTemplateRelativePath),
            model);
        return true;
    }
}
