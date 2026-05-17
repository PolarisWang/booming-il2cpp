using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private static string RenderSimpleExternalRuntimeHelper(
        string returnType,
        string helperSymbol,
        string parameterSignature,
        IReadOnlyList<string> bodyLines)
    {
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
        var model = new ScriptObject
        {
            ["helper_symbol"] = GetExternalRuntimeHelperSymbol(callee),
            ["reference_type_shape_value"] = GetNativeTypeShapeValue(AotCoreIrTypeShapeKind.ReferenceType),
            ["string_type_id_symbol"] = GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"),
            ["string_type_info_symbol"] = GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"),
            ["variant_entries"] = orderedVariants,
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
