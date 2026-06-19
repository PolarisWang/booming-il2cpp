using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Reflection.PropertyInfo::get_CanWrite (SimpleForward stub returning false/0)
        /// </summary>
        private static void RegisterSystemReflectionPropertyInfo_2(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.PropertyInfo", "get_CanWrite", [],
                ShapeKind.SimpleForward, "ChaosReflectionPropertyGetCanWrite",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.String::ToLower (stub returning null)
        /// </summary>
        private static void RegisterSystemStringToLower(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.String", "ToLower", [],
                ShapeKind.SimpleForward, "ChaosStringToLower",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.String::ToUpper (stub returning null)
        /// </summary>
        private static void RegisterSystemStringToUpper(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.String", "ToUpper", [],
                ShapeKind.SimpleForward, "ChaosStringToUpper",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.String::Trim (stub returning null)
        /// </summary>
        private static void RegisterSystemStringTrim(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.String", "Trim", [],
                ShapeKind.SimpleForward, "ChaosStringTrim",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Net.WebUtility stubs (HtmlEncode/Decode, UrlEncode/Decode)
        /// </summary>
        private static void RegisterSystemNetWebUtility(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Net.WebUtility", "HtmlEncode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityHtmlEncode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
            registry.Register("System.Net.WebUtility", "HtmlDecode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityHtmlDecode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
            registry.Register("System.Net.WebUtility", "UrlEncode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityUrlEncode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
            registry.Register("System.Net.WebUtility", "UrlDecode", ["System.String"],
                ShapeKind.SimpleForward, "ChaosWebUtilityUrlDecode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}
