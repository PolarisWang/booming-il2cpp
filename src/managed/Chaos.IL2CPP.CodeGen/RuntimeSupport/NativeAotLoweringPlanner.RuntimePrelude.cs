using System.Text;

using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

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

        if (externalRuntimeHelpers.Any(helper => IsCollectionRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitCollectionRuntimePrelude(builder);
        }

        if (externalRuntimeHelpers.Any(helper => IsMonitorRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitMonitorRuntimePrelude(builder);
        }

        if (externalRuntimeHelpers.Any(helper => IsThreadRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitThreadRuntimePrelude(builder);
        }

        if (externalRuntimeHelpers.Any(helper => IsAsyncRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitAsyncRuntimePrelude(builder);
        }
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
                    ["sanitized_field_subject_id"] = SanitizeSubjectId(entry.FieldSubjectId),
                    ["field_handle_literal"] = GetFieldHandleLiteral(entry.FieldSubjectId),
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

