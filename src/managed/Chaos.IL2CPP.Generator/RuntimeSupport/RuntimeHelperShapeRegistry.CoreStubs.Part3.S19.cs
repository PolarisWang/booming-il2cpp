using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Math::Cos (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemMathCos(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "Cos", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosMathCos",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Char::IsDigit (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemCharIsDigit(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Char", "IsDigit", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosCharIsDigit",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Char::IsLetter (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemCharIsLetter(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Char", "IsLetter", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosCharIsLetter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Char::IsWhiteSpace (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemCharIsWhiteSpace(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Char", "IsWhiteSpace", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosCharIsWhiteSpace",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.TimeSpan::.ctor (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTimeSpanctor(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.TimeSpan", ".ctor", ["System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosTimeSpanCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.TimeSpan", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

        }

    }
}
