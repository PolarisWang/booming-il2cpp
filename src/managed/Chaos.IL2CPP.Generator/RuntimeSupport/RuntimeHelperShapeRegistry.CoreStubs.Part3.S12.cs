using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Type::get_IsNestedPrivate (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeget_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsNestedPrivate", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNestedPrivate",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::get_IsNestedPublic (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeget_2(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsNestedPublic", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNestedPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::get_IsNotPublic (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeget_3(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsNotPublic", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNotPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::get_IsPublic (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeget_4(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsPublic", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsPublic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::get_UnderlyingSystemType (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeget_5(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_UnderlyingSystemType", [],
                ShapeKind.SimpleForward, "ChaosTypeGetUnderlyingSystemType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}
