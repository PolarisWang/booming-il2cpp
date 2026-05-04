using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private const string NativeRuntimeEventSourcePlatformResidualContractId =
        "native-runtime-event-source-platform-residual-v1";

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Diagnostics.Tracing.NativeRuntimeEventSource::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("native-runtime-event-source-platform-capability-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundNativeRuntimeEventSourcePlatformCapabilityCore(
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

    private static bool TryBuildAssemblyBoundNativeRuntimeEventSourcePlatformCapabilityCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (TryBuildAssemblyBoundNativeRuntimeEventSourceResidualStub(
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                stubName,
                out stub))
        {
            return true;
        }

        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi.TryCreate(method, out var abi))
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundNativeRuntimeEventSourceResidualStub(
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !string.Equals(method.MethodRole, "imported-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "no-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        MetadataRegistrationEntry methodMetadata;
        try
        {
            methodMetadata = GetRequiredMetadataRegistration(metadataRegistration.Registrations, "method", subjectId, subjectId);
        }
        catch
        {
            return false;
        }

        if (methodMetadata.IsImported != true ||
            !string.Equals(methodMetadata.ImportModuleName, "QCall", StringComparison.Ordinal) ||
            !string.Equals(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(subjectId),
                "System.Void",
                StringComparison.Ordinal))
        {
            return false;
        }

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
            "System.Void",
            out var returnShape,
            RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType,
            includeIndirectFlag: true);

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = NativeRuntimeEventSourcePlatformResidualContractId,
            ["capability_area"] = "qcall-residual",
            ["arg_field_declarations"] = string.Join("\n    ", argumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = string.Join("\n    ", argumentShapes.Select(shape => shape.ValidationStatement).Where(static statement => !string.IsNullOrWhiteSpace(statement))),
            ["return_managed_type"] = returnShape.ManagedType,
            ["return_field_declarations"] = returnShape.FieldDeclarations,
            ["return_value_validation_statement"] = returnShape.ValidationStatement,
            ["return_value_declaration"] = returnShape.ReturnValueDeclaration,
            ["helper_statements"] = "// QCall logging residual intentionally elided on the proof runtime.",
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalPlatformFastPathStubTemplateRelativePath),
            model);
        return true;
    }
}
