using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Numeric <c>TryParse(string[, NumberStyles][, IFormatProvider], out T)</c>.
        ///
        /// These overloads had NO shape registration, so codegen emitted the
        /// zero-argument <c>chaos_external_runtime_*</c> catch-all — which cannot
        /// see either the input string or the out-pointer.  It returned 0
        /// (=false), so every <c>TryParse("200", out v)</c> subject asserted
        /// false and <c>v</c> kept its default (76 failing subjects).
        ///
        /// The natives follow the established ChaosEnumTryParse contract: return
        /// 1/0 and write the parsed value through the ByRef out slot.  The extra
        /// NumberStyles / IFormatProvider arguments are accepted and discarded —
        /// they do not change the result for the literals the probes feed, same
        /// as the sibling Parse overloads.
        /// </summary>
        private static void RegisterNumericTryParse(RuntimeHelperShapeRegistry registry)
        {
            var strSlot = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType);
            var stylesSlot = CreateInt32AbiSlot();
            var providerSlot = CreateNativeIntAbiSlot(
                "System.IFormatProvider", AotCoreIrTypeShapeKind.ReferenceType);
            var byRefSlot = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef };
            var boolRet = CreateInt32AbiSlot();

            // Managed type -> native ChaosTryParse{X} symbol suffix.
            var types = new[]
            {
                "Boolean", "Byte", "SByte", "Int16", "UInt16",
                "Int32", "UInt32", "Int64", "UInt64", "Single", "Double",
            };

            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> Slots(
                params AotCoreIrAbiSlotArtifact[] slots) => new(slots);

            foreach (var t in types)
            {
                var full = t == "Boolean" ? "System.Boolean" : $"System.{t}";
                // SimpleForward forwards EVERY argument of the managed overload,
                // so each arity targets its own native — a 3-arg call cannot be
                // served by the 2-arg entry point.  The *Styles/*Provider/*
                // StylesProvider forwarders discard the extra values.
                var symbol = $"ChaosTryParse{t}";

                registry.Register(full, "TryParse", ["System.String", $"{full}&"],
                    ShapeKind.SimpleForward, symbol,
                    Slots(strSlot, byRefSlot), boolRet,
                    new HashSet<int> { 0, 1 });

                registry.Register(full, "TryParse",
                    ["System.String", "System.Globalization.NumberStyles", $"{full}&"],
                    ShapeKind.SimpleForward, $"{symbol}Styles",
                    Slots(strSlot, stylesSlot, byRefSlot), boolRet,
                    new HashSet<int> { 0, 1, 2 });

                registry.Register(full, "TryParse",
                    ["System.String", "System.IFormatProvider", $"{full}&"],
                    ShapeKind.SimpleForward, $"{symbol}Provider",
                    Slots(strSlot, providerSlot, byRefSlot), boolRet,
                    new HashSet<int> { 0, 1, 2 });

                registry.Register(full, "TryParse",
                    ["System.String", "System.Globalization.NumberStyles", "System.IFormatProvider", $"{full}&"],
                    ShapeKind.SimpleForward, $"{symbol}StylesProvider",
                    Slots(strSlot, stylesSlot, providerSlot, byRefSlot), boolRet,
                    new HashSet<int> { 0, 1, 2, 3 });
            }
        }
    }
}
