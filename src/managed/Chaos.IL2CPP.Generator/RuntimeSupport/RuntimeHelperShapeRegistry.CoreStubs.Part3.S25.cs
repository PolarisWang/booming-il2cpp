using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Scalar INumber&lt;T&gt; predicates and BitOperations static members.
        ///
        /// These are static-abstract interface members surfaced as static methods
        /// on each numeric type (<c>System.Double.IsFinite</c>,
        /// <c>System.UInt32.IsEvenInteger</c>, <c>System.Byte.LeadingZeroCount</c>, …).
        /// The registry only carried the <c>Vector128</c>/<c>Vector256</c> variants
        /// (see <c>RegisterVectorUnaryOp</c> in Part2.S3) and the
        /// <c>System.Numerics.BitOperations</c> static class — never the scalar
        /// numeric types.  Every such subject therefore lowered to the
        /// zero-argument <c>chaos_external_runtime_*</c> catch-all, which has no
        /// operand access and returns 0, so the assertion compared a fabricated 0
        /// against <c>true</c>.
        ///
        /// The matrix is type × predicate; the native side is generic in width
        /// (see ChaosScalarIs* in math_stubs.cpp), so the registration enumerates
        /// the managed types while reusing a small set of symbols.
        /// </summary>
        private static void RegisterScalarINumberMembers(RuntimeHelperShapeRegistry registry)
        {
            var i32Slot = CreateInt32AbiSlot();
            AotCoreIrAbiSlotArtifact Slot(string carrier) => new()
            {
                CarrierKindCode = Enum.Parse<AotCoreIrAbiCarrierKind>(carrier),
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            };

            // Managed type -> (native loader/symbol suffix, carrier kind).
            // The width selects which ChaosScalarIs* / ChaosBitOps* symbol the
            // shape forwards to; D/F are the float widths.
            var intTypes = new (string Type, string Suffix, string Carrier)[]
            {
                ("System.SByte",   "32", "Int32"),
                ("System.Byte",    "32", "Int32"),
                ("System.Int16",   "32", "Int32"),
                ("System.UInt16",  "32", "Int32"),
                ("System.Int32",   "32", "Int32"),
                ("System.UInt32",  "32", "UInt32"),
                ("System.Int64",   "64", "Int64"),
                ("System.UInt64",  "64", "UInt64"),
                ("System.Char",    "32", "Int32"),
                ("System.Int128",  "64", "Int64"),
                ("System.UInt128", "64", "UInt64"),
                ("System.IntPtr",  "64", "Int64"),
                ("System.UIntPtr", "64", "UInt64"),
            };
            var floatTypes = new (string Type, string Suffix, string Carrier)[]
            {
                ("System.Half",   "F", "Float32"),
                ("System.Single", "F", "Float32"),
                ("System.Double", "D", "Float64"),
            };

            void Predicate(string type, string carrier, string method, string symbol)
                => registry.Register(type, method, [type],
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(Slot(carrier)),
                    i32Slot,
                    new HashSet<int> { 0 });

            // — Integer widths: integer semantics only —
            foreach (var (type, suffix, carrier) in intTypes)
            {
                // LeadingZeroCount/TrailingZeroCount/PopCount are INumber<T>
                // members: they count within T, not within the machine word.
                // Byte.LeadingZeroCount(0) is 8, so the narrow widths need their
                // own entry points rather than the 32-bit intrinsic.
                string bw = suffix == "32" && (type is "System.Byte" or "System.SByte") ? "8"
                          : suffix == "32" && (type is "System.Int16" or "System.UInt16" or "System.Char") ? "16"
                          : suffix;
                registry.Register(type, "LeadingZeroCount", [type],
                    ShapeKind.SimpleForward, $"ChaosBitOpsLeadingZeroCount{bw}",
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(Slot(carrier)),
                    i32Slot,
                    new HashSet<int> { 0 });
                registry.Register(type, "TrailingZeroCount", [type],
                    ShapeKind.SimpleForward, $"ChaosBitOpsTrailingZeroCount{bw}",
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(Slot(carrier)),
                    i32Slot,
                    new HashSet<int> { 0 });
                registry.Register(type, "PopCount", [type],
                    ShapeKind.SimpleForward, $"ChaosBitOpsPopCount{bw}",
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(Slot(carrier)),
                    i32Slot,
                    new HashSet<int> { 0 });

                // Integer predicates.  Unsigned types need their own
                // IsPositive/IsNegative entry points: per .NET an unsigned value
                // is positive iff non-zero and never negative, whereas the signed
                // template would misread values >= 2^31.
                bool unsigned = carrier is "UInt32" or "UInt64";
                string w = suffix;                       // "32" or "64"

                Predicate(type, carrier, "IsEvenInteger", $"ChaosScalarIsEvenInteger{w}");
                Predicate(type, carrier, "IsOddInteger", $"ChaosScalarIsOddInteger{w}");
                Predicate(type, carrier, "IsPositive",
                    unsigned ? $"ChaosScalarIsPositiveU{w}" : $"ChaosScalarIsPositive{w}");
                Predicate(type, carrier, "IsNegative",
                    unsigned ? $"ChaosScalarIsNegativeU{w}" : $"ChaosScalarIsNegative{w}");
                Predicate(type, carrier, "IsNormal", $"ChaosScalarIsNormal{w}");
                Predicate(type, carrier, "IsSubnormal", $"ChaosScalarIsSubnormal{w}");
                Predicate(type, carrier, "IsRealNumber", $"ChaosScalarIsRealNumber{w}");
            }

            // — Floating widths: full IEEE-754 predicate set —
            foreach (var (type, suffix, carrier) in floatTypes)
            {
                Predicate(type, carrier, "IsFinite", $"ChaosScalarIsFinite{suffix}");
                Predicate(type, carrier, "IsNaN", $"ChaosScalarIsNaN_{suffix}");
                Predicate(type, carrier, "IsInfinity", $"ChaosScalarIsInfinity{suffix}");
                Predicate(type, carrier, "IsNegativeInfinity", $"ChaosScalarIsNegativeInfinity{suffix}");
                Predicate(type, carrier, "IsPositiveInfinity", $"ChaosScalarIsPositiveInfinity{suffix}");
                Predicate(type, carrier, "IsNormal", $"ChaosScalarIsNormal{suffix}");
                Predicate(type, carrier, "IsSubnormal", $"ChaosScalarIsSubnormal{suffix}");
                Predicate(type, carrier, "IsInteger", $"ChaosScalarIsInteger{suffix}");
                Predicate(type, carrier, "IsEvenInteger", $"ChaosScalarIsEvenInteger{suffix}");
                Predicate(type, carrier, "IsOddInteger", $"ChaosScalarIsOddInteger{suffix}");
                Predicate(type, carrier, "IsPositive", $"ChaosScalarIsPositive{suffix}");
                Predicate(type, carrier, "IsNegative", $"ChaosScalarIsNegative{suffix}");
                Predicate(type, carrier, "IsRealNumber", $"ChaosScalarIsRealNumber{suffix}");
            }
        }
    }
}
