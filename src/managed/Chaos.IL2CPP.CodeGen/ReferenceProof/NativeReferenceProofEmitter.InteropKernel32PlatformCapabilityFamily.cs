using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInteropKernel32PlatformCapabilityFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/Interop+Kernel32::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonInteropKernel32PlatformCapabilityCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("interop-kernel32-platform-capability-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInteropKernel32PlatformCapabilityCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundInteropKernel32PlatformCapabilityCore(
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

    private static bool TryBuildAssemblyBoundInteropKernel32PlatformCapabilityCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;

        if (methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            if (RuntimeSkeletonInteropKernel32PlatformCore.TryCreateFastPath(method, out var fastPathPlan))
            {
                stub = RenderInteropKernel32FastPathStub(stubName, fastPathPlan);
                return true;
            }

            if (RuntimeSkeletonInteropKernel32ManagedInvokeAbi.TryCreate(method, out var abi))
            {
                string targetMethodTokenLiteral;
                try
                {
                    targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId));
                }
                catch
                {
                    targetMethodTokenLiteral = string.Empty;
                }

                if (!string.IsNullOrEmpty(targetMethodTokenLiteral))
                {
                    stub = RenderInteropKernel32ManagedInvokeStub(stubName, assemblyName, targetMethodTokenLiteral, abi);
                    return true;
                }
            }
        }

        if (TryBuildInteropKernel32ResidualStub(subjectId, stubName, out stub))
        {
            return true;
        }

        return false;
    }

    private static string RenderInteropKernel32FastPathStub(
        string stubName,
        RuntimeSkeletonInteropKernel32FastPathPlan fastPathPlan)
    {
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = fastPathPlan.ContractId,
            ["capability_area"] = fastPathPlan.CapabilityArea,
            ["arg_field_declarations"] = string.Join("\n    ", fastPathPlan.ArgumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = string.Join("\n    ", fastPathPlan.ArgumentShapes.Select(shape => shape.ValidationStatement).Where(static s => !string.IsNullOrWhiteSpace(s))),
            ["return_managed_type"] = fastPathPlan.ReturnShape.ManagedType,
            ["return_field_declarations"] = fastPathPlan.ReturnShape.FieldDeclarations,
            ["return_value_validation_statement"] = fastPathPlan.ReturnShape.ValidationStatement,
            ["return_value_declaration"] = fastPathPlan.ReturnShape.ReturnValueDeclaration,
            ["helper_statements"] = fastPathPlan.HelperStatements,
            ["this_field_declaration"] = string.Empty,
            ["this_validation_statement"] = string.Empty,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalPlatformFastPathStubTemplateRelativePath),
            model);
    }

    private static string RenderInteropKernel32ManagedInvokeStub(
        string stubName,
        string assemblyName,
        string targetMethodTokenLiteral,
        RuntimeSkeletonInteropKernel32ManagedInvokeAbi abi)
    {
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = abi.ContractId,
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["this_field_declaration"] = string.Empty,
            ["this_validation_statement"] = string.Empty,
            ["this_argument_expression"] = "nullptr",
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
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonTaskManagedInvokeStubTemplateRelativePath),
            model);
    }

    private static bool TryBuildInteropKernel32ResidualStub(
        string subjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        var parameterTypes = GetMethodParameterTypesFromSubjectId(subjectId);
        var argumentShapes = parameterTypes
            .Select((_, index) => new RuntimeSkeletonManagedInvokePointerArgShape(
                $"void* arg{index};",
                string.Empty,
                $"request->arg{index}"))
            .ToArray();

        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(subjectId),
                out var returnShape,
                managedType => !IsInteropKernel32KnownValueType(managedType),
                includeIndirectFlag: true))
        {
            return false;
        }

        var helperStatements = BuildInteropKernel32ResidualHelperStatements(returnShape.ManagedType);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = RuntimeSkeletonInteropKernel32PlatformCore.FastPathContractId,
            ["capability_area"] = "platform-residual",
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

    private static string BuildInteropKernel32ResidualHelperStatements(string managedReturnType)
    {
        return managedReturnType switch
        {
            "System.Void" => "// no-op residual platform stub",
            "System.Boolean" or "Interop+BOOL" => "*static_cast<bool*>(return_value_ptr) = false;",
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
            _ => "*request->return_value = nullptr;",
        };
    }

    private static bool IsInteropKernel32KnownValueType(string managedType)
    {
        return managedType is
            "System.Void" or "System.Boolean" or "Interop+BOOL" or "System.Byte" or "System.SByte" or
            "System.Char" or "System.Int16" or "System.UInt16" or "System.Int32" or "System.UInt32" or
            "System.Int64" or "System.UInt64" or "System.IntPtr" or "System.UIntPtr";
    }
}
