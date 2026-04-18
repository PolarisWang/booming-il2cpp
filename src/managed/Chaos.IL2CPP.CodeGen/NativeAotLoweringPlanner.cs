using System.Globalization;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private const string OverflowExceptionTypeSubjectId = "System.Private.CoreLib/System.OverflowException";
    private const string ObjectTypeSubjectId = "System.Private.CoreLib/System.Object";
    private const string ObjectCtorMethodSubjectId = "System.Private.CoreLib/System.Object::.ctor()";
    private const string DelegateTypeSubjectId = "System.Private.CoreLib/System.Delegate";
    private const string MulticastDelegateTypeSubjectId = "System.Private.CoreLib/System.MulticastDelegate";
    private const string StringTypeSubjectId = "System.Private.CoreLib/System.String";
    private const string PairStringConcatMethodSubjectId =
        "System.Private.CoreLib/System.String::Concat(System.String,System.String)";
    private const string QuadStringConcatMethodSubjectId =
        "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String,System.String)";
    private const string StringEqualityMethodSubjectId =
        "System.Private.CoreLib/System.String::op_Equality(System.String,System.String)";
    private const string StringGetLengthMethodSubjectId =
        "System.Private.CoreLib/System.String::get_Length()";
    private const string StringStartsWithComparisonMethodSubjectId =
        "System.Private.CoreLib/System.String::StartsWith(System.String,System.StringComparison)";
    private const string StringContainsComparisonMethodSubjectId =
        "System.Private.CoreLib/System.String::Contains(System.String,System.StringComparison)";
    private const string StringJoinStringEnumerableMethodSubjectId =
        "System.Private.CoreLib/System.String::Join(System.String,System.Collections.Generic.IEnumerable<System.String>)";
    private const string StringJoinGenericEnumerableMethodPrefix =
        "System.Private.CoreLib/System.String::Join<";
    private const string ExceptionCtorWithMessageMethodSubjectId =
        "System.Private.CoreLib/System.Exception::.ctor(System.String)";
    private const string InvalidOperationExceptionCtorWithMessageMethodSubjectId =
        "System.Private.CoreLib/System.InvalidOperationException::.ctor(System.String)";
    private const string ArgumentOutOfRangeExceptionCtorWithParamNameAndMessageMethodSubjectId =
        "System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor(System.String,System.String)";
    private const string ExceptionGetMessageMethodSubjectId =
        "System.Private.CoreLib/System.Exception::get_Message()";
    private const string ArgumentExceptionGetParamNameMethodSubjectId =
        "System.Private.CoreLib/System.ArgumentException::get_ParamName()";
    private const string TypeTypeSubjectId = "System.Private.CoreLib/System.Type";
    private const string MethodInfoTypeSubjectId = "System.Private.CoreLib/System.Reflection.MethodInfo";
    private const string ConstructorInfoTypeSubjectId = "System.Private.CoreLib/System.Reflection.ConstructorInfo";
    private const string FieldInfoTypeSubjectId = "System.Private.CoreLib/System.Reflection.FieldInfo";
    private const string ParameterInfoTypeSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo";
    private const string AssemblyTypeSubjectId = "System.Private.CoreLib/System.Reflection.Assembly";
    private const string AssemblyNameTypeSubjectId = "System.Private.CoreLib/System.Reflection.AssemblyName";
    private const string MemberInfoIsDefinedMethodSubjectId =
        "System.Private.CoreLib/System.Reflection.MemberInfo::IsDefined(System.Type,System.Boolean)";
    private const string MemberInfoGetNameMethodSubjectId =
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_Name()";
    private const string MemberInfoGetDeclaringTypeMethodSubjectId =
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType()";
    private const string MemberInfoGetMetadataTokenMethodSubjectId =
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken()";
    private const string ObjectEqualsMethodSubjectId = "System.Private.CoreLib/System.Object::Equals(System.Object)";
    private const string EnvironmentGetCurrentManagedThreadIdMethodSubjectId =
        "System.Private.CoreLib/System.Environment::get_CurrentManagedThreadId()";
    private const string GcCollectMethodSubjectId = "System.Private.CoreLib/System.GC::Collect()";
    private const string GcWaitForPendingFinalizersMethodSubjectId = "System.Private.CoreLib/System.GC::WaitForPendingFinalizers()";
    private const string GcKeepAliveMethodSubjectId = "System.Private.CoreLib/System.GC::KeepAlive(System.Object)";
    private const string MonitorEnterMethodSubjectId = "System.Threading/Monitor::Enter(System.Object,System.Boolean&)";
    private const string MonitorExitMethodSubjectId = "System.Threading/Monitor::Exit(System.Object)";
    private const string MonitorTryEnterTimeSpanMethodSubjectId = "System.Threading/Monitor::TryEnter(System.Object,System.TimeSpan,System.Boolean&)";
    private const string ThreadTypeSubjectId = "System.Threading.Thread/System.Threading.Thread";
    private const string ThreadStartDelegateTypeSubjectId = "System.Threading.Thread/System.Threading.ThreadStart";
    private const string ThreadConstructorMethodSubjectId =
        "System.Threading.Thread/System.Threading.Thread::.ctor(System.Threading.ThreadStart)";
    private const string ThreadStartMethodSubjectId = "System.Threading.Thread/System.Threading.Thread::Start()";
    private const string ThreadJoinMethodSubjectId = "System.Threading.Thread/System.Threading.Thread::Join()";
    private const string ThreadCurrentThreadGetterMethodSubjectId =
        "System.Threading.Thread/System.Threading.Thread::get_CurrentThread()";
    private const string ThreadGetNameMethodSubjectId = "System.Threading.Thread/System.Threading.Thread::get_Name()";
    private const string ThreadSetNameMethodSubjectId =
        "System.Threading.Thread/System.Threading.Thread::set_Name(System.String)";
    private const string DelegateCombineMethodSubjectId = "System.Private.CoreLib/System.Delegate::Combine(System.Delegate,System.Delegate)";
    private const string DelegateRemoveMethodSubjectId = "System.Private.CoreLib/System.Delegate::Remove(System.Delegate,System.Delegate)";
    private const string MethodBaseInvokeMethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::Invoke(System.Object,System.Object[])";
    private const string OperatingSystemIsWindowsMethodSubjectId = "System.Private.CoreLib/System.OperatingSystem::IsWindows()";
    private const string OperatingSystemIsLinuxMethodSubjectId = "System.Private.CoreLib/System.OperatingSystem::IsLinux()";
    private const string OperatingSystemIsMacOsMethodSubjectId = "System.Private.CoreLib/System.OperatingSystem::IsMacOS()";
    private const string InterlockedCompareExchangeMethodPrefix = "System.Threading/Interlocked::CompareExchange<";
    private const string ListTypeSubjectIdPrefix = "System.Collections/System.Collections.Generic.List<";
    private const string DictionaryTypeSubjectIdPrefix = "System.Collections/System.Collections.Generic.Dictionary<";
    private const string ReadOnlyCollectionTypeSubjectIdPrefix = "System.Private.CoreLib/System.Collections.Generic.IReadOnlyCollection<";
    private const string ReadOnlyListTypeSubjectIdPrefix = "System.Private.CoreLib/System.Collections.Generic.IReadOnlyList<";
    private const string SpanTypeSubjectIdPrefix = "System.Private.CoreLib/System.Span<";
    private const string ReadOnlySpanTypeSubjectIdPrefix = "System.Private.CoreLib/System.ReadOnlySpan<";
    private const string MemoryTypeSubjectIdPrefix = "System.Private.CoreLib/System.Memory<";
    private const string TimeSpanTypeSubjectId = "System.Private.CoreLib/System.TimeSpan";
    private const string TimeSpanFromMillisecondsMethodSubjectId =
        "System.Private.CoreLib/System.TimeSpan::FromMilliseconds(System.Double)";
    private const string InitializeArrayMethodSubjectId =
        "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray(System.Array,System.RuntimeFieldHandle)";
    private const string RuntimeHelpersCreateSpanMethodPrefix =
        "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan<";
    private const string MemoryExtensionsAsSpanMethodPrefix =
        "System.Memory/System.MemoryExtensions::AsSpan<";
    private const string MemoryExtensionsAsMemoryMethodPrefix =
        "System.Memory/System.MemoryExtensions::AsMemory<";
    private const string GetTypeFromHandleMethodSubjectId = "System.Private.CoreLib/System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)";
    private const string GetTypeHandleMethodSubjectId = "System.Private.CoreLib/System.Type::get_TypeHandle()";
    private const string GetFieldMethodSubjectId = "System.Private.CoreLib/System.Type::GetField(System.String)";
    private const string GetMethodByNameMethodSubjectId = "System.Private.CoreLib/System.Type::GetMethod(System.String)";
    private const string GetAssemblyMethodSubjectId = "System.Private.CoreLib/System.Type::get_Assembly()";
    private const string GetGenericArgumentsMethodSubjectId = "System.Private.CoreLib/System.Type::GetGenericArguments()";
    private const string GetGenericTypeDefinitionMethodSubjectId = "System.Private.CoreLib/System.Type::GetGenericTypeDefinition()";
    private const string GetConstructorsMethodSubjectId = "System.Private.CoreLib/System.Type::GetConstructors(System.Reflection.BindingFlags)";
    private const string AssemblyGetTypeMethodSubjectId = "System.Private.CoreLib/System.Reflection.Assembly::GetType(System.String)";
    private const string TypeGetTypeByNameMethodSubjectId = "System.Private.CoreLib/System.Type::GetType(System.String)";
    private const string AssemblyGetNameMethodSubjectId = "System.Private.CoreLib/System.Reflection.Assembly::GetName()";
    private const string AssemblyNameGetNameMethodSubjectId = "System.Private.CoreLib/System.Reflection.AssemblyName::get_Name()";
    private const string GetMethodMethodSubjectId = "System.Private.CoreLib/System.Type::GetMethod(System.String,System.Reflection.BindingFlags)";
    private const string GetMethodHandleMethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle()";
    private const string GetParametersMethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()";
    private const string MakeGenericMethodMethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodInfo::MakeGenericMethod(System.Type[])";
    private const string ParameterInfoGetNameMethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name()";
    private const string Int32ToStringMethodSubjectId = "System.Private.CoreLib/System.Int32::ToString()";
    private const string SingleToStringWithFormatMethodSubjectId = "System.Private.CoreLib/System.Single::ToString(System.String)";
    private const string DoubleToStringWithFormatMethodSubjectId = "System.Private.CoreLib/System.Double::ToString(System.String)";
    private const string DefaultInterpolatedStringHandlerCtorMethodSubjectId =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(System.Int32,System.Int32)";
    private const string DefaultInterpolatedStringHandlerAppendFormattedStringMethodSubjectId =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted(System.String)";
    private const string DefaultInterpolatedStringHandlerAppendLiteralMethodSubjectId =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(System.String)";
    private const string DefaultInterpolatedStringHandlerAppendFormattedInt32MethodSubjectId =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<System.Int32>(System.Int32)";
    private const string DefaultInterpolatedStringHandlerToStringAndClearMethodSubjectId =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()";
    private const string MarshalGetFunctionPointerForDelegateMethodPrefix =
        "System.Runtime.InteropServices/Marshal::GetFunctionPointerForDelegate<";
    private const string MarshalGetDelegateForFunctionPointerMethodPrefix =
        "System.Runtime.InteropServices/Marshal::GetDelegateForFunctionPointer<";
    private const string MarshalStringToCoTaskMemUtf8MethodSubjectId =
        "System.Runtime.InteropServices/Marshal::StringToCoTaskMemUTF8(System.String)";
    private const string MarshalPtrToStringUtf8MethodSubjectId =
        "System.Runtime.InteropServices/Marshal::PtrToStringUTF8(System.IntPtr)";
    private const string MarshalFreeCoTaskMemMethodSubjectId =
        "System.Runtime.InteropServices/Marshal::FreeCoTaskMem(System.IntPtr)";
    private const string MarshalAllocHGlobalInt32MethodSubjectId =
        "System.Runtime.InteropServices/Marshal::AllocHGlobal(System.Int32)";
    private const string MarshalFreeHGlobalMethodSubjectId =
        "System.Runtime.InteropServices/Marshal::FreeHGlobal(System.IntPtr)";
    private const string MarshalSizeOfMethodPrefix =
        "System.Runtime.InteropServices/Marshal::SizeOf<";
    private const string MarshalStructureToPtrMethodPrefix =
        "System.Runtime.InteropServices/Marshal::StructureToPtr<";
    private const string MarshalPtrToStructureMethodPrefix =
        "System.Runtime.InteropServices/Marshal::PtrToStructure<";
    private const int StringComparisonOrdinalValue = 4;
    private const string DllImportAttributeDisplayName = "DllImportAttribute";
    private const string DllImportAttributeTypeSubjectId = "System.Runtime.InteropServices/DllImportAttribute";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    private IReadOnlyDictionary<string, AotCoreIrMethodArtifact> _methodsBySubjectId =
        new Dictionary<string, AotCoreIrMethodArtifact>(StringComparer.Ordinal);

    private IReadOnlyDictionary<string, string?> _referenceTypeBaseSubjectIds =
        new Dictionary<string, string?>(StringComparer.Ordinal);

    private IReadOnlyDictionary<string, HashSet<string>> _referenceTypeImplementedInterfaceSubjectIds =
        new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);

    private IReadOnlySet<string> _valueTypeSubjectIds =
        new HashSet<string>(StringComparer.Ordinal);

    private CustomAttributeSupportModel _customAttributeSupport = CustomAttributeSupportModel.Empty;
    private AssemblyReflectionSupportModel _assemblyReflectionSupport = AssemblyReflectionSupportModel.Empty;
    private ReflectionMemberSupportModel _reflectionMemberSupport = ReflectionMemberSupportModel.Empty;
    private StaticFieldDataSupportModel _staticFieldDataSupport = StaticFieldDataSupportModel.Empty;

    private static readonly IReadOnlySet<int> EmptyRawArgumentIndices = new HashSet<int>();

    private readonly record struct InvocationTarget(
        string TargetSymbol,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
        AotCoreIrAbiSlotArtifact ReturnAbi,
        IReadOnlySet<int> RawArgumentIndices);

    private sealed record ExternalRuntimeHelperDefinition(
        string SubjectId,
        string TargetSymbol,
        string Source,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
        AotCoreIrAbiSlotArtifact ReturnAbi,
        IReadOnlySet<int> RawArgumentIndices);

    private sealed record EnumerableJoinSupportVariant(
        string EnumerableTypeSubjectId,
        AotCoreIrMethodArtifact GetEnumeratorMethod,
        AotCoreIrMethodArtifact MoveNextMethod,
        AotCoreIrMethodArtifact GetCurrentMethod);

    private sealed record AssemblyReflectionTypeEntry(
        string AssemblyName,
        string TypeSubjectId,
        string TypeDisplayName);

    private sealed record AssemblyReflectionSupportModel(
        IReadOnlyList<AssemblyReflectionTypeEntry> TypeEntries)
    {
        public static readonly AssemblyReflectionSupportModel Empty = new([]);
    }

    private sealed record ReflectionMemberTypeEntry(
        string TypeSubjectId,
        string TypeName,
        string? GenericDefinitionTypeSubjectId,
        IReadOnlyList<string> GenericArgumentTypeSubjectIds,
        int GenericParameterCount,
        int MetadataToken);

    private sealed record ReflectionMemberFieldEntry(
        string DeclaringTypeSubjectId,
        string FieldName,
        int MetadataToken);

    private sealed record ReflectionMemberMethodEntry(
        string MethodSubjectId,
        string DeclaringTypeSubjectId,
        string MethodName,
        IReadOnlyList<string> ParameterNames,
        bool IsConstructor,
        int MetadataToken);

    private sealed record ReflectionMemberSupportModel(
        IReadOnlyList<ReflectionMemberTypeEntry> TypeEntries,
        IReadOnlyList<ReflectionMemberFieldEntry> FieldEntries,
        IReadOnlyList<ReflectionMemberMethodEntry> MethodEntries)
    {
        public static readonly ReflectionMemberSupportModel Empty = new([], [], []);
    }

    private sealed record StaticFieldDataEntry(
        string FieldSubjectId,
        string MemberType,
        IReadOnlyList<byte> Bytes);

    private sealed record StaticFieldDataSupportModel(
        IReadOnlyDictionary<string, StaticFieldDataEntry> EntriesBySubjectId)
    {
        public static readonly StaticFieldDataSupportModel Empty =
            new(new Dictionary<string, StaticFieldDataEntry>(StringComparer.Ordinal));
    }

    private sealed record CatchOnlyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact ExceptionRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record FilterOnlyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact FilterRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> FilterInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record CatchAndFinallyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact CatchRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> InnerTryInstructions,
        FinallyHandlerShape? InnerFinallyHandler,
        IReadOnlyList<AotCoreIrInstructionArtifact> PostInnerTryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<FinallyHandlerShape> OuterFinallyHandlers,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record FinallyOnlyExceptionMethodShape(
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<FinallyHandlerShape> FinallyHandlers,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record FinallyHandlerShape(
        AotCoreIrExceptionRegionArtifact ExceptionRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> Instructions);

    private sealed record FinallyHandlerGuardShape(
        IReadOnlyList<AotCoreIrInstructionArtifact> ConditionInstructions,
        bool BranchWhenNonZeroToEnd);

    private sealed record FinallyHandlerEmissionPlan(
        FinallyHandlerGuardShape? Guard,
        IReadOnlyList<AotCoreIrInstructionArtifact> BodyInstructions);

    private sealed record FilterAndFinallyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact FilterRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> FilterInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<FinallyHandlerShape> FinallyHandlers,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record CustomAttributeSupportModel(
        IReadOnlyDictionary<string, string> QueryAttributeTypeByCallee,
        IReadOnlyDictionary<string, string> SyntheticGetterFieldByMethodSubjectId,
        IReadOnlyList<CustomAttributeMaterializationPlan> Materializations,
        IReadOnlySet<string> AdditionalReferenceTypeSubjectIds,
        IReadOnlySet<string> AdditionalInstanceFieldSubjectIds,
        bool RequiresStringSupport,
        bool UsesMemberInfoIsDefined)
    {
        public static readonly CustomAttributeSupportModel Empty = new(
            new Dictionary<string, string>(StringComparer.Ordinal),
            new Dictionary<string, string>(StringComparer.Ordinal),
            [],
            new HashSet<string>(StringComparer.Ordinal),
            new HashSet<string>(StringComparer.Ordinal),
            false,
            false);
    }

    private enum CustomAttributeTargetKind : byte
    {
        Type = 1,
        Method = 2,
    }

    private enum CustomAttributeLiteralKind : byte
    {
        Null = 0,
        Boolean = 1,
        Byte = 2,
        Int16 = 3,
        Int32 = 4,
        Int64 = 5,
        UInt16 = 6,
        UInt32 = 7,
        UInt64 = 8,
        String = 9,
    }

    private sealed record CustomAttributeLiteralValue(
        CustomAttributeLiteralKind Kind,
        object? Value);

    private sealed record CustomAttributeFieldAssignment(
        string FieldSubjectId,
        CustomAttributeLiteralValue Value);

    private sealed record CustomAttributeMaterializationPlan(
        CustomAttributeTargetKind TargetKind,
        string TargetSubjectId,
        string AttributeTypeSubjectId,
        IReadOnlyList<CustomAttributeFieldAssignment> Assignments);

    private readonly record struct MetadataTypeIdentity(
        string AssemblyName,
        string NamespaceName,
        string TypeName)
    {
        public string SubjectId => ManagedNaming.CreateTypeSubjectId(AssemblyName, NamespaceName, TypeName);

        public string DisplayName => ManagedNaming.CreateTypeDisplayName(AssemblyName, NamespaceName, TypeName);
    }

    public NativeAotTemplateModel Create(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod,
        ManagedClosureManifestArtifact closureManifest,
        MetadataRegistrationArtifact metadataRegistration,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        ArgumentNullException.ThrowIfNull(loweringPlan);
        ArgumentNullException.ThrowIfNull(aotCoreIr);
        ArgumentNullException.ThrowIfNull(entryMethod);
        ArgumentNullException.ThrowIfNull(closureManifest);
        ArgumentNullException.ThrowIfNull(metadataRegistration);
        ArgumentNullException.ThrowIfNull(supplementalMetadataTemplate);

        ValidateEntryMethod(entryMethod);
        if (!string.Equals(entryMethod.NativeSymbol, loweringPlan.EntrySymbol, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot entry symbol '{loweringPlan.EntrySymbol}' does not match aot-core-ir symbol '{entryMethod.NativeSymbol}'");
        }

        _methodsBySubjectId = aotCoreIr.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        _referenceTypeBaseSubjectIds = CollectReferenceTypeBaseSubjectIds(aotCoreIr);
        _referenceTypeImplementedInterfaceSubjectIds = CollectReferenceTypeImplementedInterfaceSubjectIds(aotCoreIr);
        _valueTypeSubjectIds = CollectValueTypeSubjectIds(aotCoreIr);

        var reachableMethods = CollectReachableMethods(aotCoreIr, entryMethod);
        _customAttributeSupport = BuildCustomAttributeSupportModel(
            reachableMethods,
            closureManifest,
            supplementalMetadataTemplate);
        _assemblyReflectionSupport = BuildAssemblyReflectionSupportModel(
            reachableMethods,
            closureManifest,
            supplementalMetadataTemplate);
        _reflectionMemberSupport = BuildReflectionMemberSupportModel(
            reachableMethods,
            closureManifest,
            supplementalMetadataTemplate);
        _staticFieldDataSupport = BuildStaticFieldDataSupportModel(
            reachableMethods,
            closureManifest,
            metadataRegistration);
        _staticInitializationSupport = BuildStaticInitializationSupportModel(
            reachableMethods,
            closureManifest);
        var externalRuntimeHelpers = CollectExternalRuntimeHelpers(reachableMethods, _staticInitializationSupport);
        var objectModelBuilder = new StringBuilder();
        EmitRuntimePrelude(objectModelBuilder, externalRuntimeHelpers, _staticFieldDataSupport);
        EmitObjectModelDeclarations(objectModelBuilder, reachableMethods);
        EmitReachableMethodForwardDeclarations(objectModelBuilder, reachableMethods);
        if (externalRuntimeHelpers.Any(helper => IsSpanRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitSpanRuntimePrelude(objectModelBuilder, _staticFieldDataSupport);
        }
        EmitDelegateRuntimeSupportDefinitions(objectModelBuilder, reachableMethods, externalRuntimeHelpers);
        EmitExternalRuntimeHelperDefinitions(objectModelBuilder, externalRuntimeHelpers);
        EmitStaticInitializationDefinitions(objectModelBuilder);

        var methodDeclarations = reachableMethods
            .Select(FormatMethodDeclaration)
            .ToArray();
        var methods = reachableMethods
            .Select(method => new NativeAotMethodTemplateModel
            {
                SubjectId = method.SubjectId,
                MethodSource = BuildMethodSource(method),
            })
            .ToArray();
        var entryBridgeArguments = BuildEntryBridgeArguments(entryMethod);

        return new NativeAotTemplateModel
        {
            Includes =
            [
                "<array>",
                "<cstddef>",
                "<cstdint>",
                "<cstdio>",
                "<cstdlib>",
                "<cstring>",
                "<limits>",
                "<chrono>",
                "<memory>",
                "<mutex>",
                "<string>",
                "<thread>",
                "<unordered_map>",
                "<utility>",
                "<vector>",
            ],
            ObjectModelCode = objectModelBuilder.ToString().TrimEnd(),
            MethodDeclarations = methodDeclarations,
            Methods = methods,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            EntrySymbol = loweringPlan.EntrySymbol,
            EntryNativeSymbol = entryMethod.NativeSymbol,
            NativeEntryFunctionName = loweringPlan.NativeEntryFunctionName,
            EntryBridgeArguments = entryBridgeArguments,
        };
    }

    private static string BuildEntryBridgeArguments(AotCoreIrMethodArtifact entryMethod)
    {
        ArgumentNullException.ThrowIfNull(entryMethod);

        if (entryMethod.ParameterCount == 0)
        {
            return "";
        }

        if (entryMethod.ParameterCount != 1 || entryMethod.ParameterAbis.Count != 1)
        {
            throw new NotSupportedException(
                $"native-aot entry bridge supports only zero-parameter or single-int32 entry methods, but '{entryMethod.SubjectId}' has {entryMethod.ParameterCount} parameters.");
        }

        var carrierKind = entryMethod.ParameterAbis[0].CarrierKindCode;
        if (carrierKind != AotCoreIrAbiCarrierKind.Int32)
        {
            throw new NotSupportedException(
                $"native-aot entry bridge supports only System.Int32 entry parameters, but '{entryMethod.SubjectId}' uses carrier '{carrierKind}'.");
        }

        return "chaos_entry_index";
    }

    private string BuildMethodSource(AotCoreIrMethodArtifact method)
    {
        var builder = new StringBuilder();
        if (method.GenericContext is not null)
        {
            builder.AppendLine(FormatGenericContextComment(method.GenericContext));
        }

        EmitManagedMethod(builder, method);
        return builder.ToString().TrimEnd();
    }

    private static void EmitExternalRuntimeHelperDefinitions(
        StringBuilder builder,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
    {
        foreach (var helper in externalRuntimeHelpers)
        {
            builder.AppendLine(helper.Source);
            builder.AppendLine();
        }
    }

    private void EmitDelegateRuntimeSupportDefinitions(
        StringBuilder builder,
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
    {
        if (!externalRuntimeHelpers.Any(helper =>
                string.Equals(helper.SubjectId, DelegateCombineMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(helper.SubjectId, DelegateRemoveMethodSubjectId, StringComparison.Ordinal)))
        {
            return;
        }

        var delegateTypeSubjectIds = CollectReachableDelegateTypeSubjectIds(reachableMethods)
            .Where(subjectId =>
                !string.Equals(subjectId, DelegateTypeSubjectId, StringComparison.Ordinal) &&
                !string.Equals(subjectId, MulticastDelegateTypeSubjectId, StringComparison.Ordinal))
            .ToArray();

        builder.AppendLine("using chaos_delegate_invocation_list = std::vector<std::intptr_t>;");
        builder.AppendLine();
        builder.AppendLine("chaos_type_System_Private_CoreLib_System_Delegate* chaos_require_delegate(std::intptr_t chaos_delegate_value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_delegate_value == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return reinterpret_cast<chaos_type_System_Private_CoreLib_System_Delegate*>(chaos_delegate_value);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("const chaos_delegate_invocation_list* chaos_try_get_delegate_invocation_list(");
        builder.AppendLine("    const chaos_type_System_Private_CoreLib_System_Delegate* chaos_delegate) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_delegate == nullptr ||");
        builder.AppendLine("        chaos_delegate->chaos_delegate_invocation_list == static_cast<std::intptr_t>(0) ||");
        builder.AppendLine("        chaos_delegate->chaos_delegate_invocation_count <= static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return nullptr;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    return reinterpret_cast<const chaos_delegate_invocation_list*>(chaos_delegate->chaos_delegate_invocation_list);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("bool chaos_delegate_single_entry_equals(std::intptr_t chaos_left_value, std::intptr_t chaos_right_value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_left_value == chaos_right_value)");
        builder.AppendLine("    {");
        builder.AppendLine("        return true;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_left_value == static_cast<std::intptr_t>(0) ||");
        builder.AppendLine("        chaos_right_value == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return false;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto* chaos_left = reinterpret_cast<const chaos_type_System_Private_CoreLib_System_Delegate*>(chaos_left_value);");
        builder.AppendLine("    const auto* chaos_right = reinterpret_cast<const chaos_type_System_Private_CoreLib_System_Delegate*>(chaos_right_value);");
        builder.AppendLine("    return chaos_left->header.type_id == chaos_right->header.type_id &&");
        builder.AppendLine("           chaos_left->chaos_delegate_target == chaos_right->chaos_delegate_target &&");
        builder.AppendLine("           chaos_left->chaos_delegate_method_ptr == chaos_right->chaos_delegate_method_ptr;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("void chaos_delegate_append_flattened_entries(");
        builder.AppendLine("    chaos_delegate_invocation_list& chaos_entries,");
        builder.AppendLine("    std::intptr_t chaos_delegate_value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_delegate_value == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    auto* chaos_delegate = chaos_require_delegate(chaos_delegate_value);");
        builder.AppendLine("    const auto* chaos_invocation_list = chaos_try_get_delegate_invocation_list(chaos_delegate);");
        builder.AppendLine("    if (chaos_invocation_list == nullptr)");
        builder.AppendLine("    {");
        builder.AppendLine("        if (chaos_delegate->chaos_delegate_method_ptr == static_cast<std::intptr_t>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        chaos_entries.push_back(chaos_delegate_value);");
        builder.AppendLine("        return;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    if (static_cast<std::intptr_t>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    for (const auto chaos_entry_value : *chaos_invocation_list)");
        builder.AppendLine("    {");
        builder.AppendLine("        chaos_delegate_append_flattened_entries(chaos_entries, chaos_entry_value);");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("void chaos_delegate_validate_entry_types(const chaos_delegate_invocation_list& chaos_entries)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_entries.empty())");
        builder.AppendLine("    {");
        builder.AppendLine("        return;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto* chaos_first = chaos_require_delegate(chaos_entries.front());");
        builder.AppendLine("    for (const auto chaos_entry_value : chaos_entries)");
        builder.AppendLine("    {");
        builder.AppendLine("        const auto* chaos_entry = chaos_require_delegate(chaos_entry_value);");
        builder.AppendLine("        if (chaos_entry->header.type_id != chaos_first->header.type_id)");
        builder.AppendLine("        {");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_delegate_allocate_with_type_id(std::intptr_t chaos_delegate_type_id)");
        builder.AppendLine("{");
        builder.AppendLine("    switch (chaos_delegate_type_id)");
        builder.AppendLine("    {");
        foreach (var delegateTypeSubjectId in delegateTypeSubjectIds)
        {
            builder.AppendLine($"        case {GetNativeTypeIdSymbol(delegateTypeSubjectId)}:");
            builder.AppendLine("        {");
            builder.AppendLine($"            auto* chaos_delegate = new {GetNativeTypeSymbol(delegateTypeSubjectId)}{{}};");
            builder.AppendLine($"            chaos_delegate->header.type_id = {GetNativeTypeIdSymbol(delegateTypeSubjectId)};");
            builder.AppendLine("            return reinterpret_cast<std::intptr_t>(chaos_delegate);");
            builder.AppendLine("        }");
        }

        builder.AppendLine("        default:");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_delegate_create_multicast_like(");
        builder.AppendLine("    std::intptr_t chaos_template_delegate_value,");
        builder.AppendLine("    const chaos_delegate_invocation_list& chaos_entries)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_entries.empty())");
        builder.AppendLine("    {");
        builder.AppendLine("        return static_cast<std::intptr_t>(0);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_entries.size() == 1)");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_entries.front();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto* chaos_template_delegate = chaos_require_delegate(chaos_template_delegate_value);");
        builder.AppendLine(
            "    const auto chaos_delegate_value = chaos_delegate_allocate_with_type_id(chaos_template_delegate->header.type_id);");
        builder.AppendLine("    auto* chaos_delegate = chaos_require_delegate(chaos_delegate_value);");
        builder.AppendLine("    auto* chaos_invocation_list = new chaos_delegate_invocation_list(chaos_entries);");
        builder.AppendLine("    chaos_delegate->chaos_delegate_target = static_cast<std::intptr_t>(0);");
        builder.AppendLine("    chaos_delegate->chaos_delegate_method_ptr = static_cast<std::intptr_t>(0);");
        builder.AppendLine(
            "    chaos_delegate->chaos_delegate_invocation_list = reinterpret_cast<std::intptr_t>(chaos_invocation_list);");
        builder.AppendLine(
            "    chaos_delegate->chaos_delegate_invocation_count = static_cast<std::intptr_t>(chaos_invocation_list->size());");
        builder.AppendLine("    return chaos_delegate_value;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_delegate_combine(std::intptr_t chaos_left_value, std::intptr_t chaos_right_value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_left_value == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_right_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_right_value == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_left_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    chaos_delegate_invocation_list chaos_entries{};");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_entries, chaos_left_value);");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_entries, chaos_right_value);");
        builder.AppendLine("    chaos_delegate_validate_entry_types(chaos_entries);");
        builder.AppendLine("    return chaos_delegate_create_multicast_like(chaos_left_value, chaos_entries);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_delegate_remove(std::intptr_t chaos_source_value, std::intptr_t chaos_value_to_remove)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_source_value == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return static_cast<std::intptr_t>(0);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_value_to_remove == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_source_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    chaos_delegate_invocation_list chaos_source_entries{};");
        builder.AppendLine("    chaos_delegate_invocation_list chaos_remove_entries{};");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_source_entries, chaos_source_value);");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_remove_entries, chaos_value_to_remove);");
        builder.AppendLine("    if (chaos_remove_entries.empty() || chaos_source_entries.size() < chaos_remove_entries.size())");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_source_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    for (std::intptr_t chaos_start = static_cast<std::intptr_t>(chaos_source_entries.size() - chaos_remove_entries.size());");
        builder.AppendLine("         chaos_start >= static_cast<std::intptr_t>(0);");
        builder.AppendLine("         --chaos_start)");
        builder.AppendLine("    {");
        builder.AppendLine("        bool chaos_matches = true;");
        builder.AppendLine("        for (std::size_t chaos_index = 0; chaos_index < chaos_remove_entries.size(); ++chaos_index)");
        builder.AppendLine("        {");
        builder.AppendLine(
            "            if (!chaos_delegate_single_entry_equals(chaos_source_entries[static_cast<std::size_t>(chaos_start) + chaos_index], chaos_remove_entries[chaos_index]))");
        builder.AppendLine("            {");
        builder.AppendLine("                chaos_matches = false;");
        builder.AppendLine("                break;");
        builder.AppendLine("            }");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        if (!chaos_matches)");
        builder.AppendLine("        {");
        builder.AppendLine("            continue;");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine(
            "        chaos_source_entries.erase(chaos_source_entries.begin() + static_cast<std::size_t>(chaos_start), chaos_source_entries.begin() + static_cast<std::size_t>(chaos_start) + chaos_remove_entries.size());");
        builder.AppendLine("        if (chaos_source_entries.empty())");
        builder.AppendLine("        {");
        builder.AppendLine("            return static_cast<std::intptr_t>(0);");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        if (chaos_source_entries.size() == 1)");
        builder.AppendLine("        {");
        builder.AppendLine("            return chaos_source_entries.front();");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        return chaos_delegate_create_multicast_like(chaos_source_value, chaos_source_entries);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return chaos_source_value;");
        builder.AppendLine("}");
        builder.AppendLine();
    }

    private IReadOnlyList<string> CollectReachableDelegateTypeSubjectIds(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var subjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (instruction.TargetReference is { Kind: AotCoreIrReferenceKind.Type } targetReference &&
                    IsDelegateTypeSubjectId(targetReference.SubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(targetReference.SubjectId);
                }

                if (!string.IsNullOrWhiteSpace(instruction.Callee) &&
                    instruction.Callee.Contains("::Invoke(", StringComparison.Ordinal))
                {
                    var declaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(instruction.Callee);
                    if (IsDelegateTypeSubjectId(declaringTypeSubjectId, _referenceTypeBaseSubjectIds))
                    {
                        subjectIds.Add(declaringTypeSubjectId);
                    }
                }

                if (!string.IsNullOrWhiteSpace(instruction.Callee) &&
                    TryReadSingleGenericTypeArgument(
                        instruction.Callee,
                        MarshalGetFunctionPointerForDelegateMethodPrefix,
                        out var marshalDelegateTypeSubjectId) &&
                    TryResolveReferenceTypeSubjectId(marshalDelegateTypeSubjectId, out marshalDelegateTypeSubjectId) &&
                    IsDelegateTypeSubjectId(marshalDelegateTypeSubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(marshalDelegateTypeSubjectId);
                }

                if (!string.IsNullOrWhiteSpace(instruction.Callee) &&
                    TryReadSingleGenericTypeArgument(
                        instruction.Callee,
                        MarshalGetDelegateForFunctionPointerMethodPrefix,
                        out marshalDelegateTypeSubjectId) &&
                    TryResolveReferenceTypeSubjectId(marshalDelegateTypeSubjectId, out marshalDelegateTypeSubjectId) &&
                    IsDelegateTypeSubjectId(marshalDelegateTypeSubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(marshalDelegateTypeSubjectId);
                }
            }
        }

        if (subjectIds.Count > 0)
        {
            subjectIds.Add(DelegateTypeSubjectId);
            subjectIds.Add(MulticastDelegateTypeSubjectId);
        }

        return subjectIds
            .OrderBy(subjectId => subjectId, StringComparer.Ordinal)
            .ToArray();
    }

    private static IReadOnlyDictionary<string, string> BuildClosureAssemblyPathByName(
        ManagedClosureManifestArtifact closureManifest)
    {
        var pathsByAssemblyName = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (var assemblyPath in EnumerateClosureAssemblyPaths(closureManifest))
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
            {
                continue;
            }

            var metadataReader = peReader.GetMetadataReader();
            var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
            pathsByAssemblyName[assemblyName] = assemblyPath;
        }

        return pathsByAssemblyName;
    }

    private static bool TryParseStaticFieldDataSize(
        string memberType,
        out int size)
    {
        const string marker = "__StaticArrayInitTypeSize=";
        size = 0;
        var markerIndex = memberType.IndexOf(marker, StringComparison.Ordinal);
        if (markerIndex < 0)
        {
            return false;
        }

        return int.TryParse(
            memberType[(markerIndex + marker.Length)..],
            NumberStyles.None,
            CultureInfo.InvariantCulture,
            out size) &&
               size >= 0;
    }

    private static bool TryLoadStaticFieldDataBytes(
        string assemblyPath,
        string fieldSubjectId,
        int size,
        out IReadOnlyList<byte> bytes)
    {
        bytes = Array.Empty<byte>();
        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        if (!peReader.HasMetadata)
        {
            return false;
        }

        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        if (!TryResolveFieldDefinitionHandle(
                metadataReader,
                assemblyName,
                fieldSubjectId,
                out var fieldHandle))
        {
            return false;
        }

        var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);
        var relativeVirtualAddress = fieldDefinition.GetRelativeVirtualAddress();
        if (relativeVirtualAddress <= 0)
        {
            return false;
        }

        var sectionData = peReader.GetSectionData(relativeVirtualAddress);
        if (sectionData.Length < size)
        {
            return false;
        }

        bytes = sectionData.GetReader(0, size).ReadBytes(size);
        return true;
    }

    private static bool TryResolveFieldDefinitionHandle(
        MetadataReader metadataReader,
        string assemblyName,
        string fieldSubjectId,
        out FieldDefinitionHandle fieldHandle)
    {
        fieldHandle = default;
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(fieldSubjectId);
        var fieldName = GetFieldName(fieldSubjectId);

        foreach (var candidateTypeHandle in metadataReader.TypeDefinitions)
        {
            if (!TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    assemblyName,
                    candidateTypeHandle,
                    out var typeIdentity) ||
                !string.Equals(typeIdentity.SubjectId, declaringTypeSubjectId, StringComparison.Ordinal))
            {
                continue;
            }

            var typeDefinition = metadataReader.GetTypeDefinition(candidateTypeHandle);
            foreach (var candidateFieldHandle in typeDefinition.GetFields())
            {
                var candidateFieldDefinition = metadataReader.GetFieldDefinition(candidateFieldHandle);
                if (string.Equals(
                        metadataReader.GetString(candidateFieldDefinition.Name),
                        fieldName,
                        StringComparison.Ordinal))
                {
                    fieldHandle = candidateFieldHandle;
                    return true;
                }
            }

            return false;
        }

        return false;
    }

    private static bool TryResolveTypeDefinitionHandleForReflectionMemberEntry(
        SupplementalMetadataTypeTemplateEntry typeEntry,
        IReadOnlyList<SupplementalMetadataTypeTemplateEntry> assemblyTypeEntries,
        out TypeDefinitionHandle typeDefinitionHandle)
    {
        if (TryCreateMetadataEntityHandle(typeEntry.MetadataToken, out var handle) &&
            handle.Kind == HandleKind.TypeDefinition)
        {
            typeDefinitionHandle = (TypeDefinitionHandle)handle;
            return true;
        }

        if (string.IsNullOrWhiteSpace(typeEntry.DefinitionSubjectId) ||
            string.Equals(typeEntry.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal))
        {
            typeDefinitionHandle = default;
            return false;
        }

        var definitionEntry = assemblyTypeEntries.FirstOrDefault(candidate =>
            string.Equals(candidate.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal));
        if (definitionEntry is null)
        {
            typeDefinitionHandle = default;
            return false;
        }

        if (TryCreateMetadataEntityHandle(definitionEntry.MetadataToken, out handle) &&
            handle.Kind == HandleKind.TypeDefinition)
        {
            typeDefinitionHandle = (TypeDefinitionHandle)handle;
            return true;
        }

        typeDefinitionHandle = default;
        return false;
    }

    private static bool TryResolveMethodDefinitionForReflectionMemberEntry(
        MetadataReader metadataReader,
        SupplementalMetadataMethodTemplateEntry methodEntry,
        IReadOnlyList<SupplementalMetadataTypeTemplateEntry> assemblyTypeEntries,
        out MethodDefinitionHandle methodDefinitionHandle)
    {
        if (TryCreateMetadataEntityHandle(methodEntry.MetadataToken, out var handle) &&
            handle.Kind == HandleKind.MethodDefinition)
        {
            methodDefinitionHandle = (MethodDefinitionHandle)handle;
            return true;
        }

        if (string.IsNullOrWhiteSpace(methodEntry.DefinitionSubjectId))
        {
            methodDefinitionHandle = default;
            return false;
        }

        var definitionDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(methodEntry.DefinitionSubjectId);
        var definitionTypeEntry = assemblyTypeEntries.FirstOrDefault(candidate =>
            string.Equals(candidate.SubjectId, definitionDeclaringTypeSubjectId, StringComparison.Ordinal));
        if (definitionTypeEntry is null ||
            !TryResolveTypeDefinitionHandleForReflectionMemberEntry(
                definitionTypeEntry,
                assemblyTypeEntries,
                out var typeDefinitionHandle))
        {
            methodDefinitionHandle = default;
            return false;
        }

        var metadataMethodName = GetMetadataMethodNameForReflectionMemberDefinition(methodEntry.DefinitionSubjectId);
        var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
        foreach (var candidateHandle in typeDefinition.GetMethods())
        {
            var candidateDefinition = metadataReader.GetMethodDefinition(candidateHandle);
            if (!string.Equals(
                    metadataReader.GetString(candidateDefinition.Name),
                    metadataMethodName,
                    StringComparison.Ordinal))
            {
                continue;
            }

            if (GetMethodParameterCount(metadataReader, candidateDefinition) != methodEntry.ParameterCount)
            {
                continue;
            }

            methodDefinitionHandle = candidateHandle;
            return true;
        }

        methodDefinitionHandle = default;
        return false;
    }

    private static bool TryCreateMetadataEntityHandle(int metadataToken, out EntityHandle handle)
    {
        if (metadataToken == 0)
        {
            handle = default;
            return false;
        }

        try
        {
            handle = MetadataTokens.EntityHandle(metadataToken);
            return !handle.IsNil;
        }
        catch (ArgumentException)
        {
            handle = default;
            return false;
        }
    }

    private static string GetMetadataMethodNameForReflectionMemberDefinition(string methodSubjectId)
    {
        var methodName = GetMethodName(methodSubjectId);
        var genericArgumentIndex = methodName.IndexOf('<');
        if (genericArgumentIndex >= 0)
        {
            methodName = methodName[..genericArgumentIndex];
        }

        return ManagedNaming.StripGenericArity(methodName);
    }

    private static int GetMethodParameterCount(MetadataReader metadataReader, MethodDefinition methodDefinition)
    {
        return methodDefinition.GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Count(parameter => parameter.SequenceNumber > 0);
    }

    private void CollectCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string currentAssemblyName,
        string targetSubjectId,
        CustomAttributeTargetKind targetKind,
        CustomAttributeHandleCollection attributeHandles,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        foreach (var attributeHandle in attributeHandles)
        {
            if (!TryGetAttributeTypeIdentity(metadataReader, currentAssemblyName, attributeHandle, out var attributeTypeIdentity))
            {
                continue;
            }

            var isExplicitQuery = queriedDisplayNames.Contains(attributeTypeIdentity.DisplayName);
            var isRequestedByIsDefined = memberInfoIsDefinedAttributeTypeSubjectIds.Contains(attributeTypeIdentity.SubjectId);
            if (!isExplicitQuery && !isRequestedByIsDefined)
            {
                continue;
            }

            if (isExplicitQuery)
            {
                RegisterCustomAttributeTypeSubjectId(
                    attributeTypeIdentity.DisplayName,
                    attributeTypeIdentity.SubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)targetKind}:{targetSubjectId}:{attributeTypeIdentity.SubjectId}";
            if (!materializationKeys.Add(key))
            {
                continue;
            }

            materializations.Add(CreateCustomAttributeMaterializationPlan(
                metadataReader,
                targetSubjectId,
                targetKind,
                attributeHandle,
                attributeTypeIdentity.SubjectId));
        }
    }

    private void CollectSyntheticMethodCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string targetSubjectId,
        MethodDefinition methodDefinition,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        if ((memberInfoIsDefinedAttributeTypeSubjectIds.Contains(DllImportAttributeTypeSubjectId) ||
             queriedDisplayNames.Contains(DllImportAttributeDisplayName)) &&
            methodDefinition.Attributes.HasFlag(MethodAttributes.PinvokeImpl))
        {
            if (queriedDisplayNames.Contains(DllImportAttributeDisplayName))
            {
                RegisterCustomAttributeTypeSubjectId(
                    DllImportAttributeDisplayName,
                    DllImportAttributeTypeSubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)CustomAttributeTargetKind.Method}:{targetSubjectId}:{DllImportAttributeTypeSubjectId}";
            if (materializationKeys.Add(key))
            {
                materializations.Add(CreateDllImportAttributeMaterializationPlan(metadataReader, targetSubjectId, methodDefinition));
            }
        }
    }

    private CustomAttributeMaterializationPlan CreateDllImportAttributeMaterializationPlan(
        MetadataReader metadataReader,
        string targetSubjectId,
        MethodDefinition methodDefinition)
    {
        var import = methodDefinition.GetImport();
        var moduleReference = metadataReader.GetModuleReference(import.Module);
        var moduleName = metadataReader.GetString(moduleReference.Name);
        var entryPointName = import.Name.IsNil
            ? metadataReader.GetString(methodDefinition.Name)
            : metadataReader.GetString(import.Name);
        var assignments = new List<CustomAttributeFieldAssignment>
        {
            new(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "Value"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, moduleName)),
            new(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "EntryPoint"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, entryPointName)),
        };

        if (import.Attributes.HasFlag(MethodImportAttributes.ExactSpelling))
        {
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "ExactSpelling"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, true)));
        }

        if (import.Attributes.HasFlag(MethodImportAttributes.SetLastError))
        {
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "SetLastError"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, true)));
        }

        return new CustomAttributeMaterializationPlan(
            CustomAttributeTargetKind.Method,
            targetSubjectId,
            DllImportAttributeTypeSubjectId,
            assignments);
    }

    private CustomAttributeMaterializationPlan CreateCustomAttributeMaterializationPlan(
        MetadataReader metadataReader,
        string targetSubjectId,
        CustomAttributeTargetKind targetKind,
        CustomAttributeHandle attributeHandle,
        string attributeTypeSubjectId)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        var decodedValue = attribute.DecodeValue(NativeAotCustomAttributeTypeProvider.Instance);
        var constructorParameterNames = GetAttributeConstructorParameterNames(metadataReader, attribute.Constructor);
        var assignments = new List<CustomAttributeFieldAssignment>();

        for (var index = 0; index < decodedValue.FixedArguments.Length; index++)
        {
            var memberName = ResolveFixedArgumentMemberName(
                attributeTypeSubjectId,
                constructorParameterNames,
                index,
                decodedValue.FixedArguments.Length);
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(attributeTypeSubjectId, memberName),
                CreateCustomAttributeLiteralValue(decodedValue.FixedArguments[index].Value)));
        }

        foreach (var namedArgument in decodedValue.NamedArguments)
        {
            if (string.IsNullOrWhiteSpace(namedArgument.Name))
            {
                throw new NotSupportedException(
                    $"native-aot custom-attribute materialization found an unnamed argument on '{attributeTypeSubjectId}'.");
            }

            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(attributeTypeSubjectId, namedArgument.Name),
                CreateCustomAttributeLiteralValue(namedArgument.Value)));
        }

        return new CustomAttributeMaterializationPlan(
            targetKind,
            targetSubjectId,
            attributeTypeSubjectId,
            assignments);
    }

    private static IReadOnlySet<string> CollectMemberInfoIsDefinedAttributeTypeSubjectIds(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var attributeTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            for (var index = 0; index < method.Instructions.Count; index++)
            {
                var instruction = method.Instructions[index];
                if (!string.Equals(instruction.Callee, MemberInfoIsDefinedMethodSubjectId, StringComparison.Ordinal))
                {
                    continue;
                }

                if (!TryResolveMemberInfoIsDefinedAttributeTypeSubjectId(method.Instructions, index, out var attributeTypeSubjectId) ||
                    string.IsNullOrWhiteSpace(attributeTypeSubjectId))
                {
                    throw new NotSupportedException(
                        $"native-aot custom-attribute IsDefined requires a direct typeof(T) attribute argument in '{method.SubjectId}' at IL offset {instruction.IlOffset}.");
                }

                attributeTypeSubjectIds.Add(attributeTypeSubjectId!);
            }
        }

        return attributeTypeSubjectIds;
    }

    private static bool TryResolveMemberInfoIsDefinedAttributeTypeSubjectId(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int callIndex,
        out string? attributeTypeSubjectId)
    {
        attributeTypeSubjectId = null;
        if (callIndex < 3 ||
            !string.Equals(instructions[callIndex - 1].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[callIndex - 2].Callee, GetTypeFromHandleMethodSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var loadTokenInstruction = instructions[callIndex - 3];
        if (!string.Equals(loadTokenInstruction.Op, "ldtoken", StringComparison.Ordinal))
        {
            return false;
        }

        if (loadTokenInstruction.TargetReference?.Kind == AotCoreIrReferenceKind.Type &&
            !string.IsNullOrWhiteSpace(loadTokenInstruction.TargetReference.SubjectId))
        {
            attributeTypeSubjectId = loadTokenInstruction.TargetReference.SubjectId;
            return true;
        }

        if (loadTokenInstruction.Operand is string directSubjectId &&
            !string.IsNullOrWhiteSpace(directSubjectId))
        {
            attributeTypeSubjectId = directSubjectId;
            return true;
        }

        if (loadTokenInstruction.Operand is JsonElement { ValueKind: JsonValueKind.String } element)
        {
            var jsonSubjectId = element.GetString();
            if (!string.IsNullOrWhiteSpace(jsonSubjectId))
            {
                attributeTypeSubjectId = jsonSubjectId;
                return true;
            }
        }

        return false;
    }

    private static IReadOnlyList<string> GetAttributeConstructorParameterNames(
        MetadataReader metadataReader,
        EntityHandle constructorHandle)
    {
        if (constructorHandle.Kind != HandleKind.MethodDefinition)
        {
            return [];
        }

        var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorHandle);
        return methodDefinition.GetParameters()
            .Select(handle => metadataReader.GetString(metadataReader.GetParameter(handle).Name))
            .Where(name => !string.IsNullOrWhiteSpace(name))
            .ToArray();
    }

    private static void RegisterCustomAttributeTypeSubjectId(
        string displayName,
        string subjectId,
        IDictionary<string, string> displayNameToSubjectId)
    {
        if (displayNameToSubjectId.TryGetValue(displayName, out var existingSubjectId) &&
            !string.Equals(existingSubjectId, subjectId, StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"native-aot custom-attribute lookup found ambiguous attribute type '{displayName}'.");
        }

        displayNameToSubjectId[displayName] = subjectId;
    }

    private string ResolveFixedArgumentMemberName(
        string attributeTypeSubjectId,
        IReadOnlyList<string> constructorParameterNames,
        int argumentIndex,
        int totalArgumentCount)
    {
        if (argumentIndex < constructorParameterNames.Count &&
            !string.IsNullOrWhiteSpace(constructorParameterNames[argumentIndex]))
        {
            var parameterName = constructorParameterNames[argumentIndex];
            return char.ToUpperInvariant(parameterName[0]) + parameterName[1..];
        }

        if (totalArgumentCount == 1)
        {
            return "Value";
        }

        throw new NotSupportedException(
            $"native-aot custom-attribute materialization could not bind fixed argument #{argumentIndex} for '{attributeTypeSubjectId}'.");
    }

    private string ResolveAttributeStorageField(
        string attributeTypeSubjectId,
        string memberName)
    {
        var getterSubjectId = ManagedNaming.CreateMethodSubjectId(attributeTypeSubjectId, $"get_{memberName}", []);
        if (_methodsBySubjectId.TryGetValue(getterSubjectId, out var getterMethod) &&
            TryGetAutoGetterStorageFieldSubjectId(getterMethod, out var fieldSubjectId) &&
            !string.IsNullOrWhiteSpace(fieldSubjectId))
        {
            return fieldSubjectId!;
        }

        return ManagedNaming.CreateFieldSubjectId(attributeTypeSubjectId, memberName);
    }

    private static bool TryGetAutoGetterStorageFieldSubjectId(
        AotCoreIrMethodArtifact method,
        out string? fieldSubjectId)
    {
        fieldSubjectId = null;
        if (method.IsStatic || method.ParameterCount != 0)
        {
            return false;
        }

        var fieldLoads = method.Instructions
            .Where(instruction =>
                string.Equals(instruction.Op, "ldfld", StringComparison.Ordinal) &&
                instruction.TargetReference?.Kind == AotCoreIrReferenceKind.Field &&
                !string.IsNullOrWhiteSpace(instruction.TargetReference.SubjectId))
            .Select(instruction => instruction.TargetReference!.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .ToArray();
        if (fieldLoads.Length != 1)
        {
            return false;
        }

        fieldSubjectId = fieldLoads[0];
        return true;
    }

    private static CustomAttributeLiteralValue CreateCustomAttributeLiteralValue(object? value)
    {
        return value switch
        {
            null => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Null, null),
            bool booleanValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, booleanValue),
            byte byteValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Byte, byteValue),
            sbyte byteValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int16, (short)byteValue),
            short shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int16, shortValue),
            int intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, intValue),
            long longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int64, longValue),
            ushort shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt16, shortValue),
            uint intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt32, intValue),
            ulong longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt64, longValue),
            char charValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt16, (ushort)charValue),
            string stringValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, stringValue),
            _ => throw new NotSupportedException(
                $"native-aot custom-attribute materialization does not support literal value '{value.GetType().FullName}'."),
        };
    }

    private static IEnumerable<string> EnumerateClosureAssemblyPaths(
        ManagedClosureManifestArtifact closureManifest)
    {
        yield return Path.GetFullPath(closureManifest.InputAssemblyPath);

        if (closureManifest.AdditionalAssemblyPaths is null)
        {
            yield break;
        }

        foreach (var assemblyPath in closureManifest.AdditionalAssemblyPaths)
        {
            yield return Path.GetFullPath(assemblyPath);
        }
    }

    private static bool TryParseCustomAttributeQueryCallee(
        string? callee,
        out string? attributeDisplayName)
    {
        const string prefix = "System.Private.CoreLib/System.Reflection.CustomAttributeExtensions::GetCustomAttribute<";
        const string suffix = ">(System.Reflection.MemberInfo)";
        attributeDisplayName = null;
        if (string.IsNullOrWhiteSpace(callee) ||
            !callee.StartsWith(prefix, StringComparison.Ordinal) ||
            !callee.EndsWith(suffix, StringComparison.Ordinal))
        {
            return false;
        }

        attributeDisplayName = callee[prefix.Length..^suffix.Length];
        return !string.IsNullOrWhiteSpace(attributeDisplayName);
    }

    private static bool TryParseAttributeGetterMethodSubjectId(
        string? subjectId,
        out string? attributeTypeSubjectId,
        out string? memberName)
    {
        attributeTypeSubjectId = null;
        memberName = null;
        if (string.IsNullOrWhiteSpace(subjectId) ||
            !subjectId.EndsWith("()", StringComparison.Ordinal))
        {
            return false;
        }

        var separatorIndex = subjectId.IndexOf("::get_", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            return false;
        }

        attributeTypeSubjectId = subjectId[..separatorIndex];
        memberName = subjectId.Substring(separatorIndex + "::get_".Length, subjectId.Length - separatorIndex - "::get_".Length - 2);
        return !string.IsNullOrWhiteSpace(attributeTypeSubjectId) &&
               !string.IsNullOrWhiteSpace(memberName);
    }

    private static bool TryGetAttributeTypeIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        CustomAttributeHandle attributeHandle,
        out MetadataTypeIdentity attributeTypeIdentity)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        return TryResolveTypeIdentity(metadataReader, currentAssemblyName, attribute.Constructor, out attributeTypeIdentity);
    }

    private static bool TryResolveTypeIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        EntityHandle constructorOrTypeHandle,
        out MetadataTypeIdentity typeIdentity)
    {
        switch (constructorOrTypeHandle.Kind)
        {
            case HandleKind.MethodDefinition:
                var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorOrTypeHandle);
                return TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    currentAssemblyName,
                    methodDefinition.GetDeclaringType(),
                    out typeIdentity);
            case HandleKind.MemberReference:
                var memberReference = metadataReader.GetMemberReference((MemberReferenceHandle)constructorOrTypeHandle);
                return TryResolveTypeIdentity(metadataReader, currentAssemblyName, memberReference.Parent, out typeIdentity);
            case HandleKind.TypeDefinition:
                return TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    currentAssemblyName,
                    (TypeDefinitionHandle)constructorOrTypeHandle,
                    out typeIdentity);
            case HandleKind.TypeReference:
                return TryResolveTypeReferenceIdentity(
                    metadataReader,
                    currentAssemblyName,
                    (TypeReferenceHandle)constructorOrTypeHandle,
                    out typeIdentity);
            default:
                typeIdentity = default;
                return false;
        }
    }

    private static bool TryResolveTypeDefinitionIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeDefinitionHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil &&
            TryResolveTypeDefinitionIdentity(metadataReader, currentAssemblyName, declaringTypeHandle, out var declaringTypeIdentity))
        {
            typeIdentity = declaringTypeIdentity with
            {
                TypeName = $"{declaringTypeIdentity.TypeName}+{typeName}",
            };
            return true;
        }

        typeIdentity = new MetadataTypeIdentity(currentAssemblyName, namespaceName, typeName);
        return true;
    }

    private static bool TryResolveTypeReferenceIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeReferenceHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        var typeReference = metadataReader.GetTypeReference(handle);
        var typeName = metadataReader.GetString(typeReference.Name);
        var namespaceName = metadataReader.GetString(typeReference.Namespace);
        switch (typeReference.ResolutionScope.Kind)
        {
            case HandleKind.AssemblyReference:
                var assemblyReference = metadataReader.GetAssemblyReference((AssemblyReferenceHandle)typeReference.ResolutionScope);
                typeIdentity = new MetadataTypeIdentity(
                    metadataReader.GetString(assemblyReference.Name),
                    namespaceName,
                    typeName);
                return true;

            case HandleKind.TypeReference:
                if (TryResolveTypeReferenceIdentity(
                        metadataReader,
                        currentAssemblyName,
                        (TypeReferenceHandle)typeReference.ResolutionScope,
                        out var declaringTypeIdentity))
                {
                    typeIdentity = declaringTypeIdentity with
                    {
                        TypeName = $"{declaringTypeIdentity.TypeName}+{typeName}",
                    };
                    return true;
                }

                break;

            case HandleKind.ModuleDefinition:
            case HandleKind.AssemblyDefinition:
                typeIdentity = new MetadataTypeIdentity(currentAssemblyName, namespaceName, typeName);
                return true;
        }

        typeIdentity = default;
        return false;
    }
}

