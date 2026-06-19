using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// GCLatencyMode property getter/setter
        /// </summary>
        private static void RegisterGCLatencyModepropertygetter(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// GC.CollectionCount(int) — returns collection count per generation
        /// </summary>
        private static void RegisterGCCollectionCountreturns(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.GC", "CollectionCount", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_gc_get_collection_count",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// GC.GetTotalPauseDuration — returns total accumulated pause (ns) as Int64
        /// </summary>
        private static void RegisterGCGetTotalPauseDurationreturns(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.GC", "GetTotalPauseDuration", [],
                ShapeKind.SimpleForward, "chaos_gc_get_total_pause_duration",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// GC.GetAllocatedBytesForCurrentThread — returns per-thread allocated bytes (Int64)
        /// </summary>
        private static void RegisterGCGetAllocatedBytesForCurrentThreadreturns(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.GC", "GetAllocatedBytesForCurrentThread", [],
                ShapeKind.SimpleForward, "chaos_gc_get_allocated_bytes_for_current_thread",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// GC.TryStartNoGCRegion — two overloads
        /// </summary>
        private static void RegisterGCTryStartNoGCRegiontwo(RuntimeHelperShapeRegistry registry)
        {
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

        }

    }
}
