using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Reflection.FieldInfo::get_FieldHandle (SimpleForward stub returning 0)
        /// </summary>
        private static void RegisterSystemReflectionFieldInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.FieldInfo", "get_FieldHandle", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetFieldHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Reflection.FieldInfo::get_IsLiteral (SimpleForward stub returning false/0)
        /// </summary>
        private static void RegisterSystemReflectionFieldInfo_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.FieldInfo", "get_IsLiteral", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetIsLiteral",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Reflection.FieldInfo::get_IsStatic (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemReflectionFieldInfo_2(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.FieldInfo", "get_IsStatic", [],
                ShapeKind.SimpleForward, "ChaosReflectionFieldGetIsStatic",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Reflection.PropertyInfo::GetIndexParameters (SimpleForward stub returning null)
        /// </summary>
        private static void RegisterSystemReflectionPropertyInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.PropertyInfo", "GetIndexParameters", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetIndexParameters",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Reflection.PropertyInfo::get_CanRead (SimpleForward stub returning false/0)
        /// </summary>
        private static void RegisterSystemReflectionPropertyInfo_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.PropertyInfo", "get_CanRead", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetCanRead",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

    }
}