internal sealed class NativeAotCustomAttributeTypeProvider : ICustomAttributeTypeProvider<string>
{
    public static readonly NativeAotCustomAttributeTypeProvider Instance = new();

    private NativeAotCustomAttributeTypeProvider()
    {
    }

    public string GetPrimitiveType(PrimitiveTypeCode typeCode)
    {
        return typeCode switch
        {
            PrimitiveTypeCode.Boolean => "bool",
            PrimitiveTypeCode.Byte => "byte",
            PrimitiveTypeCode.Char => "char",
            PrimitiveTypeCode.Double => "double",
            PrimitiveTypeCode.Int16 => "short",
            PrimitiveTypeCode.Int32 => "int",
            PrimitiveTypeCode.Int64 => "long",
            PrimitiveTypeCode.Object => "object",
            PrimitiveTypeCode.SByte => "sbyte",
            PrimitiveTypeCode.Single => "float",
            PrimitiveTypeCode.String => "string",
            PrimitiveTypeCode.UInt16 => "ushort",
            PrimitiveTypeCode.UInt32 => "uint",
            PrimitiveTypeCode.UInt64 => "ulong",
            _ => typeCode.ToString(),
        };
    }

    public string GetSystemType()
    {
        return "System.Type";
    }

    public string GetSZArrayType(string elementType)
    {
        return $"{elementType}[]";
    }

