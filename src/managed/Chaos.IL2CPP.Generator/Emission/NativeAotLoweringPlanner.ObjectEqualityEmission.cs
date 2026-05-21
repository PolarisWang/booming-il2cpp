using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Replaces <see cref="EmitObjectEqualityHelpers"/> — builds a data model
    /// and renders via Scriban template instead of StringBuilder.
    /// Returns the complete equality helper C++ code, or empty if unused.
    /// </summary>
    private string BuildObjectEqualityHelpersCode(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        IReadOnlySet<string> referenceTypeSubjectIds,
        IReadOnlySet<string> boxedTypeSubjectIds)
    {
        if (!UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) =>
                MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.Object", "Equals", "System.Object")
                || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.String", "op_Equality", "System.String", "System.String")
                || IsCollectionRuntimeHelperSubjectId(instruction.Callee ?? string.Empty)))
        {
            return string.Empty;
        }

        bool hasStringType = referenceTypeSubjectIds.Contains(
            ManagedNaming.NormalizeSubjectIdAssembly("System.Private.CoreLib/System.String"));

        var boxedTypeEntries = boxedTypeSubjectIds
            .OrderBy(value => value, StringComparer.Ordinal)
            .Select(item =>
            {
                var entry = new ScriptObject
                {
                    ["type_id_symbol"] = GetNativeBoxTypeIdSymbol(item),
                    ["box_type_symbol"] = GetNativeBoxTypeSymbol(item),
                    ["is_structured"] = IsStructuredValueTypeSubjectId(item),
                    ["value_type_symbol"] = GetNativeValueTypeSymbol(item),
                };
                return entry;
            })
            .ToArray();

        var model = new ScriptObject
        {
            ["has_string_type"] = hasStringType,
            ["string_type_info_symbol"] = GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"),
            ["string_type_symbol"] = GetNativeTypeSymbol("System.Private.CoreLib/System.String"),
            ["boxed_type_entries"] = boxedTypeEntries,
            ["indentation"] = ScribanTemplateRenderer.Indentation(1),
        };

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetObjectEqualityTemplate(), model);
    }

    private void EmitObjectEqualityHelpers(
        StringBuilder builder,
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        IReadOnlySet<string> referenceTypeSubjectIds,
        IReadOnlySet<string> boxedTypeSubjectIds)
    {
        var code = BuildObjectEqualityHelpersCode(reachableMethods, referenceTypeSubjectIds, boxedTypeSubjectIds);
        if (!string.IsNullOrEmpty(code))
        {
            builder.AppendLine(code);
            builder.AppendLine();
        }
    }
}
