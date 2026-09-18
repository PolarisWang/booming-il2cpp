using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Interlocked::Increment
        /// </summary>
        private static void RegisterInterlockedIncrement(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "Increment", ["System.Int64&"],
                ShapeKind.SimpleForward, "ChaosInterlockedIncrementInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // Int32 / UInt32 / UInt64 overloads.  Registering only the Int64
            // form left these callees matching no shape, so they lowered to the
            // zero-argument external-runtime catch-all and returned 0 —
            // `Assert.AreEqual(1, Interlocked.Increment(ref x))` then failed
            // with a wrong value.  The callee shape is the same `T(T&)` the
            // Int64 entry uses; only the carrier width differs.
            RegisterInterlockedUnaryArithmetic(registry, "Increment",
                "System.Int32&", "ChaosInterlockedIncrementInt32", AotCoreIrAbiCarrierKind.Int32);
            RegisterInterlockedUnaryArithmetic(registry, "Increment",
                "System.UInt32&", "ChaosInterlockedIncrementUInt32", AotCoreIrAbiCarrierKind.UInt32);
            RegisterInterlockedUnaryArithmetic(registry, "Increment",
                "System.UInt64&", "ChaosInterlockedIncrementUInt64", AotCoreIrAbiCarrierKind.UInt64);
        }

        /// <summary>
        /// Interlocked::Decrement
        /// </summary>
        private static void RegisterInterlockedDecrement(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "Decrement", ["System.Int64&"],
                ShapeKind.SimpleForward, "ChaosInterlockedDecrementInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // See RegisterInterlockedIncrement — same int-width gap, Decrement.
            RegisterInterlockedUnaryArithmetic(registry, "Decrement",
                "System.Int32&", "ChaosInterlockedDecrementInt32", AotCoreIrAbiCarrierKind.Int32);
            RegisterInterlockedUnaryArithmetic(registry, "Decrement",
                "System.UInt32&", "ChaosInterlockedDecrementUInt32", AotCoreIrAbiCarrierKind.UInt32);
            RegisterInterlockedUnaryArithmetic(registry, "Decrement",
                "System.UInt64&", "ChaosInterlockedDecrementUInt64", AotCoreIrAbiCarrierKind.UInt64);
        }

        /// <summary>
        /// `Interlocked.&lt;op&gt;(ref T)` for a non-Int64 carrier — one by-ref
        /// parameter in, one value out, both the same width.
        ///
        /// Kept as a helper rather than six hand-written blocks because the
        /// registered entries must stay shape-identical apart from the carrier:
        /// a copy-paste slip here silently re-creates the very bug this fixes
        /// (a callee that matches no shape and falls to the catch-all), and
        /// that failure is invisible in the registry — it only shows up as a
        /// wrong answer at runtime.
        /// </summary>
        private static void RegisterInterlockedUnaryArithmetic(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string paramType,
            string nativeSymbol,
            AotCoreIrAbiCarrierKind carrier)
        {
            registry.Register("Interlocked", methodName, [paramType],
                ShapeKind.SimpleForward, nativeSymbol,
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = carrier,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });
        }

        /// <summary>
        /// Interlocked::Exchange
        /// </summary>
        private static void RegisterInterlockedExchange(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "Exchange", ["System.Int64&", "System.Int64"],
                ShapeKind.SimpleForward, "ChaosInterlockedExchangeInt64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Interlocked::CompareExchange (Int32)
        /// </summary>
        private static void RegisterInterlockedCompareExchange(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "CompareExchange", ["System.Int32&", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosInterlockedCompareExchangeInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Interlocked::CompareExchange (Int64)
        /// </summary>
        private static void RegisterInterlockedCompareExchange_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "CompareExchange", ["System.Int64&", "System.Int64", "System.Int64"],
                ShapeKind.SimpleForward, "ChaosInterlockedCompareExchangeInt64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });
        }

    }
}
