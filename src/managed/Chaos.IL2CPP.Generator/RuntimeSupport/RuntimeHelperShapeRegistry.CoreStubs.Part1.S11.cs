using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Monitor
        /// </summary>
        private static void RegisterMonitor(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// Thread
        /// </summary>
        private static void RegisterThread(RuntimeHelperShapeRegistry registry)
        {
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


        }

        /// <summary>
        /// Thread::Sleep
        /// </summary>
        private static void RegisterThreadSleep(RuntimeHelperShapeRegistry registry)
        {
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

        }

        /// <summary>
        /// Thread::Yield
        /// </summary>
        private static void RegisterThreadYield(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "Yield", [],
                ShapeKind.SimpleForward, "chaos_thread_yield",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Thread::GetDomainID
        /// </summary>
        /// <remarks>
        /// Without this registration the callee matched no shape and lowered to
        /// the zero-argument external-runtime catch-all, which returns 0 — and
        /// `Assert.AreEqual(1, Thread.GetDomainID())` then failed.  .NET Core has
        /// a single AppDomain, so 1 is the real value, not a placeholder.
        /// </remarks>
        private static void RegisterThreadGetDomainID(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "GetDomainID", [],
                ShapeKind.SimpleForward, "chaos_thread_get_domain_id",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Thread::GetCurrentProcessorId
        /// </summary>
        /// <remarks>
        /// Without this registration the callee matched no shape and lowered to
        /// the zero-argument external-runtime catch-all, which returns 0 — so the
        /// method was never actually implemented.
        ///
        /// The subject's expectation cannot be satisfied by any fixed value: the
        /// ATG probe recorded ITS OWN host's reading (`Assert.AreEqual(6, …)` on a
        /// 6-logical-CPU probe box), while GetCurrentProcessorId is a per-call
        /// property of the calling thread.  The expectation is therefore
        /// env-sensitive and is bucketed as such by the fact layer
        /// (`_get_env_sensitive_subject_ids` in stages/fact_chunk.py) — the same
        /// treatment Thread.Yield gets (see c013be67c).  Registering the shape
        /// makes the method genuinely implemented and observable rather than a
        /// silent 0 from the catch-all.
        /// </remarks>
        private static void RegisterThreadGetCurrentProcessorId(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "GetCurrentProcessorId", [],
                ShapeKind.SimpleForward, "chaos_thread_get_current_processor_id",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Thread::VolatileRead (reference-typed overload)
        /// </summary>
        /// <remarks>
        /// Registered against `System.Object&` and routed to ChaosVolatileReadObject,
        /// NOT to the Int32 ChaosVolatileRead used by System.Volatile::Read — that
        /// one returns INT32 and would truncate a 64-bit object reference to its
        /// low 32 bits, producing a corrupt reference rather than a clean failure.
        /// </remarks>
        private static void RegisterThreadVolatileReadObject(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "VolatileRead", ["System.Object&"],
                ShapeKind.SimpleForward, "ChaosVolatileReadObject",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Thread::Abort
        /// </summary>
        private static void RegisterThreadAbort(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "Abort", ["System.Object"],
                ShapeKind.SimpleForward, "chaos_thread_abort",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

    }
}
