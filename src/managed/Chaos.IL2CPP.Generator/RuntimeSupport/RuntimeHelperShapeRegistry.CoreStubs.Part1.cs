// Auto-generated partial
using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;
namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        private static void RegisterCoreStubs_Part1(RuntimeHelperShapeRegistry registry)
        {
            // === String operations ===
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

            // === Exception operations ===
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

            // === GC operations (inline body — native fn symbol is not a real function call) ===
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

            // === Managed GC API (P2-1: GetTotalMemory, AddMemoryPressure, RemoveMemoryPressure) ===
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

            // === GC.Collect(int, GCCollectionMode) — overload with mode ===
            registry.Register("System.GC", "Collect", ["System.Int32", "System.GCCollectionMode"],
                ShapeKind.SimpleForward, "chaos_gc_collect_with_mode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),  // generation
                    CreateInt32AbiSlot(),  // mode
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === GCLatencyMode property getter/setter ===
            registry.Register("System.GC", "get_LatencyMode", [],
                ShapeKind.SimpleForward, "chaos_gc_get_latency_mode",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "set_LatencyMode", ["System.GCLatencyMode"],
                ShapeKind.SimpleForward, "chaos_gc_set_latency_mode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === GC.CollectionCount(int) — returns collection count per generation ===
            registry.Register("System.GC", "CollectionCount", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_get_collection_count",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === GC.GetTotalPauseDuration — returns total accumulated pause (ns) as Int64 ===
            registry.Register("System.GC", "GetTotalPauseDuration", [],
                ShapeKind.SimpleForward, "chaos_gc_get_total_pause_duration",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                EmptyRawArgumentIndices);

            // === GC.GetAllocatedBytesForCurrentThread — returns per-thread allocated bytes (Int64) ===
            registry.Register("System.GC", "GetAllocatedBytesForCurrentThread", [],
                ShapeKind.SimpleForward, "chaos_gc_get_allocated_bytes_for_current_thread",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                EmptyRawArgumentIndices);

            // === GC.TryStartNoGCRegion — two overloads ===
            // 1-arg overload: native API requires a 2nd arg (disallow_full_blocking_gc = 0).
            // Use GenericShapeDescriptor to inject the default parameter.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.GC",
                MethodName: "TryStartNoGCRegion",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INT64 chaos_arg_0",
                    [
                        "    return chaos_gc_try_start_no_gc_region(chaos_arg_0, 0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            registry.Register("System.GC", "TryStartNoGCRegion", ["System.Int64", "System.Boolean"],
                ShapeKind.SimpleForward, "chaos_gc_try_start_no_gc_region",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),  // returns bool (0/1)
                new HashSet<int> { 0, 1 });

            // === GC.EndNoGCRegion — returns Int32 (0=Success, 1=GCTriggered) ===
            registry.Register("System.GC", "EndNoGCRegion", [],
                ShapeKind.SimpleForward, "chaos_gc_end_no_gc_region",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // === Full GC Notification ===
            registry.Register("System.GC", "RegisterForFullGCNotification", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_enable_full_gc_notification",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.GC", "CancelFullGCNotification", [],
                ShapeKind.SimpleForward, "chaos_gc_disable_full_gc_notification",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.GC", "WaitForFullGCApproach", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_wait_for_full_gc_approach",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.GC", "WaitForFullGCComplete", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_wait_for_full_gc_complete",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === GCMemoryInfo ===
            // Matches the BCL InternalCall signature: GetMemoryInfo(GCMemoryInfoData data, int kind)
            // GCMemoryInfoData is a class; codegen passes the object reference as a native int (pointer),
            // and the native function computes the interior pointer (past MethodTable*).
            registry.Register("System.GC", "GetMemoryInfo", ["System.GCMemoryInfoData", "System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_get_memory_info",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Delegate ===
            registry.Register("System.Delegate", "Combine", ["System.Delegate", "System.Delegate"],
                ShapeKind.SimpleForward, "chaos_delegate_combine",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Delegate", "Remove", ["System.Delegate", "System.Delegate"],
                ShapeKind.SimpleForward, "chaos_delegate_remove",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Object ===
            registry.Register("System.Object", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosObjectCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // System.Attribute..ctor(void) — base constructor called from attribute
            // subclasses (e.g. FactAttribute).  Must be registered as an external
            // runtime helper to ensure the chaos_external_runtime_* declaration is
            // emitted in the generated header.
            registry.Register("System.Attribute", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosExternalRuntimeFallback",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Object", "Equals", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_object_equals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });


            // === Object.GetHashCode (SimpleForward) ===
            registry.Register("System.Object", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosObjectGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Object.ToString (SimpleForward) ===
            registry.Register("System.Object", "ToString", [],
                ShapeKind.SimpleForward, "ChaosObjectToString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Object.GetType (SimpleForward) ===
            registry.Register("System.Object", "GetType", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosObjectGetType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Object.MemberwiseClone (SimpleForward) ===
            registry.Register("System.Object", "MemberwiseClone", [],
                ShapeKind.SimpleForward, "ChaosObjectMemberwiseClone",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Object::Equals static (two-arg overload) ===
            registry.Register("System.Object", "Equals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "ChaosObjectEqualsStatic",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Object::ReferenceEquals ===
            registry.Register("System.Object", "ReferenceEquals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "chaos_object_reference_equals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Stream/IO (SimpleForward) ===
            registry.Register("System.IO.Stream", "Flush", [],
                ShapeKind.SimpleForward, "ChaosStreamFlush",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.Stream", "Seek", ["System.Int64", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamSeek",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.IO.Stream", "SetLength", ["System.Int64"],
                ShapeKind.SimpleForward, "ChaosStreamSetLength",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.Stream", "get_Length", [],
                ShapeKind.SimpleForward, "ChaosStreamGetLength",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.Stream", "get_Position", [],
                ShapeKind.SimpleForward, "ChaosStreamGetPosition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.MemoryStream", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosMemoryStreamCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.StringReader", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosStringReaderCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.StringWriter", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosStringWriterCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextReader", "ReadLine", [],
                ShapeKind.SimpleForward, "ChaosTextReaderReadLine",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextReader", "ReadToEnd", [],
                ShapeKind.SimpleForward, "ChaosTextReaderReadToEnd",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextWriter", "Write", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTextWriterWrite",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.TextWriter", "WriteLine", [],
                ShapeKind.SimpleForward, "ChaosTextWriterWriteLine",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextWriter", "WriteLine", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTextWriterWriteLineStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── Stream.Read/Write/CopyTo ──
            registry.Register("System.IO.Stream", "Read", ["System.Byte[]", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamRead",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.IO.Stream", "Write", ["System.Byte[]", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamWrite",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.IO.Stream", "CopyTo", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosStreamCopyTo",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── MemoryStream(byte[]) ──
            registry.Register("System.IO.MemoryStream", ".ctor", ["System.Byte[]"],
                ShapeKind.SimpleForward, "ChaosMemoryStreamCtorWithBuffer",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── BinaryReader ──
            registry.Register("System.IO.BinaryReader", ".ctor", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosBinaryReaderCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryReader", "ReadInt32", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.BinaryReader", "ReadString", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.BinaryReader", "ReadDouble", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // ── BinaryWriter ──
            registry.Register("System.IO.BinaryWriter", ".ctor", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryWriter", "Write", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterWriteInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryWriter", "Write", ["System.String"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterWriteString",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ═══════════════════════════════════════════════════════════════
            // System.Buffers — InlineShape
            // ═══════════════════════════════════════════════════════════════

            static string? GetReverseEndiannessExpression(string cppType)
            {
                return cppType switch
                {
                    "CHAOS_IL2CPP_INT8" or "CHAOS_IL2CPP_UINT8" => "{0}",
                    "CHAOS_IL2CPP_INT16" =>
                        "static_cast<CHAOS_IL2CPP_INT16>((static_cast<CHAOS_IL2CPP_UINT16>({0}) << 8) | (static_cast<CHAOS_IL2CPP_UINT16>({0}) >> 8))",
                    "CHAOS_IL2CPP_UINT16" =>
                        "({0} << 8) | ({0} >> 8)",
                    "CHAOS_IL2CPP_INT32" =>
                        "static_cast<CHAOS_IL2CPP_INT32>((static_cast<CHAOS_IL2CPP_UINT32>({0}) << 24) | ((static_cast<CHAOS_IL2CPP_UINT32>({0}) & 0x0000FF00) << 8) | ((static_cast<CHAOS_IL2CPP_UINT32>({0}) >> 8) & 0x0000FF00) | (static_cast<CHAOS_IL2CPP_UINT32>({0}) >> 24))",
                    "CHAOS_IL2CPP_UINT32" =>
                        "({0} << 24) | (({0} & 0x0000FF00) << 8) | (({0} >> 8) & 0x0000FF00) | ({0} >> 24)",
                    "CHAOS_IL2CPP_INT64" =>
                        "static_cast<CHAOS_IL2CPP_INT64>([&]() -> CHAOS_IL2CPP_UINT64 { CHAOS_IL2CPP_UINT64 _v = static_cast<CHAOS_IL2CPP_UINT64>({0}); _v = (_v & 0x00000000FFFFFFFF) << 32 | (_v >> 32); _v = (_v & 0x0000FFFF0000FFFF) << 16 | ((_v >> 16) & 0x0000FFFF0000FFFF); _v = (_v & 0x00FF00FF00FF00FF) << 8 | ((_v >> 8) & 0x00FF00FF00FF00FF); return _v; }())",
                    "CHAOS_IL2CPP_UINT64" =>
                        "[&]() -> CHAOS_IL2CPP_UINT64 { CHAOS_IL2CPP_UINT64 _v = {0}; _v = (_v & 0x00000000FFFFFFFF) << 32 | (_v >> 32); _v = (_v & 0x0000FFFF0000FFFF) << 16 | ((_v >> 16) & 0x0000FFFF0000FFFF); _v = (_v & 0x00FF00FF00FF00FF) << 8 | ((_v >> 8) & 0x00FF00FF00FF00FF); return _v; }()",
                    _ => null,
                };
            }

            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Buffers.Binary.BinaryPrimitives",
                MethodName: "ReverseEndianness",
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var cppType = MapTypeArgToCppType(paramTypes[0]);
                    if (cppType == null) return null;
                    return GetReverseEndiannessExpression(cppType);
                }));

            // Base64.GetMaxEncodedToUtf8Length(int) → ((length + 2) / 3) * 4
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Buffers.Text.Base64",
                MethodName: "GetMaxEncodedToUtf8Length",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32") return null;
                    return "(({0} + 2) / 3) * 4";
                }));

            // Base64.GetMaxDecodedFromUtf8Length(int) → (length / 4) * 3
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Buffers.Text.Base64",
                MethodName: "GetMaxDecodedFromUtf8Length",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32") return null;
                    return "({0} / 4) * 3";
                }));

            // === Array.Empty<T> (GenericShapeDescriptor -- resolves to ChaosArrayEmpty stub) ===
            // NOTE: Must be GenericShapeDescriptor, not SimpleForward, because the codegen
            // includes generic type args in the method name (e.g. "Empty<System.Byte>"),
            // which causes exact hash lookup to fail against the non-generic registration.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Empty",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var body = new[] { "    return ChaosArrayEmpty();" };
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "", body);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosArrayEmpty_Inline");
                }));

            // === Type::GetMethod generic handler (GenericShapeDescriptor -- handles various overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetMethod",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    (void)chaos_arg_0; (void)chaos_arg_1;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    var src2 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src2, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1, 2 });
                }));


            // === ThrowHelper (dead-code safety stubs — should never be called in well-formed tests) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.ThrowHelper",
                MethodName: "ThrowArgumentNullException",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    CHAOS_IL2CPP_FAIL();", "    return 0;"]);
                    return new GenericShapeResolution(src, symbol,
                        new AotCoreIrAbiSlotArtifact[] { CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType) },
                        CreateVoidAbiSlot(), EmptyRawArgumentIndices);
                }));

            // === Common framework method stubs ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.CompilerServices.Unsafe",
                MethodName: "As",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    (void)chaos_arg_0;", "    CHAOS_IL2CPP_FAIL();", "    return 0;"]);
                    return new GenericShapeResolution(src, symbol,
                        new AotCoreIrAbiSlotArtifact[] { CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType) },
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType), EmptyRawArgumentIndices);
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.CompilerServices.Unsafe",
                MethodName: "SkipInit",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Unsafe.SkipInit<T>(ref T value) — no-op: leave ref uninitialized.
                    // CHAOS_IL2CPP_INTPTR return + () params avoids C2733 with
                    // AddExternalRuntimeStubs' separate extern declaration.
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        ["    return 0;"]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                        new HashSet<int> { 0 });
                }));
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.ArgumentNullException",
                MethodName: "ThrowIfNull",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        ["    (void)chaos_arg_0; (void)chaos_arg_1;", "    CHAOS_IL2CPP_FAIL();"]);
                    return new GenericShapeResolution(src, symbol,
                        new AotCoreIrAbiSlotArtifact[] {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                        },
                        CreateVoidAbiSlot(), EmptyRawArgumentIndices);
                }));

            // === Environment ===
            registry.Register("System.Environment", "get_CurrentManagedThreadId", [],
                ShapeKind.SimpleForward, "chaos_current_managed_thread_id",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // === Console (stubs for verification pipelines — tests track via ChaosAssertState.ExitCode) ===
            registry.Register("System.Console", "get_Error", [],
                ShapeKind.SimpleForward, "ChaosConsoleGetError",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Console",
                MethodName: "WriteLine",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        // WriteLine() — 0-arg static
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol, "",
                        [
                            "",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateVoidAbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    // WriteLine(string) — 1-arg static
                    var src1 = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    (void)chaos_arg_0;",
                    ]);
                    return new GenericShapeResolution(src1, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === OperatingSystem platform checks (dispatch via GenericShapeDescriptors below) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.OperatingSystem",
                MethodName: "IsWindows",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "#if defined(_WIN32)",
                        "    return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "#else",
                        "    return 0;",
                        "#endif",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.OperatingSystem",
                MethodName: "IsLinux",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "#if defined(__linux__)",
                        "    return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "#else",
                        "    return 0;",
                        "#endif",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.OperatingSystem",
                MethodName: "IsMacOS",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "#if defined(__APPLE__)",
                        "    return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
                        "#else",
                        "    return 0;",
                        "#endif",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            // === Numeric formatting ===
            // Int32.ToString() fully handled by GenericShapeDescriptor below (bypass Intern, direct GC alloc)
            // NOTE: The inline GcAllocateAtomic may trigger nursery GC. Conservative stack scanning
            // updates chaos_locals[] on the stack but NOT CPU registers, so callers that have cached
            // a GC-tracked local in a register across the call boundary will read a stale pointer.
            // GC-safe reloads are added at call sites that cross a GC safepoint.

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Int32",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);",
                        "    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot);",
                        "    // Compute digit count via branch chain (no generic formatting library).",
                        "    auto chaos_tmp = static_cast<CHAOS_IL2CPP_UINT32>(chaos_value);",
                        "    CHAOS_IL2CPP_SIZE chaos_len = 1;",
                        "    if (chaos_tmp >= 10000) { chaos_len = 5; goto chaos_alloc; }",
                        "    if (chaos_tmp >= 1000) { chaos_len = 4; goto chaos_alloc; }",
                        "    if (chaos_tmp >= 100)  { chaos_len = 3; goto chaos_alloc; }",
                        "    if (chaos_tmp >= 10)  { chaos_len = 2; }",
                        "chaos_alloc:",
                        "    auto* chaos_raw = static_cast<char*>(",
                        "        chaos::il2cpp::runtime_core::GcAllocateAtomicFastNoZero(",
                        "            sizeof(chaos_type_System_Private_CoreLib_System_String) + chaos_len + 1));",
                        "    auto* chaos_str = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_raw);",
                        "    chaos_str->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();",
                        "    chaos_str->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_len);",
                        "    chaos_str->utf8_data = chaos_raw + sizeof(chaos_type_System_Private_CoreLib_System_String);",
                        "    chaos_str->string_id = 0;",
                        "    // Format digits directly into utf8_data (backward fill).",
                        "    auto* chaos_p = const_cast<char*>(chaos_str->utf8_data + chaos_len);",
                        "    *chaos_p = '\\0';",
                        "    do {",
                        "        *--chaos_p = static_cast<char>('0' + (chaos_tmp % 10));",
                        "        chaos_tmp /= 10;",
                        "    } while (chaos_tmp != 0);",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_str);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Single",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    const char* chaos_format_utf8 = nullptr;",
                        "    if (chaos_arg_1 != 0)",
                        "    {",
                        "        chaos_format_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    }",
                        string.Empty,
                        "    if (chaos_format_utf8 != nullptr && CHAOS_IL2CPP_STRCMP(chaos_format_utf8, \"F1\") != 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_float(ChaosLoadFloat32(chaos_arg_0));",
                        "    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
                        "        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
                        "    return chaos_make_string_id_value(chaos_id);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Double",
                MethodName: "ToString",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    const char* chaos_format_utf8 = nullptr;",
                        "    if (chaos_arg_1 != 0)",
                        "    {",
                        "        chaos_format_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
                        "    }",
                        string.Empty,
                        "    if (chaos_format_utf8 != nullptr && CHAOS_IL2CPP_STRCMP(chaos_format_utf8, \"F1\") != 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_double(ChaosLoadFloat64(chaos_arg_0));",
                        "    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
                        "        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
                        "    return chaos_make_string_id_value(chaos_id);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 });
                }));

            // === Marshal non-generic operations (inline body with runtime core calls) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "AllocHGlobal",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    if (chaos_arg_0 < 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalAllocHGlobal(",
                        "        chaos_runtime, static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "FreeHGlobal",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    (void)chaos::il2cpp::runtime_core::MarshalFreeHGlobal(chaos_runtime, chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "FreeCoTaskMem",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    (void)chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(chaos_runtime, chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.StringToHGlobalAnsi(String) → IntPtr ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToHGlobalAnsi",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0) return 0;",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToHGlobalAnsi(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.StringToHGlobalUni(String) → IntPtr ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToHGlobalUni",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0) return 0;",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToHGlobalUni(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.PtrToStringAnsi(IntPtr, int) → String ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStringAnsi",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return chaos::il2cpp::runtime_core::MarshalPtrToStringAnsiIcall(",
                        "        chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateInt32AbiSlot(),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 });
                }));

            // === Marshal.UnsafeAddrOfPinnedArrayElement(Array, int) → IntPtr ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "UnsafeAddrOfPinnedArrayElement",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return chaos::il2cpp::runtime_core::MarshalUnsafeAddrOfPinnedArrayElement(",
                        "        reinterpret_cast<void*>(chaos_arg_0), chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToCoTaskMemUTF8",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);",
                        "    (void)chaos_length; (void)chaos_utf8_data;",
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStringUTF8",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return chaos_arg_0 == 0",
                        "        ? 0",
                        "        : chaos_reflection_create_string_literal(reinterpret_cast<const char*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.SizeOf<T> — return marshalled struct size via descriptor ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "SizeOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    return chaos_desc != nullptr",
                        "        ? static_cast<CHAOS_IL2CPP_INT32>(chaos_desc->total_size)",
                        "        : 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                        CreateInt32AbiSlot(),
                        new HashSet<int>());
                }));

            // === Marshal.OffsetOf<T>(string) — return field offset by name via descriptor ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "OffsetOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var fieldNamesSymbol = NativeAotLoweringPlanner.GetNativeStructFieldNamesSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ::chaos::il2cpp::runtime_core::MarshalOffsetOf(",
                        $"        {structDescSymbol},",
                        $"        {fieldNamesSymbol},",
                        "        chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.Copy (element type and direction extracted from parameter types) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "Copy",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 4) return null;

                    bool isArrayToPtr;
                    string elementTypeName;
                    if (paramTypes[0].EndsWith("[]", StringComparison.Ordinal))
                    {
                        isArrayToPtr = true;
                        elementTypeName = paramTypes[0].Substring(0, paramTypes[0].Length - 2);
                    }
                    else if (string.Equals(paramTypes[0], "System.IntPtr", StringComparison.Ordinal) &&
                             paramTypes[1].EndsWith("[]", StringComparison.Ordinal))
                    {
                        isArrayToPtr = false;
                        elementTypeName = paramTypes[1].Substring(0, paramTypes[1].Length - 2);
                    }
                    else
                    {
                        return null;
                    }

                    if (!MarshalCopyElementTypeMap.TryGetValue(elementTypeName, out var cppElementType))
                        return null;

                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);

                    if (isArrayToPtr)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3",
                        [
                            "    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_0);",
                            "    auto chaos_start_index = chaos_arg_1;",
                            "    auto chaos_dest = chaos_arg_2;",
                            "    auto chaos_length = chaos_arg_3;",
                            "    chaos::il2cpp::runtime_core::MarshalCopyArrayToPtr<" + cppElementType + ">(chaos_array, chaos_start_index, chaos_dest, chaos_length);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(),
                                CreateInt32AbiSlot(),
                            }), CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });
                    }
                    else
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3",
                        [
                            "    auto chaos_source = chaos_arg_0;",
                            "    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_1);",
                            "    auto chaos_start_index = chaos_arg_2;",
                            "    auto chaos_length = chaos_arg_3;",
                            "    chaos::il2cpp::runtime_core::MarshalCopyPtrToArray<" + cppElementType + ">(chaos_source, chaos_array, chaos_start_index, chaos_length);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }), CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });
                    }
                }));

            // === Marshal.StructureToPtr<T> — deep-copy managed struct to native memory ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StructureToPtr",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    if (chaos_desc != nullptr) {",
                        "        if (chaos_arg_2 != 0) {",
                        "            chaos::il2cpp::struct_marshal::DestroyMarshalledStruct(",
                        "                chaos_desc, reinterpret_cast<unsigned char*>(chaos_arg_1), chaos_runtime);",
                        "        }",
                        "        chaos::il2cpp::struct_marshal::MarshalStructManagedToNative(",
                        "            chaos_desc,",
                        "            reinterpret_cast<unsigned char*>(chaos_arg_1),",
                        "            reinterpret_cast<unsigned char*>(chaos_arg_0),",
                        "            chaos_runtime, nullptr);",
                        "    }",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(typeArgs[0], AotCoreIrTypeShapeKind.ValueType),
                            CreateNativeIntAbiSlot(),
                            CreateInt32AbiSlot(),
                        }), CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));

            // === Marshal.PtrToStructure<T> — deep-copy native memory to managed struct ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStructure",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    if (chaos_desc == nullptr) return 0;",
                        "    auto* chaos_blob = static_cast<unsigned char*>(",
                        "        CHAOS_IL2CPP_MALLOC(chaos_desc->total_size));",
                        "    if (chaos_blob == nullptr) return 0;",
                        "    chaos::il2cpp::struct_marshal::MarshalStructNativeToManaged(",
                        "        chaos_desc, chaos_blob,",
                        "        reinterpret_cast<const unsigned char*>(chaos_arg_0),",
                        "        chaos_runtime, nullptr);",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_blob);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(typeArgs[0], AotCoreIrTypeShapeKind.ValueType),
                        new HashSet<int> { 0 });
                }));

            // === Marshal.DestroyStructure — free native resources for marshalled struct ===
            // Generic overload: DestroyStructure<T>(IntPtr) — uses static descriptor.
            // Non-generic overload: DestroyStructure(IntPtr, Type) — V1: no-op (runtime reflection path).
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "DestroyStructure",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 1)
                    {
                        var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                        var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                            "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                            $"    const auto* chaos_desc = {structDescSymbol};",
                            "    if (chaos_desc != nullptr) {",
                            "        chaos::il2cpp::struct_marshal::DestroyMarshalledStruct(",
                            "            chaos_desc, reinterpret_cast<unsigned char*>(chaos_arg_0), chaos_runtime);",
                            "    }",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 });
                    }
                    // Non-generic overload: Marshal.DestroyStructure(IntPtr, Type)
                    // Extracts TypeInfoHot* from the managed Type object via runtime helper.
                    var nonGenericSymbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var nonGenericSrc = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", nonGenericSymbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ::chaos::il2cpp::runtime_core::ChaosDestroyStructureByType(",
                        "        chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(nonGenericSrc, nonGenericSymbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }), CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));


            // === GCHandle stubs (DirectNativeSymbol) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.GCHandle",
                MethodName: "Alloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosGCHandleAlloc(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGCHandleAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.GCHandle",
                MethodName: "Free",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosGCHandleFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGCHandleFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.GCHandle",
                MethodName: "FromIntPtr",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosGCHandleGetTarget(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGCHandleGetTarget");
                }));

            // === Marshal HR stubs (DirectNativeSymbol) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetExceptionForHR",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    return ChaosMarshalGetExceptionForHR(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateInt32AbiSlot()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosMarshalGetExceptionForHR");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "ThrowExceptionForHR",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    ChaosMarshalThrowExceptionForHR(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateInt32AbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosMarshalThrowExceptionForHR");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetHRForException",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosMarshalGetHRForException(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosMarshalGetHRForException");
                }));


            // === NativeMemory stubs (DirectNativeSymbol) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Alloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosNativeMemoryAlloc(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Free",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosNativeMemoryFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Realloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosNativeMemoryRealloc(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosNativeMemoryRealloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedAlloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosNativeMemoryAlignedAlloc(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedFree",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosNativeMemoryAlignedFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedRealloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    return ChaosNativeMemoryAlignedRealloc(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedRealloc");
                }));

            // === SafeBuffer stubs (DirectNativeSymbol) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "get_ByteLength",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosSafeBufferGetByteLength(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosSafeBufferGetByteLength");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "Read",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_UINT8", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosSafeBufferReadByte(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosSafeBufferReadByte");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "Write",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_UINT8 chaos_arg_2",
                    [
                        "    ChaosSafeBufferWriteByte(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot(), CreateInt32AbiSlot() }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosSafeBufferWriteByte");
                }));

            // === String.Join (IEnumerable<T> — resolves variants at planning time) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Join",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (!TryGetStringJoinEnumerableElementType(callee, out var elementType))
                        return null;
                    var variants = planner.ResolveEnumerableJoinSupportVariants(elementType!);
                    if (variants.Count == 0) return null;
                    ExternalRuntimeHelperDefinition? def = null;
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
                    if (def == null) return null;
                    return new GenericShapeResolution(def.Source, def.TargetSymbol,
                        def.ParameterAbis, def.ReturnAbi, def.RawArgumentIndices,
                        def.ReferencedStaticFieldSubjectIds);
                }));

            // === String::Join(System.String,System.String[]) (non-generic overload, stub returning null) ===
            registry.Register("System.String", "Join", ["System.String", "System.String[]"],
                ShapeKind.SimpleForward, "ChaosStringJoinSs",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === CustomAttribute: MemberInfo.IsDefined (planning-time condition checked in Resolver) ===
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

            // === InterpolatedStringHandler ===
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

            // === RuntimeHelpers ===
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "InitializeArray",
                ["System.Array", "System.RuntimeFieldHandle"],
                ShapeKind.SimpleForward, "chaos_initialize_array_from_field_data_int32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 1 });

            // === Monitor ===
            registry.Register("Monitor", "Enter", ["System.Object", "System.Boolean&"],
                ShapeKind.SimpleForward, "chaos_monitor_enter",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("Monitor", "Exit", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_monitor_exit",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread ===
            registry.Register("System.Threading.Thread", ".ctor",
                ["System.Threading.ThreadStart"],
                ShapeKind.SimpleForward, "chaos_thread_ctor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Threading.ThreadStart", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Threading.Thread", "Start", [],
                ShapeKind.SimpleForward, "chaos_thread_start",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Threading.Thread", "Join", [],
                ShapeKind.SimpleForward, "chaos_thread_join",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Threading.Thread", "get_CurrentThread", [],
                ShapeKind.SimpleForward, "chaos_thread_get_current",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);


            // === Thread::Sleep ===
            registry.Register("System.Threading.Thread", "Sleep", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_thread_sleep",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    }),
                CreateVoidAbiSlot(),
                EmptyRawArgumentIndices);

            // === Thread::Yield ===
            registry.Register("System.Threading.Thread", "Yield", [],
                ShapeKind.SimpleForward, "chaos_thread_yield",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // === Thread::Abort ===
            registry.Register("System.Threading.Thread", "Abort", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_thread_abort",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // === Thread::Interrupt ===
            registry.Register("System.Threading.Thread", "Interrupt", [],
                ShapeKind.SimpleForward, "chaos_thread_interrupt",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_IsBackground ===
            registry.Register("System.Threading.Thread", "get_IsBackground", [],
                ShapeKind.SimpleForward, "chaos_thread_is_background",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::set_IsBackground ===
            registry.Register("System.Threading.Thread", "set_IsBackground", ["System.Boolean"],
                ShapeKind.SimpleForward, "chaos_thread_set_background",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_Priority ===
            registry.Register("System.Threading.Thread", "get_Priority", [],
                ShapeKind.SimpleForward, "chaos_thread_get_priority",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::set_Priority ===
            registry.Register("System.Threading.Thread", "set_Priority", ["System.Threading.ThreadPriority"],
                ShapeKind.SimpleForward, "chaos_thread_set_priority",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_ThreadState ===
            registry.Register("System.Threading.Thread", "get_ThreadState", [],
                ShapeKind.SimpleForward, "chaos_thread_get_state",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Thread::get_IsThreadPoolThread ===
            registry.Register("System.Threading.Thread", "get_IsThreadPoolThread", [],
                ShapeKind.SimpleForward, "chaos_thread_is_threadpool",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === DateTime ===
            registry.Register("System.DateTime", "get_UtcNow", [],
                ShapeKind.SimpleForward, "ChaosDatetimeGetUtcNow",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    TypeSubjectId = "System.Private.CoreLib/System.DateTime"
                },
                EmptyRawArgumentIndices);

            // === TimeSpan ===
            // FromMilliseconds: emit inline expression to avoid native dependency.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.TimeSpan",
                MethodName: "FromMilliseconds",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT64", symbol,
                        "double chaos_arg_0",
                    [
                        "    return static_cast<CHAOS_IL2CPP_INT64>(chaos_arg_0 * 10000.0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                        new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                        new HashSet<int> { 0 });
                }));

            // === Reflection: Type ===
            registry.Register("System.Type", "GetTypeFromHandle", ["System.RuntimeTypeHandle"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFromHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "op_Inequality", ["System.Type", "System.Type"],
                ShapeKind.SimpleForward, "ChaosTypeInequality",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Type", "get_Assembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssembly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetGenericTypeDefinition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericTypeDefinition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetGenericArguments", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericArguments",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Reflection: Assembly ===
            registry.Register("System.Reflection.Assembly", "GetType", ["System.String"],
                ShapeKind.SimpleForward, "chaos_reflection_get_type_from_assembly",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Reflection.Assembly", "GetName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.AssemblyName", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssemblyNameValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Reflection: MethodBase ===
            registry.Register("System.Reflection.MethodBase", "get_MethodHandle", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethodHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MethodBase", "GetParameters", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameters",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MethodBase", "Invoke", ["System.Object", "System.Object[]"],
                ShapeKind.SimpleForward, "ChaosReflectionInvokeMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            // === MethodBase::Invoke with extra params (GenericShapeDescriptor -- handles BindingFlags/Binder/CultureInfo overloads) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.MethodBase",
                MethodName: "Invoke",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Build parameter ABI slots from param types
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count + 1);
                    abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)); // this
                    foreach (var pt in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)); // all refs
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Reflection: MemberInfo ===
            registry.Register("System.Reflection.MemberInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetMemberName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MemberInfo", "get_DeclaringType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetDeclaringType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.MemberInfo", "get_MetadataToken", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetMetadataToken",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // === Activator ===
            registry.Register("System.Activator", "CreateInstance", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionCreateInstance",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Reflection: Type (additional) ===
            registry.Register("System.Type", "get_TypeHandle", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetField", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetField",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Type::GetField with BindingFlags (GenericShapeDescriptor) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetField",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Type::GetEvent (all overloads via GenericShapeDescriptor) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetEvent",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.EventInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Type::GetProperty (all overloads via GenericShapeDescriptor) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetProperty",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            registry.Register("System.Type", "GetMethod", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Type", "GetType", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeByName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetConstructors", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetConstructors",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Type::GetConstructors (0-param fallback) ===
            registry.Register("System.Type", "GetConstructors", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetConstructorsDefault",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetMethod", ["System.String", "System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Reflection: MethodInfo ===
            registry.Register("System.Reflection.MethodInfo", "MakeGenericMethod", ["System.Type[]"],
                ShapeKind.SimpleForward, "ChaosReflectionMakeGenericMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Assembly::GetType(System.String,System.Boolean) overload ===
            registry.Register("System.Reflection.Assembly", "GetType", ["System.String", "System.Boolean"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFromAssemblyBool",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            // === Reflection: ParameterInfo ===
            registry.Register("System.Reflection.ParameterInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameterName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === Async: Task.Yield / YieldAwaitable ===
            registry.Register("System.Threading.Tasks.Task", "Yield", [],
                ShapeKind.SimpleForward, "chaos_async_yield_create",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable", "GetAwaiter", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_awaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "get_IsCompleted", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_is_completed",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "GetResult", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_result",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // === Decimal ===
            registry.Register("System.Decimal", ".ctor", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_decimal_ctor_int32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // Decimal::op_Explicit(Decimal) -> Int32 — forward to ChaosDecimalToInt32
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Decimal",
                MethodName: "op_Explicit",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosDecimalToInt32(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // === Nullable<T>.get_HasValue — reads the hasValue field at offset sizeof(ThinLockableHeader) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Nullable`1",
                MethodName: "get_HasValue",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        // get_HasValue() instance method: hasValue is stored as int32 at the start of the value struct
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    if (chaos_arg_0 == 0) return 0;",
                            "    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType)),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        "    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            // === Nullable<T>.GetValueOrDefault() — reads the value field after ThinLockableHeader+hasValue ===
            // Overloads: GetValueOrDefault() and GetValueOrDefault(T)
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Nullable`1",
                MethodName: "GetValueOrDefault",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    bool hasDefaultArg = paramTypes.Count > 0;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var returnAbi = CreateNativeIntAbiSlot();
                    var retType = "CHAOS_IL2CPP_INTPTR";
                    List<AotCoreIrAbiSlotArtifact> paramAbis = new()
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType)
                    };
                    string paramSig = "CHAOS_IL2CPP_INTPTR chaos_arg_0";
                    var rawIndices = new HashSet<int> { 0 };
                    var bodyLines = new List<string>
                    {
                        "    // nullable struct layout: ThinLockableHeader(16B) | hasValue(int32, 4B) | value(T)",
                        "    if (chaos_arg_0 == 0) return 0;",
                        "    auto* chaos_has_value = reinterpret_cast<CHAOS_IL2CPP_INT32*>(reinterpret_cast<char*>(chaos_arg_0) + sizeof(ThinLockableHeader));",
                    };
                    if (hasDefaultArg)
                    {
                        paramAbis.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType));
                        paramSig += ", CHAOS_IL2CPP_INTPTR chaos_arg_1";
                        rawIndices.Add(1);
                        bodyLines.Add("    if (*chaos_has_value == 0) return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_1);");
                    }
                    if (typeArgs != null && typeArgs.Count > 0 && typeArgs[0] == "System.Int32")
                    {
                        retType = "CHAOS_IL2CPP_INT32";
                        returnAbi = CreateInt32AbiSlot();
                        bodyLines.Add("    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(reinterpret_cast<char*>(chaos_arg_0) + sizeof(ThinLockableHeader) + 4);");
                    }
                    else
                    {
                        bodyLines.Add("    return *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(chaos_arg_0) + sizeof(ThinLockableHeader) + 4);");
                    }
                    var src = RenderSimpleExternalRuntimeHelper(retType, symbol,
                        paramSig,
                        bodyLines.ToArray());
                    return new GenericShapeResolution(src, symbol,
                        paramAbis,
                        returnAbi,
                        rawIndices);
                }));

            // === Convert.ToChar (GenericShapeDescriptor — native bridge to convert.cpp) ===
            // All overloads delegate to chaos_convert_tochar_* functions in convert.cpp.
            // This eliminates 200+ lines of duplicate StringId/exception body code per overload.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var retAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16, TypeShape = AotCoreIrTypeShapeKind.ValueType };

                    // Map parameter types to native chaos_convert_tochar_* symbols
                    string nativeFn = paramTypes.Count switch
                    {
                        0 => "chaos_convert_tochar_int32",  // no-arg → return 0
                        1 => GetToCharNativeSymbol(paramTypes[0]),
                        2 when paramTypes[0] == "System.Object" => "chaos_convert_tochar_object_provider",
                        2 when paramTypes[0] == "System.String" => "chaos_convert_tochar_string_provider",
                        _ => "chaos_convert_tochar_int32",
                    };

                    // Generate thin forwarding body that calls the native function.
                    // For String overloads: emit inline ResolveWithGlobalCache to eliminate
                    // the extern "C" call barrier and TLS indirect addressing.
                    var args = abiSlots.Count == 0 ? "" :
                        string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"chaos_arg_{i}"));
                    string[] bodyLines;
                    if (abiSlots.Count == 0)
                    {
                        bodyLines = ["    return static_cast<CHAOS_IL2CPP_UINT16>(0);"];
                    }
                    else if (abiSlots.Count == 1 && paramTypes[0] == "System.String")
                    {
                        bodyLines =
                        [
                            "    auto _v = chaos::il2cpp::string_table::ResolveWithGlobalCache(chaos_extract_string_id(chaos_arg_0));",
                            "    if (_v.byte_count == 0)",
                            "    {",
                            "        chaos::il2cpp::runtime_core::chaos_raise_exception(0);",
                            "        return 0;",
                            "    }",
                            "    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<unsigned char>(_v.utf8_data[0]));",
                        ];
                    }
                    else if (abiSlots.Count == 2 && paramTypes[0] == "System.String" && paramTypes[1] == "System.IFormatProvider")
                    {
                        bodyLines =
                        [
                            "    (void)chaos_arg_1;",
                            "    auto _v = chaos::il2cpp::string_table::ResolveWithGlobalCache(chaos_extract_string_id(chaos_arg_0));",
                            "    if (_v.byte_count == 0)",
                            "    {",
                            "        chaos::il2cpp::runtime_core::chaos_raise_exception(0);",
                            "        return 0;",
                            "    }",
                            "    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<unsigned char>(_v.utf8_data[0]));",
                        ];
                    }
                    else
                    {
                        bodyLines = [$"    return {nativeFn}({args});"];
                    }
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_UINT16", symbol, paramSig, bodyLines);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        retAbi,
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)),
                        DirectNativeSymbol: nativeFn,
                        DirectNativeHeader: "\"convert.h\"");
                }));

            // === Convert.ToChar — inline shapes for value-type overloads ===
            // These emit a direct static_cast at the call site, eliminating
            // external runtime helper function call overhead (dispatch table
            // lookup + prolog/epilog).  Managed JIT inlines these to a single
            // mov instruction — this makes native AOT match that perf.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var firstParam = paramTypes[0];
                    if (firstParam is "System.Byte" or "System.SByte" or "System.Int16" or "System.UInt16"
                        or "System.Int32" or "System.UInt32" or "System.Int64" or "System.UInt64"
                        or "System.Char")
                    {
                        // Direct truncation cast — matches JIT inlining for (char)intValue
                        return "static_cast<CHAOS_IL2CPP_UINT16>({0})";
                    }
                    return null;
                }));

            // === Convert.ToChar — always-throw overloads (no possible valid conversion) ===
            // These emit throw chaos_managed_exception{0} directly at the call site,
            // eliminating the extern "C" bridge function call, 3 TLS reads, and
            // ResolveTypeByName module iteration. The arg is consumed from eval stack
            // but unused in the template (the throw terminates execution).
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var firstParam = paramTypes[0];
                    if (firstParam is "System.Boolean" or "System.DateTime")
                    {
                        // Comma operator: throw terminates execution, second operand
                        // provides the result type for the caller's eval stack.
                        // The raw argument value ({0}) is NOT a valid managed object pointer —
                        // e.g., true → 1, DateTime → 64-bit ticks. Passing it as the exception
                        // object causes the catch handler to crash when it tries to dereference
                        // the "header" at that invalid address to check the exception type.
                        // Use nullptr instead: the catch handler skips type checking when the
                        // header is null, which is correct because this inline only applies to
                        // methods that always throw InvalidCastException.
                        return "(chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)), static_cast<CHAOS_IL2CPP_UINT16>(0))";
                    }
                    return null;
                }));

            // === Convert.ToChar(String) — inline TLS cache peek + first-byte return ===
            // Inlines the full string-to-char conversion at the call site, eliminating
            // the extern "C" bridge function call + string_table::Resolve function call.
            // ResolveFast() checks the TLS cache inline (1 compare) and only calls the
            // full Resolve() on cache miss, which is ~0.01% of calls in benchmarks.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: "ToChar",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.String")
                    {
                        // {0} is guaranteed to be a string ID for System.String-typed
                        // parameters — no chaos_is_string_id check needed.
                        // Uses ResolveWithGlobalCache (direct-mapped global cache,
                        // no TLS indirect addressing — ~1 global load + 1 compare
                        // on hit instead of 3 TLS loads).
                        return """
                            [&]() -> CHAOS_IL2CPP_UINT16 {
                                auto _v = chaos::il2cpp::string_table::ResolveWithGlobalCache(chaos_extract_string_id({0}));
                                if (_v.byte_count == 0) {
                                    chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr));
                                    return 0;
                                }
                                return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<unsigned char>(_v.utf8_data[0]));
                            }()
                            """.Replace("\r\n", "\n").Trim();
                    }
                    if (paramTypes.Count == 2 && paramTypes[0] == "System.String" && paramTypes[1] == "System.IFormatProvider")
                    {
                        return """
                            [&]() -> CHAOS_IL2CPP_UINT16 {
                                (void){1};
                                auto _v = chaos::il2cpp::string_table::ResolveWithGlobalCache(chaos_extract_string_id({0}));
                                if (_v.byte_count == 0) {
                                    chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr));
                                    return 0;
                                }
                                return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<unsigned char>(_v.utf8_data[0]));
                            }()
                            """.Replace("\r\n", "\n").Trim();
                    }
                    return null;
                }));

            // === Int32.GetHashCode — inline pointer dereference ===
            // Eliminates ChaosInt32GetHashCode function call overhead. The hash of
            // an Int32 IS the value itself. Inline C++: (ptr==0?0:*reinterpret_cast<int32_t*>(ptr)).
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Int32",
                MethodName: "GetHashCode",
                Resolver: static (callee, paramTypes) =>
                {
                    return "({0} == 0 ? 0 : *reinterpret_cast<CHAOS_IL2CPP_INT32*>({0}))";
                })
            { IsInstanceMethod = true });

            // === Array::Copy (GenericShapeDescriptor -- calls ChaosArrayCopy for 5-param overload) ===
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Copy",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 5)
                    {
                        var srcFive = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3, CHAOS_IL2CPP_INT32 chaos_arg_4",
                        [
                            "    ChaosArrayCopy(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);",
                        ]);
                        return new GenericShapeResolution(srcFive, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[5]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3, 4 },
                            DirectNativeSymbol: "ChaosArrayCopy_Unsafe_Inline");
                    }
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("void", symbol, "",
                        [
                            "    return;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateVoidAbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    if (paramTypes.Count == 3)
                    {
                        var srcThree = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                        [
                            "    ChaosArrayCopy3(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                        ]);
                        return new GenericShapeResolution(srcThree, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2 },
                            DirectNativeSymbol: "ChaosArrayCopy3_Unsafe_Inline");
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                    {
                        if (pt.Contains("Int64") || pt.Contains("Double") || pt.Contains("Single"))
                            abiSlots.Add(new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType });
                        else if (pt == "System.Int32" || pt == "System.Int32&" || pt == "System.Boolean")
                            abiSlots.Add(CreateInt32AbiSlot());
                        else
                            abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcFallback = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(srcFallback, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));
        }

        private static string GetTypeDisplayNameFromSubjectId(string subjectId)
        {
            var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
            var typePart = separatorIndex >= 0 ? subjectId[..separatorIndex] : subjectId;
            var slashIndex = typePart.IndexOf('/');
            return slashIndex >= 0 ? typePart[(slashIndex + 1)..] : typePart;
        }

        private static string GetMethodNameFromSubjectId(string subjectId)
        {
            var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
            if (separatorIndex < 0) return string.Empty;
            var afterSeparator = subjectId[(separatorIndex + 2)..];
            var colonIndex = afterSeparator.IndexOf(':');
            var parenIndex = afterSeparator.IndexOf('(');
            if (colonIndex >= 0 && parenIndex >= 0)
            {
                return afterSeparator[..Math.Min(colonIndex, parenIndex)];
            }
            if (colonIndex >= 0)
            {
                return afterSeparator[..colonIndex];
            }
            return parenIndex >= 0 ? afterSeparator[..parenIndex] : afterSeparator;
        }

        // P3: cache for SubjectId -> parameter types (parsing is pure)
        private static readonly Dictionary<string, IReadOnlyList<string>> s_methodParamTypeCache
            = new(StringComparer.Ordinal);

        private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectId(string subjectId)
        {
            if (s_methodParamTypeCache.TryGetValue(subjectId, out var cached))
                return cached;
            var result = GetMethodParameterTypesFromSubjectIdImpl(subjectId);
            s_methodParamTypeCache[subjectId] = result;
            return result;
        }

        private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectIdImpl(string subjectId)
        {
            var parenOpen = subjectId.IndexOf('(');
            var parenClose = subjectId.LastIndexOf(')');
            if (parenOpen < 0 || parenClose <= parenOpen) return [];
            var paramsPart = subjectId[(parenOpen + 1)..parenClose];
            if (string.IsNullOrEmpty(paramsPart)) return [];
            return paramsPart.Split(',');
        }

        /// <summary>Map Convert.ToChar parameter type to the corresponding chaos_convert_tochar_* native function.</summary>
        private static string GetToCharNativeSymbol(string paramType)
        {
            return paramType switch
            {
                "System.Boolean" => "chaos_convert_tochar_boolean",
                "System.Byte" => "chaos_convert_tochar_byte",
                "System.Char" => "chaos_convert_tochar_char",
                "System.DateTime" => "chaos_convert_tochar_datetime",
                "System.Decimal" => "chaos_convert_tochar_decimal",
                "System.Double" => "chaos_convert_tochar_double",
                "System.Int16" => "chaos_convert_tochar_int16",
                "System.Int32" => "chaos_convert_tochar_int32",
                "System.Int64" => "chaos_convert_tochar_int64",
                "System.Object" => "chaos_convert_tochar_object",
                "System.SByte" => "chaos_convert_tochar_sbyte",
                "System.Single" => "chaos_convert_tochar_single",
                "System.String" => "chaos_convert_tochar_string",
                "System.UInt16" => "chaos_convert_tochar_uint16",
                "System.UInt32" => "chaos_convert_tochar_uint32",
                "System.UInt64" => "chaos_convert_tochar_uint64",
                _ => "chaos_convert_tochar_int32",
            };
        }

        private static string SanitizeForEnumName(string name)
        {
            var sb = new StringBuilder(name.Length);
            foreach (var ch in name)
                sb.Append(char.IsAsciiLetterOrDigit(ch) || ch == '_' ? ch : '_');
            return sb.ToString().ToUpperInvariant();
        }

        /// <summary>Parse generic type arguments from a type name suffix like "`1[[System.Int32]]".</summary>
        private static bool TryParseGenericTypeArgsFromTypeName(
            string genericSuffix,
            [NotNullWhen(true)] out IReadOnlyList<string>? typeArgs)
        {
            typeArgs = null;
            // Format: `1[[System.Int32]] or `2[[System.Int32],[System.String]]
            if (genericSuffix.Length < 2 || genericSuffix[0] != '`') return false;

            var bracketStart = genericSuffix.IndexOf("[[", StringComparison.Ordinal);
            if (bracketStart < 0) return false;

            var inner = genericSuffix.Substring(bracketStart + 2);
            var bracketEnd = inner.LastIndexOf("]]", StringComparison.Ordinal);
            if (bracketEnd < 0) return false;

            inner = inner.Substring(0, bracketEnd);
            if (string.IsNullOrEmpty(inner))
            {
                typeArgs = Array.Empty<string>();
                return true;
            }

            var args = new List<string>();
            var depth = 0;
            var start = 0;
            for (var i = 0; i < inner.Length; i++)
            {
                if (inner[i] == '[') depth++;
                else if (inner[i] == ']') depth--;
                else if (inner[i] == ',' && depth == 0)
                {
                    args.Add(inner.Substring(start, i - start));
                    start = i + 1;
                }
            }
            if (start < inner.Length)
                args.Add(inner.Substring(start));

            typeArgs = args;
            return args.Count > 0;
        }

        /// <summary>
        /// Extract the delegate Invoke parameter count from the delegate type's subjectId.
        /// For Func`N types: N-1 parameters (one type arg is return type).
        /// For Action`N types: N parameters.
        /// For other delegate types: returns 0 (caller must handle).
        /// </summary>
        private static int ExtractDelegateArity(string delegateTypeSubjectId)
        {
            // SubjectId format: "Assembly/TypeName`N[[...]]"
            var slash = delegateTypeSubjectId.IndexOf('/');
            if (slash < 0) return 0;
            var typeName = delegateTypeSubjectId.Substring(slash + 1);

            // Check for backtick-arity suffix: TypeName`N
            var backtick = typeName.IndexOf('`');
            if (backtick < 0) return 0;

            var arityStr = string.Empty;
            for (int i = backtick + 1; i < typeName.Length; i++)
            {
                if (char.IsDigit(typeName[i]))
                    arityStr += typeName[i];
                else
                    break;
            }
            if (string.IsNullOrEmpty(arityStr) || !int.TryParse(arityStr, out int arity))
                return 0;

            // Extract display name before backtick for Func/Action detection.
            var baseName = typeName.Substring(0, backtick);
            // Get the segment after the last '.' for short name.
            var lastDot = baseName.LastIndexOf('.');
            var shortName = lastDot >= 0 ? baseName.Substring(lastDot + 1) : baseName;

            return shortName switch
            {
                "Func" when arity >= 1 => arity - 1,  // One type arg is return type
                "Action" => arity,                     // All type args are parameters
                _ => 0,  // Custom delegate type — arity unknown from name alone
            };
        }

        /// <summary>Register Convert.ToXxx inline shapes for all numeric value-type overloads.</summary>
        private static void RegisterConvertNumericInline(RuntimeHelperShapeRegistry registry, string methodName, string cppCastType)
        {
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var pt = paramTypes[0];
                    if (pt is "System.Byte" or "System.SByte" or "System.Int16" or "System.UInt16"
                        or "System.Int32" or "System.UInt32" or "System.Int64" or "System.UInt64"
                        or "System.Char" or "System.Single" or "System.Double")
                    {
                        return $"static_cast<{cppCastType}>({{0}})";
                    }
                    if (pt is "System.Boolean" or "System.DateTime")
                    {
                        return $"(chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)), static_cast<{cppCastType}>(0))";
                    }
                    return null;
                }
            ));
        }

        private static void RegisterCryptoStubs(RuntimeHelperShapeRegistry registry)
        {
            // ── SHA family: HashData(byte[]) -> byte[] ────────────────
            RegisterShaStub(registry, "SHA1", "ChaosSha1Hash");
            RegisterShaStub(registry, "SHA256", "ChaosSha256Hash");
            RegisterShaStub(registry, "SHA384", "ChaosSha384Hash");
            RegisterShaStub(registry, "SHA512", "ChaosSha512Hash");
            RegisterShaStub(registry, "SHA3_256", "ChaosSha3_256Hash");
            RegisterShaStub(registry, "SHA3_384", "ChaosSha3_384Hash");
            RegisterShaStub(registry, "SHA3_512", "ChaosSha3_512Hash");

            // ── HMAC family: HashData(byte[], byte[]) -> byte[] ───────
            RegisterHmacStub(registry, "HMACSHA1", "ChaosHmacSha1");
            RegisterHmacStub(registry, "HMACSHA256", "ChaosHmacSha256");
            RegisterHmacStub(registry, "HMACSHA384", "ChaosHmacSha384");
            RegisterHmacStub(registry, "HMACSHA512", "ChaosHmacSha512");
            RegisterHmacStub(registry, "HMACSHA3_256", "ChaosHmacSha3_256");
            RegisterHmacStub(registry, "HMACSHA3_384", "ChaosHmacSha3_384");
            RegisterHmacStub(registry, "HMACSHA3_512", "ChaosHmacSha3_512");

            // ── MD5 family: HashData(byte[]) -> byte[] ────────────────
            RegisterShaStub(registry, "MD5", "ChaosMd5Hash");

            // ── HMACMD5 family: HashData(byte[], byte[]) -> byte[] ────
            RegisterHmacStub(registry, "HMACMD5", "ChaosHmacMd5");

            // ── RNG: GetBytes(int) -> byte[] ──────────────────────────
            registry.Register(
                "RandomNumberGenerator",
                "GetBytes",
                new[] { "System.Int32" },
                ShapeKind.SimpleForward, "ChaosCngGetBytes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // ── RNG: Fill(byte[]) ─────────────────────────────────────
            registry.Register(
                "RandomNumberGenerator",
                "Fill",
                new[] { "System.Byte[]" },
                ShapeKind.SimpleForward, "ChaosCngFill",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // ── BCrypt P/Invoke stubs (Interop+BCrypt methods) ────────────
            // These methods are DllImport declarations in System.Private.CoreLib
            // that the runtime's ChaosExternalRuntimeFallback would otherwise handle
            // via _TryExecuteViaPInvoke.  By registering them as SimpleForward stubs,
            // codegen generates direct native call wrappers that invoke the
            // ChaosBCrypt* functions defined in crypto_stubs.cpp/crypto_stubs.h.
            // This avoids going through ChaosExternalRuntimeFallback at runtime
            // and enables actual BCrypt function execution.
            RegisterBCryptStub(registry, "BCryptOpenAlgorithmProvider",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32"],
                "ChaosBCryptOpenAlgorithmProvider", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptCloseAlgorithmProvider",
                ["System.IntPtr", "System.Int32"],
                "ChaosBCryptCloseAlgorithmProvider", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptCreateHash",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptCreateHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDestroyHash",
                ["System.IntPtr"],
                "ChaosBCryptDestroyHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptHashData",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptHashData", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptFinishHash",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptFinishHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptHash",
                ["System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGenerateSymmetricKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptGenerateSymmetricKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDestroyKey",
                ["System.IntPtr"],
                "ChaosBCryptDestroyKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptEncrypt",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32"],
                "ChaosBCryptEncrypt", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDecrypt",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32"],
                "ChaosBCryptDecrypt", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptImportKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32",
                 "System.Int32"],
                "ChaosBCryptImportKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptExportKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptExportKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGetProperty",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptGetProperty", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptSetProperty",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.Int32", "System.Int32"],
                "ChaosBCryptSetProperty", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGenerateKeyPair",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptGenerateKeyPair", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptFinalizeKeyPair",
                ["System.IntPtr", "System.Int32"],
                "ChaosBCryptFinalizeKeyPair", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptImportKeyPair",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptImportKeyPair", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptSignHash",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptSignHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptVerifySignature",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptVerifySignature", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptSecretAgreement",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32"],
                "ChaosBCryptSecretAgreement", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDestroySecret",
                ["System.IntPtr"],
                "ChaosBCryptDestroySecret", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDeriveKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptDeriveKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptKeyDerivation",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptKeyDerivation", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGenRandom",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptGenRandom", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptIsAvailable", [],
                "ChaosBCryptIsAvailable", CreateInt32AbiSlot());
        }

        private static void RegisterShaStub(RuntimeHelperShapeRegistry registry, string algoName, string nativeFn)
        {
            registry.Register(
                algoName,
                "HashData",
                new[] { "System.Byte[]" },
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
        }

        private static void RegisterHmacStub(RuntimeHelperShapeRegistry registry, string algoName, string nativeFn)
        {
            registry.Register(
                algoName,
                "HashData",
                new[] { "System.Byte[]", "System.Byte[]" },
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Register a BCrypt/NCrypt P/Invoke method as a SimpleForward stub.
        /// The type name is "Interop+BCrypt" for BCrypt methods and "Interop+NCrypt" for NCrypt methods.
        /// Each IntPtr parameter maps to a NativeInt ABI slot, and each Int32 maps to an Int32 ABI slot.
        /// The return type is always Int32 (NTSTATUS) for BCrypt API functions.
        /// </summary>
        private static void RegisterBCryptStub(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string[] paramTypeDisplayNames,
            string nativeFn,
            AotCoreIrAbiSlotArtifact returnAbi)
        {
            var typeName = methodName.StartsWith("NCrypt", StringComparison.Ordinal)
                ? "Interop+NCrypt"
                : "Interop+BCrypt";

            var abiSlots = new AotCoreIrAbiSlotArtifact[paramTypeDisplayNames.Length];
            var rawIndices = new HashSet<int>();
            for (int i = 0; i < paramTypeDisplayNames.Length; i++)
            {
                rawIndices.Add(i);
                abiSlots[i] = paramTypeDisplayNames[i] switch
                {
                    "System.Int32" => CreateInt32AbiSlot(),
                    _ => CreateNativeIntAbiSlot(),  // IntPtr -> NativeInt
                };
            }

            registry.Register(
                typeName,
                methodName,
                paramTypeDisplayNames,
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots),
                returnAbi,
                rawIndices);
        }

    }
}
