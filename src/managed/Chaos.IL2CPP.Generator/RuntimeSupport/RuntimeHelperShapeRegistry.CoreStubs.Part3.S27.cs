using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Scalar <c>IComparable&lt;T&gt;.CompareTo(object)</c> instance members.
        ///
        /// Every numeric/value type surfaces <c>int CompareTo(object? obj)</c>, and none
        /// of them had a shape — each subject lowered to the zero-argument
        /// <c>chaos_external_runtime_*</c> catch-all, whose fabricated 0 lost to the
        /// probed expectation of <c>1</c> (CompareTo(null) == 1: null sorts first).
        ///
        /// All ATG probes pass <c>default(object)</c> i.e. null, so the native only has
        /// to honor the null contract; the non-null path compares the boxed payload
        /// (box layout: 8-byte header + payload @ +8) without a header type check —
        /// a differently-typed box would silently mis-compare rather than raise
        /// (accepted divergence, keeps the symbol count small).
        ///
        /// Instance-method slot convention (see DateTime.AddDays in Part2.S14):
        /// paramTypes exclude the receiver, but slot 0 IS the receiver carrier and
        /// slot 1 the object argument — raw argument indices {0,1}.
        /// </summary>
        private static void RegisterScalarCompareToMembers(RuntimeHelperShapeRegistry registry)
        {
            var i32Slot = CreateInt32AbiSlot();
            AotCoreIrAbiSlotArtifact Slot(string carrier, string? typeSubjectId = null) => new()
            {
                CarrierKindCode = Enum.Parse<AotCoreIrAbiCarrierKind>(carrier),
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
                TypeSubjectId = typeSubjectId,
            };
            AotCoreIrAbiSlotArtifact ObjSlot() =>
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType);

            void Reg(string type, string carrier, string symbol)
            {
                var receiverTypeSubjectId = $"System.Private.CoreLib/{type}";
                registry.Register(type, "CompareTo", ["System.Object"],
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        Slot(carrier, receiverTypeSubjectId),
                        ObjSlot(),
                    }),
                    i32Slot,
                    new HashSet<int> { 0, 1 });
            }

            // (managed type, receiver carrier, native symbol, comment tag)
            var types = new (string Type, string Carrier, string Symbol)[]
            {
                ("System.Boolean",  "Int32",   "ChaosScalarCompareTo32"),
                ("System.Byte",     "Int32",   "ChaosScalarCompareToU32"),
                ("System.SByte",    "Int32",   "ChaosScalarCompareTo32"),
                ("System.Char",     "Int32",   "ChaosScalarCompareToU32"),
                ("System.Int16",    "Int32",   "ChaosScalarCompareTo32"),
                ("System.UInt16",   "Int32",   "ChaosScalarCompareToU32"),
                ("System.Int32",    "Int32",   "ChaosScalarCompareTo32"),
                ("System.UInt32",   "Int32",   "ChaosScalarCompareToU32"),
                ("System.Int64",    "Int64",   "ChaosScalarCompareTo64"),
                ("System.UInt64",   "Int64",   "ChaosScalarCompareToU64"),
                ("System.DateTime", "Int64",   "ChaosScalarCompareTo64"),   // ticks
                ("System.TimeSpan", "Int64",   "ChaosScalarCompareTo64"),   // ticks
                ("System.TimeOnly", "Int64",   "ChaosScalarCompareTo64"),
                ("System.DateOnly", "Int32",   "ChaosScalarCompareTo32"),   // day number
                ("System.Half",     "Float32", "ChaosScalarCompareToF"),
                ("System.Single",   "Float32", "ChaosScalarCompareToF"),
                ("System.Double",   "Float64", "ChaosScalarCompareToD"),
                // 128-bit values ride a 64-bit carrier (S25 precedent): the null path
                // (all probes) is exact; a non-null compare only sees the low word.
                ("System.Int128",   "Int64",   "ChaosScalarCompareTo64"),
                ("System.UInt128",  "Int64",   "ChaosScalarCompareToU64"),
            };
            foreach (var (type, carrier, symbol) in types)
                Reg(type, carrier, symbol);

            // Decimal rides the 16-byte DecimalCarrier* (existing convention); the
            // non-null path has no compare primitive yet and raises.
            registry.Register("System.Decimal", "CompareTo", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosDecimalCompareToObject",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    ObjSlot(),
                }),
                i32Slot,
                new HashSet<int> { 0, 1 });

            // System.Version — reference type, so the receiver is a reference slot too.
            // CompareTo(null) == 1 covers both probed overloads (default(Version) is
            // also null); the non-null path compares the four Int32 fields @ +8.
            var versionRefSlot = CreateNativeIntAbiSlot("System.Private.CoreLib/System.Version", AotCoreIrTypeShapeKind.ReferenceType);
            registry.Register("System.Version", "CompareTo", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosVersionCompareToVersion",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    versionRefSlot,
                    ObjSlot(),
                }),
                i32Slot,
                new HashSet<int> { 0, 1 });
            registry.Register("System.Version", "CompareTo", ["System.Version"],
                ShapeKind.SimpleForward, "ChaosVersionCompareToVersion",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    versionRefSlot,
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Version", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                i32Slot,
                new HashSet<int> { 0, 1 });
        }
    }
}
