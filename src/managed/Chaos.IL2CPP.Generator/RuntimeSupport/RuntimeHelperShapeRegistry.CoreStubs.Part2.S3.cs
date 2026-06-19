using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Numerics::BitOperations (SimpleForward stubs)
        /// </summary>
        private static void RegisterSystemNumericsBitOperations(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Numerics.BitOperations", "PopCount", ["System.UInt32"],
                ShapeKind.SimpleForward, "ChaosBitOpsPopCount32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "PopCount", ["System.UInt64"],
                ShapeKind.SimpleForward, "ChaosBitOpsPopCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // ── Missing overloads for PopCount ──
            registry.Register("System.Numerics.BitOperations", "PopCount", ["System.UIntPtr"],
                ShapeKind.SimpleForward, "ChaosBitOpsPopCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "LeadingZeroCount", ["System.UInt32"],
                ShapeKind.SimpleForward, "ChaosBitOpsLeadingZeroCount32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "LeadingZeroCount", ["System.UInt64"],
                ShapeKind.SimpleForward, "ChaosBitOpsLeadingZeroCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // ── Missing overload for LeadingZeroCount ──
            registry.Register("System.Numerics.BitOperations", "LeadingZeroCount", ["System.UIntPtr"],
                ShapeKind.SimpleForward, "ChaosBitOpsLeadingZeroCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "TrailingZeroCount", ["System.UInt32"],
                ShapeKind.SimpleForward, "ChaosBitOpsTrailingZeroCount32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "TrailingZeroCount", ["System.UInt64"],
                ShapeKind.SimpleForward, "ChaosBitOpsTrailingZeroCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // ── Missing overloads for TrailingZeroCount ──
            registry.Register("System.Numerics.BitOperations", "TrailingZeroCount", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitOpsTrailingZeroCount32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "TrailingZeroCount", ["System.Int64"],
                ShapeKind.SimpleForward, "ChaosBitOpsTrailingZeroCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "TrailingZeroCount", ["System.IntPtr"],
                ShapeKind.SimpleForward, "ChaosBitOpsTrailingZeroCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "TrailingZeroCount", ["System.UIntPtr"],
                ShapeKind.SimpleForward, "ChaosBitOpsTrailingZeroCount64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "RotateLeft", ["System.UInt32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitOpsRotateLeft32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "RotateLeft", ["System.UInt64", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitOpsRotateLeft64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // ── Missing overloads for RotateLeft ──
            registry.Register("System.Numerics.BitOperations", "RotateLeft", ["System.UIntPtr", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitOpsRotateLeft64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "RotateRight", ["System.UInt32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitOpsRotateRight32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "RotateRight", ["System.UInt64", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitOpsRotateRight64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            // ── Missing overloads for RotateRight ──
            registry.Register("System.Numerics.BitOperations", "RotateRight", ["System.UIntPtr", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitOpsRotateRight64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    CreateInt32AbiSlot(),
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "Crc32C", ["System.UInt32", "System.Byte"],
                ShapeKind.SimpleForward, "ChaosBitOpsCrc32CByte",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "Crc32C", ["System.UInt32", "System.UInt16"],
                ShapeKind.SimpleForward, "ChaosBitOpsCrc32CUInt16",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "Crc32C", ["System.UInt32", "System.UInt32"],
                ShapeKind.SimpleForward, "ChaosBitOpsCrc32CUInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "Crc32C", ["System.UInt32", "System.UInt64"],
                ShapeKind.SimpleForward, "ChaosBitOpsCrc32CUInt64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Numerics.BitOperations", "RoundUpToPowerOf2", ["System.UInt32"],
                ShapeKind.SimpleForward, "ChaosBitOpsRoundUpToPowerOf232",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Numerics.BitOperations", "RoundUpToPowerOf2", ["System.UInt64"],
                ShapeKind.SimpleForward, "ChaosBitOpsRoundUpToPowerOf264",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // ── Missing overload for RoundUpToPowerOf2 ──
            registry.Register("System.Numerics.BitOperations", "RoundUpToPowerOf2", ["System.UIntPtr"],
                ShapeKind.SimpleForward, "ChaosBitOpsRoundUpToPowerOf264",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Numerics::BitOperations::IsPow2 (inline shape)
        /// </summary>
        private static void RegisterSystemNumericsBitOperations_1(RuntimeHelperShapeRegistry registry)
        {
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "System.Numerics.BitOperations",
                        MethodName: "IsPow2",
                        Resolver: static (callee, paramTypes) =>
                        {
                            if (paramTypes.Count != 1) return null;
                            // uint and ulong: same C++ expression works for both
                            return "(({0} & ({0} - 1)) == 0 && {0} != 0)";
                        }));

                    // ═══════════════════════════════════════════════════════════════
                    // System.Runtime.Intrinsics.Vector128 / Vector256 — InlineShape
                    // ═══════════════════════════════════════════════════════════════
                    // Maps Vector128<T> and Vector256<T> methods to SIMD hardware
                    // intrinsic stubs (simd_stubs.h/simd_stubs.cpp) with lane-by-lane
                    // fallback via vector_fixed_templates.h.  Eliminates the generic
                    // managed dispatch overhead (~0.9ms → ~0.3µs per call) and replaces
                    // scalar lane loops with _mm_* / _mm256_* SSE/AVX instructions.
                    //
                    // Extracts the element type T from param types like
                    // "System.Runtime.Intrinsics.Vector128<System.Int32>" and maps it
                    // to the corresponding C++ scalar type for template instantiation.

                    static string? ExtractVectorElementType(string callee, IReadOnlyList<string> paramTypes)
                    {
                        // Try to extract from first Vector128<X> or Vector256<X> parameter
                        string? candidate = null;
                        for (var i = 0; i < paramTypes.Count; i++)
                        {
                            var m = System.Text.RegularExpressions.Regex.Match(paramTypes[i], @"Vector(?:64|128|256|512)<([^>]+)>");
                            if (m.Success)
                            {
                                candidate = m.Groups[1].Value;
                                break;
                            }
                        }
                        // Fall back to matching in the callee SubjectId
                        if (candidate == null)
                        {
                            var m = System.Text.RegularExpressions.Regex.Match(callee, @"Vector(?:64|128|256|512)<([^>]+)>");
                            if (m.Success) candidate = m.Groups[1].Value;
                        }
                        if (candidate == null || candidate.Contains('<')) return null;
                        return candidate;
                    }

                    static string? MapTypeArgToCppType(string typeArg)
                    {
                        return typeArg switch
                        {
                            "System.Byte" => "CHAOS_IL2CPP_UINT8",
                            "System.SByte" => "CHAOS_IL2CPP_INT8",
                            "System.Int16" => "CHAOS_IL2CPP_INT16",
                            "System.UInt16" => "CHAOS_IL2CPP_UINT16",
                            "System.Int32" => "CHAOS_IL2CPP_INT32",
                            "System.UInt32" => "CHAOS_IL2CPP_UINT32",
                            "System.Int64" => "CHAOS_IL2CPP_INT64",
                            "System.UInt64" => "CHAOS_IL2CPP_UINT64",
                            "System.Single" => "float",
                            "System.Double" => "double",
                            "System.IntPtr" => "CHAOS_IL2CPP_INTPTR",
                            "System.UIntPtr" => "CHAOS_IL2CPP_UINTPTR",
                            _ => null,
                        };
                    }

                    static string? InferVectorCarrierType(string callee)
                    {
                        if (callee.Contains("Vector512")) return "RuntimeIntrinsicVector512Carrier";
                        if (callee.Contains("Vector256")) return "RuntimeIntrinsicVector256Carrier";
                        if (callee.Contains("Vector64")) return "RuntimeIntrinsicVector64Carrier";
                        return "RuntimeIntrinsicVector128Carrier"; // default for Vector128
                    }

                    static string? MakeVectorInlineExpression(string callee, IReadOnlyList<string> paramTypes,
            string templateFn, bool requiresScalar)
                    {
                        // DISABLED: AOT eval stack stores Vector128/256 as CHAOS_IL2CPP_INTPTR (pointer to
                        // 16/32-byte carrier), but template functions expect carrier by value. The InlineShape
                        // framework needs support for >8-byte value types on the eval stack.
                        // TODO: Enable when AOT codegen supports large value types on eval stack.
                        // return null;  -- ENABLED: inline shapes now deref pointer/heap-alloc
                        var elemType = ExtractVectorElementType(callee, paramTypes);
                        if (elemType == null) return null;
                        var cppType = MapTypeArgToCppType(elemType);
                        if (cppType == null) return null;
                        var carrier = InferVectorCarrierType(callee);
                        if (carrier == null) return null;

                        // AOT eval stack stores Vector128/256 values as CHAOS_IL2CPP_INTPTR
                        // which points to a heap-allocated carrier struct (16 or 32 bytes).
                        // The InlineShape must dereference the pointer, call the function,
                        // then heap-allocate a new carrier for the result and return its pointer.
                        const string ns = "chaos::il2cpp::vector_fixed::";
                        var tc = cppType + ", " + carrier;
                        // Compare functions (VectorFixedCompareEqual, etc.) need 3 type params:
                        // <TInputScalar, TMaskScalar, TCarrier> where TMaskScalar = TInputScalar.
                        string TcForCompare() => cppType + ", " + cppType + ", " + carrier;
                        // Select the right type-argument string for the template function.
                        string TcForTemplateFn(string fn) =>
                            fn.StartsWith("VectorFixedCompare") ? TcForCompare() : tc;
                        string Deref(int i) =>
                            i < paramTypes.Count && (paramTypes[i].Contains("Vector64<") || paramTypes[i].Contains("Vector128<") || paramTypes[i].Contains("Vector256<") || paramTypes[i].Contains("Vector512<"))
                                ? $"*reinterpret_cast<{carrier}*>({{{i}}})"
                                : $"{{{i}}}";

                        // For arithmetic binary ops (Add, Subtract, Multiply, Divide where both
                        // operands must be carriers), broadcast any scalar params to carrier.
                        // Shift ops (ShiftLeft, ShiftRight*) take CHAOS_IL2CPP_INT32 as second arg
                        // and must NOT broadcast — use Deref() for those.
                        static bool IsArithmeticBinaryOp(string fn) => fn switch
                        {
                            "VectorFixedAdd" or "VectorFixedSubtract" or
                            "VectorFixedMultiply" or "VectorFixedDivide" => true,
                            _ => false,
                        };
                        string Arg(int i) =>
                            IsArithmeticBinaryOp(templateFn)
                                ? (i < paramTypes.Count && (paramTypes[i].Contains("Vector128<") || paramTypes[i].Contains("Vector256<"))
                                    ? $"*reinterpret_cast<{carrier}*>({{{i}}})"
                                    : $"{ns}VectorFixedBroadcast<{cppType}, {carrier}>(static_cast<{cppType}>({{{i}}}))")
                                : Deref(i);

                        // Check for SIMD hardware intrinsic stub first
                        // SIMD stubs expect ALL operands to be vector types (Vector128<T> or Vector256<T>).
                        // If ANY argument is a scalar (e.g., Multiply(Vector256<int>, int)), fall back to
                        // the template approach which handles mixed vector/scalar arguments correctly.
                        var simdStub = TryGetSimdStub(templateFn, carrier, requiresScalar ? cppType : null);
                        if (simdStub != null)
                        {
                            // Only use SIMD stub when all parameters are vector types
                            var allParamsAreVector = true;
                            for (var pi = 0; pi < paramTypes.Count; pi++)
                            {
                                if (!paramTypes[pi].Contains("Vector64<") && !paramTypes[pi].Contains("Vector128<") && !paramTypes[pi].Contains("Vector256<") && !paramTypes[pi].Contains("Vector512<"))
                                {
                                    allParamsAreVector = false;
                                    break;
                                }
                            }

                            if (allParamsAreVector)
                            {
                                if (paramTypes.Count >= 2)
                                    return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {simdStub}({Deref(0)}, {Deref(1)}); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {simdStub}({Deref(0)}); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                            }
                        }

                        // VectorFixedGetElement returns a scalar, not a carrier
                        if (templateFn == "VectorFixedGetElement")
                            return $"static_cast<CHAOS_IL2CPP_INTPTR>({ns}VectorFixedGetElement<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), static_cast<CHAOS_IL2CPP_INT32>({{1}})))";

                        // VectorFixedBroadcast (get_Zero / AllBitsSet) — no vector params
                        if (templateFn == "VectorFixedBroadcast" && paramTypes.Count == 0)
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {ns}VectorFixedBroadcast<{tc}>(0); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";

                        // VectorFixedAbs / VectorFixedNegate need <TInputScalar, TOutputScalar, TCarrier>
                        if (templateFn == "VectorFixedAbs" || templateFn == "VectorFixedNegate")
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {ns}{templateFn}<{cppType}, {cppType}, {carrier}>({Deref(0)}); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";

                        // VectorFixedCreateScalar — scalar param, returns carrier
                        if (templateFn == "VectorFixedCreateScalar" && paramTypes.Count == 1)
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {ns}VectorFixedCreateScalar<{tc}>(static_cast<{cppType}>({{0}})); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";

                        // Binary ops: deref inputs (2 or 3), call function, heap-alloc result
                        if (paramTypes.Count >= 2 && paramTypes.Count <= 3)
                        {
                            var argList = string.Join(", ", Enumerable.Range(0, paramTypes.Count).Select(i => Arg(i)));
                            var fnCall = requiresScalar
                                ? $"{ns}{templateFn}<{TcForTemplateFn(templateFn)}>({argList})"
                                : $"{ns}{templateFn}<{carrier}>({argList})";
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {fnCall}; auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }
                        if (paramTypes.Count >= 2)
                        {
                            var fnCall = requiresScalar
                                ? $"{ns}{templateFn}<{TcForTemplateFn(templateFn)}>({Arg(0)}, {Arg(1)})"
                                : $"{ns}{templateFn}<{carrier}>({Arg(0)}, {Arg(1)})";
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {fnCall}; auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }

                        // Unary ops: deref input, call function, heap-alloc result
                        var unaryFnCall = requiresScalar
                            ? $"{ns}{templateFn}<{tc}>({Deref(0)})"
                            : $"{ns}{templateFn}<{carrier}>({Deref(0)})";
                        return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {unaryFnCall}; auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                    }

                    // SIMD stub lookup: maps (templateFn, carrier, cppType) to the
                    // corresponding _mm_* / _mm256_* hardware intrinsic stub name.
                    // Returns null when no SIMD stub is available for the combo.
                    static string? TryGetSimdStub(string templateFn, string carrier, string? cppType)
                    {
                        // Bitwise operations are type-agnostic
                        if (templateFn == "VectorFixedBitwiseAnd")
                            return carrier == "RuntimeIntrinsicVector128Carrier" ? "ChaosSimd_V128_And" :
                                   carrier == "RuntimeIntrinsicVector256Carrier" ? "ChaosSimd_V256_And" : null;
                        if (templateFn == "VectorFixedBitwiseOr")
                            return carrier == "RuntimeIntrinsicVector128Carrier" ? "ChaosSimd_V128_Or" :
                                   carrier == "RuntimeIntrinsicVector256Carrier" ? "ChaosSimd_V256_Or" : null;
                        if (templateFn == "VectorFixedBitwiseXor")
                            return carrier == "RuntimeIntrinsicVector128Carrier" ? "ChaosSimd_V128_Xor" :
                                   carrier == "RuntimeIntrinsicVector256Carrier" ? "ChaosSimd_V256_Xor" : null;

                        // Type-specific operations need both carrier and element type
                        if (cppType == null) return null;
                        var prefix = carrier == "RuntimeIntrinsicVector128Carrier" ? "V128" :
                                     carrier == "RuntimeIntrinsicVector256Carrier" ? "V256" : null;
                        if (prefix == null) return null;

                        var suffix = (templateFn, cppType) switch
                        {
                            ("VectorFixedAdd", "CHAOS_IL2CPP_INT32") => "Add_I32",
                            ("VectorFixedAdd", "float") => "Add_F32",
                            ("VectorFixedAdd", "double") => "Add_F64",
                            ("VectorFixedSubtract", "CHAOS_IL2CPP_INT32") => "Sub_I32",
                            ("VectorFixedSubtract", "float") => "Sub_F32",
                            ("VectorFixedSubtract", "double") => "Sub_F64",
                            ("VectorFixedMultiply", "CHAOS_IL2CPP_INT32") => "Mul_I32",
                            ("VectorFixedMultiply", "float") => "Mul_F32",
                            ("VectorFixedMultiply", "double") => "Mul_F64",
                            ("VectorFixedCompareEqual", "CHAOS_IL2CPP_INT32") => "CmpEq_I32",
                            ("VectorFixedCompareEqual", "float") => "CmpEq_F32",
                            ("VectorFixedShiftLeft", "CHAOS_IL2CPP_INT32") => "Shl_I32",
                            _ => null,
                        };
                        return suffix != null ? $"ChaosSimd_{prefix}_{suffix}" : null;
                    }

                    // Helper: register a binary vector op for both Vector128 and Vector256.
                    // Uses MakeVectorInlineExpression directly (not captured variables) to avoid
                    // CS8820 (static lambda cannot capture local).
                    void RegisterVectorBinOp(string methodName, string templateFn, bool requiresScalar)
                    {
                        var fn = templateFn;
                        var rs = requiresScalar;
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            var p = prefix;
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: p,
                                MethodName: methodName,
                                Resolver: (callee, paramTypes) =>
                                    MakeVectorInlineExpression(callee, paramTypes, fn, rs)));
                        }
                    }

                    // Helper: register a unary vector op for Vector64/128/256/512
                    // Uses MakeVectorInlineExpression for correct pointer-based ABI.
                    void RegisterVectorUnaryOp(string methodName, string templateFn)
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            var p = prefix;
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: p,
                                MethodName: methodName,
                                Resolver: (callee, paramTypes) =>
                                    MakeVectorInlineExpression(callee, paramTypes, templateFn, true)));
                        }
                    }

                    // ── Arithmetic ──
                    RegisterVectorBinOp("Add", "VectorFixedAdd", true);
                    RegisterVectorBinOp("Subtract", "VectorFixedSubtract", true);
                    RegisterVectorBinOp("Multiply", "VectorFixedMultiply", true);
                    RegisterVectorBinOp("Divide", "VectorFixedDivide", true);

                    // ── Bitwise (no scalar needed) ──
                    RegisterVectorBinOp("BitwiseAnd", "VectorFixedBitwiseAnd", false);
                    RegisterVectorBinOp("BitwiseOr", "VectorFixedBitwiseOr", false);
                    RegisterVectorBinOp("Xor", "VectorFixedBitwiseXor", false);
                    RegisterVectorBinOp("OnesComplement", "VectorFixedOnesComplement", false);
                    RegisterVectorBinOp("AndNot", "VectorFixedBitwiseAndNot", false);
                    RegisterVectorBinOp("ConditionalSelect", "VectorFixedBitwiseSelect", false);

                    // ── Comparison (returns mask vector) ──
                    RegisterVectorBinOp("Equals", "VectorFixedCompareEqual", true);
                    RegisterVectorBinOp("GreaterThan", "VectorFixedCompareGreaterThan", true);
                    RegisterVectorBinOp("LessThan", "VectorFixedCompareLessThan", true);
                    RegisterVectorBinOp("GreaterThanOrEqual", "VectorFixedCompareGreaterThanOrEqual", true);
                    RegisterVectorBinOp("LessThanOrEqual", "VectorFixedCompareLessThanOrEqual", true);

                    // ── Vector reductions: compare + any/all ──
                    void RegisterVectorReduction(string methodName, string nativeSymbol)
                    {
                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Numerics.Vector",
                            MethodName: methodName,
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                // Determine element type suffix for the native stub name.
                                // typeArgs contains the concrete generic args from the instantiated
                                // Vector<T> type (e.g. ["System.Int32"] for Vector<int>).
                                // Map each element type to the corresponding native stub suffix.
                                string suffix = "i32"; // default for int32
                                string elemType = (typeArgs != null && typeArgs.Count > 0)
                                    ? typeArgs[0].Trim() : "";
                                if (elemType.Contains("System.Int64")) suffix = "i64";
                                else if (elemType.Contains("System.UInt64")) suffix = "u64";
                                else if (elemType.Contains("System.UInt32")) suffix = "u32";
                                else if (elemType.Contains("System.Single")) suffix = "f";
                                else if (elemType.Contains("System.Double")) suffix = "d";
                                else if (elemType.Contains("System.Int16")) suffix = "i16";
                                else if (elemType.Contains("System.UInt16")) suffix = "u16";
                                else if (elemType.Contains("System.Byte")) suffix = "u8";
                                else if (elemType.Contains("System.SByte")) suffix = "i8";

                                var typedSymbol = nativeSymbol + "_" + suffix;
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                return new GenericShapeResolution(
                                    string.Empty, symbol,
                                    [CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot()],
                                    CreateInt32AbiSlot(),
                                    EmptyRawArgumentIndices,
                                    DirectNativeSymbol: typedSymbol);
                            }));
                    }
                    RegisterVectorReduction("GreaterThanAny", "chaos_vector_greater_than_any");
                    RegisterVectorReduction("GreaterThanAll", "chaos_vector_greater_than_all");
                    RegisterVectorReduction("GreaterThanOrEqualAny", "chaos_vector_greater_than_or_equal_any");
                    RegisterVectorReduction("GreaterThanOrEqualAll", "chaos_vector_greater_than_or_equal_all");
                    RegisterVectorReduction("LessThanAny", "chaos_vector_less_than_any");
                    RegisterVectorReduction("LessThanAll", "chaos_vector_less_than_all");
                    RegisterVectorReduction("LessThanOrEqualAny", "chaos_vector_less_than_or_equal_any");
                    RegisterVectorReduction("LessThanOrEqualAll", "chaos_vector_less_than_or_equal_all");

                    // ── Shift (scalar shift amount) ──
                    RegisterVectorBinOp("ShiftLeft", "VectorFixedShiftLeft", true);
                    RegisterVectorBinOp("ShiftRightLogical", "VectorFixedShiftRightLogical", true);
                    RegisterVectorBinOp("ShiftRightArithmetic", "VectorFixedShiftRightArithmetic", true);

                    // ── Unary math ──
                    RegisterVectorUnaryOp("Abs", "VectorFixedAbs");
                    RegisterVectorUnaryOp("Negate", "VectorFixedNegate");

                    // ── Min/Max ──
                    RegisterVectorBinOp("Min", "VectorFixedMin", true);
                    RegisterVectorBinOp("Max", "VectorFixedMax", true);

                    // ── Create ──
                    void RegisterVectorCreateZero()
                    {
                        // get_Zero: VectorFixedBroadcast<TScalar, TCarrier>(0)
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: "get_Zero",
                                Resolver: static (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = chaos::il2cpp::vector_fixed::VectorFixedBroadcast<{cppType}, {carrier}>(static_cast<{cppType}>(0)); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                                }));
                        }
                    }
                    RegisterVectorCreateZero();

                    void RegisterVectorAllBitsSet()
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: "get_AllBitsSet",
                                Resolver: static (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = chaos::il2cpp::vector_fixed::VectorFixedBroadcast<{cppType}, {carrier}>(~static_cast<{cppType}>(0)); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                                }));
                        }
                    }
                    RegisterVectorAllBitsSet();

                    // ── CreateScalar / CreateScalarUnsafe ──
                    void RegisterVectorCreateScalar()
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            foreach (var methodName in new[] { "CreateScalar", "CreateScalarUnsafe" })
                            {
                                registry.RegisterInline(new InlineShapeDescriptor(
                                    TypeDisplayNamePrefix: prefix,
                                    MethodName: methodName,
                                    Resolver: static (callee, paramTypes) =>
                                    {
                                        var elemType = ExtractVectorElementType(callee, paramTypes);
                                        if (elemType == null) return null;
                                        var cppType = MapTypeArgToCppType(elemType);
                                        if (cppType == null) return null;
                                        var carrier = InferVectorCarrierType(callee);
                                        if (carrier == null) return null;
                                        return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = chaos::il2cpp::vector_fixed::VectorFixedCreateScalar<{cppType}, {carrier}>(static_cast<{cppType}>({{0}})); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                                    }));
                            }
                        }
                    }
                    RegisterVectorCreateScalar();

                    // ── GetElement / ToScalar ──
                    void RegisterVectorAccess()
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: "GetElement",
                                Resolver: static (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    return $"chaos::il2cpp::vector_fixed::VectorFixedGetElement<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), {{1}})";
                                }));
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: "ToScalar",
                                Resolver: static (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    return $"chaos::il2cpp::vector_fixed::VectorFixedGetElement<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), 0)";
                                }));
                        }
                    }
                    RegisterVectorAccess();

                    // ── Reinterpret casts (As, AsByte, AsDouble, AsInt16, etc.) ──
                    // These methods reinterpret the bit pattern of a Vector128/256 carrier
                    // as a different element type. No computation — just copy the carrier.
                    void RegisterVectorReinterpretCast()
                    {
                        var reinterpretMethods = new[]
                        {
                            "As", "AsByte", "AsDouble", "AsInt16", "AsInt32", "AsInt64",
                            "AsNInt", "AsNUInt", "AsSByte", "AsSingle", "AsUInt16",
                            "AsUInt32", "AsUInt64",
                        };
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            foreach (var methodName in reinterpretMethods)
                            {
                                registry.RegisterInline(new InlineShapeDescriptor(
                                    TypeDisplayNamePrefix: prefix,
                                    MethodName: methodName,
                                    Resolver: (callee, paramTypes) =>
                                    {
                                        var carrier = InferVectorCarrierType(callee);
                                        if (carrier == null) return null;
                                        return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = *reinterpret_cast<{carrier}*>({{0}}); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                                    }));
                            }
                        }
                    }
                    RegisterVectorReinterpretCast();

                    // ── AsVector128 / AsVector256 — cross-carrier reinterpret ──
                    // AsVector256: Vector128<T> → Vector256<T> (zero-extend lower 16→32 bytes)
                    // AsVector128: Vector256<T> → Vector128<T> (take lower 16 bytes)
                    void RegisterVectorCrossCast()
                    {
                        // Vector128<T>::AsVector256 → Vector256<T>
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Vector128",
                            MethodName: "AsVector256",
                            Resolver: static (callee, paramTypes) =>
                            {
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector256Carrier __r{{}}; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector128Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                            }));
                        // Vector256<T>::AsVector128 → Vector128<T>
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Vector256",
                            MethodName: "AsVector128",
                            Resolver: static (callee, paramTypes) =>
                            {
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector128Carrier __r; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector256Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                            }));
                    }
                    RegisterVectorCrossCast();

                    // ── Boolean predicates (unary → scalar) ──
                    // IsZero, AllWhereAllBitsSet, AnyWhereAllBitsSet, CountWhereAllBitsSet, IndexOf, etc.
                    // These return bool/int scalars, not vectors.
                    void RegisterVectorPredicate(string methodName, string exprTemplate)
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: methodName,
                                Resolver: (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    var ns = "chaos::il2cpp::vector_fixed::";
                                    var expr = exprTemplate
                                        .Replace("{NS}", ns)
                                        .Replace("{CPPTYPE}", cppType)
                                        .Replace("{CARRIER}", carrier);
                                    return expr;
                                }));
                        }
                    }

                    // IsZero — returns Vector128/256 mask (carrier), not bool
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix,
                            MethodName: "IsZero",
                            Resolver: (callee, paramTypes) =>
                            {
                                var elemType = ExtractVectorElementType(callee, paramTypes);
                                if (elemType == null) return null;
                                var cppType = MapTypeArgToCppType(elemType);
                                if (cppType == null) return null;
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = chaos::il2cpp::vector_fixed::VectorFixedIsZero<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}})); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                            }));
                    }

                    // AllWhereAllBitsSet — every lane has all bits set
                    RegisterVectorPredicate("AllWhereAllBitsSet",
                        $"[&]() -> CHAOS_IL2CPP_INTPTR {{ return static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedAllLanesSet<{{CPPTYPE}}, {{CARRIER}}>(*reinterpret_cast<{{CARRIER}}*>({{0}})) ? 1 : 0); }}()");

                    // AnyWhereAllBitsSet — any lane has all bits set
                    RegisterVectorPredicate("AnyWhereAllBitsSet",
                        $"[&]() -> CHAOS_IL2CPP_INTPTR {{ return static_cast<CHAOS_IL2CPP_INTPTR>(!{{NS}}VectorFixedIsAllZeros(*reinterpret_cast<{{CARRIER}}*>({{0}})) ? 1 : 0); }}()");

                    // CountWhereAllBitsSet — count lanes with all bits set
                    RegisterVectorPredicate("CountWhereAllBitsSet",
                        $"static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedCountWhereAllBitsSet<{{CPPTYPE}}, {{CARRIER}}>(*reinterpret_cast<{{CARRIER}}*>({{0}})))");

                    // IndexOf — first lane matching scalar param
                    RegisterVectorPredicate("IndexOf",
                        $"static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedIndexOf<{{CPPTYPE}}, {{CARRIER}}>(*reinterpret_cast<{{CARRIER}}*>({{0}}), static_cast<{{CPPTYPE}}>({{1}})))");

                    // LastIndexOf — last lane matching scalar param
                    RegisterVectorPredicate("LastIndexOf",
                        $"static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedLastIndexOf<{{CPPTYPE}}, {{CARRIER}}>(*reinterpret_cast<{{CARRIER}}*>({{0}}), static_cast<{{CPPTYPE}}>({{1}})))");

                    // IndexOfWhereAllBitsSet — index of first lane with all bits set
                    RegisterVectorPredicate("IndexOfWhereAllBitsSet",
                        $"static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedIndexOf<{{CPPTYPE}}, {{CARRIER}}>(*reinterpret_cast<{{CARRIER}}*>({{0}}), static_cast<{{CPPTYPE}}>(~static_cast<{{CPPTYPE}}>(0))))");

                    // LastIndexOfWhereAllBitsSet — index of last lane with all bits set
                    RegisterVectorPredicate("LastIndexOfWhereAllBitsSet",
                        $"static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedLastIndexOf<{{CPPTYPE}}, {{CARRIER}}>(*reinterpret_cast<{{CARRIER}}*>({{0}}), static_cast<{{CPPTYPE}}>(~static_cast<{{CPPTYPE}}>(0))))");

                    // ── Saturating arithmetic ──
                    RegisterVectorBinOp("AddSaturate", "VectorFixedAddSaturate", true);
                    RegisterVectorBinOp("SubtractSaturate", "VectorFixedSubtractSaturate", true);

                    // ── Math (float/double only) ──
                    RegisterVectorUnaryOp("Sqrt", "VectorFixedSqrt");
                    RegisterVectorUnaryOp("Ceiling", "VectorFixedCeiling");
                    RegisterVectorUnaryOp("Floor", "VectorFixedFloor");
                    RegisterVectorUnaryOp("Truncate", "VectorFixedTruncate");

                    // ── CopySign ──
                    RegisterVectorBinOp("CopySign", "VectorFixedCopySign", true);

                    // ── Min/Max variants ──
                    RegisterVectorBinOp("MaxNative", "VectorFixedMaxNative", true);
                    RegisterVectorBinOp("MinNative", "VectorFixedMinNative", true);
                    RegisterVectorBinOp("MaxMagnitude", "VectorFixedMax", true);
                    RegisterVectorBinOp("MinMagnitude", "VectorFixedMin", true);
                    RegisterVectorBinOp("MaxMagnitudeNumber", "VectorFixedMax", true);
                    RegisterVectorBinOp("MinMagnitudeNumber", "VectorFixedMin", true);
                    RegisterVectorBinOp("MaxNumber", "VectorFixedMax", true);
                    RegisterVectorBinOp("MinNumber", "VectorFixedMin", true);

                    // ── ClampNative (ternary) ──
                    RegisterVectorBinOp("ClampNative", "VectorFixedClampNative", true);

                    // ── Sum / Dot (reduction → scalar) ──
                    void RegisterVectorReductionScalar(string methodName, string templateFn)
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: methodName,
                                Resolver: (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    var ns = "chaos::il2cpp::vector_fixed::";
                                    return $"static_cast<CHAOS_IL2CPP_INTPTR>({ns}{templateFn}<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}})))";
                                }));
                        }
                    }
                    RegisterVectorReductionScalar("Sum", "VectorFixedSum");
                    RegisterVectorReductionScalar("ExtractMostSignificantBits", "VectorFixedExtractMostSignificantBits");

                    // ── Count ──
                    RegisterVectorPredicate("Count",
                        $"static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedCountWhereAllBitsSet<{{CPPTYPE}}, {{CARRIER}}>(*reinterpret_cast<{{CARRIER}}*>({{0}})))");

                    // ── ConvertTo* — these use VectorFixedConvertToVector ──
                    // Specialized inline shape that handles type conversion
                    void RegisterVectorConvertTo(string methodName)
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: methodName,
                                Resolver: (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var fromType = MapTypeArgToCppType(elemType);
                                    if (fromType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    // Determine target type from method name
                                    string? toType = methodName switch
                                    {
                                        "ConvertToInt32" or "ConvertToInt32Native" => "CHAOS_IL2CPP_INT32",
                                        "ConvertToInt64" or "ConvertToInt64Native" => "CHAOS_IL2CPP_INT64",
                                        "ConvertToUInt32" or "ConvertToUInt32Native" => "CHAOS_IL2CPP_UINT32",
                                        "ConvertToUInt64" or "ConvertToUInt64Native" => "CHAOS_IL2CPP_UINT64",
                                        "ConvertToSingle" => "float",
                                        "ConvertToDouble" => "double",
                                        _ => null
                                    };
                                    if (toType == null) return null;
                                    // Original template uses <TOutputScalar, TInputScalar, ...> ordering
                                    return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = chaos::il2cpp::vector_fixed::VectorFixedConvertToVector<{toType}, {fromType}, {carrier}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}})); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                                }));
                        }
                    }
                    RegisterVectorConvertTo("ConvertToInt32");
                    RegisterVectorConvertTo("ConvertToInt64");
                    RegisterVectorConvertTo("ConvertToUInt32");
                    RegisterVectorConvertTo("ConvertToUInt64");
                    RegisterVectorConvertTo("ConvertToSingle");
                    RegisterVectorConvertTo("ConvertToDouble");
                    RegisterVectorConvertTo("ConvertToInt32Native");
                    RegisterVectorConvertTo("ConvertToInt64Native");
                    RegisterVectorConvertTo("ConvertToUInt32Native");
                    RegisterVectorConvertTo("ConvertToUInt64Native");

                    // ── GetLower / GetUpper ──
                    // GetLower: Vector256<T> → Vector128<T> (lower 128 bits)
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector256",
                        MethodName: "GetLower",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector128Carrier __r; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector256Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));
                    // GetUpper: Vector256<T> → Vector128<T> (upper 128 bits)
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector256",
                        MethodName: "GetUpper",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector128Carrier __r; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector256Carrier*>({{0}})) + 16, 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));

                    // ── ToVector128 / ToVector256 ──
                    // ToVector128: Vector256<T> → Vector128<T> (truncate)
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector256",
                        MethodName: "ToVector128",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector128Carrier __r; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector256Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));
                    // ToVector128Unsafe: same as ToVector128
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector256",
                        MethodName: "ToVector128Unsafe",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector128Carrier __r; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector256Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));
                    // ToVector256: Vector128<T> → Vector256<T> (extend)
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector128",
                        MethodName: "ToVector256",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector256Carrier __r{{}}; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector128Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector128",
                        MethodName: "ToVector256Unsafe",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector256Carrier __r{{}}; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector128Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));

                    // ── CreateScalarUnsafe — same as CreateScalar but unchecked ──
                    // Already covered by RegisterVectorCreateScalar

                    // ── CreateSequence ──
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix,
                            MethodName: "CreateSequence",
                            Resolver: (callee, paramTypes) =>
                            {
                                var elemType = ExtractVectorElementType(callee, paramTypes);
                                if (elemType == null) return null;
                                var cppType = MapTypeArgToCppType(elemType);
                                if (cppType == null) return null;
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ " +
                                    $"constexpr CHAOS_IL2CPP_SIZE N = sizeof({carrier}) / sizeof({cppType}); " +
                                    $"{carrier} __r{{}}; " +
                                    $"auto* rl = reinterpret_cast<{cppType}*>(&__r); " +
                                    $"for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) rl[i] = static_cast<{cppType}>({{0}}) + static_cast<{cppType}>(i * static_cast<{cppType}>({{1}})); " +
                                    $"auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                            }));
                    }

                    // ── Float/double predicates (return carrier mask) ──
                    var floatPredicates = new[] {
                        "IsNaN", "IsInfinity", "IsFinite", "IsNegative", "IsPositive",
                        "IsNegativeInfinity", "IsPositiveInfinity", "IsNormal", "IsSubnormal",
                        "IsInteger", "IsEvenInteger", "IsOddInteger"
                    };
                    foreach (var methodName in floatPredicates)
                        RegisterVectorUnaryOp(methodName, $"VectorFixed{methodName}");

                    // ── FusedMultiplyAdd (ternary) ──
                    RegisterVectorBinOp("FusedMultiplyAdd", "VectorFixedFusedMultiplyAdd", true);

                    // ── Dot (binary reduction → scalar) ──
                    void RegisterVectorDot()
                    {
                        foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                        {
                            var ns = "chaos::il2cpp::vector_fixed::";
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix,
                                MethodName: "Dot",
                                Resolver: (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    return $"static_cast<CHAOS_IL2CPP_INTPTR>({ns}VectorFixedDot<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), *reinterpret_cast<{carrier}*>({{1}})))";
                                }));
                        }
                    }
                    RegisterVectorDot();

                    // ── EqualsAll / EqualsAny ──
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix, MethodName: "EqualsAll",
                            Resolver: (callee, paramTypes) =>
                            {
                                var elemType = ExtractVectorElementType(callee, paramTypes);
                                if (elemType == null) return null;
                                var cppType = MapTypeArgToCppType(elemType);
                                if (cppType == null) return null;
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto cmp = chaos::il2cpp::vector_fixed::VectorFixedCompareEqual<{cppType}, {cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), *reinterpret_cast<{carrier}*>({{1}})); return static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::vector_fixed::VectorFixedAllLanesSet<{cppType}, {carrier}>(cmp) ? 1 : 0); }}()";
                            }));
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix, MethodName: "EqualsAny",
                            Resolver: (callee, paramTypes) =>
                            {
                                var elemType = ExtractVectorElementType(callee, paramTypes);
                                if (elemType == null) return null;
                                var cppType = MapTypeArgToCppType(elemType);
                                if (cppType == null) return null;
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto cmp = chaos::il2cpp::vector_fixed::VectorFixedCompareEqual<{cppType}, {cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), *reinterpret_cast<{carrier}*>({{1}})); return static_cast<CHAOS_IL2CPP_INTPTR>(!chaos::il2cpp::vector_fixed::VectorFixedIsAllZeros(cmp) ? 1 : 0); }}()";
                            }));
                    }

                    // ── WithElement ──
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix, MethodName: "WithElement",
                            Resolver: (callee, paramTypes) =>
                            {
                                var elemType = ExtractVectorElementType(callee, paramTypes);
                                if (elemType == null) return null;
                                var cppType = MapTypeArgToCppType(elemType);
                                if (cppType == null) return null;
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                var ns = "chaos::il2cpp::vector_fixed::";
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {ns}VectorFixedWithElement<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), static_cast<CHAOS_IL2CPP_INT32>({{1}}), static_cast<{cppType}>({{2}})); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                            }));
                    }

                    // ── WithLower (V256 only) ──
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector256", MethodName: "WithLower",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = chaos::il2cpp::vector_fixed::VectorFixedWithLower(*reinterpret_cast<RuntimeIntrinsicVector256Carrier*>({{0}}), *reinterpret_cast<RuntimeIntrinsicVector128Carrier*>({{1}})); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));

                    // ── WithUpper (V256 only) ──
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector256", MethodName: "WithUpper",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = chaos::il2cpp::vector_fixed::VectorFixedWithUpper(*reinterpret_cast<RuntimeIntrinsicVector256Carrier*>({{0}}), *reinterpret_cast<RuntimeIntrinsicVector128Carrier*>({{1}})); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));

                    // ── Transcendental ──
                    RegisterVectorUnaryOp("Cos", "VectorFixedCos");
                    RegisterVectorUnaryOp("Sin", "VectorFixedSin");
                    RegisterVectorUnaryOp("Exp", "VectorFixedExp");
                    RegisterVectorUnaryOp("Round", "VectorFixedRound");

                    // ── Additional math ──
                    RegisterVectorBinOp("Hypot", "VectorFixedHypot", true);
                    RegisterVectorUnaryOp("DegreesToRadians", "VectorFixedDegreesToRadians");
                    RegisterVectorUnaryOp("RadiansToDegrees", "VectorFixedRadiansToDegrees");

                    // ── All / Any (scalar comparison) ──
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        foreach (var methodName in new[] { "All", "Any" })
                        {
                            var templateFn = methodName == "All" ? "VectorFixedAllEqual" : "VectorFixedAnyEqual";
                            registry.RegisterInline(new InlineShapeDescriptor(
                                TypeDisplayNamePrefix: prefix, MethodName: methodName,
                                Resolver: (callee, paramTypes) =>
                                {
                                    var elemType = ExtractVectorElementType(callee, paramTypes);
                                    if (elemType == null) return null;
                                    var cppType = MapTypeArgToCppType(elemType);
                                    if (cppType == null) return null;
                                    var carrier = InferVectorCarrierType(callee);
                                    if (carrier == null) return null;
                                    var ns = "chaos::il2cpp::vector_fixed::";
                                    return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ return static_cast<CHAOS_IL2CPP_INTPTR>({ns}{templateFn}<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), static_cast<{cppType}>({{1}})) ? 1 : 0); }}()";
                                }));
                        }
                    }

                    // ── LoadUnsafe ──
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix, MethodName: "LoadUnsafe",
                            Resolver: (callee, paramTypes) =>
                            {
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                var ns = "chaos::il2cpp::vector_fixed::";
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ auto __r = {ns}VectorFixedLoadUnsafe<{carrier}>({{0}}); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                            }));
                    }

                    // ── AsVector128Unsafe (same as AsVector128) ──
                    registry.RegisterInline(new InlineShapeDescriptor(
                        TypeDisplayNamePrefix: "Vector256", MethodName: "AsVector128Unsafe",
                        Resolver: static (callee, paramTypes) =>
                        {
                            return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ RuntimeIntrinsicVector128Carrier __r; memcpy(reinterpret_cast<char*>(&__r), reinterpret_cast<const char*>(reinterpret_cast<const RuntimeIntrinsicVector256Carrier*>({{0}})), 16); auto* __p = (decltype(__r)*)std::malloc(sizeof(__r)); *__p = __r; return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__p); }}()";
                        }));

                    // ── None (boolean) ──
                    RegisterVectorPredicate("None",
                        $"[&]() -> CHAOS_IL2CPP_INTPTR {{ return static_cast<CHAOS_IL2CPP_INTPTR>({{NS}}VectorFixedIsAllZeros(*reinterpret_cast<{{CARRIER}}*>({{0}})) ? 1 : 0); }}()");

                    // ── Shuffle / ShuffleNative ──
                    RegisterVectorBinOp("Shuffle", "VectorFixedShuffle", true);
                    RegisterVectorBinOp("ShuffleNative", "VectorFixedShuffle", true);

                    // ── StoreUnsafe ──
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix, MethodName: "StoreUnsafe",
                            Resolver: (callee, paramTypes) =>
                            {
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                var ns = "chaos::il2cpp::vector_fixed::";
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ {ns}VectorFixedStoreUnsafe<{carrier}>({{0}}, *reinterpret_cast<{carrier}*>({{1}})); return static_cast<CHAOS_IL2CPP_INTPTR>(0); }}()";
                            }));
                    }

                    // ── CopyTo ──
                    foreach (var prefix in new[] { "Vector64", "Vector128", "Vector256", "Vector512" })
                    {
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: prefix, MethodName: "CopyTo",
                            Resolver: (callee, paramTypes) =>
                            {
                                var elemType = ExtractVectorElementType(callee, paramTypes);
                                if (elemType == null) return null;
                                var cppType = MapTypeArgToCppType(elemType);
                                if (cppType == null) return null;
                                var carrier = InferVectorCarrierType(callee);
                                if (carrier == null) return null;
                                var ns = "chaos::il2cpp::vector_fixed::";
                                return $"[&]() -> CHAOS_IL2CPP_INTPTR {{ {ns}VectorFixedCopyTo<{cppType}, {carrier}>(*reinterpret_cast<{carrier}*>({{0}}), reinterpret_cast<{cppType}*>({{1}}), 0); return static_cast<CHAOS_IL2CPP_INTPTR>(0); }}()";
                            }));
                    }

        }

        /// <summary>
        /// Activator::CreateInstance with param array
        /// </summary>
        private static void RegisterActivatorCreateInstancewith(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Activator",
                MethodName: "CreateInstance",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    if (abiSlots.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    CHAOS_IL2CPP_FAIL();",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            EmptyRawArgumentIndices);
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Assembly::GetExecutingAssembly
        /// </summary>
        private static void RegisterAssemblyGetExecutingAssembly(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "GetExecutingAssembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetExecutingAssembly",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// Type::MakeGenericType
        /// </summary>
        private static void RegisterTypeMakeGenericType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "MakeGenericType", ["System.Type[]"],
                ShapeKind.SimpleForward, "ChaosReflectionMakeGenericType",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

    }
}
