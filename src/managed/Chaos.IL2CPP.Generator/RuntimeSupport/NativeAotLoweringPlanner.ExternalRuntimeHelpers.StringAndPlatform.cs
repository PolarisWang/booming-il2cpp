using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private static string RenderSimpleExternalRuntimeHelper(
        string returnType,
        string helperSymbol,
        string parameterSignature,
        IReadOnlyList<string> bodyLines)
    {
        // Sentinel fix: for pointer-returning stubs where the body is just
        // a bare "return 0;", replace with sentinel address so that generated
        // code null-checks do not immediately CHAOS_IL2CPP_FAIL.
        if (returnType == "CHAOS_IL2CPP_INTPTR" &&
            bodyLines.Count > 0 &&
            bodyLines[bodyLines.Count - 1].Contains("return 0"))
        {
            var lines = bodyLines.ToList();
            bool isStub = true;
            foreach (var line in lines)
            {
                if (line.Contains("return ") && line != bodyLines[bodyLines.Count - 1])
                {
                    isStub = false;
                    break;
                }
            }
            if (isStub)
            {
                lines.Insert(lines.Count - 1, "    static CHAOS_IL2CPP_UINT8 s_sentinel = 0;");
                lines[^1] = "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);";
                bodyLines = lines;
            }
        }
        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetSimpleExternalRuntimeHelperTemplate(),
            new ScriptObject
            {
                ["return_type"] = returnType,
                ["helper_symbol"] = helperSymbol,
                ["parameter_signature"] = parameterSignature,
                ["body_lines"] = bodyLines.ToArray(),
            }).TrimEnd();
    }

    private ExternalRuntimeHelperDefinition CreateStringJoinInt32EnumerableRuntimeHelperDefinition(string callee, IReadOnlyList<EnumerableJoinSupportVariant> variants)
    {
        var orderedVariants = variants
            .OrderBy(item => item.EnumerableTypeSubjectId, StringComparer.Ordinal)
            .Select(item => new ScriptObject
            {
                ["enumerable_type_id_symbol"] = GetNativeTypeIdSymbol(item.EnumerableTypeSubjectId),
                ["enumerable_type_info_symbol"] = GetNativeTypeInfoSymbol(item.EnumerableTypeSubjectId),
                ["get_enumerator_symbol"] = item.GetEnumeratorMethod.NativeSymbol,
                ["move_next_symbol"] = item.MoveNextMethod.NativeSymbol,
                ["get_current_symbol"] = item.GetCurrentMethod.NativeSymbol,
            })
            .ToArray();
        var model = new ScriptObject
        {
            ["helper_symbol"] = GetExternalRuntimeHelperSymbol(callee),
            ["variant_entries"] = orderedVariants,
        };
        var rendered = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetStringJoinInt32EnumerableTemplate(),
            model);
        return new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), rendered.TrimEnd(), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
        {
            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
        }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
    }

    private ExternalRuntimeHelperDefinition CreateStringJoinStringEnumerableRuntimeHelperDefinition(string callee, IReadOnlyList<EnumerableJoinSupportVariant> variants)
    {
        var orderedVariants = variants
            .OrderBy(item => item.EnumerableTypeSubjectId, StringComparer.Ordinal)
            .Select(item => new ScriptObject
            {
                ["enumerable_type_id_symbol"] = GetNativeTypeIdSymbol(item.EnumerableTypeSubjectId),
                ["enumerable_type_info_symbol"] = GetNativeTypeInfoSymbol(item.EnumerableTypeSubjectId),
                ["get_enumerator_symbol"] = item.GetEnumeratorMethod.NativeSymbol,
                ["move_next_symbol"] = item.MoveNextMethod.NativeSymbol,
                ["get_current_symbol"] = item.GetCurrentMethod.NativeSymbol,
            })
            .ToArray();
        // List-family fast path: List<T> and ReadOnlyCollection<T> embed a
        // chaos_list_fields block (items_array/size/version) at offset +8. Their
        // GetEnumerator/MoveNext/Current methods are NOT AOT-compiled subjects
        // (JIT-generated in .NET), so ResolveEnumerableJoinSupportVariants yields no
        // enumerator variant for them. Emit a dedicated type_info-dispatched branch
        // that iterates the shared backing buffer directly. Works for both the List
        // and the ReadOnlyCollection<T> view returned by List<T>.AsReadOnly().
        var elementType = TryGetStringJoinEnumerableElementType(callee, out var et) ? et : null;
        var orderedListVariants = elementType != null
            ? ResolveListEnumerableJoinInfoVariants(elementType)
                .OrderBy(id => id, StringComparer.Ordinal)
                .Select(id => new ScriptObject
                {
                    ["enumerable_type_info_symbol"] = GetNativeTypeInfoSymbol(id),
                })
                .ToArray()
            : Array.Empty<ScriptObject>();
        var model = new ScriptObject
        {
            ["helper_symbol"] = GetExternalRuntimeHelperSymbol(callee),
            ["reference_type_shape_value"] = GetNativeTypeShapeValue(AotCoreIrTypeShapeKind.ReferenceType),
            ["string_type_id_symbol"] = GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"),
            ["string_type_info_symbol"] = GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"),
            ["variant_entries"] = orderedVariants,
            ["list_variant_entries"] = orderedListVariants,
        };
        var rendered = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetStringJoinStringEnumerableTemplate(),
            model);
        return new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), rendered.TrimEnd(), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
        {
            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
        }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
    }

    /// <summary>
    /// Resolve the List-family reference-type subject ids (List&lt;T&gt; and
    /// ReadOnlyCollection&lt;T&gt;) whose closed element type matches
    /// <paramref name="elementTypeDisplayName"/>. Only types that were actually
    /// emitted as reference types (present in <see cref="_referenceTypeBaseSubjectIds"/>)
    /// are returned, guaranteeing their type_info symbol resolves at build time.
    /// </summary>
    internal IReadOnlyList<string> ResolveListEnumerableJoinInfoVariants(string elementTypeDisplayName)
    {
        var matches = new List<string>();
        foreach (var subjectId in _referenceTypeBaseSubjectIds.Keys)
        {
            // Match the TYPE PATH regardless of assembly prefix: the same closed
            // generic can be registered under different assembly aliases
            // (System.Collections/, System.Private.CoreLib/, …) depending on which
            // reference introduced it, and the MethodTable symbol is derived from
            // the full subject id either way.
            var slash = subjectId.IndexOf('/');
            var typePath = slash > 0 ? subjectId[(slash + 1)..] : subjectId;
            string? openPrefix = null;
            if (typePath.StartsWith("System.Collections.Generic.List<", StringComparison.Ordinal))
                openPrefix = "System.Collections.Generic.List<";
            else if (typePath.StartsWith("System.Collections.ObjectModel.ReadOnlyCollection<", StringComparison.Ordinal))
                openPrefix = "System.Collections.ObjectModel.ReadOnlyCollection<";
            if (openPrefix == null)
                continue;
            if (!TryParseClosedSingleGenericArgument2(subjectId, openPrefix, out var elementTypeNameOrSubjectId))
                continue;
            if (string.Equals(GetTypeDisplayName(elementTypeNameOrSubjectId), elementTypeDisplayName, StringComparison.Ordinal))
                matches.Add(subjectId);
        }
        return matches;
    }

    private static bool TryParseClosedSingleGenericArgument2(string subjectId, string openPrefix, out string elementTypeNameOrSubjectId)
    {
        elementTypeNameOrSubjectId = string.Empty;
        if (!TryReadGenericArgumentList(subjectId, openPrefix, out var genericArgumentList))
            return false;
        var parts = SplitTopLevelGenericArguments(genericArgumentList);
        if (parts.Count != 1 || string.IsNullOrWhiteSpace(parts[0]))
            return false;
        elementTypeNameOrSubjectId = parts[0];
        return true;
    }

    private static readonly IReadOnlyDictionary<string, string> MarshalCopyElementTypeMap = new Dictionary<string, string>
    {
        ["System.Byte"] = "CHAOS_IL2CPP_UINT8",
        ["System.Int16"] = "CHAOS_IL2CPP_INT16",
        ["System.Int32"] = "CHAOS_IL2CPP_INT32",
        ["System.Int64"] = "CHAOS_IL2CPP_INT64",
        ["System.IntPtr"] = "CHAOS_IL2CPP_INTPTR",
        ["System.Single"] = "CHAOS_IL2CPP_FLOAT32",
        ["System.Double"] = "CHAOS_IL2CPP_FLOAT64",
        ["System.Boolean"] = "CHAOS_IL2CPP_UINT8",
        ["System.Char"] = "CHAOS_IL2CPP_UINT16",
    };

}
