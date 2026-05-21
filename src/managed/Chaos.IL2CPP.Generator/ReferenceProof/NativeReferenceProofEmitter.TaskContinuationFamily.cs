using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonTaskContinuationFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Threading.Tasks.Task::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonTaskContinuationCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("task-continuation-family-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonTaskContinuationCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundTaskContinuationCore(
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

    private static bool TryBuildAssemblyBoundTaskContinuationCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (TryBuildAssemblyBoundTaskContinuationResidualStub(subjectId, stubName, out stub))
        {
            return true;
        }

        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (RuntimeSkeletonTaskPlatformCore.TryCreateFastPath(method, out var fastPathPlan))
        {
            var fastModel = new ScriptObject
            {
                ["stub_name"] = stubName,
                ["contract_id"] = fastPathPlan.ContractId,
                ["capability_area"] = fastPathPlan.CapabilityArea,
                ["this_field_declaration"] = string.Empty,
                ["this_validation_statement"] = string.Empty,
                ["arg_field_declarations"] = string.Join("\n    ", fastPathPlan.ArgumentShapes.Select(shape => shape.FieldDeclaration)),
                ["arg_validation_statements"] = string.Join("\n    ", fastPathPlan.ArgumentShapes.Select(shape => shape.ValidationStatement).Where(static s => !string.IsNullOrWhiteSpace(s))),
                ["return_managed_type"] = fastPathPlan.ReturnShape.ManagedType,
                ["return_field_declarations"] = fastPathPlan.ReturnShape.FieldDeclarations,
                ["return_value_validation_statement"] = fastPathPlan.ReturnShape.ValidationStatement,
                ["return_value_declaration"] = fastPathPlan.ReturnShape.ReturnValueDeclaration,
                ["helper_statements"] = fastPathPlan.HelperStatements,
            };
            stub = ScribanTemplateRenderer.RenderTemplate(
                ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonTaskKernelFastPathStubTemplateRelativePath),
                fastModel);
            return true;
        }

        if (!RuntimeSkeletonTaskManagedInvokeAbi.TryCreate(method, out var abi))
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

    private static bool TryBuildAssemblyBoundTaskContinuationResidualStub(
        string subjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        string managedReturnType;
        string helperStatements;

        switch (subjectId)
        {
            case "System.Private.CoreLib/System.Threading.Tasks.Task::get_StateFlagsForDebugger:System.Int32()":
                managedReturnType = "System.Int32";
                helperStatements = """
const auto result = static_cast<CHAOS_IL2CPP_INT32>(0);
    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;
""";
                break;
            case "System.Private.CoreLib/System.Threading.Tasks.Task::get_ExecutingTaskScheduler:System.Threading.Tasks.TaskScheduler()":
                managedReturnType = "System.Threading.Tasks.TaskScheduler";
                helperStatements = "*request->return_value = nullptr;";
                break;
            default:
                return false;
        }

        RuntimeSkeletonManagedInvokeReturnContract returnShape;
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                managedReturnType,
                out returnShape,
                managedType => managedType == "System.Threading.Tasks.TaskScheduler",
                includeIndirectFlag: true))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = RuntimeSkeletonTaskPlatformCore.FastPathContractId,
            ["capability_area"] = "introspection",
            ["this_field_declaration"] = string.Empty,
            ["this_validation_statement"] = string.Empty,
            ["arg_field_declarations"] = string.Empty,
            ["arg_validation_statements"] = string.Empty,
            ["return_managed_type"] = returnShape.ManagedType,
            ["return_field_declarations"] = returnShape.FieldDeclarations,
            ["return_value_validation_statement"] = returnShape.ValidationStatement,
            ["return_value_declaration"] = returnShape.ReturnValueDeclaration,
            ["helper_statements"] = helperStatements,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonTaskKernelFastPathStubTemplateRelativePath),
            model);
        return true;
    }
}