    public string GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
    {
        return NativeAotCustomAttributeTypeNameResolver.GetTypeName(reader, handle);
    }

    public string GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
    {
        return NativeAotCustomAttributeTypeNameResolver.GetTypeName(reader, handle);
    }

    public string GetTypeFromSerializedName(string name)
    {
        return name;
    }

    public PrimitiveTypeCode GetUnderlyingEnumType(string type)
    {
        return PrimitiveTypeCode.Int32;
    }

    public bool IsSystemType(string type)
    {
        return string.Equals(type, "System.Type", StringComparison.Ordinal);
    }
}

internal static class NativeAotCustomAttributeTypeNameResolver
{
    public static string GetTypeName(MetadataReader metadataReader, TypeDefinitionHandle handle)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil)
        {
            return $"{GetTypeName(metadataReader, declaringTypeHandle)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }

    public static string GetTypeName(MetadataReader metadataReader, TypeReferenceHandle handle)
    {
        var typeReference = metadataReader.GetTypeReference(handle);
        var typeName = metadataReader.GetString(typeReference.Name);
        var namespaceName = metadataReader.GetString(typeReference.Namespace);
        if (typeReference.ResolutionScope.Kind == HandleKind.TypeReference)
        {
            return $"{GetTypeName(metadataReader, (TypeReferenceHandle)typeReference.ResolutionScope)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }
}

public sealed record NativeAotTemplateModel
{
    public required IReadOnlyList<string> Includes { get; init; }

    public required string ObjectModelCode { get; init; }

    public required IReadOnlyList<string> MethodDeclarations { get; init; }

    public required IReadOnlyList<NativeAotMethodTemplateModel> Methods { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string EntrySymbol { get; init; }

    public required string EntryNativeSymbol { get; init; }

    public required string NativeEntryFunctionName { get; init; }

    public required string EntryBridgeArguments { get; init; }
}

public sealed record NativeAotMethodTemplateModel
{
    public required string SubjectId { get; init; }

    public required string MethodSource { get; init; }
}


