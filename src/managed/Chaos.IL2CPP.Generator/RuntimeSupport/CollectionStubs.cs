using System;
using System.Collections.Generic;
using System.Linq;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>Register collection stub GenericShapeDescriptors.</summary>
    private static void RegisterCollectionStubs(RuntimeHelperShapeRegistry registry)
    {
        RegisterDictionaryStubs(registry);
        RegisterHashSetStubs(registry);
        RegisterQueueStubs(registry);
        RegisterStackStubs(registry);
    }

    private static void RegisterDictionaryStubs(RuntimeHelperShapeRegistry registry)
    {
        // === Dictionary<K,V>::TryGetValue ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
            MethodName: "TryGetValue",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR* chaos_arg_2",
                [
                    "    return CollectionDictionaryTryGetValue(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1, 2 });
            }));

        // === Dictionary<K,V>::get_Item ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
            MethodName: "get_Item",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    return CollectionDictionaryGetItem(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateNativeIntAbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Dictionary<K,V>::ContainsKey ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
            MethodName: "ContainsKey",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    return CollectionDictionaryContainsKey(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Dictionary<K,V>::Add ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
            MethodName: "Add",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                [
                    "    CollectionDictionaryAdd(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateVoidAbiSlot(),
                    new HashSet<int> { 0, 1, 2 });
            }));

        // === Dictionary<K,V>::Remove ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
            MethodName: "Remove",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    return CollectionDictionaryRemove(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Dictionary<K,V>::get_Count ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
            MethodName: "get_Count",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionDictionaryGetCount(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0 });
            }));
    }

    private static void RegisterHashSetStubs(RuntimeHelperShapeRegistry registry)
    {
        // === HashSet<T>::Contains ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
            MethodName: "Contains",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    return CollectionHashSetContains(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === HashSet<T>::Remove ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
            MethodName: "Remove",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    return CollectionHashSetRemove(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === HashSet<T>::Add ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
            MethodName: "Add",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    return CollectionHashSetAdd(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === HashSet<T>::get_Count ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.HashSet",
            MethodName: "get_Count",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionHashSetGetCount(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0 });
            }));
    }

    private static void RegisterQueueStubs(RuntimeHelperShapeRegistry registry)
    {
        // === Queue<T>::get_Count ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Queue",
            MethodName: "get_Count",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionQueueGetCount(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0 });
            }));

        // === Queue<T>::Enqueue ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Queue",
            MethodName: "Enqueue",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    CollectionQueueEnqueue(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateVoidAbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Queue<T>::Dequeue ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Queue",
            MethodName: "Dequeue",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionQueueDequeue(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateNativeIntAbiSlot(),
                    new HashSet<int> { 0 });
            }));

        // === Queue<T>::Peek ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Queue",
            MethodName: "Peek",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionQueuePeek(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateNativeIntAbiSlot(),
                    new HashSet<int> { 0 });
            }));

        // === Queue<T>::TryDequeue ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Queue",
            MethodName: "TryDequeue",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR* chaos_arg_1",
                [
                    "    return CollectionQueueTryDequeue(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Queue<T>::TryPeek ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Queue",
            MethodName: "TryPeek",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR* chaos_arg_1",
                [
                    "    return CollectionQueueTryPeek(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Queue<T>::Clear ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Queue",
            MethodName: "Clear",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    CollectionQueueClear(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateVoidAbiSlot(),
                    new HashSet<int> { 0 });
            }));
    }

    private static void RegisterStackStubs(RuntimeHelperShapeRegistry registry)
    {
        // === Stack<T>::get_Count ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Stack",
            MethodName: "get_Count",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionStackGetCount(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0 });
            }));

        // === Stack<T>::Push ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Stack",
            MethodName: "Push",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                [
                    "    CollectionStackPush(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateVoidAbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Stack<T>::Pop ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Stack",
            MethodName: "Pop",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionStackPop(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateNativeIntAbiSlot(),
                    new HashSet<int> { 0 });
            }));

        // === Stack<T>::Peek ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Stack",
            MethodName: "Peek",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    return CollectionStackPeek(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateNativeIntAbiSlot(),
                    new HashSet<int> { 0 });
            }));

        // === Stack<T>::TryPop ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Stack",
            MethodName: "TryPop",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR* chaos_arg_1",
                [
                    "    return CollectionStackTryPop(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Stack<T>::TryPeek ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Stack",
            MethodName: "TryPeek",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR* chaos_arg_1",
                [
                    "    return CollectionStackTryPeek(chaos_arg_0, chaos_arg_1);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    }),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1 });
            }));

        // === Stack<T>::Clear ===
        registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
            TypeDisplayNamePrefix: "System.Collections.Generic.Stack",
            MethodName: "Clear",
            Resolver: static (planner, callee, typeArgs) =>
            {
                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                [
                    "    CollectionStackClear(chaos_arg_0);",
                ]);
                return new RuntimeHelperShapeRegistry.GenericShapeResolution(src, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                    CreateVoidAbiSlot(),
                    new HashSet<int> { 0 });
            }));
    }
}
