using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// RuntimeHelpers::Equals
        /// </summary>
        private static void RegisterRuntimeHelpersEquals(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "Equals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "ChaosRuntimeHelpersEquals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// String::Substring (GenericShapeDescriptor -- calls ChaosStringSubstring for 3-param overload)
        /// </summary>
        private static void RegisterStringSubstring(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Substring",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcSubstr = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                        [
                            "    return ChaosStringSubstring(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                        ]);
                        return new GenericShapeResolution(srcSubstr, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1, 2 });
                    }
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            EmptyRawArgumentIndices);
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcFallback = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(srcFallback, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// List<T>::Clear — InlineShapeDescriptor (no function call)
        /// </summary>
        private static void RegisterListTClear(RuntimeHelperShapeRegistry registry)
        {
            // Generates direct field assignment at call site, matching JIT inlining.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Clear",
                Resolver: static (callee, paramTypes) =>
                {
                    // Skip paramTypes check for diagnostic — accept any param count
                    // Comma expression: size=0, version++, result ignored (void return)
                    return "(reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8)->size = 0, reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8)->version++)";
                })
            { IsInstanceMethod = true });

        }

        /// <summary>
        /// List<T>::Contains — InlineShapeDescriptor (no function call)
        /// </summary>
        private static void RegisterListTContains(RuntimeHelperShapeRegistry registry)
        {
            // IILE lambda performs linear scan on the inline field buffer at call site.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Contains",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    return "([&]() -> CHAOS_IL2CPP_INT32 { auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8); auto* _elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(_list->items_array) + sizeof(CHAOS_IL2CPP_INT32)); for (CHAOS_IL2CPP_INT32 _i = 0; _i < _list->size; _i++) { if (_elems[_i] == ({1})) return 1; } return 0; })()";
                })
            { IsInstanceMethod = true });

        }

        /// <summary>
        /// List<T>::IndexOf — InlineShapeDescriptor (no function call)
        /// </summary>
        private static void RegisterListTIndexOf(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "IndexOf",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    return "([&]() -> CHAOS_IL2CPP_INT32 { auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8); auto* _elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(_list->items_array) + sizeof(CHAOS_IL2CPP_INT32)); for (CHAOS_IL2CPP_INT32 _i = 0; _i < _list->size; _i++) { if (_elems[_i] == ({1})) return _i; } return -1; })()";
                })
            { IsInstanceMethod = true });

        }

    }
}
