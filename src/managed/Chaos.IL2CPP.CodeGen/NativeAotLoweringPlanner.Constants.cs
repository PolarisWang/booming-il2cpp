using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private const string OverflowExceptionTypeSubjectId = "System.Private.CoreLib/System.OverflowException";
    private const string ObjectTypeSubjectId = "System.Private.CoreLib/System.Object";
    private const string ObjectCtorMethodSubjectId = "System.Private.CoreLib/System.Object::.ctor:System.Void()";
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
        "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)";
    private const string RuntimeHelpersCreateSpanMethodPrefix =
        "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan<";
    private const string MemoryExtensionsAsSpanMethodPrefix =
        "System.Memory/System.MemoryExtensions::AsSpan<";
    private const string MemoryExtensionsAsMemoryMethodPrefix =
        "System.Memory/System.MemoryExtensions::AsMemory<";
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
    private const string DefaultInterpolatedStringHandlerAppendFormattedMethodSubjectPrefix =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<";
    private const string DefaultInterpolatedStringHandlerToStringAndClearMethodSubjectId =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()";
    private const string MarshalGetFunctionPointerForDelegateMethodPrefix =
        "System.Runtime.InteropServices/Marshal::GetFunctionPointerForDelegate<";
    private const string MarshalGetDelegateForFunctionPointerMethodPrefix =
        "System.Runtime.InteropServices/Marshal::GetDelegateForFunctionPointer<";
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
    private const string UnmanagedCallersOnlyAttributeDisplayName = "UnmanagedCallersOnlyAttribute";
    private const string UnmanagedCallersOnlyAttributeTypeSubjectId = "System.Runtime.InteropServices/UnmanagedCallersOnlyAttribute";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    private static readonly IReadOnlySet<int> EmptyRawArgumentIndices = new HashSet<int>();
}
