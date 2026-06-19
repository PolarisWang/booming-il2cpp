using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// String operations
        /// </summary>
        private static void RegisterStringoperations(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.String", "Concat", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionConcatStringPairValues",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // Concat 3-arg and 4-arg use pair-call composition (not single forward), so they need GenericShapeDescriptors.
            // The InlineBody entries below are used for C++ dispatch header generation only.
            registry.Register("System.String", "Concat", ["System.String", "System.String", "System.String"],
                ShapeKind.InlineBody, "ChaosReflectionConcatStringPairValues",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.String", "Concat", ["System.String", "System.String", "System.String", "System.String"],
                ShapeKind.InlineBody, "ChaosReflectionConcatStringPairValues",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2, 3 });

            // GenericShapeDescriptor for String.Concat — resolves 2-arg/3-arg/4-arg by checking parameter count
            // 2-arg: fully inlined with direct field access + GcAllocateAtomic (avoid ABI/strlen/stack buffer)
            // 3-arg/4-arg: pair-call composition via ChaosReflectionConcatStringPairValues
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Concat",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var stringRetAbi = CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType);
                    var stringRefAbi = CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType);

                    if (paramTypes.Count == 2)
                    {
                        // Fused: String.Concat(string, int32) → single ChaosStringConcatWithFormattedInt32 call
                        if (paramTypes[1].Trim() == "System.Int32")
                        {
                            var fusedSrc = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                            [
                                "    auto val = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(chaos_arg_1);",
                                "    return chaos::il2cpp::runtime_core::ChaosStringConcatWithFormattedInt32(chaos_arg_0, val);",
                            ]);
                            return new GenericShapeResolution(fusedSrc, symbol,
                                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                    CreateNativeIntAbiSlot(),
                                }),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                new HashSet<int> { 0, 1 });
                        }

                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringConcat2(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                stringRefAbi, stringRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosStringConcat2");
                    }

                    if (paramTypes.Count == 3)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                        [
                            "    const auto chaos_left_pair = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);",
                            "    return ChaosReflectionConcatStringPairValues(chaos_left_pair, chaos_arg_2);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                stringRefAbi, stringRefAbi, stringRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1, 2 });
                    }

                    if (paramTypes.Count == 4)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3",
                        [
                            "    const auto chaos_left_pair = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);",
                            "    const auto chaos_right_pair = ChaosReflectionConcatStringPairValues(chaos_arg_2, chaos_arg_3);",
                            "    return ChaosReflectionConcatStringPairValues(chaos_left_pair, chaos_right_pair);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                stringRefAbi, stringRefAbi, stringRefAbi, stringRefAbi,
                            }), stringRetAbi, new HashSet<int> { 0, 1, 2, 3 });
                    }

                    return null;
                }));

            registry.Register("System.String", "op_Equality", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "chaos_object_equals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.String", "get_Length", [],
                ShapeKind.SimpleForward, "chaos_string_get_length",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.String", "StartsWith", ["System.String", "System.StringComparison"],
                ShapeKind.SimpleForward, "ChaosStringStartsWith",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.String", "Contains", ["System.String", "System.StringComparison"],
                ShapeKind.SimpleForward, "ChaosStringContains",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // GenericShapeDescriptors for String shapes with complex body logic

            // String.op_Equality — returns chaos_object_equals result as 0/1 native int
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "op_Equality",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return chaos_object_equals(chaos_arg_0, chaos_arg_1)",
                        "        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)",
                        "        : 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        }), CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // String.get_Length — handles string_id and native string pointer
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "get_Length",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    if (chaos_is_string_id(chaos_arg_0))",
                        "    {",
                        "        return static_cast<CHAOS_IL2CPP_INT32>(",
                        "            chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_0)).byte_count);",
                        "    }",
                        string.Empty,
                        "    auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // String.get_Chars(int32) → char (DirectNativeSymbol via ChaosStringGetChars)
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "get_Chars",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return ChaosStringGetChars(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosStringGetChars");
                }));

            // String.IsNullOrEmpty(string) → bool (DirectNativeSymbol via ChaosStringIsNullOrEmpty)
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "IsNullOrEmpty",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosStringIsNullOrEmpty(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosStringIsNullOrEmpty");
                }));

            // String.StartsWith — ordinal comparison with memcmp
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "StartsWith",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    if (chaos_arg_2 != 4)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    if (chaos_arg_1 == 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
                        "    const char* chaos_prefix_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    const auto chaos_instance_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_0)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_0)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0)->length));",
                        "    const auto chaos_prefix_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_1)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_1)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_1)->length));",
                        "    if (chaos_prefix_length > chaos_instance_length)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    if (chaos_prefix_length == 0)",
                        "    {",
                        "        return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "    }",
                        string.Empty,
                        "    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
                        "        (chaos_prefix_utf8 == nullptr && chaos_prefix_length != 0))",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    return CHAOS_IL2CPP_MEMCMP(chaos_instance_utf8, chaos_prefix_utf8, chaos_prefix_length) == 0",
                        "        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)",
                        "        : 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }), CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            // String.Contains — ordinal substring search with memcmp loop
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Contains",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 2) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    if (chaos_arg_2 != 4)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    if (chaos_arg_1 == 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
                        "    const char* chaos_value_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    const auto chaos_instance_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_0)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_0)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0)->length));",
                        "    const auto chaos_value_length = static_cast<CHAOS_IL2CPP_SIZE>(",
                        "        chaos_is_string_id(chaos_arg_1)",
                        "            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
                        "                chaos_extract_string_id(chaos_arg_1)).byte_count)",
                        "            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_1)->length));",
                        "    if (chaos_value_length == 0)",
                        "    {",
                        "        return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "    }",
                        string.Empty,
                        "    if (chaos_value_length > chaos_instance_length)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
                        "        (chaos_value_utf8 == nullptr && chaos_value_length != 0))",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const auto chaos_last_start = chaos_instance_length - chaos_value_length;",
                        "    for (CHAOS_IL2CPP_SIZE chaos_index = 0; chaos_index <= chaos_last_start; chaos_index++)",
                        "    {",
                        "        if (CHAOS_IL2CPP_MEMCMP(chaos_instance_utf8 + chaos_index, chaos_value_utf8, chaos_value_length) == 0)",
                        "        {",
                        "            return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "        }",
                        "    }",
                        string.Empty,
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }), CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

        }

        /// <summary>
        /// Exception operations
        /// </summary>
        private static void RegisterExceptionoperations(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Exception", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosReflectionInitDefaultException",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Exception", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionSetExceptionMetadata",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.InvalidOperationException", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionSetExceptionMetadata",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.ArgumentOutOfRangeException", ".ctor", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionSetExceptionMetadata_2params",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Exception", "get_Message", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetExceptionMessage",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.ArgumentException", "get_ParamName", [],
                ShapeKind.SimpleForward, "chaos_reflection_get_exception_param_name",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// GC operations (inline body — native fn symbol is not a real function call)
        /// </summary>
        private static void RegisterGCoperations(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.GC", "Collect", [],
                ShapeKind.InlineBody, "chaos_gc_collect()",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "WaitForPendingFinalizers", [],
                ShapeKind.InlineBody, "chaos_gc_wait_for_pending_finalizers()",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "KeepAlive", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_gc_keepalive",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "SuppressFinalize", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_gc_suppress_finalize",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "ReRegisterForFinalize", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_gc_reregister_finalize",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Managed GC API (P2-1: GetTotalMemory, AddMemoryPressure, RemoveMemoryPressure)
        /// </summary>
        private static void RegisterManagedGCAPI(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.GC", "GetTotalMemory", ["System.Boolean"],
                ShapeKind.SimpleForward, "chaos_gc_get_total_memory",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),  // bool -> Int32 in unmanaged ABI
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.GC", "AddMemoryPressure", ["System.Int64"],
                ShapeKind.SimpleForward, "chaos_gc_add_memory_pressure",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "RemoveMemoryPressure", ["System.Int64"],
                ShapeKind.SimpleForward, "chaos_gc_remove_memory_pressure",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// GC.Collect(int, GCCollectionMode) — overload with mode
        /// </summary>
        private static void RegisterGCCollectoverload(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.GC", "Collect", ["System.Int32", "System.GCCollectionMode"],
                ShapeKind.SimpleForward, "chaos_gc_collect_with_mode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),  // generation
                    CreateInt32AbiSlot(),  // mode
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

    }
}
