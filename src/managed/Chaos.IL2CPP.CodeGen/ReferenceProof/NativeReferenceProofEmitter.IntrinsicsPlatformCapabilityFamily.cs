using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private const string IntrinsicsResidualContractId = "intrinsics-platform-residual-v1";

    [RuntimeSkeletonFamilyPlugin("intrinsics-platform", CodeGenPluginProductLine.NativeReference, 133, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIntrinsicsPlatformCapabilityFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!IsIntrinsicsSubjectId(buildContext.SubjectId))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonIntrinsicsPlatformCapabilityCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("intrinsics-platform-capability-unsupported-shape");
    }

    private static bool IsIntrinsicsSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Runtime.Intrinsics.", StringComparison.Ordinal);
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIntrinsicsPlatformCapabilityCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundIntrinsicsPlatformCapabilityCore(
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

    private static bool TryBuildAssemblyBoundIntrinsicsPlatformCapabilityCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;

        if (TryBuildAssemblyBoundIntrinsicsResidualStub(
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                stubName,
                out stub))
        {
            return true;
        }

        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonIntrinsicsManagedInvokeAbi.TryCreate(method, out var abi))
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonTaskManagedInvokeStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundIntrinsicsResidualStub(
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

        var returnManagedType = RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(subjectId);
        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);

        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                returnManagedType,
                out var returnShape,
                managedType => !IsIntrinsicsKnownValueType(managedType),
                includeIndirectFlag: true))
        {
            return false;
        }

        var helperStatements = BuildIntrinsicsResidualHelperStatements(returnManagedType);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = IntrinsicsResidualContractId,
            ["capability_area"] = "intrinsics-residual",
            ["this_field_declaration"] = string.Empty,
            ["this_validation_statement"] = string.Empty,
            ["arg_field_declarations"] = string.Join("\n    ", argumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = string.Empty,
            ["return_managed_type"] = returnShape.ManagedType,
            ["return_field_declarations"] = returnShape.FieldDeclarations,
            ["return_value_validation_statement"] = returnShape.ValidationStatement,
            ["return_value_declaration"] = returnShape.ReturnValueDeclaration,
            ["helper_statements"] = helperStatements,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalPlatformFastPathStubTemplateRelativePath),
            model);
        return true;
    }

    private static string BuildIntrinsicsResidualHelperStatements(string managedReturnType)
    {
        return managedReturnType switch
        {
            "System.Void" => "// no-op residual intrinsics stub (proof runtime has no SIMD)",
            "System.Boolean" => "*static_cast<bool*>(return_value_ptr) = false;",
            "System.Byte" => "*static_cast<CHAOS_IL2CPP_UINT8*>(return_value_ptr) = 0u;",
            "System.SByte" => "*static_cast<CHAOS_IL2CPP_INT8*>(return_value_ptr) = 0;",
            "System.Char" => "*static_cast<CHAOS_IL2CPP_UINT16*>(return_value_ptr) = 0u;",
            "System.Int16" => "*static_cast<CHAOS_IL2CPP_INT16*>(return_value_ptr) = 0;",
            "System.UInt16" => "*static_cast<CHAOS_IL2CPP_UINT16*>(return_value_ptr) = 0u;",
            "System.Int32" => "*static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = 0;",
            "System.UInt32" => "*static_cast<CHAOS_IL2CPP_UINT32*>(return_value_ptr) = 0u;",
            "System.Int64" => "*static_cast<CHAOS_IL2CPP_INT64*>(return_value_ptr) = 0;",
            "System.UInt64" => "*static_cast<CHAOS_IL2CPP_UINT64*>(return_value_ptr) = 0u;",
            "System.IntPtr" => "*static_cast<CHAOS_IL2CPP_INTPTR*>(return_value_ptr) = 0;",
            "System.UIntPtr" => "*static_cast<CHAOS_IL2CPP_UINTPTR*>(return_value_ptr) = 0u;",
            "System.Single" => "*static_cast<float*>(return_value_ptr) = 0.0f;",
            "System.Double" => "*static_cast<double*>(return_value_ptr) = 0.0;",
            _ => "// unsupported return type for intrinsics residual stub\n    *request->return_value = nullptr;",
        };
    }

    private static bool IsIntrinsicsKnownValueType(string managedType)
    {
        return managedType is
            "System.Void" or "System.Boolean" or "System.Byte" or "System.SByte" or
            "System.Char" or "System.Int16" or "System.UInt16" or "System.Int32" or "System.UInt32" or
            "System.Int64" or "System.UInt64" or "System.IntPtr" or "System.UIntPtr" or
            "System.Single" or "System.Double";
    }
}
