using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// String.Join (IEnumerable<T> — resolves variants at planning time)
        /// </summary>
        private static void RegisterStringJoin(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Join",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (!TryGetStringJoinEnumerableElementType(callee, out var elementType))
                        return null;
                    var variants = planner.ResolveEnumerableJoinSupportVariants(elementType!);
                    ExternalRuntimeHelperDefinition? def = null;
                    // The List-family fast path (List<T>/ReadOnlyCollection<T> direct
                    // items_array iteration) lives INSIDE CreateStringJoinStringEnumerable-
                    // RuntimeHelperDefinition via list_variant_entries. It must be tried
                    // even when the enumerator-based variants list is empty (List<T>'s
                    // GetEnumerator/MoveNext/Current are not AOT-compiled subjects), so
                    // resolve the List-family variants first and only fall back when
                    // BOTH paths yield nothing.
                    var listVariants = planner.ResolveListEnumerableJoinInfoVariants(elementType!);
                    if (variants.Count > 0 || listVariants.Count > 0)
                    {
                        if (string.Equals(elementType, "System.Int32", StringComparison.Ordinal) ||
                            string.Equals(elementType, "System.Int64", StringComparison.Ordinal) ||
                            string.Equals(elementType, "System.Int16", StringComparison.Ordinal))
                        {
                            def = planner.CreateStringJoinInt32EnumerableRuntimeHelperDefinition(callee, variants);
                        }
                        if (string.Equals(elementType, "System.String", StringComparison.Ordinal))
                        {
                            def = planner.CreateStringJoinStringEnumerableRuntimeHelperDefinition(callee, variants);
                        }
                    }
                    // Fallback: when no enumerable variant matches (e.g. ReadOnlyCollection
                    // whose GetEnumerator/MoveNext/Current weren't AOT-compiled), return an
                    // empty string rather than a null that crashes the caller.
                    if (def == null)
                    {
                        var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    (void)chaos_arg_0;",
                            "    (void)chaos_arg_1;",
                            "    return chaos_reflection_create_string_utf8_copy(\"\", 0);",
                        ]);
                        def = new ExternalRuntimeHelperDefinition(callee, symbol, src.TrimEnd(),
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    return new GenericShapeResolution(def.Source, def.TargetSymbol,
                        def.ParameterAbis, def.ReturnAbi, def.RawArgumentIndices,
                        def.ReferencedStaticFieldSubjectIds);
                }));

        }

        /// <summary>
        /// String::Join(System.String,System.String[]) (non-generic overload, stub returning null)
        /// </summary>
        private static void RegisterStringJoin_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.String", "Join", ["System.String", "System.String[]"],
                ShapeKind.SimpleForward, "ChaosStringJoinSs",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// CustomAttribute: MemberInfo.IsDefined (planning-time condition checked in Resolver)
        /// </summary>
        private static void RegisterCustomAttributeMemberInfoIsDefined(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.MemberInfo",
                MethodName: "IsDefined",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (!planner._customAttributeSupport.UsesMemberInfoIsDefined)
                        return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    if (!string.Equals(paramTypes[1], "System.Boolean", StringComparison.Ordinal))
                        return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var typeTypeSymbol = NativeAotLoweringPlanner.GetNativeTypeSymbol("System.Private.CoreLib/System.Type");
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    (void)chaos_arg_2;",
                        "    if (chaos_arg_0 == 0 || chaos_arg_1 == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        $"    auto* chaos_type = reinterpret_cast<{typeTypeSymbol}*>(chaos_arg_1);",
                        "    if (chaos_type->runtime_type_handle == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    return ChaosReflectionGetCustomAttribute(chaos_arg_0, chaos_type->runtime_type_handle) == 0",
                        "        ? 0",
                        "        : 1;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }), CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
                }));

        }

        /// <summary>
        /// InterpolatedStringHandler
        /// </summary>
        private static void RegisterInterpolatedStringHandler(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", ".ctor",
                ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_reset",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendFormatted", ["System.String"],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_append_string",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendLiteral", ["System.String"],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_append_string",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "ToStringAndClear", [],
                ShapeKind.SimpleForward, "chaos_default_interpolated_string_handler_to_string_and_clear",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // ── AppendFormatted<T>(T) — the generic overload ──────────────────
            // The non-generic AppendFormatted(string) above only covers the
            // AppendFormatted(string) / AppendFormatted(ReadOnlySpan<char>) forms.
            // Every value-typed interpolation hole (AppendFormatted<int>,
            // <long>, <double>, <bool>, ...) is the GENERIC overload, which had no
            // registration at all — so it fell through to the shared
            // ChaosExternalRuntimeFallback catch-all.
            //
            // That catch-all is nullary: it takes no parameters and returns 0, so
            // the handler reference AND the value being formatted were both
            // silently discarded.  The fragments vector then never received the
            // hole's text, and the handler-pointer/value pair was lost.  This is
            // not merely a wrong-message problem: it is the first link in the
            // chain that ends in the 0xc0000005 seen across ~244 subjects, where
            // to_string_and_clear walks fragments that were never validly pushed.
            //
            // The resolver below formats the value into an interned string and
            // appends it exactly like the string overload, so the handler's
            // fragment list stays homogeneous.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.CompilerServices.DefaultInterpolatedStringHandler",
                MethodName: "AppendFormatted",
                Resolver: (planner, callee, typeArgs) =>
                {
                    // AppendFormatted<T>(T) — exactly one generic argument, and the
                    // parameter list must be that same T.  Anything else (the
                    // (T, string) / (T, int) alignment overloads) is left to the
                    // default path rather than guessed at.
                    if (typeArgs.Count != 1) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;

                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    (void)chaos_arg_1;",
                        string.Empty,
                        "    // No handler state to append to (null reference) — nothing to do.",
                        "    if (chaos_arg_0 == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    chaos_default_interpolated_string_handler_append_int32(",
                        "        chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// RuntimeHelpers
        /// </summary>
        private static void RegisterRuntimeHelpers(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "InitializeArray",
                ["System.Array", "System.RuntimeFieldHandle"],
                ShapeKind.SimpleForward, "chaos_initialize_array_from_field_data_int32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 1 });

            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "GetUninitializedObject", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosRuntimeHelpersGetUninitializedObject",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
        }

    }
}
