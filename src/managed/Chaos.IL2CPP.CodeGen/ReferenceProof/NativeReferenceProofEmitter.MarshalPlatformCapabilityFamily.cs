using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMarshalPlatformCapabilityFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Runtime.InteropServices.Marshal::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonMarshalPlatformCapabilityCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("marshal-platform-capability-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMarshalPlatformCapabilityCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundMarshalPlatformCapabilityCore(
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

    private static bool TryBuildAssemblyBoundMarshalPlatformCapabilityCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (TryBuildAssemblyBoundMarshalPlatformResidualStub(subjectId, stubName, out stub))
        {
            return true;
        }

        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (RuntimeSkeletonMarshalPlatformCore.TryCreateFastPath(method, out var fastPathPlan))
        {
            var fastModel = new ScriptObject
            {
                ["stub_name"] = stubName,
                ["contract_id"] = fastPathPlan.ContractId,
                ["capability_area"] = fastPathPlan.CapabilityArea,
                ["arg_field_declarations"] = string.Join("\n    ", fastPathPlan.ArgumentShapes.Select(shape => shape.FieldDeclaration)),
                ["arg_validation_statements"] = string.Join("\n    ", fastPathPlan.ArgumentShapes.Select(shape => shape.ValidationStatement).Where(static statement => !string.IsNullOrWhiteSpace(statement))),
                ["return_managed_type"] = fastPathPlan.ReturnShape.ManagedType,
                ["return_field_declarations"] = fastPathPlan.ReturnShape.FieldDeclarations,
                ["return_value_validation_statement"] = fastPathPlan.ReturnShape.ValidationStatement,
                ["return_value_declaration"] = fastPathPlan.ReturnShape.ReturnValueDeclaration,
                ["helper_statements"] = fastPathPlan.HelperStatements,
            };
            stub = ScribanTemplateRenderer.RenderTemplate(
                NativeReferenceProofCatalog.GetRuntimeSkeletonMarshalPlatformFastPathStubTemplate(),
                fastModel);
            return true;
        }

        if (!RuntimeSkeletonMarshalManagedInvokeAbi.TryCreate(method, out var fallbackAbi))
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

        var fallbackModel = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = fallbackAbi.ContractId,
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", fallbackAbi.ArgumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_refs"] = string.Join(",\n        ", fallbackAbi.ArgumentShapes.Select(shape => shape.ArgReferenceExpression)),
            ["argc"] = fallbackAbi.ArgumentCount,
            ["arg_storage_size"] = fallbackAbi.ArgumentStorageSize,
            ["return_managed_type"] = fallbackAbi.ReturnShape.ManagedType,
            ["return_field_declarations"] = fallbackAbi.ReturnShape.FieldDeclarations,
            ["return_value_validation_statement"] = fallbackAbi.ReturnShape.ValidationStatement,
            ["return_value_declaration"] = fallbackAbi.ReturnShape.ReturnValueDeclaration,
            ["return_value_argument"] = fallbackAbi.ReturnShape.ReturnValueArgument,
            ["return_value_size"] = fallbackAbi.ReturnShape.ReturnValueSizeExpression,
            ["return_value_is_indirect"] = fallbackAbi.ReturnShape.ReturnValueIsIndirectExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonUnsafeManagedInvokeStubTemplate(),
            fallbackModel);
        return true;
    }

    private static bool TryBuildAssemblyBoundMarshalPlatformResidualStub(
        string subjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        string capabilityArea;
        string helperStatements;
        string managedReturnType;
        IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> argShapes;

        switch (subjectId)
        {
            case "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::<CreateBindCtx>g____PInvoke|76_0:System.Int32(System.UInt32,System.IntPtr*)":
                capabilityArea = "com-activation";
                managedReturnType = "System.Int32";
                argShapes =
                [
                    new("void* arg0;", "if (request->arg0 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg0"),
                    new("void* arg1;", "if (request->arg1 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg1"),
                ];
                helperStatements = """
const auto chaos_out_ptr = *static_cast<void**>(request->arg1);
    if (chaos_out_ptr != nullptr) {
        *static_cast<void**>(chaos_out_ptr) = nullptr;
    }
    const auto result = static_cast<CHAOS_IL2CPP_INT32>(0x80004001u);
    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;
""";
                break;
            case "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::<BindMoniker>g____PInvoke|78_0:System.Int32(System.IntPtr,System.UInt32,System.Guid*,System.IntPtr*)":
                capabilityArea = "com-activation";
                managedReturnType = "System.Int32";
                argShapes =
                [
                    new("void* arg0;", "if (request->arg0 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg0"),
                    new("void* arg1;", "if (request->arg1 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg1"),
                    new("void* arg2;", "if (request->arg2 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg2"),
                    new("void* arg3;", "if (request->arg3 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg3"),
                ];
                helperStatements = """
const auto chaos_out_ptr = *static_cast<void**>(request->arg3);
    if (chaos_out_ptr != nullptr) {
        *static_cast<void**>(chaos_out_ptr) = nullptr;
    }
    const auto result = static_cast<CHAOS_IL2CPP_INT32>(0x80004001u);
    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;
""";
                break;
            case "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::<MkParseDisplayName>g____PInvoke|77_0:System.Int32(System.IntPtr,System.UInt16*,System.UInt32*,System.IntPtr*)":
                capabilityArea = "com-activation";
                managedReturnType = "System.Int32";
                argShapes =
                [
                    new("void* arg0;", "if (request->arg0 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg0"),
                    new("void* arg1;", "if (request->arg1 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg1"),
                    new("void* arg2;", "if (request->arg2 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg2"),
                    new("void* arg3;", "if (request->arg3 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg3"),
                ];
                helperStatements = """
const auto chaos_eaten_ptr = *static_cast<CHAOS_IL2CPP_UINT32**>(request->arg2);
    if (chaos_eaten_ptr != nullptr) {
        *chaos_eaten_ptr = 0u;
    }
    const auto chaos_object_ptr = *static_cast<void**>(request->arg3);
    if (chaos_object_ptr != nullptr) {
        *static_cast<void**>(chaos_object_ptr) = nullptr;
    }
    const auto result = static_cast<CHAOS_IL2CPP_INT32>(0x80004001u);
    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;
""";
                break;
            case "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::<GetTypeFromCLSID>g____PInvoke|34_0:System.Void(System.Guid*,System.UInt16*,System.Runtime.CompilerServices.ObjectHandleOnStack)":
                capabilityArea = "com-activation";
                managedReturnType = "System.Void";
                argShapes =
                [
                    new("void* arg0;", "if (request->arg0 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg0"),
                    new("void* arg1;", "if (request->arg1 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg1"),
                    new("void* arg2;", "if (request->arg2 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg2"),
                ];
                helperStatements = """
auto* chaos_handle_bytes = static_cast<unsigned char*>(request->arg2);
    CHAOS_IL2CPP_MEMSET(chaos_handle_bytes, 0, sizeof(void*));
""";
                break;
            case "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::<IsBuiltInComSupportedInternal>g____PInvoke|27_0:System.Int32()":
                capabilityArea = "com-core";
                managedReturnType = "System.Int32";
                argShapes = [];
                helperStatements = """
const auto result = static_cast<CHAOS_IL2CPP_INT32>(0);
    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;
""";
                break;
            case "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetHINSTANCE:System.IntPtr(System.Runtime.CompilerServices.QCallModule)":
                capabilityArea = "structure-layout";
                managedReturnType = "System.IntPtr";
                argShapes =
                [
                    new("void* arg0;", "if (request->arg0 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg0"),
                ];
                helperStatements = """
const auto result = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    *static_cast<CHAOS_IL2CPP_INTPTR*>(return_value_ptr) = result;
""";
                break;
            case "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::InternalPrelink:System.Void(System.RuntimeMethodHandleInternal)":
                capabilityArea = "structure-layout";
                managedReturnType = "System.Void";
                argShapes =
                [
                    new("void* arg0;", "if (request->arg0 == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }", "request->arg0"),
                ];
                helperStatements = "// no-op";
                break;
            default:
                return false;
        }

        RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
            managedReturnType,
            out var returnShape,
            RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType,
            includeIndirectFlag: true);

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = RuntimeSkeletonMarshalPlatformCore.FastPathContractId,
            ["capability_area"] = capabilityArea,
            ["arg_field_declarations"] = string.Join("\n    ", argShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = string.Join("\n    ", argShapes.Select(shape => shape.ValidationStatement)),
            ["return_managed_type"] = returnShape.ManagedType,
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
}
