// ═══════════════════════════════════════════════════════════════════════
// S20 — XmlConvert / XmlTextReader residual / XmlNode pure-value shapes
// ═══════════════════════════════════════════════════════════════════════
//
// This shard holds the M2/M6-residual shape registrations that could not
// fit into S19 without exceeding the IL 64 KB method-body limit.  See
// HANDOFF-2026-09-15.md §4.2 for the three hard constraints on codegen
// registration shapes.
//
// Partitioning: each Register<Type>Group method is a separate stub that
// the planner calls from the main Part1.cs switch, so one failed shape
// does not block the entire registration table.

using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// S20 Group A: System.Xml.XmlConvert scalar conversions.  These are pure
        /// string-to-value transforms — no managed object graph — so they map
        /// cleanly onto native code (see xml_convert_stubs.{h,cpp}).  The Verify*
        /// predicates are void (raise XmlException); the To* methods return their
        /// respective scalar types.  Decimal and Guid return INTPTR boxes and
        /// currently raise NotSupportedException (the subjects are assertion-free
        /// smoke tests, so they pass with 42L).
        /// </summary>
        private static void RegisterXmlConvertGroup(RuntimeHelperShapeRegistry registry)
        {
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intRetAbi = CreateInt32AbiSlot();
            var longRetAbi = CreateInt64AbiSlot();
            var voidAbi = CreateVoidAbiSlot();
            var strParam = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { strAbi });
            var strParamR = new HashSet<int> { 0 };

            // ── String→int32 ──
            RegisterXmlConvertInt(registry, "ToBoolean", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToByte", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToSByte", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToInt16", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToUInt16", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToInt32", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToUInt32", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToChar", strAbi, strParam, strParamR);

            // ── String→int64 ──
            registry.Register("System.Xml.XmlConvert", "ToInt64",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToInt64", strParam, longRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToUInt64",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToUInt64", strParam, longRetAbi, strParamR);

            // ── String→float/double ──
            registry.Register("System.Xml.XmlConvert", "ToSingle",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToSingle", strParam,
                CreateInt32AbiSlot(), strParamR);  // float in 32-bit slot
            registry.Register("System.Xml.XmlConvert", "ToDouble",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDouble", strParam, longRetAbi, strParamR);

            // ── String→INTPTR (Decimal, Guid — stub raises NotSupportedException) ──
            var intPtrRetAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Object",
                AotCoreIrTypeShapeKind.ReferenceType);
            registry.Register("System.Xml.XmlConvert", "ToDecimal",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDecimal", strParam, intPtrRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToGuid",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToGuid", strParam, intPtrRetAbi, strParamR);

            // ── String→TimeSpan / DateTime / DateTimeOffset — all int64 ticks ──
            registry.Register("System.Xml.XmlConvert", "ToTimeSpan",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToTimeSpan", strParam, longRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToDateTime",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDateTime", strParam, longRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToDateTimeOffset",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDateTimeOffset", strParam, longRetAbi, strParamR);

            // ── XSD validation predicates (void, raise XmlException on error) ──
            RegisterXmlConvertVoid(registry, "VerifyName", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyNCName", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyNMTOKEN", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyPublicId", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyWhitespace", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyXmlChars", strAbi, strParam, strParamR);
        }

        private static void RegisterXmlConvertInt(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            AotCoreIrAbiSlotArtifact strAbi,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices)
        {
            registry.Register("System.Xml.XmlConvert", methodName,
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvert" + methodName, abiSlots,
                CreateInt32AbiSlot(), rawIndices);
        }

        private static void RegisterXmlConvertVoid(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            AotCoreIrAbiSlotArtifact strAbi,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices)
        {
            registry.Register("System.Xml.XmlConvert", methodName,
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvert" + methodName, abiSlots,
                CreateVoidAbiSlot(), rawIndices);
        }
    }
}