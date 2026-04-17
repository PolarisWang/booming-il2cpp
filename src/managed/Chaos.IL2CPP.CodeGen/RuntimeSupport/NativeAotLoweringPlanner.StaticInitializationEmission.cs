using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private void EmitStaticInitializationDefinitions(StringBuilder builder)
    {
        foreach (var plan in _staticInitializationSupport.PlansByTypeSubjectId.Values.OrderBy(
                     value => value.TypeSubjectId,
                     StringComparer.Ordinal))
        {
            builder.AppendLine($"std::once_flag {GetNativeTypeInitializationOnceFlagSymbol(plan.TypeSubjectId)};");
            builder.AppendLine();
            builder.AppendLine($"void {GetNativeTypeInitializationFunctionSymbol(plan.TypeSubjectId)}()");
            builder.AppendLine("{");
            builder.AppendLine($"    std::call_once({GetNativeTypeInitializationOnceFlagSymbol(plan.TypeSubjectId)}, []()");
            builder.AppendLine("    {");
            foreach (var action in plan.Actions)
            {
                EmitStaticInitializationAction(builder, action, "        ");
            }

            builder.AppendLine("    });");
            builder.AppendLine("}");
            builder.AppendLine();
        }
    }

    private void EmitStaticInitializationPrologue(StringBuilder builder, AotCoreIrMethodArtifact method)
    {
        if (!method.IsStatic ||
            string.IsNullOrWhiteSpace(method.Identity.DeclaringTypeSubjectId) ||
            !TryGetStaticInitializationPlanForType(method.Identity.DeclaringTypeSubjectId, out var plan) ||
            plan is null)
        {
            return;
        }

        builder.AppendLine($"    {GetNativeTypeInitializationFunctionSymbol(plan.TypeSubjectId)}();");
    }

    private void EmitStaticInitializationForField(
        StringBuilder builder,
        string fieldSubjectId,
        string indentation)
    {
        if (!TryGetStaticInitializationPlanForField(fieldSubjectId, out var plan) ||
            plan is null)
        {
            return;
        }

        builder.AppendLine($"{indentation}{GetNativeTypeInitializationFunctionSymbol(plan.TypeSubjectId)}();");
    }

    private void EmitStaticInitializationAction(
        StringBuilder builder,
        StaticInitializationAction action,
        string indentation)
    {
        InvocationTarget? invocationTarget = null;
        if (!action.ElideConstructorCall)
        {
            invocationTarget = TryResolveDirectInvocationTarget(action.ConstructorSubjectId);
            if (invocationTarget is null)
            {
                throw new NotSupportedException(
                    $"native-aot static initializer does not support unresolved constructor '{action.ConstructorSubjectId}'.");
            }

            var resolvedInvocationTarget = invocationTarget.Value;
            if (resolvedInvocationTarget.ParameterAbis.Count != 1)
            {
                throw new NotSupportedException(
                    $"native-aot static initializer only supports parameterless constructors, got '{action.ConstructorSubjectId}'.");
            }

            if (resolvedInvocationTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
            {
                throw new NotSupportedException(
                    $"native-aot static initializer requires void constructor return ABI for '{action.ConstructorSubjectId}'.");
            }
        }

        builder.AppendLine($"{indentation}if ({GetNativeStaticFieldSymbol(action.FieldSubjectId)} == static_cast<std::intptr_t>(0))");
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    auto* chaos_object = new {GetNativeTypeSymbol(action.ConstructedTypeSubjectId)}{{}};");
        builder.AppendLine($"{indentation}    chaos_object->header.type_id = {GetNativeTypeIdSymbol(action.ConstructedTypeSubjectId)};");
        if (!action.ElideConstructorCall)
        {
            var resolvedInvocationTarget = invocationTarget!.Value;
            builder.AppendLine($"{indentation}    const auto chaos_arg_0 = reinterpret_cast<std::intptr_t>(chaos_object);");
            builder.AppendLine($"{indentation}    {resolvedInvocationTarget.TargetSymbol}({FormatAbiInvocationArgumentList(resolvedInvocationTarget.ParameterAbis)});");
        }

        builder.AppendLine($"{indentation}    {GetNativeStaticFieldSymbol(action.FieldSubjectId)} = reinterpret_cast<std::intptr_t>(chaos_object);");
        builder.AppendLine($"{indentation}}}");
    }
}
