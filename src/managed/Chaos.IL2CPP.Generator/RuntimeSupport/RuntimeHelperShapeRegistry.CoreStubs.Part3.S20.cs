using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.TimeSpan::FromHours (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTimeSpanFromHours(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.TimeSpan", "FromHours", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosTimeSpanFromHours",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.TimeSpan::FromMinutes (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTimeSpanFromMinutes(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.TimeSpan", "FromMinutes", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosTimeSpanFromMinutes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.DateTime::.ctor (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemDateTimector(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", ".ctor", ["System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosDateTimeCtor3",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.DateTime", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.DateTime", ".ctor", ["System.Int32", "System.Int32", "System.Int32", "System.Int32", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosDateTimeCtor6",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[7]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.DateTime", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3, 4, 5, 6 });

        }

        /// <summary>
        /// System.DateTime::Parse (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemDateTimeParse(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosDateTimeParse",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.TimeSpan::Parse (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTimeSpanParse(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.TimeSpan", "Parse", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTimeSpanParse",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.TimeSpan" },
                new HashSet<int> { 0 });

        }

    }
}
