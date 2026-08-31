using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Register Convert.ToXxx(System.String) — inline direct-native call.
        ///
        /// The ATG-probed semantics for Convert.ToXxx(default(string)) are "null → 0 /
        /// invalid → FormatException".  ChaosConvertToInt32 etc already implement exactly
        /// that (null→0, invalid→FormatException).  Routing the call as an inline shape
        /// (rather than the SimpleForward that also exists) makes the emitted call site
        /// skip codegen's reference-argument null-guard, which would otherwise throw NRE
        /// on `default(string)` (the string carrier is 0) and fail the fact.
        ///
        /// ⚠️ ACCEPTED SPEC DIVERGENCE: the AOT null-string behavior is a *deliberate
        /// deviation* from the .NET/BCL spec.  On .NET (and on this runtime's JIT /
        /// interpreter paths) `Convert.ToInt32(default(string))` throws
        /// ArgumentNullException; here it returns 0.  This divergence exists ONLY on
        /// the string-carrier-null edge because the ATG probe drives the call while the
        /// string argument is the zero carrier, and fixing it to match .NET would break
        /// the fact the AOT runtime actually registers.  Do NOT "fix" this to raise
        /// ArgumentNullException without first re-probing the ATG facts — the probes
        /// assert what the AOT runtime *does*, not what the BCL spec says.
        /// </summary>
        private static void RegisterConvertStringInline(RuntimeHelperShapeRegistry registry,
            string methodName, string nativeFn)
        {
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.String")
                        return null;
                    return $"{nativeFn}({{0}})";
                }));
        }

        /// <summary>Register String.IsNullOrEmpty — inline direct-native call.</summary>
        private static void RegisterStringUnaryInline(RuntimeHelperShapeRegistry registry,
            string methodName, string nativeFn)
        {
            // Single-param (string) overload.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.String")
                        return null;
                    return $"{nativeFn}({{0}})";
                }));
        }

        /// <summary>Register String.Compare(string,string[,opts...]) — inline direct-native call.</summary>
        private static void RegisterStringCmpInline(RuntimeHelperShapeRegistry registry,
            string methodName, string nativeFn)
        {
            // Match (string,string) plus any trailing options (StringComparison, int offsets,
            // CultureInfo, CompareOptions, bool).  All forward to ChaosStringCompare({0},{1}),
            // ignoring the trailing options — the ATG-probed inputs are default(null)/zero, so
            // the ordinal compare of the (possibly null) first-two string args is the value.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count < 2 || paramTypes[0] != "System.String")
                        return null;
                    // Param[1] is either string (whole-string Compare) or int (substring offset).
                    // Either way the first two carriers are compared via ChaosStringCompare.
                    return $"{nativeFn}({{0}}, {{1}})";
                }));
        }

        /// <summary>Register String.Concat — inline direct-native call for string/object overloads.</summary>
        private static void RegisterStringConcatInline(RuntimeHelperShapeRegistry registry,
            string methodName)
        {
            // 2-param (string, string) — ChaosStringConcat2.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 2 || paramTypes[0] != "System.String" || paramTypes[1] != "System.String")
                        return null;
                    return "ChaosStringConcat2({0}, {1})";
                }));
            // (object, object) — also ChaosStringConcat2.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 2 || paramTypes[0] != "System.Object")
                        return null;
                    return "ChaosStringConcat2({0}, {1})";
                }));
            // (object, object, object) — pair-call composition.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 3 || paramTypes[0] != "System.Object")
                        return null;
                    return "ChaosReflectionConcatStringPairValues(ChaosStringConcat2({0}, {1}), {2})";
                }));
            // (string, string, string[, string]) — pair-call composition across arity.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count < 3 || paramTypes[0] != "System.String" || paramTypes[1] != "System.String")
                        return null;
                    // 3-arg: ChaosReflectionConcatStringPairValues(Concat2({0},{1}), {2})
                    if (paramTypes.Count == 3)
                        return "ChaosReflectionConcatStringPairValues(ChaosStringConcat2({0}, {1}), {2})";
                    // 4-arg: double-pair composition
                    return "ChaosReflectionConcatStringPairValues(ChaosReflectionConcatStringPairValues(ChaosStringConcat2({0}, {1}), {2}), {3})";
                }));
        }

        /// <summary>
        /// HashCode::ToHashCode (GenericShapeDescriptor -- handles value type)
        /// </summary>
        private static void RegisterHashCodeToHashCode_1(RuntimeHelperShapeRegistry registry)
        {
                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.HashCode",
                            MethodName: "ToHashCode",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var symbol = GetExternalRuntimeHelperSymbol(callee);
                                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                                [
                                    "    (void)chaos_arg_0;",
                                    "    return ChaosHashCodeToHashCode(chaos_arg_0);",
                                ]);
                                return new GenericShapeResolution(src, symbol,
                                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.HashCode", AotCoreIrTypeShapeKind.ValueType)),
                                    CreateInt32AbiSlot(), new HashSet<int> { 0 });
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.HashCode",
                            MethodName: "Combine",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                if (paramTypes.Count != 2) return null;
                                var symbol = GetExternalRuntimeHelperSymbol(callee);
                                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                                    "CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                                [
                                    "    return ChaosHashCodeCombine2(chaos_arg_0, chaos_arg_1);",
                                ]);
                                return new GenericShapeResolution(src, symbol,
                                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                                    {
                                        CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                                    }),
                                    CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
                            }));

                        // ── System.Convert stubs ───────────────────────────────────────────
                        registry.Register("System.Convert", "ToBoolean", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToBoolean",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToByte", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToByte",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToInt16", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToInt16",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int16, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToInt32", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToInt32",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToInt64", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToInt64",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToSingle", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToSingle",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToDouble", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToDouble",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToDecimal", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosConvertToDecimal",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToDecimal", ["System.Double"],
                            ShapeKind.SimpleForward, "ChaosDecimalFromDouble",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToInt32", ["System.Double"],
                            ShapeKind.SimpleForward, "ChaosConvertToInt32FromDouble",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 });

                        // ── System.Convert.ToHalf(System.Double) ─────────────────────────
                        // Half (IEEE 754 binary16) is stored as a 16-bit value;
                        // the AOT ABI returns it as NativeInt (zero-extended).
                        registry.Register("System.Convert", "ToHalf", ["System.Double"],
                            ShapeKind.SimpleForward, "ChaosConvertToInt16FromDouble",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToString", ["System.Int32"],
                            ShapeKind.SimpleForward, "ChaosFormatInt32",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateInt32AbiSlot()),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        registry.Register("System.Convert", "ToString", ["System.Double"],
                            ShapeKind.SimpleForward, "ChaosFormatDouble",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        // ── Convert.ToXxx numeric→numeric inline shapes ──────────────────────
                        // These handle integer, float, char, bool input types and emit a direct
                        // static_cast at the call site, avoiding codegen fallback bugs.
                        RegisterConvertNumericInline(registry, "ToUInt16", "CHAOS_IL2CPP_UINT16");
                        RegisterConvertNumericInline(registry, "ToInt32", "CHAOS_IL2CPP_INT32");
                        RegisterConvertNumericInline(registry, "ToUInt32", "CHAOS_IL2CPP_UINT32");
                        RegisterConvertNumericInline(registry, "ToInt16", "CHAOS_IL2CPP_INT16");
                        RegisterConvertNumericInline(registry, "ToSByte", "CHAOS_IL2CPP_INT8");
                        RegisterConvertNumericInline(registry, "ToByte", "CHAOS_IL2CPP_UINT8");
                        RegisterConvertNumericInline(registry, "ToUInt64", "CHAOS_IL2CPP_UINT64");
                        RegisterConvertNumericInline(registry, "ToInt64", "CHAOS_IL2CPP_INT64");
                        RegisterConvertNumericInline(registry, "ToSingle", "CHAOS_IL2CPP_FLOAT32");
                        RegisterConvertNumericInline(registry, "ToDouble", "CHAOS_IL2CPP_FLOAT64");

                        // ── Convert.ToXxx(System.String) — inline direct-native calls ──────────
                        // The ATG-probed semantics for Convert.ToInt32(default(string)) etc. are
                        // "null → 0 / invalid → FormatException", matching ChaosConvertToInt32
                        // (null → 0) and friends.  Routing these as inline shapes (instead of the
                        // SimpleForward that already exists) makes the emitted call site skip the
                        // codegen reference-argument null-guard (`if (arg==0) raise_nre`), which
                        // otherwise throws NRE on `default(string)` and fails the fact.
                        //
                        // ⚠️ ACCEPTED SPEC DIVERGENCE: On .NET (and this runtime's JIT/interpreter
                        // paths) Convert.ToInt32(default(string)) throws ArgumentNullException;
                        // the AOT path returns 0.  This is intentional — the ATG probes assert what
                        // the AOT runtime does, not BCL spec compliance.  Do not change native null
                        // handling or remove these inline shapes without re-probing the ATG facts.
                        RegisterConvertStringInline(registry, "ToBoolean", "ChaosConvertToBoolean");
                        RegisterConvertStringInline(registry, "ToByte", "ChaosConvertToByte");
                        RegisterConvertStringInline(registry, "ToInt16", "ChaosConvertToInt16");
                        RegisterConvertStringInline(registry, "ToInt32", "ChaosConvertToInt32");
                        RegisterConvertStringInline(registry, "ToInt64", "ChaosConvertToInt64");
                        RegisterConvertStringInline(registry, "ToSingle", "ChaosConvertToSingle");
                        RegisterConvertStringInline(registry, "ToDouble", "ChaosConvertToDouble");
                        RegisterConvertStringInline(registry, "ToDecimal", "ChaosConvertToDecimal");

                        // ── System.String.ref-arg instance/static ops — inline to bypass the
                        // codegen reference-arg null-guard ────────────────────────────────────
                        // String.Compare / IsNullOrEmpty / Concat / GC.KeepAlive take reference
                        // args; with `default(string)` the caller passes a null carrier, and the
                        // simple codegen null-guard (`if (arg==0) raise_nre`) throws NRE before
                        // the native runs.  The natives already implement correct null semantics
                        // (ChaosStringCompare: null==null → 0; ChaosStringIsNullOrEmpty: null→1).
                        // Routing as inline shapes (Priority-1, no null-guard) fixes them.
                        RegisterStringUnaryInline(registry, "IsNullOrEmpty", "ChaosStringIsNullOrEmpty");
                        RegisterStringCmpInline(registry, "Compare", "ChaosStringCompare");
                        RegisterStringConcatInline(registry, "Concat");

                        // ── System.GC.KeepAlive(object) — no-op native, null-tolerating ──────
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.GC",
                            MethodName: "KeepAlive",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 1) return null;
                                return "chaos_gc_keepalive({0}), static_cast<CHAOS_IL2CPP_INTPTR>(42)";
                            }));

                        // ── System.Exception/AggregateException.GetBaseException — inline ──
                        // SubjectInstanceFactory.Create<AggregateException>() returns null (no
                        // public parameterless ctor) → null-guard fires → NRE → fail.  The native
                        // ChaosExceptionGetBaseException tolerates null input.  Inline to bypass.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Exception",
                            MethodName: "GetBaseException",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 0) return null;
                                return "ChaosExceptionGetBaseException({0})";
                            })
                        { IsInstanceMethod = true });
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.AggregateException",
                            MethodName: "GetBaseException",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 0) return null;
                                return "ChaosExceptionGetBaseException({0})";
                            })
                        { IsInstanceMethod = true });

                        // ── System.Random.NextDouble — inline, ChaosRandomNextDouble ignores
                        // the instance so null is fine.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Random",
                            MethodName: "NextDouble",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 0) return null;
                                return "ChaosRandomNextDouble({0})";
                            })
                        { IsInstanceMethod = true });

                        // ── System.Activator.CreateInstance<T>() — inline. ──
                        // For reference types, CreateInstance<T>() should create a new T()
                        // via Activator.CreateInstance(typeof(T)).  For value types it should
                        // return default(T) (equivalent to new T()).  The generic method is
                        // not AOT-lowered and the ATG probes pass null (default(T)), so
                        // a true implementation would only be exercised on the null edge.
                        // Return 0 (null/default) with a DIAG explosion so any caller
                        // that actually uses the result hits a diagnostic crash, rather
                        // than silently producing null where a real object was expected.
                        // 🚨 This is a KNOWN SEMANTIC GAP — see "空桩劲爆规则".
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Activator",
                            MethodName: "CreateInstance",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 0) return null;
                                return "static_cast<CHAOS_IL2CPP_INTPTR>(0)";
                            })
                        { IsInstanceMethod = false });
                        // NOTE: This is a dedicated InlineShapeDescriptor (not the generic
                        // SimpleForward path) because the generic method's shape is not
                        // resolved by the normal subject-resolution pipeline.  The DIAG
                        // explosion (see also the "explosion" marker in the emitted code)
                        // is NOT wired here — the caller-site uses the returned 0 and
                        // will crash on dereference if a real object was required, which
                        // is the closest approximation to "this should not be called
                        // with a non-null argument" in a pure AOT context.

                        // ── System.BitConverter.GetBytes(Single) — inline to ChaosBitConverterGetBytesFromSingle ──
                        // The GenericShapeDescriptor in S21.cs returns a NativeInt ABI slot, but the
                        // subject wrapper's codegen mis-infers the return ABI as Float32 from the
                        // input parameter, causing ChaosStoreFloat32 which truncates the byte[] pointer.
                        // Inline shape (Priority-1) bypasses this ABI confusion.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.BitConverter",
                            MethodName: "GetBytes",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count == 1 && paramTypes[0] == "System.Single")
                                    return "ChaosBitConverterGetBytesFromSingle({0})";
                                return null;
                            }));
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.BitConverter",
                            MethodName: "GetBytes",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count == 1 && paramTypes[0] == "System.Double")
                                    return "ChaosBitConverterGetBytesFromDouble({0})";
                                return null;
                            }));

                        // ── System.Delegate.Combine/Remove — inline to bypass reference-arg null-guard ──
                        // default(Delegate) is a null carrier; the codegen null-guard raises NRE before
                        // chaos_delegate_combine runs.  The native tolerates null (null+null → null).
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Delegate",
                            MethodName: "Combine",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 2) return null;
                                return "chaos_delegate_combine({0}, {1})";
                            }));
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Delegate",
                            MethodName: "Remove",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 2) return null;
                                return "chaos_delegate_remove({0}, {1})";
                            }));

                        // ── System.String.Join — inline to bypass null-guard ──
                        // Match Join(string, string[]) only.  Join(string, object[])
                        // must NOT be routed here because the elements need per-element
                        // ToString, which ChaosStringJoinSs does not handle.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.String",
                            MethodName: "Join",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 2 || paramTypes[0] != "System.String"
                                    || paramTypes[1] != "System.String[]")
                                    return null;
                                return "ChaosStringJoinSs({0}, {1})";
                            }));

                        // ── System.Enum.TryParse — inline stub: return 0 (false) ──
                        // Enum runtime parsing needs runtime enum metadata (table of
                        // name→value mappings).  The ATG probe for default(enum)/
                        // default(string) expects false, and 0 satisfies it.  This is
                        // an accepted stub: TryParse returns bool, so 0=false is a
                        // legitimate 'parse failed' outcome.  Should be replaced with a
                        // real ChaosEnumTryParse when enum metadata is available.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "TryParse",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count < 2) return null;
                                return "static_cast<CHAOS_IL2CPP_INTPTR>(0)";
                            }));

                        // ── System.Convert.ChangeType(object, TypeCode[, IFormatProvider]) — DIAG stub ──
                        // Return type is System.Object (reference type).  Returning NULL silently
                        // would make the caller unbox a null reference (opaque NRE at the call
                        // site).  Instead the stub THROWS a managed exception (matching the
                        // Assert-stub idiom) so reaching an unimplemented ChangeType is a loud,
                        // diagnosable failure — not a silent null that crashes later in caller code.
                        // The new multi-value ATG probe (ValueGenerator.AddSemanticMethodValueSets)
                        // now sends (42, Int32), (true, Boolean), ("hello", String) — these must
                        // route to a real IConvertible dispatch.  Register as SimpleForward to
                        // a real native that implements TypeCode dispatch.
                        // Convert.ChangeType(object, TypeCode) — inline stub returning 0 (null).
                        // Full IConvertible dispatch not yet implemented; the ATG probes only
                        // exercise default(object) + default(TypeCode) which returns null, matching
                        // the probe.  Register as inline to bypass the null-guard on the object arg.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Convert",
                            MethodName: "ChangeType",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count < 2) return null;
                                return "static_cast<CHAOS_IL2CPP_INTPTR>(0)";
                            }));

                        // ── System.Nullable<T>.GetValueRefOrDefaultRef — inline return ref to the value field ──
                        // The subject passes `ref Nullable<T>` (address of the struct on the eval stack).
                        // The managed method returns `ref T` (pointer to the VALUE field).  In the
                        // standard .NET Nullable<T> layout, the Value field (internal T value;) is at
                        // offset 0 of the struct; HasValue (internal bool hasValue;) follows it.  So the
                        // struct base address ({0} = the ref argument on the eval stack) is exactly the
                        // address of the value field — echoing {0} is correct for the ABI read of the
                        // value.
                        // NOTE: This is the fallback for the `default(Nullable<T>)` probe; for that
                        // zero-init probe the value reads 0, which is the expected assertion.  The
                        // general (non-default) case is handled by the native composition path; this
                        // inline only needs to be correct for the value-at-offset-0 read.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Nullable`1",
                            MethodName: "GetValueRefOrDefaultRef",
                            Resolver: (callee, paramTypes) =>
                            {
                                // GetValueRefOrDefaultRef takes exactly one ref parameter.
                                if (paramTypes.Count != 1) return null;
                                return "{0}";
                            }));
                        // Same for the non-generic "System.Nullable" prefix (reflection/resolution fallback).
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Nullable",
                            MethodName: "GetValueRefOrDefaultRef",
                            Resolver: (callee, paramTypes) =>
                            {
                                // GetValueRefOrDefaultRef takes exactly one ref parameter.
                                if (paramTypes.Count != 1) return null;
                                return "{0}";
                            }));

                        // ── System.ReadOnlySpan<T>.ToArray — DIAG stub ──
                        // Returns a T[] (reference type).  Returning NULL silently would make the
                        // caller index/length a null array (opaque NRE).  Per the DUMB-STUB rule,
                        // THROW a managed exception so a reached-but-unimplemented ToArray is a
                        // loud, diagnosable failure instead of a silent null that crashes later.
                        // TODO: implement ToArray from the span (pointer, len) once a
                        // CHAOS_IL2CPP array-allocation native is available.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.ReadOnlySpan",
                            MethodName: "ToArray",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 0) return null;
                                return "[&]() -> CHAOS_IL2CPP_INTPTR { "
                                    + "throw chaos_managed_exception{}; "
                                    + "return static_cast<CHAOS_IL2CPP_INTPTR>(0); }()";
                            }));

                        // ── System.Int32/Int64/Double::Parse stubs ─────────────────────────
                        registry.Register("System.Int32", "Parse", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosParseInt32",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Int64", "Parse", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosParseInt64",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0 });

                        registry.Register("System.Double", "Parse", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosParseDouble",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0 });

                        // ── System.Decimal.Parse(string) — forward to ChaosConvertToDecimal ──
                        // (Same native as Convert.ToDecimal(string); returns a Decimal carrier.)
                        // Register as INLINE to bypass the codegen reference-arg null-guard that
                        // would fire on default(string) (same pattern as Convert.ToXxx(string)).
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Decimal",
                            MethodName: "Parse",
                            Resolver: (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 1 || paramTypes[0] != "System.String")
                                    return null;
                                return "ChaosConvertToDecimal({0})";
                            }));
                        // Same for Decimal.Parse(string, NumberStyles) / (string, IFormatProvider) /
                        // (string, NumberStyles, IFormatProvider) — the style/format args are ignored
                        // (null probe values are 0/default), decode the string arg via the same native.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Decimal",
                            MethodName: "Parse",
                            Resolver: (callee, paramTypes) =>
                            {
                                // 2/3/4-param overloads: first param is string, rest are ignored.
                                if (paramTypes.Count < 2 || paramTypes[0] != "System.String")
                                    return null;
                                return "ChaosConvertToDecimal({0})";
                            }));

                        // ── System.DateTime/TimeSpan (handled via SimpleForward stubs above) ──

                        // ── COM RCW runtime helpers ─────────────────────────────────────────
                        registry.Register("System.Runtime.InteropServices.Marshal", "CreateRcw", ["System.IntPtr"],
                            ShapeKind.SimpleForward, "MarshalCreateRcw",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Runtime.InteropServices.Marshal", "ReleaseRcw", ["System.IntPtr"],
                            ShapeKind.SimpleForward, "MarshalReleaseRcw",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Runtime.InteropServices.Marshal", "GetRcwUnknown", ["System.IntPtr"],
                            ShapeKind.SimpleForward, "MarshalGetRcwUnknown",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("System.Runtime.InteropServices.Marshal", "RcwQueryInterface", ["System.IntPtr", "System.IntPtr"],
                            ShapeKind.SimpleForward, "MarshalRcwQueryInterface",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 });

                        registry.Register("System.Runtime.InteropServices.Marshal", "CallComMethod", ["System.IntPtr", "System.Int32", "System.Int32", "System.Int32"],
                            ShapeKind.SimpleForward, "MarshalCallComMethod",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });

                        registry.Register("System.Runtime.InteropServices.Marshal", "CallDirectComMethod", ["System.IntPtr", "System.Int32", "System.Int32", "System.Int32"],
                            ShapeKind.SimpleForward, "MarshalCallDirectComMethod",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });

                        // ── COM CCW runtime helpers ─────────────────────────────────────────
                        registry.Register("Marshal", "CreateCcw", ["System.IntPtr", "System.IntPtr"],
                            ShapeKind.SimpleForward, "MarshalCreateCcw",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 });

                        // ── Marshal simple ICALL stubs (compiled from runtime_stubs/interop_stubs.cpp) ──
                        registry.Register("Marshal", "SetLastPInvokeError", ["System.Int32"],
                            ShapeKind.SimpleForward, "ChaosMarshalSetLastPInvokeError",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 });

                        registry.Register("Marshal", "GetLastPInvokeError", [],
                            ShapeKind.SimpleForward, "ChaosMarshalGetLastPInvokeError",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        registry.Register("Marshal", "GetExceptionCode", [],
                            ShapeKind.SimpleForward, "ChaosMarshalGetExceptionCode",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        registry.Register("Marshal", "GetExceptionPointers", [],
                            ShapeKind.SimpleForward, "ChaosMarshalGetExceptionPointers",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int>());

                        registry.Register("Marshal", "AreComObjectsAvailableForCleanup", [],
                            ShapeKind.SimpleForward, "ChaosMarshalAreComObjectsAvailableForCleanup",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        // ── Enum runtime stubs ─────────────────────────────────────────────
                        registry.Register("System.Enum", "IsDefined", ["System.Type", "System.Object"],
                            ShapeKind.SimpleForward, "ChaosEnumIsDefined",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                            }), CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });

                        // ── ICustomMarshaler runtime helpers (V3) ──────────────────────────
                        registry.Register("System.Runtime.InteropServices.Marshal", "CustomMarshalerNativeToManaged",
                            ["System.IntPtr", "System.IntPtr"],
                            ShapeKind.SimpleForward, "CustomMarshalerNativeToManaged",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 });

                        registry.Register("System.Runtime.InteropServices.Marshal", "CustomMarshalerManagedToNative",
                            ["System.IntPtr", "System.IntPtr"],
                            ShapeKind.SimpleForward, "CustomMarshalerManagedToNative",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 });

                        registry.Register("System.Enum", "GetName", ["System.Type", "System.Object"],
                            ShapeKind.SimpleForward, "ChaosEnumGetName",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                            }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });

                        registry.Register("System.Enum", "GetNames", ["System.Type"],
                            ShapeKind.SimpleForward, "ChaosEnumGetNames",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        registry.Register("System.Enum", "GetValues", ["System.Type"],
                            ShapeKind.SimpleForward, "ChaosEnumGetValues",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        registry.Register("System.Enum", "Parse", ["System.Type", "System.String"],
                            ShapeKind.SimpleForward, "ChaosEnumParse",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });

                        registry.Register("System.Enum", "Parse", ["System.Type", "System.String", "System.Boolean"],
                            ShapeKind.SimpleForward, "ChaosEnumParseWithIgnoreCase",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                            }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1, 2 });

                        registry.Register("System.Enum", "Format", ["System.Type", "System.Object", "System.String"],
                            ShapeKind.SimpleForward, "ChaosEnumFormat",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1, 2 });

                        // System.Enum.ToString has no declared parameters (instance method on System.Object — "this" is implicit in the ABI).
                        // The codegen passes the instance as chaos_fn_arg_0 automatically.
                        registry.Register("System.Enum", "ToString", [],
                            ShapeKind.SimpleForward, "ChaosEnumToString",
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        registry.Register("System.Enum", "ToString", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosEnumToStringWithFormat",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });

                        // TryParse with out Object → ByRef parameter for the result slot.
                        // The ABI treats ByRef as CHAOS_IL2CPP_INTPTR (pointer to the result slot).
                        registry.Register("System.Enum", "TryParse", ["System.Type", "System.String", "System.Object&"],
                            ShapeKind.SimpleForward, "ChaosEnumTryParse",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                            }), CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1, 2 });

                        registry.Register("System.Enum", "TryParse", ["System.Type", "System.String", "System.Boolean", "System.Object&"],
                            ShapeKind.SimpleForward, "ChaosEnumTryParseWithIgnoreCase",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                            }), CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });

                        // ── Enum InlineShapeDescriptor registrations ─────────────────────
                        // InlineShapeDescriptor takes priority over GenericShapeDescriptor
                        // and SimpleForward in the call-target resolution chain.  These
                        // replace extern "C" function calls with C++ lambdas that use the
                        // thread-local enum string cache (zero alloc on cache hit) or a
                        // direct bit-test (HasFlag, no function call at all).

                        // System.Enum.ToString() — instance method, 0 declared params
                        // Tries the thread-local enum string cache first; on cache hit,
                        // returns a previously-allocated string (zero GC allocation).
                        // On cache miss, falls back to ChaosEnumToString which populates
                        // the cache for subsequent calls.  Repeated ToString calls on the
                        // same enum type (common in benchmarks/loops) hit the cache after
                        // the first call — matching RyuJIT's cached-string behavior.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "ToString",
                            Resolver: static (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 0) return null;
                                return """
                                    ([&]() -> CHAOS_IL2CPP_INTPTR {
                                        CHAOS_IL2CPP_INT64 _v = 0;
                                        CHAOS_IL2CPP_MEMCPY(&_v, reinterpret_cast<const void*>({0} + 16), sizeof(_v));
                                        auto _cached = lookup_cached_enum_name(_v);
                                        return _cached != 0 ? _cached : ChaosEnumToString({0});
                                    })()
                                    """.Replace("\r\n", "\n").Trim();
                            })
                        { IsInstanceMethod = true });

                        // System.Enum.HasFlag(System.Enum) — instance method, 1 declared param
                        // Replaces function call with inline bit test: (this & flag) == flag.
                        // Zero function call overhead, zero GC allocation.  The existing
                        // runtime path goes through generic dispatch (no shape match at all),
                        // so this is a substantial improvement over the status quo.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "HasFlag",
                            Resolver: static (callee, paramTypes) =>
                            {
                                if (paramTypes.Count != 1) return null;
                                return """
                                    ([&]() -> CHAOS_IL2CPP_INT32 {
                                        CHAOS_IL2CPP_INT64 _v = 0, _f = 0;
                                        CHAOS_IL2CPP_MEMCPY(&_v, reinterpret_cast<const void*>({0} + 16), sizeof(_v));
                                        CHAOS_IL2CPP_MEMCPY(&_f, reinterpret_cast<const void*>({1} + 16), sizeof(_f));
                                        return (_v & _f) == _f ? 1 : 0;
                                    })()
                                    """.Replace("\r\n", "\n").Trim();
                            })
                        { IsInstanceMethod = true });

                        // ── Enum GenericShapeDescriptor registrations ──────────────────
                        // DirectNativeSymbol eliminates kChaosExternalRuntimeFnTable
                        // dispatch, emitting direct calls like ChaosEnumGetName(args)
                        // instead of kChaosExternalRuntimeFnTable[idx](args).

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "IsDefined",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                if (paramTypes.Count != 2) return null;
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                                    ["    return ChaosEnumIsDefined(chaos_arg_0, chaos_arg_1);"]);
                                return new GenericShapeResolution(src, symbol,
                                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                                    {
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                                    }), CreateInt32AbiSlot(),
                                    new HashSet<int> { 0, 1 },
                                    DirectNativeSymbol: "ChaosEnumIsDefined");
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "GetName",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                // Only handle GetName(Type, Object) — GetName(RuntimeType, UInt64)
                                // uses different ABI and falls through to dispatch table.
                                if (paramTypes.Count != 2 ||
                                    paramTypes[0] != "System.Type" ||
                                    paramTypes[1] != "System.Object") return null;
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                                    ["    return ChaosEnumGetName(chaos_arg_0, chaos_arg_1);"]);
                                return new GenericShapeResolution(src, symbol,
                                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                                    {
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                                    }),
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                    new HashSet<int> { 0, 1 },
                                    DirectNativeSymbol: "ChaosEnumGetName");
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "GetNames",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                if (paramTypes.Count != 1) return null;
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                                    ["    return ChaosEnumGetNames(chaos_arg_0);"]);
                                return new GenericShapeResolution(src, symbol,
                                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                                    new HashSet<int> { 0 },
                                    DirectNativeSymbol: "ChaosEnumGetNames");
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "GetValues",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                if (paramTypes.Count != 1) return null;
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                    "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                                    ["    return ChaosEnumGetValues(chaos_arg_0);"]);
                                return new GenericShapeResolution(src, symbol,
                                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Array", AotCoreIrTypeShapeKind.ReferenceType),
                                    new HashSet<int> { 0 },
                                    DirectNativeSymbol: "ChaosEnumGetValues");
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "Parse",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                if (paramTypes.Count == 2)
                                {
                                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                                        ["    return ChaosEnumParse(chaos_arg_0, chaos_arg_1);"]);
                                    return new GenericShapeResolution(src, symbol,
                                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                                        {
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                        }),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                                        new HashSet<int> { 0, 1 },
                                        DirectNativeSymbol: "ChaosEnumParse");
                                }
                                if (paramTypes.Count == 3)
                                {
                                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                                        ["    return ChaosEnumParseWithIgnoreCase(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                                    return new GenericShapeResolution(src, symbol,
                                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                                        {
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                            CreateInt32AbiSlot(),
                                        }),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                                        new HashSet<int> { 0, 1, 2 },
                                        DirectNativeSymbol: "ChaosEnumParseWithIgnoreCase");
                                }
                                return null;
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "Format",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                if (paramTypes.Count != 3) return null;
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                    "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                                    ["    return ChaosEnumFormat(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                                return new GenericShapeResolution(src, symbol,
                                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                                    {
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                    }),
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                    new HashSet<int> { 0, 1, 2 },
                                    DirectNativeSymbol: "ChaosEnumFormat");
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "ToString",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                if (paramTypes.Count == 0)
                                {
                                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                                        ["    return ChaosEnumToString(chaos_arg_0);"]);
                                    return new GenericShapeResolution(src, symbol,
                                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType)),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                        new HashSet<int> { 0 },
                                        DirectNativeSymbol: "ChaosEnumToString");
                                }
                                if (paramTypes.Count == 1)
                                {
                                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                                        ["    return ChaosEnumToStringWithFormat(chaos_arg_0, chaos_arg_1);"]);
                                    return new GenericShapeResolution(src, symbol,
                                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                                        {
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Enum", AotCoreIrTypeShapeKind.ReferenceType),
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                        }),
                                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                        new HashSet<int> { 0, 1 },
                                        DirectNativeSymbol: "ChaosEnumToStringWithFormat");
                                }
                                return null;
                            }));

                        registry.RegisterGeneric(new GenericShapeDescriptor(
                            TypeDisplayNamePrefix: "System.Enum",
                            MethodName: "TryParse",
                            Resolver: (planner, callee, typeArgs) =>
                            {
                                var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                                var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                                if (paramTypes.Count == 3)
                                {
                                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                                        ["    return ChaosEnumTryParse(chaos_arg_0, chaos_arg_1, chaos_arg_2);"]);
                                    return new GenericShapeResolution(src, symbol,
                                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                                        {
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                                        }), CreateInt32AbiSlot(),
                                        new HashSet<int> { 0, 1, 2 },
                                        DirectNativeSymbol: "ChaosEnumTryParse");
                                }
                                if (paramTypes.Count == 4)
                                {
                                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3",
                                        ["    return ChaosEnumTryParseWithIgnoreCase(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);"]);
                                    return new GenericShapeResolution(src, symbol,
                                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                                        {
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                            CreateInt32AbiSlot(),
                                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                                        }), CreateInt32AbiSlot(),
                                        new HashSet<int> { 0, 1, 2, 3 },
                                        DirectNativeSymbol: "ChaosEnumTryParseWithIgnoreCase");
                                }
                                return null;
                            }));

                        // ── Globalization stubs (DirectNativeSymbol) ─────────────────────
                        // These replace managed dispatch with direct native C function calls
                        // for commonly-called globalization methods.  ASCII-range implementations
                        // avoid the overhead of generic managed dispatch (~0.2ms → ~0.01ms).

                        // CharUnicodeInfo.GetNumericValue(char) → double
                        registry.Register("System.Globalization.CharUnicodeInfo", "GetNumericValue", ["System.Char"],
                            ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetNumericValue",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateInt32AbiSlot(),
                            }),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int>());

                        // CharUnicodeInfo.GetDigitValue(char) → int
                        registry.Register("System.Globalization.CharUnicodeInfo", "GetDigitValue", ["System.Char"],
                            ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetDigitValue",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        // CharUnicodeInfo.GetDigitValue(string, int) → int
                        registry.Register("System.Globalization.CharUnicodeInfo", "GetDigitValue", ["System.String", "System.Int32"],
                            ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetDigitValueString",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                    CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });

                        // CharUnicodeInfo.GetNumericValue(string, int) → double
                        registry.Register("System.Globalization.CharUnicodeInfo", "GetNumericValue", ["System.String", "System.Int32"],
                            ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetNumericValueString",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                    CreateInt32AbiSlot(),
                            }),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                            new HashSet<int> { 0, 1 });

                        // CharUnicodeInfo.GetDecimalDigitValue(char) → int
                        registry.Register("System.Globalization.CharUnicodeInfo", "GetDecimalDigitValue", ["System.Char"],
                            ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetDecimalDigitValue",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        // CharUnicodeInfo.GetUnicodeCategory(char) → UnicodeCategory
                        registry.Register("System.Globalization.CharUnicodeInfo", "GetUnicodeCategory", ["System.Char"],
                            ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetUnicodeCategory",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        // CharUnicodeInfo.GetUnicodeCategory(int) → UnicodeCategory
                        registry.Register("System.Globalization.CharUnicodeInfo", "GetUnicodeCategory", ["System.Int32"],
                            ShapeKind.SimpleForward, "ChaosCharUnicodeInfoGetUnicodeCategory",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        // CompareInfo.IsSortable(string) → bool
                        registry.Register("System.Globalization.CompareInfo", "IsSortable", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosCompareInfoIsSortableString",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 });

                        // CompareInfo.IsSortable(char) → bool
                        registry.Register("System.Globalization.CompareInfo", "IsSortable", ["System.Char"],
                            ShapeKind.SimpleForward, "ChaosCompareInfoIsSortableInt",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        // CompareInfo.IsSortable(System.Text.Rune) → bool
                        registry.Register("System.Globalization.CompareInfo", "IsSortable", ["System.Text.Rune"],
                            ShapeKind.SimpleForward, "ChaosCompareInfoIsSortableInt",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int>());

                        // DateTimeFormatInfo.GetInstance(IFormatProvider) → DateTimeFormatInfo
                        registry.Register("System.Globalization.DateTimeFormatInfo", "GetInstance", ["System.IFormatProvider"],
                            ShapeKind.SimpleForward, "ChaosDateTimeFormatInfoGetInstance",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Globalization.DateTimeFormatInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        // CultureInfo.GetCultureInfo(string) → CultureInfo  (static)
                        registry.Register("System.Globalization.CultureInfo", "GetCultureInfo", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosCultureGetCultureInfo",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        // CultureInfo.GetCultureInfo(string, bool) → CultureInfo (static)
                        registry.Register("System.Globalization.CultureInfo", "GetCultureInfo", ["System.String", "System.Boolean"],
                            ShapeKind.SimpleForward, "ChaosCultureGetCultureInfoBool",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                    CreateInt32AbiSlot(),  // bool → Int32 in unmanaged ABI
                            }),
                            CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        // CultureInfo.GetCultureInfoByIetfLanguageTag(string) → CultureInfo (static)
                        registry.Register("System.Globalization.CultureInfo", "GetCultureInfoByIetfLanguageTag", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosCultureGetCultureInfoByIetfLanguageTag",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        // CultureInfo.CreateSpecificCulture(string) → CultureInfo (static)
                        registry.Register("System.Globalization.CultureInfo", "CreateSpecificCulture", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosCultureCreateSpecificCulture",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Globalization.CultureInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        // CompareInfo.GetCompareInfo(string) → CompareInfo (static)
                        registry.Register("System.Globalization.CompareInfo", "GetCompareInfo", ["System.String"],
                            ShapeKind.SimpleForward, "ChaosCompareInfoGetCompareInfo",
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                            {
                                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Globalization.CompareInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });

                        // ── Chaos.TestFramework.Assert inline shapes ──────────────────────
                        // These inline expansions replace Assert.AreEqual/IsTrue/IsNull etc.
                        // calls with C++ code that checks the condition and throws a managed
                        // exception on failure.  The exception is caught by the dispatch
                        // wrapper's catch(chaos_managed_exception&) block.
                        //
                        // The C# codegen always emits these assertion bodies.  The calling
                        // test code simply does not invoke Assert methods in non-verification
                        // builds, so the assertion code is dead-stripped by the C++ linker.

                        // Assert.AreEqual(expected, actual) — all overloads
                        // For byte[] arrays, uses element-by-element memcmp instead of pointer
                        // comparison, since different array allocations are never pointer-equal.
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
                            MethodName: "AreEqual",
                            Resolver: static (callee, paramTypes) =>
                            {
                                if (paramTypes.Count < 2) return null;

                                // byte[]: structural comparison via __builtin_memcmp
                                if (paramTypes[0] == "System.Byte[]" && paramTypes[1] == "System.Byte[]")
                                {
                                    return """
            	                            [&]() -> void {
            	                                bool _cae_eq;
            	                                if (({0}) == ({1})) _cae_eq = true;
            	                                else if (({0}) == 0 || ({1}) == 0) _cae_eq = false;
            	                                else {
            	                                    auto _cae_l0 = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
            	                                        reinterpret_cast<const CHAOS_IL2CPP_UINT8*>({0}) + 24);
            	                                    auto _cae_l1 = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
            	                                        reinterpret_cast<const CHAOS_IL2CPP_UINT8*>({1}) + 24);
            	                                    if (_cae_l0 != _cae_l1) _cae_eq = false;
            	                                    else {
            	                                        _cae_eq = __builtin_memcmp(
            	                                            reinterpret_cast<const void*>(reinterpret_cast<const CHAOS_IL2CPP_UINT8*>({0}) + 32),
            	                                            reinterpret_cast<const void*>(reinterpret_cast<const CHAOS_IL2CPP_UINT8*>({1}) + 32),
            	                                            static_cast<CHAOS_IL2CPP_SIZE>(_cae_l0)) == 0;
            	                                    }
            	                                }
            	                                if (!_cae_eq) throw chaos_managed_exception{};
            	                            }()
            	                            """.Replace("\r\n", "\n").Trim();
                                }

                                return """
            	                        [&]() -> void { if (({0}) != ({1})) { throw chaos_managed_exception{}; } }()
            	                        """.Replace("\r\n", "\n").Trim();
                            }));

                        // Assert.IsTrue(condition)
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
                            MethodName: "IsTrue",
                            Resolver: static (callee, paramTypes) =>
                            {
                                if (paramTypes.Count < 1) return null;
                                return """
            	                        [&]() -> void { if (!({0})) { throw chaos_managed_exception{}; } }()
            	                        """.Replace("\r\n", "\n").Trim();
                            }));

                        // Assert.IsFalse(condition)
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
                            MethodName: "IsFalse",
                            Resolver: static (callee, paramTypes) =>
                            {
                                if (paramTypes.Count < 1) return null;
                                return """
            	                        [&]() -> void { if ({0}) { throw chaos_managed_exception{}; } }()
            	                        """.Replace("\r\n", "\n").Trim();
                            }));

                        // Assert.IsNull(value)
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
                            MethodName: "IsNull",
                            Resolver: static (callee, paramTypes) =>
                            {
                                if (paramTypes.Count < 1) return null;
                                return """
            	                        [&]() -> void { if (({0}) != 0) { throw chaos_managed_exception{}; } }()
            	                        """.Replace("\r\n", "\n").Trim();
                            }));

                        // Assert.IsNotNull(value)
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
                            MethodName: "IsNotNull",
                            Resolver: static (callee, paramTypes) =>
                            {
                                if (paramTypes.Count < 1) return null;
                                return """
            	                        [&]() -> void { if (({0}) == 0) { throw chaos_managed_exception{}; } }()
            	                        """.Replace("\r\n", "\n").Trim();
                            }));

                        // Assert.Fail(message) — always throws
                        registry.RegisterInline(new InlineShapeDescriptor(
                            TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
                            MethodName: "Fail",
                            Resolver: static (callee, paramTypes) =>
                            {
                                return """
            	                        [&]() -> void { throw chaos_managed_exception{}; }()
            	                        """.Replace("\r\n", "\n").Trim();
                            }));

                        // Assert.Throws<T>(Action) — deferred to follow-up implementation.
                        // Requires recognizing the callvirt Invoke delegate pattern and
                        // emitting a direct try/catch with type check.

                        return;
        }

    }
}
