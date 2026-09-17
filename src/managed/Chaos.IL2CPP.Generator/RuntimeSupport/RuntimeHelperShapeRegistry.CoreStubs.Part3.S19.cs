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
        /// System.MathF (Float32 carriers) + System.BitConverter bit reinterpretation.
        ///
        /// MathF::Cos/Pow were never registered under the System.MathF prefix (only the
        /// System.Math Double entries existed), and the MathF/parsing probes finish with
        /// BitConverter.SingleToInt32Bits/DoubleToInt64Bits, which had no shape either —
        /// so the tail of every MathF probe called the zero-argument catch-all and the
        /// assertion compared a fabricated 0.  The natives are trivial memcpy
        /// reinterpretations; the slot machinery does the float load/store (same pattern
        /// as RegisterScalarINumberMembers in Part3.S25).
        /// </summary>
        private static void RegisterMathFAndBitConverterBits(RuntimeHelperShapeRegistry registry)
        {
            AotCoreIrAbiSlotArtifact Slot(string carrier) => new()
            {
                CarrierKindCode = Enum.Parse<AotCoreIrAbiCarrierKind>(carrier),
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            };

            registry.Register("System.MathF", "Cos", ["System.Single"],
                ShapeKind.SimpleForward, "ChaosMathCosF",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(Slot("Float32")),
                Slot("Float32"),
                new HashSet<int> { 0 });
            registry.Register("System.MathF", "Pow", ["System.Single", "System.Single"],
                ShapeKind.SimpleForward, "ChaosMathPowF",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                { Slot("Float32"), Slot("Float32") }),
                Slot("Float32"),
                new HashSet<int> { 0, 1 });

            registry.Register("System.BitConverter", "SingleToInt32Bits", ["System.Single"],
                ShapeKind.SimpleForward, "ChaosSingleToInt32Bits",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(Slot("Float32")),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });
            registry.Register("System.BitConverter", "DoubleToInt64Bits", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosDoubleToInt64Bits",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(Slot("Float64")),
                CreateInt64AbiSlot(),
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
