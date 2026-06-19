using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// GC.EndNoGCRegion — returns Int32 (0=Success, 1=GCTriggered)
        /// </summary>
        private static void RegisterGCEndNoGCRegionreturns(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.GC", "EndNoGCRegion", [],
                ShapeKind.SimpleForward, "chaos_gc_end_no_gc_region",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Full GC Notification
        /// </summary>
        private static void RegisterFullGCNotification(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// GCMemoryInfo
        /// </summary>
        private static void RegisterGCMemoryInfo(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// Delegate
        /// </summary>
        private static void RegisterDelegate(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// Object
        /// </summary>
        private static void RegisterObject(RuntimeHelperShapeRegistry registry)
        {
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


        }

    }
}
