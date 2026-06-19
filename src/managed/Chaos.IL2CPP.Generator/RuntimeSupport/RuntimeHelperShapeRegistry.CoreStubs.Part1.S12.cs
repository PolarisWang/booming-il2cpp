using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Thread::Interrupt
        /// </summary>
        private static void RegisterThreadInterrupt(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "Interrupt", [],
                ShapeKind.SimpleForward, "chaos_thread_interrupt",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Thread::get_IsBackground
        /// </summary>
        private static void RegisterThreadgetIsBackground(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "get_IsBackground", [],
                ShapeKind.SimpleForward, "chaos_thread_is_background",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Thread::set_IsBackground
        /// </summary>
        private static void RegisterThreadsetIsBackground(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// Thread::get_Priority
        /// </summary>
        private static void RegisterThreadgetPriority(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "get_Priority", [],
                ShapeKind.SimpleForward, "chaos_thread_get_priority",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Thread::set_Priority
        /// </summary>
        private static void RegisterThreadsetPriority(RuntimeHelperShapeRegistry registry)
        {
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

        }

    }
}
