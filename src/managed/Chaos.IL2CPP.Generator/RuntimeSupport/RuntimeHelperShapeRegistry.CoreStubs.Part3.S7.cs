using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// String::Contains
        /// </summary>
        private static void RegisterStringContains(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.String", "Contains", ["System.String"],
                ShapeKind.SimpleForward, "ChaosStringContains",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// DateTime::Compare (SimpleForward stub)
        /// </summary>
        private static void RegisterDateTimeCompare(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", "Compare", ["System.DateTime", "System.DateTime"],
                ShapeKind.SimpleForward, "ChaosDateTimeCompare",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Type::GetFields
        /// </summary>
        private static void RegisterTypeGetFields(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetFields", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetFields",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::GetFields (BindingFlags overload)
        /// </summary>
        private static void RegisterTypeGetFields_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetFields", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetFieldsBindingflags",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::GetProperties (BindingFlags overload)
        /// </summary>
        private static void RegisterTypeGetProperties(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetProperties", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetPropertiesBindingflags",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}
