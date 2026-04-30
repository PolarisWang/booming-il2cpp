using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private const string ObjectCtorMethodSubjectId = "System.Private.CoreLib/System.Object::.ctor:System.Void()";
    private const string DelegateTypeSubjectId = "System.Private.CoreLib/System.Delegate";
    private const string MulticastDelegateTypeSubjectId = "System.Private.CoreLib/System.MulticastDelegate";
    private const string StringTypeSubjectId = "System.Private.CoreLib/System.String";
    private const string StringJoinGenericEnumerableMethodPrefix =
        "System.Private.CoreLib/System.String::Join<";
    private const string GetTypeHandleMethodSubjectId = "System.Private.CoreLib/System.Type::get_TypeHandle()";
    private const string GetFieldMethodSubjectId = "System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)";
    private const string GetPropertyMethodSubjectId = "System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)";
    private const string GetPropertiesMethodSubjectId = "System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()";
    private const string GetMethodByNameMethodSubjectId = "System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)";
    private const string GetInterfaceMethodSubjectId = "System.Private.CoreLib/System.Type::GetInterface:System.Type(System.String,System.Boolean)";
    private const string GetInterfaceMapMethodSubjectId = "System.Private.CoreLib/System.Type::GetInterfaceMap:System.Reflection.InterfaceMapping(System.Type)";
    private const string GetAssemblyMethodSubjectId = "System.Private.CoreLib/System.Type::get_Assembly()";
    private const string GetGenericArgumentsMethodSubjectId = "System.Private.CoreLib/System.Type::GetGenericArguments()";
    private const string GetGenericTypeDefinitionMethodSubjectId = "System.Private.CoreLib/System.Type::GetGenericTypeDefinition()";
    private const string GetConstructorsMethodSubjectId = "System.Private.CoreLib/System.Type::GetConstructors(System.Reflection.BindingFlags)";
    private const string GetMethodMethodSubjectId = "System.Private.CoreLib/System.Type::GetMethod(System.String,System.Reflection.BindingFlags)";
    private const string GetMethodHandleMethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle()";
    private const string GetParametersMethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()";
    private const string ParameterInfoGetNameMethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name()";
    private const string MemberInfoGetNameMethodSubjectId =
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_Name()";
    private const string MemberInfoGetDeclaringTypeMethodSubjectId =
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType()";
    private const string MemberInfoGetMetadataTokenMethodSubjectId =
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken()";
    private const string ActivatorCreateInstanceTypeMethodSubjectId =
        "System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)";
    private const string ObjectGetTypeMethodSubjectId = "System.Private.CoreLib/System.Object::GetType:System.Type()";
    private const string AssemblyGetTypeMethodSubjectId = "System.Private.CoreLib/System.Reflection.Assembly::GetType(System.String)";
    private const string TypeGetTypeByNameMethodSubjectId = "System.Private.CoreLib/System.Type::GetType(System.String)";
    private const string AssemblyGetNameMethodSubjectId = "System.Private.CoreLib/System.Reflection.Assembly::GetName()";
    private const string AssemblyNameGetNameMethodSubjectId = "System.Private.CoreLib/System.Reflection.AssemblyName::get_Name()";
    private const string DelegateCombineMethodSubjectId = "System.Private.CoreLib/System.Delegate::Combine(System.Delegate,System.Delegate)";
    private const string DelegateRemoveMethodSubjectId = "System.Private.CoreLib/System.Delegate::Remove(System.Delegate,System.Delegate)";
    private const string DefaultInterpolatedStringHandlerAppendFormattedMethodSubjectPrefix =
        "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<";
    private const string MarshalGetFunctionPointerForDelegateMethodPrefix =
        "System.Runtime.InteropServices/Marshal::GetFunctionPointerForDelegate<";
    private const string MarshalGetDelegateForFunctionPointerMethodPrefix =
        "System.Runtime.InteropServices/Marshal::GetDelegateForFunctionPointer<";
    private const string MarshalSizeOfMethodPrefix =
        "System.Runtime.InteropServices/Marshal::SizeOf<";
    private const string MarshalStructureToPtrMethodPrefix =
        "System.Runtime.InteropServices/Marshal::StructureToPtr<";
    private const string MarshalPtrToStructureMethodPrefix =
        "System.Runtime.InteropServices/Marshal::PtrToStructure<";
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
