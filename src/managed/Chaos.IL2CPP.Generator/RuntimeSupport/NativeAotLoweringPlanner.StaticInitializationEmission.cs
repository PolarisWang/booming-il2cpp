using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private void EmitStaticInitializationDefinitions(StringBuilder builder)
    {
        foreach (var plan in _staticInitializationSupport.PlansByTypeSubjectId.Values.OrderBy(
                     value => value.TypeSubjectId,
                     StringComparer.Ordinal))
        {
            var actionBuilder = new StringBuilder(512);
            foreach (var action in plan.Actions)
            {
                EmitStaticInitializationAction(actionBuilder, action, ScribanTemplateRenderer.Tab);
            }

            var model = new ScriptObject
            {
                ["once_flag_symbol"] = GetNativeTypeInitializationOnceFlagSymbol(plan.TypeSubjectId),
                ["initialization_function_symbol"] = GetNativeTypeInitializationFunctionSymbol(plan.TypeSubjectId),
                ["action_block"] = actionBuilder.Length == 0
                    ? string.Empty
                    : actionBuilder.ToString().TrimEnd() + Environment.NewLine,
            };

            builder.AppendLine(
                ScribanTemplateRenderer.RenderTemplate(
                    NativeAotTemplateCatalog.GetStaticInitializationDefinitionTemplate(),
                    model).TrimEnd());
            builder.AppendLine();
        }
    }

    private void EmitStaticInitializationPrologue(StringBuilder builder, AotCoreIrMethodArtifact method)
    {
        if (!method.IsStatic ||
            string.IsNullOrEmpty(method.Identity.DeclaringTypeSubjectId) ||
            !TryGetStaticInitializationPlanForType(method.Identity.DeclaringTypeSubjectId, out var plan) ||
            plan is null)
        {
            return;
        }

        AppendStaticInitializationCall(builder, GetNativeTypeInitializationFunctionSymbol(plan.TypeSubjectId), ScribanTemplateRenderer.Tab);
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

        AppendStaticInitializationCall(builder, GetNativeTypeInitializationFunctionSymbol(plan.TypeSubjectId), indentation);
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

        var model = new ScriptObject
        {
            ["indentation"] = indentation,
            ["static_field_symbol"] = GetNativeStaticFieldSymbol(action.FieldSubjectId),
            ["native_type_symbol"] = GetNativeTypeSymbol(action.ConstructedTypeSubjectId),
            ["native_type_id_symbol"] = GetNativeTypeIdSymbol(action.ConstructedTypeSubjectId),
            ["native_type_info_symbol"] = GetNativeTypeInfoSymbol(action.ConstructedTypeSubjectId),
            ["constructor_block"] = string.Empty,
        };

        if (!action.ElideConstructorCall)
        {
            var resolvedInvocationTarget = invocationTarget!.Value;
            var cb = new StringBuilder();
            cb.Append(indentation).Append(ScribanTemplateRenderer.Tab)
              .Append("const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);").AppendLine();
            cb.Append(indentation).Append(ScribanTemplateRenderer.Tab)
              .Append(resolvedInvocationTarget.TargetSymbol)
              .Append('(').Append(FormatAbiInvocationArgumentList(resolvedInvocationTarget.ParameterAbis)).Append(");").AppendLine();
            model["constructor_block"] = cb.ToString();
        }

        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetStaticInitializationActionTemplate(),
                model).TrimEnd());
    }

    private static void AppendStaticInitializationCall(
        StringBuilder builder,
        string initializationFunctionSymbol,
        string indentation)
    {
        var model = new ScriptObject
        {
            ["indentation"] = indentation,
            ["initialization_function_symbol"] = initializationFunctionSymbol,
        };

        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetStaticInitializationCallTemplate(),
                model).TrimEnd());
    }
}
