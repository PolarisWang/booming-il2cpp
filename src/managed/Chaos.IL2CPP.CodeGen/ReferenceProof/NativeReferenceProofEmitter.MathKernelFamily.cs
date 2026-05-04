using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMathKernelFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Math::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.MathF::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        if (!buildContext.MethodsBySubjectId.TryGetValue(buildContext.SubjectId, out var method) ||
            !RuntimeSkeletonMathKernelAbi.TryCreate(method, out var abi))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("math-kernel-unsupported-shape");
        }

        var model = new ScriptObject
        {
            ["stub_name"] = buildContext.StubName,
            ["contract_id"] = abi.ContractId,
            ["kernel_function_name"] = abi.KernelFunctionName,
            ["kernel_call_expression"] = abi.KernelCallExpression,
            ["return_managed_type"] = abi.ReturnShape.ManagedType,
            ["return_cpp_type"] = abi.ReturnShape.CppType,
            ["has_return"] = abi.HasReturn,
            ["arg_shapes"] = abi.ArgumentShapes.Select(a => new ScriptObject
            {
                ["cpp_type"] = a.CppType,
                ["name"] = a.Name,
                ["field_declaration"] = a.FieldDeclaration,
            }).ToList(),
        };

        var stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMathKernelStubTemplateRelativePath),
            model);

        return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stub);
    }
}
