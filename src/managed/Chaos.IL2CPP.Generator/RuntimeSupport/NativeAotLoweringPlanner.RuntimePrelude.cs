using System.Text;

using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

    private static void EmitRuntimePrelude(
        StringBuilder builder,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers,
        StaticFieldDataSupportModel staticFieldDataSupport)
    {
        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetRuntimePreludeTemplate(),
                new ScriptObject()).TrimEnd());
        builder.AppendLine();
    }

    private static void EmitSpanRuntimePrelude(
        StringBuilder builder,
        StaticFieldDataSupportModel staticFieldDataSupport)
    {
        var fieldDataEntries = staticFieldDataSupport.EntriesBySubjectId.Values
            .OrderBy(value => value.FieldSubjectId, StringComparer.Ordinal)
            .Select(entry =>
            {
                var byteRows = entry.Bytes.Count == 0
                    ? ["    0,"]
                    : entry.Bytes
                        .Chunk(16)
                        .Select(chunk => "    " + string.Join(", ", chunk.Select(value => $"0x{value:X2}")) + ",")
                        .ToArray();

                return new ScriptObject
                {
                    ["sanitized_field_subject_id"] = NativeAotLoweringPlanner.SanitizeSubjectId(entry.FieldSubjectId),
                    ["field_handle_literal"] = NativeAotLoweringPlanner.GetFieldHandleLiteral(entry.FieldSubjectId),
                    ["byte_rows"] = byteRows,
                };
            })
            .ToArray();

        var model = new ScriptObject
        {
            ["field_data_entries"] = fieldDataEntries,
        };

        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetSpanRuntimePreludeTemplate(),
                model).TrimEnd());
        builder.AppendLine();
    }
}

