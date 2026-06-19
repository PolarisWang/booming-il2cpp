using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Globalization.CultureInfo::get_TextInfo (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemGlobalizationCultureInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CultureInfo", "get_TextInfo", [],
                ShapeKind.SimpleForward, "ChaosCultureGetTextInfo",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Globalization.TextInfo::ToLower (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemGlobalizationTextInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.TextInfo", "ToLower", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosTextInfoToLower",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Globalization.TextInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// System.Globalization.TextInfo::ToUpper (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemGlobalizationTextInfo_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.TextInfo", "ToUpper", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosTextInfoToUpper",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Globalization.TextInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// System.Globalization.TextInfo::get_CultureName (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemGlobalizationTextInfo_2(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.TextInfo", "get_CultureName", [],
                ShapeKind.SimpleForward, "ChaosTextInfoGetCultureName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Globalization.TextInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Math::BigMul (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemMathBigMul(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Math", "BigMul", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMathBigMul",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

        }

    }
}
