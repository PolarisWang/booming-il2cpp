using System.Text;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private static void AppendRuntimeSupportTemplate(StringBuilder builder, Scriban.Template template)
    {
        builder.AppendLine(ScribanTemplateRenderer.RenderTemplate(template, new ScriptObject()).TrimEnd());
        builder.AppendLine();
    }
}
