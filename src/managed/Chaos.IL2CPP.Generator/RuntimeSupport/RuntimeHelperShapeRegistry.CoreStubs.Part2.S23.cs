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
