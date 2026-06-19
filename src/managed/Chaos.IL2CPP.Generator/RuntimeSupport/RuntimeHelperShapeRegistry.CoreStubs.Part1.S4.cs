using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Object.GetHashCode (SimpleForward)
        /// </summary>
        private static void RegisterObjectGetHashCode(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Object", "GetHashCode", [],
                ShapeKind.SimpleForward, "ChaosObjectGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Object.ToString (SimpleForward)
        /// </summary>
        private static void RegisterObjectToString(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Object", "ToString", [],
                ShapeKind.SimpleForward, "ChaosObjectToString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Object.GetType (SimpleForward)
        /// </summary>
        private static void RegisterObjectGetType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Object", "GetType", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosObjectGetType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Object.MemberwiseClone (SimpleForward)
        /// </summary>
        private static void RegisterObjectMemberwiseClone(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Object", "MemberwiseClone", [],
                ShapeKind.SimpleForward, "ChaosObjectMemberwiseClone",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Object::Equals static (two-arg overload)
        /// </summary>
        private static void RegisterObjectEqualsstatic(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Object", "Equals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "ChaosObjectEqualsStatic",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

    }
}
