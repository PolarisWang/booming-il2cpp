// Auto-generated partial
using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;
namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        private static void RegisterCoreStubs_Part1(RuntimeHelperShapeRegistry registry)
        {

            RegisterStringoperations(registry);
            RegisterExceptionoperations(registry);
            RegisterGCoperations(registry);
            RegisterManagedGCAPI(registry);
            RegisterGCCollectoverload(registry);
            RegisterGCLatencyModepropertygetter(registry);
            RegisterGCCollectionCountreturns(registry);
            RegisterGCGetTotalPauseDurationreturns(registry);
            RegisterGCGetAllocatedBytesForCurrentThreadreturns(registry);
            RegisterGCTryStartNoGCRegiontwo(registry);
            RegisterGCEndNoGCRegionreturns(registry);
            RegisterFullGCNotification(registry);
            RegisterGCMemoryInfo(registry);
            RegisterDelegate(registry);
            RegisterObject(registry);
            RegisterObjectGetHashCode(registry);
            RegisterObjectToString(registry);
            RegisterObjectGetType(registry);
            RegisterObjectMemberwiseClone(registry);
            RegisterObjectEqualsstatic(registry);
            RegisterObjectReferenceEquals(registry);
            RegisterStreamIO(registry);
            RegisterArrayEmptyT(registry);
            RegisterTypeGetMethodgeneric(registry);
            RegisterThrowHelper(registry);
            RegisterCommonframeworkmethod(registry);
            RegisterEnvironment(registry);
            RegisterConsole(registry);
            RegisterOperatingSystemplatformchecks(registry);
            RegisterNumericformatting(registry);
            RegisterMarshalnongeneric(registry);
            RegisterMarshalStringToHGlobalAnsiIntPtr(registry);
            RegisterMarshalStringToHGlobalUniIntPtr(registry);
            RegisterMarshalPtrToStringAnsiString(registry);
            RegisterMarshalUnsafeAddrOfPinnedArrayElementIntPtr(registry);
            RegisterMarshalSizeOfT(registry);
            RegisterMarshalOffsetOfT(registry);
            RegisterMarshalCopy(registry);
            RegisterMarshalStructureToPtrT(registry);
            RegisterMarshalPtrToStructureT(registry);
            RegisterMarshalDestroyStructurefree(registry);
            RegisterGCHandlestubs(registry);
            RegisterMarshalHRstubs(registry);
            RegisterNativeMemorystubs(registry);
            RegisterSafeBufferstubs(registry);
            RegisterStringJoin(registry);
            RegisterStringJoin_1(registry);
            RegisterCustomAttributeMemberInfoIsDefined(registry);
            RegisterInterpolatedStringHandler(registry);
            RegisterRuntimeHelpers(registry);
            RegisterMonitor(registry);
            RegisterThread(registry);
            RegisterThreadSleep(registry);
            RegisterThreadYield(registry);
            RegisterThreadAbort(registry);
            RegisterThreadInterrupt(registry);
            RegisterThreadgetIsBackground(registry);
            RegisterThreadsetIsBackground(registry);
            RegisterThreadgetPriority(registry);
            RegisterThreadsetPriority(registry);
            RegisterThreadgetThreadState(registry);
            RegisterThreadgetIsThreadPoolThread(registry);
            RegisterDateTime(registry);
            RegisterTimeSpan(registry);
            RegisterReflectionType(registry);
            RegisterReflectionAssembly(registry);
            RegisterReflectionMethodBase(registry);
            RegisterMethodBaseInvokewith(registry);
            RegisterReflectionMemberInfo(registry);
            RegisterActivator(registry);
            RegisterReflectionType_1(registry);
            RegisterTypeGetFieldwith(registry);
            RegisterTypeGetEvent(registry);
            RegisterTypeGetProperty(registry);
            RegisterTypeGetConstructors(registry);
            RegisterReflectionMethodInfo(registry);
            RegisterAssemblyGetTypeoverload(registry);
            RegisterReflectionParameterInfo(registry);
            RegisterAsyncTaskYield(registry);
            RegisterDecimal(registry);
            RegisterNullableTget(registry);
            RegisterNullableTGetValueOrDefault(registry);
            RegisterConvertToChar(registry);
            RegisterConvertToCharinline(registry);
            RegisterConvertToCharalways(registry);
            RegisterConvertToCharinline_1(registry);
            RegisterInt32GetHashCodeinline(registry);
            RegisterArrayCopy(registry);

        }

        private static string GetTypeDisplayNameFromSubjectId(string subjectId)
        {
            var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
            var typePart = separatorIndex >= 0 ? subjectId[..separatorIndex] : subjectId;
            var slashIndex = typePart.IndexOf('/');
            return slashIndex >= 0 ? typePart[(slashIndex + 1)..] : typePart;
        }

        private static string GetMethodNameFromSubjectId(string subjectId)
        {
            var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
            if (separatorIndex < 0) return string.Empty;
            var afterSeparator = subjectId[(separatorIndex + 2)..];
            var colonIndex = afterSeparator.IndexOf(':');
            var parenIndex = afterSeparator.IndexOf('(');
            if (colonIndex >= 0 && parenIndex >= 0)
            {
                return afterSeparator[..Math.Min(colonIndex, parenIndex)];
            }
            if (colonIndex >= 0)
            {
                return afterSeparator[..colonIndex];
            }
            return parenIndex >= 0 ? afterSeparator[..parenIndex] : afterSeparator;
        }

        // P3: cache for SubjectId -> parameter types (parsing is pure)
        private static readonly Dictionary<string, IReadOnlyList<string>> s_methodParamTypeCache
            = new(StringComparer.Ordinal);

        private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectId(string subjectId)
        {
            if (s_methodParamTypeCache.TryGetValue(subjectId, out var cached))
                return cached;
            var result = GetMethodParameterTypesFromSubjectIdImpl(subjectId);
            s_methodParamTypeCache[subjectId] = result;
            return result;
        }

        private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectIdImpl(string subjectId)
        {
            var parenOpen = subjectId.IndexOf('(');
            var parenClose = subjectId.LastIndexOf(')');
            if (parenOpen < 0 || parenClose <= parenOpen) return [];
            var paramsPart = subjectId[(parenOpen + 1)..parenClose];
            if (string.IsNullOrEmpty(paramsPart)) return [];
            return paramsPart.Split(',');
        }

        /// <summary>Map Convert.ToChar parameter type to the corresponding chaos_convert_tochar_* native function.</summary>
        private static string GetToCharNativeSymbol(string paramType)
        {
            return paramType switch
            {
                "System.Boolean" => "chaos_convert_tochar_boolean",
                "System.Byte" => "chaos_convert_tochar_byte",
                "System.Char" => "chaos_convert_tochar_char",
                "System.DateTime" => "chaos_convert_tochar_datetime",
                "System.Decimal" => "chaos_convert_tochar_decimal",
                "System.Double" => "chaos_convert_tochar_double",
                "System.Int16" => "chaos_convert_tochar_int16",
                "System.Int32" => "chaos_convert_tochar_int32",
                "System.Int64" => "chaos_convert_tochar_int64",
                "System.Object" => "chaos_convert_tochar_object",
                "System.SByte" => "chaos_convert_tochar_sbyte",
                "System.Single" => "chaos_convert_tochar_single",
                "System.String" => "chaos_convert_tochar_string",
                "System.UInt16" => "chaos_convert_tochar_uint16",
                "System.UInt32" => "chaos_convert_tochar_uint32",
                "System.UInt64" => "chaos_convert_tochar_uint64",
                _ => "chaos_convert_tochar_int32",
            };
        }

        private static string SanitizeForEnumName(string name)
        {
            var sb = new StringBuilder(name.Length);
            foreach (var ch in name)
                sb.Append(char.IsAsciiLetterOrDigit(ch) || ch == '_' ? ch : '_');
            return sb.ToString().ToUpperInvariant();
        }

        /// <summary>Parse generic type arguments from a type name suffix like "`1[[System.Int32]]".</summary>
        private static bool TryParseGenericTypeArgsFromTypeName(
            string genericSuffix,
            [NotNullWhen(true)] out IReadOnlyList<string>? typeArgs)
        {
            typeArgs = null;
            // Format: `1[[System.Int32]] or `2[[System.Int32],[System.String]]
            if (genericSuffix.Length < 2 || genericSuffix[0] != '`') return false;

            var bracketStart = genericSuffix.IndexOf("[[", StringComparison.Ordinal);
            if (bracketStart < 0) return false;

            var inner = genericSuffix.Substring(bracketStart + 2);
            var bracketEnd = inner.LastIndexOf("]]", StringComparison.Ordinal);
            if (bracketEnd < 0) return false;

            inner = inner.Substring(0, bracketEnd);
            if (string.IsNullOrEmpty(inner))
            {
                typeArgs = Array.Empty<string>();
                return true;
            }

            var args = new List<string>();
            var depth = 0;
            var start = 0;
            for (var i = 0; i < inner.Length; i++)
            {
                if (inner[i] == '[') depth++;
                else if (inner[i] == ']') depth--;
                else if (inner[i] == ',' && depth == 0)
                {
                    args.Add(inner.Substring(start, i - start));
                    start = i + 1;
                }
            }
            if (start < inner.Length)
                args.Add(inner.Substring(start));

            typeArgs = args;
            return args.Count > 0;
        }

        /// <summary>
        /// Extract the delegate Invoke parameter count from the delegate type's subjectId.
        /// For Func`N types: N-1 parameters (one type arg is return type).
        /// For Action`N types: N parameters.
        /// For other delegate types: returns 0 (caller must handle).
        /// </summary>
        private static int ExtractDelegateArity(string delegateTypeSubjectId)
        {
            // SubjectId format: "Assembly/TypeName`N[[...]]"
            var slash = delegateTypeSubjectId.IndexOf('/');
            if (slash < 0) return 0;
            var typeName = delegateTypeSubjectId.Substring(slash + 1);

            // Check for backtick-arity suffix: TypeName`N
            var backtick = typeName.IndexOf('`');
            if (backtick < 0) return 0;

            var arityStr = string.Empty;
            for (int i = backtick + 1; i < typeName.Length; i++)
            {
                if (char.IsDigit(typeName[i]))
                    arityStr += typeName[i];
                else
                    break;
            }
            if (string.IsNullOrEmpty(arityStr) || !int.TryParse(arityStr, out int arity))
                return 0;

            // Extract display name before backtick for Func/Action detection.
            var baseName = typeName.Substring(0, backtick);
            // Get the segment after the last '.' for short name.
            var lastDot = baseName.LastIndexOf('.');
            var shortName = lastDot >= 0 ? baseName.Substring(lastDot + 1) : baseName;

            return shortName switch
            {
                "Func" when arity >= 1 => arity - 1,  // One type arg is return type
                "Action" => arity,                     // All type args are parameters
                _ => 0,  // Custom delegate type — arity unknown from name alone
            };
        }

        /// <summary>Register Convert.ToXxx inline shapes for all numeric value-type overloads.</summary>
        private static void RegisterConvertNumericInline(RuntimeHelperShapeRegistry registry, string methodName, string cppCastType)
        {
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Convert",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var pt = paramTypes[0];
                    if (pt is "System.Byte" or "System.SByte" or "System.Int16" or "System.UInt16"
                        or "System.Int32" or "System.UInt32" or "System.Int64" or "System.UInt64"
                        or "System.Char" or "System.Single" or "System.Double")
                    {
                        return $"static_cast<{cppCastType}>({{0}})";
                    }
                    if (pt is "System.Boolean" or "System.DateTime")
                    {
                        return $"(chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)), static_cast<{cppCastType}>(0))";
                    }
                    return null;
                }
            ));
        }

        private static void RegisterCryptoStubs(RuntimeHelperShapeRegistry registry)
        {
            // ── SHA family: HashData(byte[]) -> byte[] ────────────────
            RegisterShaStub(registry, "SHA1", "ChaosSha1Hash");
            RegisterShaStub(registry, "SHA256", "ChaosSha256Hash");
            RegisterShaStub(registry, "SHA384", "ChaosSha384Hash");
            RegisterShaStub(registry, "SHA512", "ChaosSha512Hash");
            RegisterShaStub(registry, "SHA3_256", "ChaosSha3_256Hash");
            RegisterShaStub(registry, "SHA3_384", "ChaosSha3_384Hash");
            RegisterShaStub(registry, "SHA3_512", "ChaosSha3_512Hash");

            // ── HMAC family: HashData(byte[], byte[]) -> byte[] ───────
            RegisterHmacStub(registry, "HMACSHA1", "ChaosHmacSha1");
            RegisterHmacStub(registry, "HMACSHA256", "ChaosHmacSha256");
            RegisterHmacStub(registry, "HMACSHA384", "ChaosHmacSha384");
            RegisterHmacStub(registry, "HMACSHA512", "ChaosHmacSha512");
            RegisterHmacStub(registry, "HMACSHA3_256", "ChaosHmacSha3_256");
            RegisterHmacStub(registry, "HMACSHA3_384", "ChaosHmacSha3_384");
            RegisterHmacStub(registry, "HMACSHA3_512", "ChaosHmacSha3_512");

            // ── MD5 family: HashData(byte[]) -> byte[] ────────────────
            RegisterShaStub(registry, "MD5", "ChaosMd5Hash");

            // ── HMACMD5 family: HashData(byte[], byte[]) -> byte[] ────
            RegisterHmacStub(registry, "HMACMD5", "ChaosHmacMd5");

            // ── RNG: GetBytes(int) -> byte[] ──────────────────────────
            registry.Register(
                "RandomNumberGenerator",
                "GetBytes",
                new[] { "System.Int32" },
                ShapeKind.SimpleForward, "ChaosCngGetBytes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                EmptyRawArgumentIndices);

            // ── RNG: Fill(byte[]) ─────────────────────────────────────
            registry.Register(
                "RandomNumberGenerator",
                "Fill",
                new[] { "System.Byte[]" },
                ShapeKind.SimpleForward, "ChaosCngFill",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // ── BCrypt P/Invoke stubs (Interop+BCrypt methods) ────────────
            // These methods are DllImport declarations in System.Private.CoreLib
            // that the runtime's ChaosExternalRuntimeFallback would otherwise handle
            // via _TryExecuteViaPInvoke.  By registering them as SimpleForward stubs,
            // codegen generates direct native call wrappers that invoke the
            // ChaosBCrypt* functions defined in crypto_stubs.cpp/crypto_stubs.h.
            // This avoids going through ChaosExternalRuntimeFallback at runtime
            // and enables actual BCrypt function execution.
            RegisterBCryptStub(registry, "BCryptOpenAlgorithmProvider",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32"],
                "ChaosBCryptOpenAlgorithmProvider", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptCloseAlgorithmProvider",
                ["System.IntPtr", "System.Int32"],
                "ChaosBCryptCloseAlgorithmProvider", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptCreateHash",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptCreateHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDestroyHash",
                ["System.IntPtr"],
                "ChaosBCryptDestroyHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptHashData",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptHashData", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptFinishHash",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptFinishHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptHash",
                ["System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGenerateSymmetricKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptGenerateSymmetricKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDestroyKey",
                ["System.IntPtr"],
                "ChaosBCryptDestroyKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptEncrypt",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32"],
                "ChaosBCryptEncrypt", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDecrypt",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32"],
                "ChaosBCryptDecrypt", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptImportKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32",
                 "System.Int32"],
                "ChaosBCryptImportKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptExportKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptExportKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGetProperty",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptGetProperty", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptSetProperty",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.Int32", "System.Int32"],
                "ChaosBCryptSetProperty", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGenerateKeyPair",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptGenerateKeyPair", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptFinalizeKeyPair",
                ["System.IntPtr", "System.Int32"],
                "ChaosBCryptFinalizeKeyPair", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptImportKeyPair",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptImportKeyPair", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptSignHash",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptSignHash", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptVerifySignature",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32",
                 "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptVerifySignature", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptSecretAgreement",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr", "System.Int32"],
                "ChaosBCryptSecretAgreement", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDestroySecret",
                ["System.IntPtr"],
                "ChaosBCryptDestroySecret", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptDeriveKey",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.IntPtr", "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptDeriveKey", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptKeyDerivation",
                ["System.IntPtr", "System.IntPtr", "System.IntPtr",
                 "System.Int32", "System.IntPtr", "System.Int32"],
                "ChaosBCryptKeyDerivation", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptGenRandom",
                ["System.IntPtr", "System.IntPtr", "System.Int32", "System.Int32"],
                "ChaosBCryptGenRandom", CreateInt32AbiSlot());
            RegisterBCryptStub(registry, "BCryptIsAvailable", [],
                "ChaosBCryptIsAvailable", CreateInt32AbiSlot());
        }

        private static void RegisterShaStub(RuntimeHelperShapeRegistry registry, string algoName, string nativeFn)
        {
            registry.Register(
                algoName,
                "HashData",
                new[] { "System.Byte[]" },
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[1]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
        }

        private static void RegisterHmacStub(RuntimeHelperShapeRegistry registry, string algoName, string nativeFn)
        {
            registry.Register(
                algoName,
                "HashData",
                new[] { "System.Byte[]", "System.Byte[]" },
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Register a BCrypt/NCrypt P/Invoke method as a SimpleForward stub.
        /// The type name is "Interop+BCrypt" for BCrypt methods and "Interop+NCrypt" for NCrypt methods.
        /// Each IntPtr parameter maps to a NativeInt ABI slot, and each Int32 maps to an Int32 ABI slot.
        /// The return type is always Int32 (NTSTATUS) for BCrypt API functions.
        /// </summary>
        private static void RegisterBCryptStub(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string[] paramTypeDisplayNames,
            string nativeFn,
            AotCoreIrAbiSlotArtifact returnAbi)
        {
            var typeName = methodName.StartsWith("NCrypt", StringComparison.Ordinal)
                ? "Interop+NCrypt"
                : "Interop+BCrypt";

            var abiSlots = new AotCoreIrAbiSlotArtifact[paramTypeDisplayNames.Length];
            var rawIndices = new HashSet<int>();
            for (int i = 0; i < paramTypeDisplayNames.Length; i++)
            {
                rawIndices.Add(i);
                abiSlots[i] = paramTypeDisplayNames[i] switch
                {
                    "System.Int32" => CreateInt32AbiSlot(),
                    _ => CreateNativeIntAbiSlot(),  // IntPtr -> NativeInt
                };
            }

            registry.Register(
                typeName,
                methodName,
                paramTypeDisplayNames,
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots),
                returnAbi,
                rawIndices);
        }

    }
}
