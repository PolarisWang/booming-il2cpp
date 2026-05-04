using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEnumManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Enum::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonEnumManagedInvokeHandler(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("enum-managed-invoke-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEnumManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildRuntimeSkeletonEnumImportedBridgeCore(buildContext) is { MatchKind: RuntimeSkeletonFamilyHandlerMatchKind.Match } importedResult)
        {
            return importedResult;
        }

        if (TryBuildAssemblyBoundEnumManagedInvokeStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static bool TryBuildAssemblyBoundEnumManagedInvokeCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;

        if (TryBuildAssemblyBoundEnumImportedBridgeStub(
                assemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                methodsBySubjectId,
                pageSupportBuilder,
                stubName,
                out stub))
        {
            return true;
        }

        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonEnumManagedInvokeAbi.TryCreate(method, out var abi))
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
            ["this_field_declaration"] = abi.ThisFieldDeclaration,
            ["this_validation_statement"] = abi.ThisValidationStatement,
            ["this_argument_expression"] = abi.ThisArgumentExpression,
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
            ["return_value_is_indirect"] = abi.ReturnShape.ReturnValueIsIndirectExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonEnumManagedInvokeStubTemplateRelativePath),
            model);
        return true;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEnumImportedBridgeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundEnumImportedBridgeStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static bool TryBuildAssemblyBoundEnumImportedBridgeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !string.Equals(method.MethodRole, "imported-method", StringComparison.Ordinal) ||
            !subjectId.Contains("/System.Enum::GetEnumValuesAndNames:", StringComparison.Ordinal))
        {
            return false;
        }

        var registration = GetRequiredRegistration(methodPointers, subjectId);
        var runtimeTypeDescriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, "System.Private.CoreLib/System.RuntimeType"));
        var objectArrayTypeDescriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, "System.Private.CoreLib/System.Array"));
        var stringArrayTypeDescriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, "System.Private.CoreLib/System.String[]"));

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["runtime_type_descriptor_index"] = runtimeTypeDescriptorIndex,
            ["object_array_type_descriptor_index"] = objectArrayTypeDescriptorIndex,
            ["string_array_type_descriptor_index"] = stringArrayTypeDescriptorIndex,
            ["import_method_symbol"] = registration.Symbol,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonEnumImportedBridgeStubTemplateRelativePath),
            model);
        return true;
    }
}
