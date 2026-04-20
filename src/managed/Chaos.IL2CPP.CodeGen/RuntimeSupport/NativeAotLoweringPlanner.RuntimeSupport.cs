using System.Text;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private static void EmitCollectionRuntimePrelude(StringBuilder builder)
    {
        AppendRuntimeSupportTemplate(builder, NativeAotTemplateCatalog.GetCollectionRuntimePreludeTemplate());
    }

    private static void EmitMonitorRuntimePrelude(StringBuilder builder)
    {
        AppendRuntimeSupportTemplate(builder, NativeAotTemplateCatalog.GetMonitorRuntimePreludeTemplate());
    }

    private static void EmitThreadRuntimePrelude(StringBuilder builder)
    {
        AppendRuntimeSupportTemplate(builder, NativeAotTemplateCatalog.GetThreadRuntimePreludeTemplate());
    }

    private static void EmitAsyncRuntimePrelude(StringBuilder builder)
    {
        AppendRuntimeSupportTemplate(builder, NativeAotTemplateCatalog.GetAsyncRuntimePreludeTemplate());
    }

    private static void AppendRuntimeSupportTemplate(StringBuilder builder, Scriban.Template template)
    {
        builder.AppendLine(ScribanTemplateRenderer.RenderTemplate(template, new ScriptObject()).TrimEnd());
        builder.AppendLine();
    }
}
