// C1 — System.Text 编码族 native 实现。
//
// 背景：text chunk 的编码族此前没有 native body，调用落进
// ChaosExternalRuntimeFallback 返回 0，断言比较该 0 与 BCL 期望值而失败。
// C2 的 fact_chunk.py 改动已把这批诚实标注为 `unimplemented`；本文件逐个实现，
// 使其从 unimplemented 转为 real。实现见
// src/native/runtime-core/runtime_stubs/text_stubs.cpp。

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Text 谓词 / 尺寸查询 / 前导码族。
        /// </summary>
        /// <remarks>
        /// 参数 ABI 槽必须按实际宽度声明（byte→UInt8、char→UInt16、int→Int32）：
        /// 用 CreateInt32AbiSlot 承载窄类型会让被调方收到错位实参并与 shape 哈希
        /// 不符，调用会静默落回 catch-all。同 BitOperations::Crc32C（Part2.S3）。
        /// bool 返回统一用 Int32 槽承载。
        /// </remarks>
        private static void RegisterTextAsciiStubs(RuntimeHelperShapeRegistry registry)
        {
            RegisterTextAscii(registry);
            RegisterTextEncodingCounts(registry);
            RegisterTextEncodingPreambles(registry);
            RegisterTextRune(registry);
            RegisterTextFallbackAndBuilder(registry);
        }

        // ── System.Text.Ascii ──────────────────────────────────────────────
        private static void RegisterTextAscii(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Text.Ascii", "IsValid", ["System.Byte"],
                ShapeKind.SimpleForward, "ChaosAsciiIsValidByte",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    }),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.Ascii", "IsValid", ["System.Char"],
                ShapeKind.SimpleForward, "ChaosAsciiIsValidChar",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    }),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);
        }

        // ── Encoding 族的 GetMaxByteCount / GetMaxCharCount ────────────────
        // 均为「实例方法 + 一个 int 参数」：slot0 = receiver，slot1 = int。
        private static void RegisterTextEncodingCounts(RuntimeHelperShapeRegistry registry)
        {
            RegisterIntArgShape(registry, "System.Text.ASCIIEncoding", "GetMaxByteCount", "ChaosEncodingGetMaxByteCountASCII");
            RegisterIntArgShape(registry, "System.Text.ASCIIEncoding", "GetMaxCharCount", "ChaosEncodingGetMaxCharCountASCII");
            RegisterIntArgShape(registry, "System.Text.UTF8Encoding", "GetMaxByteCount", "ChaosEncodingGetMaxByteCountUTF8");
            RegisterIntArgShape(registry, "System.Text.UTF8Encoding", "GetMaxCharCount", "ChaosEncodingGetMaxCharCountUTF8");
            RegisterIntArgShape(registry, "System.Text.UTF7Encoding", "GetMaxByteCount", "ChaosEncodingGetMaxByteCountUTF7");
            RegisterIntArgShape(registry, "System.Text.UTF7Encoding", "GetMaxCharCount", "ChaosEncodingGetMaxCharCountUTF7");
            RegisterIntArgShape(registry, "System.Text.UnicodeEncoding", "GetMaxByteCount", "ChaosEncodingGetMaxByteCountUnicode");
            RegisterIntArgShape(registry, "System.Text.UnicodeEncoding", "GetMaxCharCount", "ChaosEncodingGetMaxCharCountUnicode");
            RegisterIntArgShape(registry, "System.Text.UTF32Encoding", "GetMaxByteCount", "ChaosEncodingGetMaxByteCountUTF32");
            RegisterIntArgShape(registry, "System.Text.UTF32Encoding", "GetMaxCharCount", "ChaosEncodingGetMaxCharCountUTF32");

            // System.Text.Encoding 的静态默认实例（Encoding.UTF8）走的也是同族入口。
            RegisterIntArgShape(registry, "System.Text.Encoding", "GetMaxByteCount", "ChaosEncodingGetMaxByteCountUTF8");
            RegisterIntArgShape(registry, "System.Text.Encoding", "GetMaxCharCount", "ChaosEncodingGetMaxCharCountUTF8");
        }

        /// <summary>实例方法 (int) → int 的统一注册体。</summary>
        private static void RegisterIntArgShape(
            RuntimeHelperShapeRegistry registry, string typeName, string methodName, string symbol)
        {
            registry.Register(typeName, methodName, ["System.Int32"],
                ShapeKind.SimpleForward, symbol,
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);
        }

        // ── Encoding 族的 GetPreamble / GetBytes ───────────────────────────
        // 无参实例方法：receiver 由注入机制提供，ParameterAbis 留空。
        private static void RegisterTextEncodingPreambles(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Text.Encoding", "GetPreamble", [],
                ShapeKind.SimpleForward, "ChaosEncodingGetPreambleUTF8",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.UTF8Encoding", "GetPreamble", [],
                ShapeKind.SimpleForward, "ChaosEncodingGetPreambleUTF8",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.UnicodeEncoding", "GetPreamble", [],
                ShapeKind.SimpleForward, "ChaosEncodingGetPreambleUnicode",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.UTF32Encoding", "GetPreamble", [],
                ShapeKind.SimpleForward, "ChaosEncodingGetPreambleUTF32",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // Encoding.GetBytes(char[], int, int) — the zero-length form.
            // Encoding is NOT in the receiver injection allowlist, so the
            // generated shim carries no slots at all; declaring the three
            // parameter slots here would make the shim demand arguments the
            // call site never pushes (C2660).  The native takes none either.
            registry.Register("System.Text.Encoding", "GetBytes",
                ["System.Char[]", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosEncodingGetBytesEmpty",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);
        }

        // ── System.Text.Rune ───────────────────────────────────────────────
        private static void RegisterTextRune(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Text.Rune", "IsValid", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosRuneIsValidInt",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.Rune", "IsValid", ["System.UInt32"],
                ShapeKind.SimpleForward, "ChaosRuneIsValidUInt",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            RegisterRuneTryCreate(registry, "System.Char", "ChaosRuneTryCreateChar",
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                });
            RegisterRuneTryCreate(registry, "System.Int32", "ChaosRuneTryCreateInt", CreateInt32AbiSlot());
            RegisterRuneTryCreate(registry, "System.UInt32", "ChaosRuneTryCreateUInt", CreateInt32AbiSlot());

            // Rune 接收者是值类型实例（占一个 INTPTR 槽）。
            registry.Register("System.Text.Rune", "GetNumericValue", ["System.Text.Rune"],
                ShapeKind.SimpleForward, "ChaosRuneGetNumericValue",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.Rune", "IsControl", ["System.Text.Rune"],
                ShapeKind.SimpleForward, "ChaosRuneIsControl",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType)),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);
        }

        /// <summary>Rune.TryCreate(T, out Rune) → bool：slot0 = 入参，slot1 = out 槽。</summary>
        private static void RegisterRuneTryCreate(
            RuntimeHelperShapeRegistry registry, string paramType, string symbol,
            AotCoreIrAbiSlotArtifact paramSlot)
        {
            registry.Register("System.Text.Rune", "TryCreate", [paramType, "System.Text.Rune&"],
                ShapeKind.SimpleForward, symbol,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    paramSlot,
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });
        }

        // ── FallbackBuffer.MovePrevious / StringBuilder.EnsureCapacity ─────
        private static void RegisterTextFallbackAndBuilder(RuntimeHelperShapeRegistry registry)
        {
            // 无参实例方法：receiver 由注入机制提供。
            registry.Register("System.Text.DecoderReplacementFallbackBuffer", "MovePrevious", [],
                ShapeKind.SimpleForward, "ChaosDecoderFallbackMovePrevious",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.EncoderReplacementFallbackBuffer", "MovePrevious", [],
                ShapeKind.SimpleForward, "ChaosEncoderFallbackMovePrevious",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Text.StringBuilder", "EnsureCapacity", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosStringBuilderEnsureCapacity",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);
        }
    }
}
