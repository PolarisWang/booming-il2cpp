using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Type::GetElementType (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeGetElementType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetElementType", [],
                ShapeKind.SimpleForward, "ChaosTypeGetElementType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::GetEvents (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeGetEvents(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetEvents", [],
                ShapeKind.SimpleForward, "ChaosTypeGetEvents",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::GetInterfaces (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeGetInterfaces(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetInterfaces", [],
                ShapeKind.SimpleForward, "ChaosTypeGetInterfaces",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::GetNestedTypes (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeGetNestedTypes(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetNestedTypes", [],
                ShapeKind.SimpleForward, "ChaosTypeGetNestedTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Type::MakePointerType (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeMakePointerType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "MakePointerType", [],
                ShapeKind.SimpleForward, "ChaosTypeMakePointerType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}
