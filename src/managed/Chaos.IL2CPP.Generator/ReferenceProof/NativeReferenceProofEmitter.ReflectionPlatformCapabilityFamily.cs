using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

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
                ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonIdentityStructManagedInvokeStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalPlatformFastPathStubTemplateRelativePath),
            model);
        return true;
    }

    /// <summary>
    /// Body for a reflection entry point that has no AOT-native implementation.
    ///
    /// These paths previously wrote a zero/null return value, which is
    /// indistinguishable from a genuine zero result: a caller could not tell a
    /// missing implementation from an empty answer. The reflection subsystem
    /// requires unreachable operations to fail loudly, so every one of them now
    /// raises NotSupportedException rather than fabricating a value.
    ///
    /// The return type is ignored: every arm produces the same raise, so the
    /// emitted shape no longer depends on the managed signature.
    /// </summary>
    private static string BuildReflectionResidualHelperStatements(string managedReturnType)
    {
        _ = managedReturnType;
        return "    chaos::il2cpp::runtime_core::RaiseManagedException(\"System.NotSupportedException\", \"reflection API not available under AOT\");" + "\n";
    }
}
