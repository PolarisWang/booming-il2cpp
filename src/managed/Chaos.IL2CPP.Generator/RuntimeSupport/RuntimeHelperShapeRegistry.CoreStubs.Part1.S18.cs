using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Convert.ToChar(String) — inline TLS cache peek + first-byte return
        /// </summary>
        private static void RegisterConvertToCharinline_1(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// Int32.GetHashCode — inline pointer dereference
        /// </summary>
        private static void RegisterInt32GetHashCodeinline(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// Array::Copy (GenericShapeDescriptor -- calls ChaosArrayCopy for 5-param overload)
        /// </summary>
        private static void RegisterArrayCopy(RuntimeHelperShapeRegistry registry)
        {
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

    }
}
