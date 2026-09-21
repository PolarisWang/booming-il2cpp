using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// IEEE-754 special functions on System.Math / System.Double / System.Single.
        ///
        /// These had NO shape registration, so every subject lowered to the
        /// zero-argument <c>chaos_external_runtime_*</c> catch-all.  That stub
        /// receives only the subject id — not the operands — and returns 0 when
        /// the interpreter paths miss.  The subject then compared a fabricated 0
        /// against the expected value.
        ///
        /// It read as "passing" for <c>BitDecrement(default(double))</c> only
        /// because the old float64 eval-stack lowering double-encoded both the
        /// computed 0 and the expected -5E-324 into garbage that happened to be
        /// equal — a false green.  Registering real native forwards (see
        /// <c>runtime_stubs/math_stubs.cpp</c> ChaosMathBitDecrement &amp; co.)
        /// makes the comparison honest.  Doing this BEFORE unifying the eval
        /// stack representation is deliberate: otherwise the fix is judged by
        /// those same false greens.
        /// </summary>
        private static void RegisterIeee754SpecialFunctions(RuntimeHelperShapeRegistry registry)
        {
            var f64Slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType };
            var i32Slot = CreateInt32AbiSlot();

            void RegisterUnary(string type, string method, string symbol)
                => registry.Register(type, method, ["System.Double"],
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(f64Slot),
                    f64Slot,
                    new HashSet<int> { 0 });

            void RegisterBinary(string type, string method, string symbol)
                => registry.Register(type, method, ["System.Double", "System.Double"],
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { f64Slot, f64Slot }),
                    f64Slot,
                    new HashSet<int> { 0, 1 });

            // System.Double::X / System.Math::X — .NET exposes these on both.
            foreach (var type in new[] { "System.Double", "System.Math" })
            {
                RegisterUnary(type, "BitDecrement", "ChaosMathBitDecrement");
                RegisterUnary(type, "BitIncrement", "ChaosMathBitIncrement");
                RegisterBinary(type, "CopySign", "ChaosMathCopySign");
                RegisterBinary(type, "MaxMagnitude", "ChaosMathMaxMagnitude");
                RegisterBinary(type, "MinMagnitude", "ChaosMathMinMagnitude");
                RegisterBinary(type, "IEEERemainder", "ChaosMathIEEERemainder");
                RegisterBinary(type, "Hypot", "ChaosMathHypot");
                RegisterUnary(type, "Acosh", "ChaosMathAcosh");
                RegisterUnary(type, "Asinh", "ChaosMathAsinh");
                RegisterUnary(type, "Atanh", "ChaosMathAtanh");
                RegisterUnary(type, "Cbrt", "ChaosMathCbrt");
                RegisterUnary(type, "Exp2", "ChaosMathExp2");
                RegisterUnary(type, "ExpM1", "ChaosMathExpM1");
                RegisterUnary(type, "Log2", "ChaosMathLog2");
                RegisterUnary(type, "LogP1", "ChaosMathLog1p");
                RegisterUnary(type, "ReciprocalEstimate", "ChaosMathReciprocalEstimate");
                RegisterUnary(type, "ReciprocalSqrtEstimate", "ChaosMathReciprocalSqrtEstimate");
            }

            // ILogB returns Int32, and ScaleB takes an Int32 exponent — both need
            // a slot layout that differs from the unary/binary doubles above.
            foreach (var type in new[] { "System.Double", "System.Math" })
            {
                registry.Register(type, "ILogB", ["System.Double"],
                    ShapeKind.SimpleForward, "ChaosMathILogB",
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(f64Slot),
                    i32Slot,
                    new HashSet<int> { 0 });

                registry.Register(type, "ScaleB", ["System.Double", "System.Int32"],
                    ShapeKind.SimpleForward, "ChaosMathScaleB",
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { f64Slot, i32Slot }),
                    f64Slot,
                    new HashSet<int> { 0, 1 });
            }

            // ── Single (float) counterparts: System.Single:: / System.MathF:: ──
            // Registered against the *F native symbols.  The float subjects
            // (MathFTests::BitDecrement_20_float, SingleTests::AcosPi_73_float,
            // …) are otherwise left on the operand-less catch-all even after the
            // double side is wired, which reads as an unexplained regression.
            var f32Slot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float32, TypeShape = AotCoreIrTypeShapeKind.ValueType };

            void RegisterUnaryF(string type, string method, string symbol)
                => registry.Register(type, method, ["System.Single"],
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(f32Slot),
                    f32Slot,
                    new HashSet<int> { 0 });

            void RegisterBinaryF(string type, string method, string symbol)
                => registry.Register(type, method, ["System.Single", "System.Single"],
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { f32Slot, f32Slot }),
                    f32Slot,
                    new HashSet<int> { 0, 1 });

            foreach (var type in new[] { "System.Single", "System.MathF" })
            {
                RegisterUnaryF(type, "BitDecrement", "ChaosMathBitDecrementF");
                RegisterUnaryF(type, "BitIncrement", "ChaosMathBitIncrementF");
                RegisterBinaryF(type, "CopySign", "ChaosMathCopySignF");
                RegisterBinaryF(type, "MaxMagnitude", "ChaosMathMaxMagnitudeF");
                RegisterBinaryF(type, "MinMagnitude", "ChaosMathMinMagnitudeF");
                RegisterBinaryF(type, "IEEERemainder", "ChaosMathIEEERemainderF");
                RegisterBinaryF(type, "Hypot", "ChaosMathHypotF");
                RegisterUnaryF(type, "Acosh", "ChaosMathAcoshF");
                RegisterUnaryF(type, "Asinh", "ChaosMathAsinhF");
                RegisterUnaryF(type, "Atanh", "ChaosMathAtanhF");
                RegisterUnaryF(type, "Cbrt", "ChaosMathCbrtF");
                RegisterUnaryF(type, "Exp2", "ChaosMathExp2F");
                RegisterUnaryF(type, "ExpM1", "ChaosMathExpM1F");
                RegisterUnaryF(type, "Log2", "ChaosMathLog2F");
                RegisterUnaryF(type, "LogP1", "ChaosMathLog1pF");
                RegisterUnaryF(type, "ReciprocalEstimate", "ChaosMathReciprocalEstimateF");
                RegisterUnaryF(type, "ReciprocalSqrtEstimate", "ChaosMathReciprocalSqrtEstimateF");

                registry.Register(type, "ILogB", ["System.Single"],
                    ShapeKind.SimpleForward, "ChaosMathILogBF",
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(f32Slot),
                    i32Slot,
                    new HashSet<int> { 0 });

                registry.Register(type, "ScaleB", ["System.Single", "System.Int32"],
                    ShapeKind.SimpleForward, "ChaosMathScaleBF",
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { f32Slot, i32Slot }),
                    f32Slot,
                    new HashSet<int> { 0, 1 });
            }

            // ── Pi-scaled trig (AcosPi/AsinPi/AtanPi/CosPi/SinPi/TanPi) ──
            // .NET 7+ exposes these on System.Double::/System.Math:: — without
            // registration they hit the operand-less catch-all too.
            foreach (var type in new[] { "System.Double", "System.Math" })
            {
                RegisterUnary(type, "AcosPi", "ChaosMathAcosPi");
                RegisterUnary(type, "AsinPi", "ChaosMathAsinPi");
                RegisterUnary(type, "AtanPi", "ChaosMathAtanPi");
                RegisterUnary(type, "CosPi", "ChaosMathCosPi");
                RegisterUnary(type, "SinPi", "ChaosMathSinPi");
                RegisterUnary(type, "TanPi", "ChaosMathTanPi");
                RegisterBinary(type, "Atan2Pi", "ChaosMathAtan2Pi");
            }

            foreach (var type in new[] { "System.Single", "System.MathF" })
            {
                RegisterUnaryF(type, "AcosPi", "ChaosMathAcosPiF");
                RegisterUnaryF(type, "AsinPi", "ChaosMathAsinPiF");
                RegisterUnaryF(type, "AtanPi", "ChaosMathAtanPiF");
                RegisterUnaryF(type, "CosPi", "ChaosMathCosPiF");
                RegisterUnaryF(type, "SinPi", "ChaosMathSinPiF");
                RegisterUnaryF(type, "TanPi", "ChaosMathTanPiF");
                RegisterBinaryF(type, "Atan2Pi", "ChaosMathAtan2PiF");
            }
        }
    }
}
