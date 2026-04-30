using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static bool IsReflectionSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.RuntimeType::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimeMethodInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimeConstructorInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimeFieldInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimePropertyInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimeEventInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimeParameterInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Type::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.Assembly::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimeAssembly::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.MemberInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.MethodBase::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.ConstructorInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.PropertyInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.FieldInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.EventInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.CustomAttributeData::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.CustomAttributeNamedArgument::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.CustomAttributeTypedArgument::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.Module::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.RuntimeModule::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.StrongNameKeyPair::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.ExceptionHandlingClause::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.LocalVariableInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.InterfaceMapping::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.ParameterInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.ManifestResourceInfo::", StringComparison.Ordinal) ||
               subjectId.Contains("/System.Reflection.Pointer::", StringComparison.Ordinal);
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonReflectionPlatformCapabilityFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!IsReflectionSubjectId(buildContext.SubjectId))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonReflectionPlatformCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("reflection-platform-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonReflectionPlatformCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundReflectionPlatformCapabilityCore(
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

    private static bool TryBuildAssemblyBoundReflectionPlatformCapabilityCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;

        // First try: managed-invoke for methods with canonical body
        if (methodsBySubjectId.TryGetValue(subjectId, out var method) &&
            RuntimeSkeletonGenericManagedInvokeAbiFactory.TryCreate(
                method,
                "reflection-managed-invoke-v1",
                out var abi,
                IsReflectionDirectReferenceReturnType))
        {
            return TryRenderValueTypeManagedInvokeStub(
                NativeReferenceProofCatalog.GetRuntimeSkeletonIdentityStructManagedInvokeStubTemplate(),
                assemblyName,
                subjectId,
                metadataRegistration,
                stubName,
                abi.ContractId,
                abi.ThisFieldDeclaration,
                abi.ThisValidationStatement,
                abi.ThisArgumentExpression,
                abi.ArgumentShapes,
                abi.ArgumentValidationStatements,
                abi.ArgumentCount,
                abi.ArgumentStorageSize,
                abi.ReturnShape,
                out stub);
        }

        // Fallback: residual stub returning default/zero
        return TryBuildReflectionResidualStub(subjectId, stubName, out stub);
    }

    private static bool TryBuildReflectionResidualStub(
        string subjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        var managedReturnType = RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(subjectId);
        var parameterTypes = GetMethodParameterTypesFromSubjectId(subjectId);
        var argumentShapes = parameterTypes
            .Select((_, index) => new RuntimeSkeletonManagedInvokePointerArgShape(
                $"void* arg{index};",
                string.Empty,
                $"request->arg{index}"))
            .ToArray();

        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                managedReturnType,
                out var returnShape,
                IsReflectionDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        var helperStatements = BuildReflectionResidualHelperStatements(managedReturnType);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = "reflection-residual-stub-v1",
            ["capability_area"] = "reflection-metadata",
            ["this_field_declaration"] = string.Empty,
            ["this_validation_statement"] = string.Empty,
            ["arg_field_declarations"] = string.Join("\n    ", argumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = string.Empty,
            ["return_managed_type"] = managedReturnType,
            ["return_field_declarations"] = returnShape.FieldDeclarations,
            ["return_value_validation_statement"] = returnShape.ValidationStatement,
            ["return_value_declaration"] = returnShape.ReturnValueDeclaration,
            ["helper_statements"] = helperStatements,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonMarshalPlatformFastPathStubTemplate(),
            model);
        return true;
    }

    private static string BuildReflectionResidualHelperStatements(string managedReturnType)
    {
        return managedReturnType switch
        {
            "System.Void" => "// reflection residual stub: no managed metadata engine",
            "System.Boolean" => "*static_cast<bool*>(return_value_ptr) = false;",
            "System.Byte" => "*static_cast<CHAOS_IL2CPP_UINT8*>(return_value_ptr) = 0u;",
            "System.SByte" => "*static_cast<CHAOS_IL2CPP_INT8*>(return_value_ptr) = 0;",
            "System.Int16" => "*static_cast<CHAOS_IL2CPP_INT16*>(return_value_ptr) = 0;",
            "System.UInt16" => "*static_cast<CHAOS_IL2CPP_UINT16*>(return_value_ptr) = 0u;",
            "System.Int32" => "*static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = 0;",
            "System.UInt32" => "*static_cast<CHAOS_IL2CPP_UINT32*>(return_value_ptr) = 0u;",
            "System.Int64" => "*static_cast<CHAOS_IL2CPP_INT64*>(return_value_ptr) = 0;",
            "System.UInt64" => "*static_cast<CHAOS_IL2CPP_UINT64*>(return_value_ptr) = 0u;",
            "System.Single" => "*static_cast<float*>(return_value_ptr) = 0.0f;",
            "System.Double" => "*static_cast<double*>(return_value_ptr) = 0.0;",
            "System.IntPtr" or "System.UIntPtr" => "*static_cast<CHAOS_IL2CPP_INTPTR*>(return_value_ptr) = 0;",
            _ => "*request->return_value = nullptr;",
        };
    }
}
