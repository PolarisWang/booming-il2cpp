// AUTO-GENERATED from RuntimeHelperShapeRegistry -- DO NOT EDIT
// This file is the single source of truth for runtime helper dispatch.
// All changes must originate in RuntimeHelperShapeRegistry.BuildDefault().

#ifndef CHAOS_IL2CPP_RUNTIME_HELPER_SHAPES_H_
#define CHAOS_IL2CPP_RUNTIME_HELPER_SHAPES_H_

#include <cstdint>
#include <cstddef>

namespace chaos { namespace il2cpp { namespace shape_dispatch {
#pragma pack(push, 1)

// ---- Shape ID enum (FNV-1a hashes of canonical shape keys) ----
enum ShapeId : uint32_t {
    SHAPE_INTERLOCKED_MEMORYBARRIER = 0xF8499018u,
    SHAPE_MONITOR_ENTER_SYSTEM_OBJECT_SYSTEM_BOOLEAN_ = 0xEB2C9BA3u,
    SHAPE_MONITOR_EXIT_SYSTEM_OBJECT = 0x04764296u,
    SHAPE_SYSTEM_ACTIVATOR_CREATEINSTANCE_SYSTEM_TYPE = 0x59AC309Eu,
    SHAPE_SYSTEM_ARGUMENTEXCEPTION_GET_PARAMNAME = 0x24D05674u,
    SHAPE_SYSTEM_ARGUMENTOUTOFRANGEEXCEPTION__CTOR_SYSTEM_STRING_SYSTEM_STRING = 0x74FA35FBu,
    SHAPE_SYSTEM_ARRAY_CLEAR_SYSTEM_ARRAY_SYSTEM_INT32_SYSTEM_INT32 = 0xF6F3337Bu,
    SHAPE_SYSTEM_ARRAY_GETLENGTH_SYSTEM_INT32 = 0x8F174D03u,
    SHAPE_SYSTEM_BUFFER_BYTELENGTH_SYSTEM_ARRAY = 0x28183643u,
    SHAPE_SYSTEM_DATETIME_GETHASHCODE = 0x50D45503u,
    SHAPE_SYSTEM_DATETIME_GET_UTCNOW = 0x1254B0E7u,
    SHAPE_SYSTEM_DECIMAL__CTOR_SYSTEM_INT32 = 0xE1CCAB35u,
    SHAPE_SYSTEM_DELEGATE_COMBINE_SYSTEM_DELEGATE_SYSTEM_DELEGATE = 0x85382755u,
    SHAPE_SYSTEM_DELEGATE_REMOVE_SYSTEM_DELEGATE_SYSTEM_DELEGATE = 0x36442080u,
    SHAPE_SYSTEM_DOUBLE_TOSTRING_SYSTEM_STRING = 0x62DBF072u,
    SHAPE_SYSTEM_ENVIRONMENT_GET_CURRENTMANAGEDTHREADID = 0xF4DDECAEu,
    SHAPE_SYSTEM_EXCEPTION_GETBASEEXCEPTION = 0xCC106A1Au,
    SHAPE_SYSTEM_EXCEPTION_GET_HRESULT = 0xA24695DCu,
    SHAPE_SYSTEM_EXCEPTION_GET_INNEREXCEPTION = 0xD7B5C090u,
    SHAPE_SYSTEM_EXCEPTION_GET_MESSAGE = 0xA28AE25Eu,
    SHAPE_SYSTEM_EXCEPTION__CTOR_SYSTEM_STRING = 0x1B319934u,
    SHAPE_SYSTEM_GC_COLLECT = 0xBC962311u,
    SHAPE_SYSTEM_GC_KEEPALIVE_SYSTEM_OBJECT = 0x4546B70Fu,
    SHAPE_SYSTEM_GC_WAITFORPENDINGFINALIZERS = 0x7E101281u,
    SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_COMPAREINFO = 0xF02CAA1Eu,
    SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_CURRENTCULTURE = 0x655FB238u,
    SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_DATETIMEFORMAT = 0x0C2387E3u,
    SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_DISPLAYNAME = 0xB32FEF48u,
    SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_INVARIANTCULTURE = 0xE46DFBDFu,
    SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_NAME = 0x4B216026u,
    SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_NUMBERFORMAT = 0x9809C133u,
    SHAPE_SYSTEM_GUID_NEWGUID = 0x421C9BADu,
    SHAPE_SYSTEM_INT32_TOSTRING = 0xE3BF8C09u,
    SHAPE_SYSTEM_INVALIDOPERATIONEXCEPTION__CTOR_SYSTEM_STRING = 0x637E36D4u,
    SHAPE_SYSTEM_MATH_SQRT_SYSTEM_DOUBLE = 0x2F5B83E9u,
    SHAPE_SYSTEM_OBJECT_EQUALS_SYSTEM_OBJECT = 0xDD4F3B4Du,
    SHAPE_SYSTEM_OBJECT_EQUALS_SYSTEM_OBJECT_SYSTEM_OBJECT = 0x5D1C5493u,
    SHAPE_SYSTEM_OBJECT_REFERENCEEQUALS_SYSTEM_OBJECT_SYSTEM_OBJECT = 0xFB5F39C6u,
    SHAPE_SYSTEM_OBJECT__CTOR = 0xB8F83EE0u,
    SHAPE_SYSTEM_RANDOM_NEXTBYTES_SYSTEM_BYTE__ = 0xE4447223u,
    SHAPE_SYSTEM_RANDOM_NEXTDOUBLE = 0xC8686216u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLYNAME_GET_NAME = 0xBDE1B28Du,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETCALLINGASSEMBLY = 0x9E40F454u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETENTRYASSEMBLY = 0xCFE61340u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETEXECUTINGASSEMBLY = 0x495BE6C8u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETNAME = 0xA8FFC0A5u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPES = 0x8B7AE599u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPE_SYSTEM_STRING = 0x150F01C4u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPE_SYSTEM_STRING_SYSTEM_BOOLEAN = 0x8246B3FBu,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GET_FULLNAME = 0x5F7B8813u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GET_IMAGERUNTIMEVERSION = 0xD114F318u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GET_LOCATION = 0xB02792F6u,
    SHAPE_SYSTEM_REFLECTION_CUSTOMATTRIBUTEEXTENSIONS_GETCUSTOMATTRIBUTE_SYSTEM_REFLECTION_ASSEMBLY_SYSTEM_TYPE = 0x61E9D75Du,
    SHAPE_SYSTEM_REFLECTION_CUSTOMATTRIBUTEEXTENSIONS_ISDEFINED_SYSTEM_REFLECTION_ASSEMBLY_SYSTEM_TYPE = 0xAD3A0BDDu,
    SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_DECLARINGTYPE = 0x8D177D62u,
    SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_METADATATOKEN = 0x9AD17BD1u,
    SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_NAME = 0x4B1B4F7Cu,
    SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_REFLECTEDTYPE = 0xFEC576E3u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GETPARAMETERS = 0x19603F22u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_CALLINGCONVENTION = 0x0B3E2AC2u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_ISPUBLIC = 0x20D0EA04u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_ISSTATIC = 0xBE770DD7u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_ISVIRTUAL = 0x2294E71Au,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_METHODHANDLE = 0xD04F4056u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_INVOKE_SYSTEM_OBJECT_SYSTEM_OBJECT__ = 0xB4A9F052u,
    SHAPE_SYSTEM_REFLECTION_METHODINFO_GETBASEDEFINITION = 0xB5E85B4Du,
    SHAPE_SYSTEM_REFLECTION_METHODINFO_MAKEGENERICMETHOD_SYSTEM_TYPE__ = 0xC85A8048u,
    SHAPE_SYSTEM_REFLECTION_MODULE_GETTYPES = 0x0E9B9AA9u,
    SHAPE_SYSTEM_REFLECTION_MODULE_GETTYPE_SYSTEM_STRING = 0x6CDFEF94u,
    SHAPE_SYSTEM_REFLECTION_MODULE_GET_ASSEMBLY = 0xC4BEC355u,
    SHAPE_SYSTEM_REFLECTION_MODULE_GET_FULLYQUALIFIEDNAME = 0xD4F1B40Cu,
    SHAPE_SYSTEM_REFLECTION_MODULE_GET_NAME = 0x3C729F8Eu,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GETREQUIREDCUSTOMMODIFIERS = 0xD061D279u,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_ATTRIBUTES = 0xD387C11Du,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_DEFAULTVALUE = 0xEEB6E626u,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_HASDEFAULTVALUE = 0x79CA1574u,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_NAME = 0x2C23EC77u,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_PARAMETERTYPE = 0x2EE62169u,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_POSITION = 0x06857AC7u,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_RAWDEFAULTVALUE = 0xEB4B55C6u,
    SHAPE_SYSTEM_RUNTIMEFIELDHANDLE_GETHASHCODE = 0x0A8071EEu,
    SHAPE_SYSTEM_RUNTIMEMETHODHANDLE_GETHASHCODE = 0x1932D0D9u,
    SHAPE_SYSTEM_RUNTIMETYPEHANDLE_GETHASHCODE = 0xA0D792DAu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDFORMATTED_SYSTEM_STRING = 0x73432179u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDLITERAL_SYSTEM_STRING = 0x7BDDA564u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_TOSTRINGANDCLEAR = 0x0D101A5Bu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER__CTOR_SYSTEM_INT32_SYSTEM_INT32 = 0x136B6B0Fu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_FORMATTABLESTRINGFACTORY_CREATE_SYSTEM_STRING_SYSTEM_OBJECT__ = 0xEFD1FAF0u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_EQUALS_SYSTEM_OBJECT_SYSTEM_OBJECT = 0x45247C70u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_GETHASHCODE_SYSTEM_OBJECT = 0x4CF4720Au,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_GETOBJECTVALUE_SYSTEM_OBJECT = 0x137ECDBDu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_INITIALIZEARRAY_SYSTEM_ARRAY_SYSTEM_RUNTIMEFIELDHANDLE = 0xC5DD5677u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEWRAPPEDEXCEPTION_GET_WRAPPEDEXCEPTION = 0x791C0CE9u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_GETAWAITER = 0xC76A5DEEu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GETRESULT = 0x03229253u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GET_ISCOMPLETED = 0x4F469568u,
    SHAPE_SYSTEM_SINGLE_TOSTRING_SYSTEM_STRING = 0x4B5B293Du,
    SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING = 0xF6869876u,
    SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING_SYSTEM_STRING = 0x08CB997Au,
    SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING_SYSTEM_STRING_SYSTEM_STRING = 0xEC14DB7Eu,
    SHAPE_SYSTEM_STRING_CONTAINS_SYSTEM_STRING = 0x3FAEA719u,
    SHAPE_SYSTEM_STRING_CONTAINS_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON = 0x4E63F846u,
    SHAPE_SYSTEM_STRING_GET_LENGTH = 0xA9CB34E1u,
    SHAPE_SYSTEM_STRING_JOIN_SYSTEM_STRING_SYSTEM_STRING__ = 0xFA63EBBAu,
    SHAPE_SYSTEM_STRING_OP_EQUALITY_SYSTEM_STRING_SYSTEM_STRING = 0x591FA874u,
    SHAPE_SYSTEM_STRING_STARTSWITH_SYSTEM_STRING = 0x830D4E01u,
    SHAPE_SYSTEM_STRING_STARTSWITH_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON = 0xCA6DE39Eu,
    SHAPE_SYSTEM_THREADING_TASKS_TASK_YIELD = 0x21E4D84Du,
    SHAPE_SYSTEM_THREADING_THREAD_GET_CURRENTTHREAD = 0x8CE16B91u,
    SHAPE_SYSTEM_THREADING_THREAD_JOIN = 0x411949DFu,
    SHAPE_SYSTEM_THREADING_THREAD_START = 0x4236E9C3u,
    SHAPE_SYSTEM_THREADING_THREAD__CTOR_SYSTEM_THREADING_THREADSTART = 0xCCAB9078u,
    SHAPE_SYSTEM_TIMESPAN_FROMMILLISECONDS_SYSTEM_DOUBLE = 0x41A528C8u,
    SHAPE_SYSTEM_TYPE_GETCONSTRUCTORS = 0x03E61EE4u,
    SHAPE_SYSTEM_TYPE_GETCONSTRUCTORS_SYSTEM_REFLECTION_BINDINGFLAGS = 0x731944A2u,
    SHAPE_SYSTEM_TYPE_GETFIELDS = 0xCC703D08u,
    SHAPE_SYSTEM_TYPE_GETFIELDS_SYSTEM_REFLECTION_BINDINGFLAGS = 0x531E00DEu,
    SHAPE_SYSTEM_TYPE_GETFIELD_SYSTEM_STRING = 0xCABE899Du,
    SHAPE_SYSTEM_TYPE_GETGENERICARGUMENTS = 0xCDF68F9Au,
    SHAPE_SYSTEM_TYPE_GETGENERICPARAMETERCONSTRAINTS = 0x712B182Du,
    SHAPE_SYSTEM_TYPE_GETGENERICTYPEDEFINITION = 0xE5F1AB95u,
    SHAPE_SYSTEM_TYPE_GETMETHODS = 0xD6912ACBu,
    SHAPE_SYSTEM_TYPE_GETMETHODS_SYSTEM_REFLECTION_BINDINGFLAGS = 0xE008A671u,
    SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING = 0x4867F576u,
    SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING_SYSTEM_REFLECTION_BINDINGFLAGS = 0x0D98C752u,
    SHAPE_SYSTEM_TYPE_GETTYPEFROMHANDLE_SYSTEM_RUNTIMETYPEHANDLE = 0xEBD6B558u,
    SHAPE_SYSTEM_TYPE_GETTYPE_SYSTEM_STRING = 0x2BB034B1u,
    SHAPE_SYSTEM_TYPE_GET_ASSEMBLY = 0x3634727Cu,
    SHAPE_SYSTEM_TYPE_GET_ASSEMBLYQUALIFIEDNAME = 0xE2E8E543u,
    SHAPE_SYSTEM_TYPE_GET_BASETYPE = 0xC987B069u,
    SHAPE_SYSTEM_TYPE_GET_CONTAINSGENERICPARAMETERS = 0xB50D7EA8u,
    SHAPE_SYSTEM_TYPE_GET_FULLNAME = 0xBAE900FAu,
    SHAPE_SYSTEM_TYPE_GET_GENERICPARAMETERPOSITION = 0x25003EBBu,
    SHAPE_SYSTEM_TYPE_GET_ISABSTRACT = 0xF5D3170Au,
    SHAPE_SYSTEM_TYPE_GET_ISARRAY = 0xA347E08Fu,
    SHAPE_SYSTEM_TYPE_GET_ISCONSTRUCTEDGENERICTYPE = 0x3D74EBA9u,
    SHAPE_SYSTEM_TYPE_GET_ISENUM = 0x05735259u,
    SHAPE_SYSTEM_TYPE_GET_ISGENERICTYPE = 0x47B2D76Bu,
    SHAPE_SYSTEM_TYPE_GET_ISGENERICTYPEDEFINITION = 0xF8DF832Eu,
    SHAPE_SYSTEM_TYPE_GET_ISINTERFACE = 0xE1052FD1u,
    SHAPE_SYSTEM_TYPE_GET_ISSEALED = 0x928D1304u,
    SHAPE_SYSTEM_TYPE_GET_ISVALUETYPE = 0x8F74F90Fu,
    SHAPE_SYSTEM_TYPE_GET_NAMESPACE = 0x19FCB789u,
    SHAPE_SYSTEM_TYPE_GET_TYPEHANDLE = 0xA5CEC5FAu,
    SHAPE_SYSTEM_TYPE_ISASSIGNABLEFROM_SYSTEM_TYPE = 0x07E008EDu,
    SHAPE_SYSTEM_TYPE_ISASSIGNABLETO_SYSTEM_TYPE = 0x37AAA11Au,
    SHAPE_SYSTEM_TYPE_ISINSTANCEOFTYPE_SYSTEM_OBJECT = 0x4479A68Du,
    SHAPE_SYSTEM_TYPE_ISSUBCLASSOF_SYSTEM_TYPE = 0x6E1ABD0Bu,
    SHAPE_SYSTEM_TYPE_MAKEGENERICTYPE_SYSTEM_TYPE__ = 0x326178A7u,
    SHAPE_VOLATILE_READ_SYSTEM_INT32_ = 0x779CC9A5u,

    SHAPE_COUNT = 150u,
};

// ---- Compile-time dispatch: NativeInt-returning shapes ----
template <ShapeId S, typename... Args>
CHAOS_IL2CPP_INTPTR DispatchNativeInt(Args... args) {
    else if constexpr (S == SHAPE_SYSTEM_ACTIVATOR_CREATEINSTANCE_SYSTEM_TYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionCreateInstance(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_ARGUMENTEXCEPTION_GET_PARAMNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_exception_param_name(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DATETIME_GET_UTCNOW) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosDatetimeGetUtcNow(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DELEGATE_COMBINE_SYSTEM_DELEGATE_SYSTEM_DELEGATE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_delegate_combine(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DELEGATE_REMOVE_SYSTEM_DELEGATE_SYSTEM_DELEGATE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_delegate_remove(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DOUBLE_TOSTRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_format_double_to_string(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_EXCEPTION_GETBASEEXCEPTION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosExceptionGetBaseException(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_EXCEPTION_GET_INNEREXCEPTION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosExceptionGetInnerException(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_EXCEPTION_GET_MESSAGE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetExceptionMessage(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_COMPAREINFO) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosCultureGetCompareInfo(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_CURRENTCULTURE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosCultureGetCurrent(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_DATETIMEFORMAT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosCultureGetDateTimeFormat(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_DISPLAYNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosCultureGetDisplayName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_INVARIANTCULTURE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosCultureGetInvariant(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosCultureGetName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GLOBALIZATION_CULTUREINFO_GET_NUMBERFORMAT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosCultureGetNumberFormat(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_GUID_NEWGUID) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosGuidNewGuid(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_INT32_TOSTRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_format_int32_to_string(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_MATH_SQRT_SYSTEM_DOUBLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosMathSqrt(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_OBJECT_EQUALS_SYSTEM_OBJECT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_object_equals(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_OBJECT_EQUALS_SYSTEM_OBJECT_SYSTEM_OBJECT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosObjectEqualsStatic(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_OBJECT_REFERENCEEQUALS_SYSTEM_OBJECT_SYSTEM_OBJECT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_object_reference_equals(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RANDOM_NEXTDOUBLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosRandomNextDouble(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLYNAME_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetAssemblyNameValue(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETCALLINGASSEMBLY) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetCallingAssembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETENTRYASSEMBLY) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetEntryAssembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETEXECUTINGASSEMBLY) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetExecutingAssembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetAssemblyName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPES) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionAssemblyGetTypes(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPE_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_type_from_assembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPE_SYSTEM_STRING_SYSTEM_BOOLEAN) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetTypeFromAssemblyBool(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GET_FULLNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetAssemblyFullName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GET_IMAGERUNTIMEVERSION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetImageRuntimeVersion(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GET_LOCATION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetAssemblyLocation(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_CUSTOMATTRIBUTEEXTENSIONS_GETCUSTOMATTRIBUTE_SYSTEM_REFLECTION_ASSEMBLY_SYSTEM_TYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetCustomAttribute(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_CUSTOMATTRIBUTEEXTENSIONS_ISDEFINED_SYSTEM_REFLECTION_ASSEMBLY_SYSTEM_TYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionIsDefined(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_DECLARINGTYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetDeclaringType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetMemberName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_REFLECTEDTYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetReflectedType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GETPARAMETERS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetParameters(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_ISPUBLIC) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsPublic(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_ISSTATIC) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_is_static(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_ISVIRTUAL) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsVirtual(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_METHODHANDLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetMethodHandle(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_INVOKE_SYSTEM_OBJECT_SYSTEM_OBJECT__) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionInvokeMethod(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODINFO_GETBASEDEFINITION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetBaseDefinition(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODINFO_MAKEGENERICMETHOD_SYSTEM_TYPE__) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionMakeGenericMethod(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MODULE_GETTYPES) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionModuleGetTypes(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MODULE_GETTYPE_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionModuleGetType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MODULE_GET_ASSEMBLY) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetModuleAssembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MODULE_GET_FULLYQUALIFIEDNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetModuleName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MODULE_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetModuleNameOnly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GETREQUIREDCUSTOMMODIFIERS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetRequiredCustomModifiers(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_DEFAULTVALUE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetDefaultValue(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_HASDEFAULTVALUE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionHasDefaultValue(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetParameterName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_PARAMETERTYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetParameterType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_RAWDEFAULTVALUE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetRawDefaultValue(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_TOSTRINGANDCLEAR) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_default_interpolated_string_handler_to_string_and_clear(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_FORMATTABLESTRINGFACTORY_CREATE_SYSTEM_STRING_SYSTEM_OBJECT__) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosFormattablestringFactoryCreate(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_EQUALS_SYSTEM_OBJECT_SYSTEM_OBJECT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosRuntimeHelpersEquals(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_GETOBJECTVALUE_SYSTEM_OBJECT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosRuntimeHelpersGetObjectValue(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEWRAPPEDEXCEPTION_GET_WRAPPEDEXCEPTION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosRuntimewrappedGetWrappedException(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_GETAWAITER) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_async_yield_get_awaiter(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GET_ISCOMPLETED) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_async_yield_get_is_completed(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_SINGLE_TOSTRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_format_single_to_string(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionConcatStringPairValues(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_CONTAINS_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosStringContains(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_CONTAINS_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosStringContains(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_JOIN_SYSTEM_STRING_SYSTEM_STRING__) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosStringJoinSs(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_OP_EQUALITY_SYSTEM_STRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_object_equals(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_STARTSWITH_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosStringStartsWith(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_STARTSWITH_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosStringStartsWith(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_TASKS_TASK_YIELD) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_async_yield_create(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD_GET_CURRENTTHREAD) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_thread_get_current(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TIMESPAN_FROMMILLISECONDS_SYSTEM_DOUBLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos::il2cpp::support::TimeSpanFromMilliseconds(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETCONSTRUCTORS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetConstructorsDefault(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETCONSTRUCTORS_SYSTEM_REFLECTION_BINDINGFLAGS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetConstructors(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETFIELDS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetFields(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETFIELDS_SYSTEM_REFLECTION_BINDINGFLAGS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetFieldsBindingflags(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETFIELD_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetField(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETGENERICARGUMENTS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetGenericArguments(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETGENERICPARAMETERCONSTRAINTS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetGenericParamConstraints(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETGENERICTYPEDEFINITION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetGenericTypeDefinition(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETMETHODS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetMethods(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETMETHODS_SYSTEM_REFLECTION_BINDINGFLAGS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetMethodsBindingflags(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetMethod(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING_SYSTEM_REFLECTION_BINDINGFLAGS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetMethod(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETTYPEFROMHANDLE_SYSTEM_RUNTIMETYPEHANDLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetTypeFromHandle(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETTYPE_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetTypeByName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ASSEMBLY) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetAssembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ASSEMBLYQUALIFIEDNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetAssemblyQualifiedName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_BASETYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetBaseType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_CONTAINSGENERICPARAMETERS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetContainsGenericParams(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_FULLNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetTypeFullName(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISABSTRACT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsAbstract(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISARRAY) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsArray(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISCONSTRUCTEDGENERICTYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsConstructedGeneric(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISENUM) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsEnum(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISGENERICTYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsGenericType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISGENERICTYPEDEFINITION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsGenericTypeDef(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISINTERFACE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsInterface(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISSEALED) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsSealed(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ISVALUETYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetIsValueType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_NAMESPACE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetNamespace(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_TYPEHANDLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionGetTypeHandle(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_ISASSIGNABLEFROM_SYSTEM_TYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionIsAssignableFrom(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_ISASSIGNABLETO_SYSTEM_TYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionIsAssignableTo(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_ISINSTANCEOFTYPE_SYSTEM_OBJECT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionIsInstanceOfType(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_ISSUBCLASSOF_SYSTEM_TYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionIsSubclassOf(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_MAKEGENERICTYPE_SYSTEM_TYPE__) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            ChaosReflectionMakeGenericType(args...));
    }
    else {
        static_assert(S != S, "Unhandled shape ID in DispatchNativeInt");
        return 0;
    }
}

// ---- Compile-time dispatch: Void-returning shapes ----
template <ShapeId S, typename... Args>
void DispatchVoid(Args... args) {
    if constexpr (S == SHAPE_INTERLOCKED_MEMORYBARRIER) {
        ChaosInterlockedMemoryBarrier(args...);
    }
    else if constexpr (S == SHAPE_MONITOR_ENTER_SYSTEM_OBJECT_SYSTEM_BOOLEAN_) {
        chaos_monitor_enter(args...);
    }
    else if constexpr (S == SHAPE_MONITOR_EXIT_SYSTEM_OBJECT) {
        chaos_monitor_exit(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_ARGUMENTOUTOFRANGEEXCEPTION__CTOR_SYSTEM_STRING_SYSTEM_STRING) {
        ChaosReflectionSetExceptionMetadata(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_ARRAY_CLEAR_SYSTEM_ARRAY_SYSTEM_INT32_SYSTEM_INT32) {
        ChaosArrayClear(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_DECIMAL__CTOR_SYSTEM_INT32) {
        chaos_decimal_ctor_int32(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_EXCEPTION__CTOR_SYSTEM_STRING) {
        ChaosReflectionSetExceptionMetadata(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_GC_KEEPALIVE_SYSTEM_OBJECT) {
        chaos_gc_keepalive(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_INVALIDOPERATIONEXCEPTION__CTOR_SYSTEM_STRING) {
        ChaosReflectionSetExceptionMetadata(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_OBJECT__CTOR) {
        ChaosObjectCtor(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RANDOM_NEXTBYTES_SYSTEM_BYTE__) {
        ChaosRandomNextBytes(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDFORMATTED_SYSTEM_STRING) {
        chaos_default_interpolated_string_handler_append_string(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDLITERAL_SYSTEM_STRING) {
        chaos_default_interpolated_string_handler_append_string(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER__CTOR_SYSTEM_INT32_SYSTEM_INT32) {
        chaos_default_interpolated_string_handler_reset(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_INITIALIZEARRAY_SYSTEM_ARRAY_SYSTEM_RUNTIMEFIELDHANDLE) {
        chaos_initialize_array_from_field_data_int32(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GETRESULT) {
        chaos_async_yield_get_result(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD_JOIN) {
        chaos_thread_join(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD_START) {
        chaos_thread_start(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD__CTOR_SYSTEM_THREADING_THREADSTART) {
        chaos_thread_ctor(args...);
    }
    else {
        static_assert(S != S, "Unhandled shape ID in DispatchVoid");
    }
}

// ---- Compile-time dispatch: Int32-returning shapes ----
template <ShapeId S, typename... Args>
CHAOS_IL2CPP_INT32 DispatchInt32(Args... args) {
    else if constexpr (S == SHAPE_SYSTEM_ARRAY_GETLENGTH_SYSTEM_INT32) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosArrayGetLength(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_BUFFER_BYTELENGTH_SYSTEM_ARRAY) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosBufferByteLength(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DATETIME_GETHASHCODE) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosDatetimeGetHashCode(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_ENVIRONMENT_GET_CURRENTMANAGEDTHREADID) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            chaos_current_managed_thread_id(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_EXCEPTION_GET_HRESULT) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosExceptionGetHresult(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_METADATATOKEN) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosReflectionGetMetadataToken(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_CALLINGCONVENTION) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosReflectionGetCallingConvention(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_ATTRIBUTES) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosReflectionGetParamAttributes(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_POSITION) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosReflectionGetParamPosition(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIMEFIELDHANDLE_GETHASHCODE) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosRuntimefieldhandleGetHashCode(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIMEMETHODHANDLE_GETHASHCODE) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosRuntimemethodhandleGetHashCode(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIMETYPEHANDLE_GETHASHCODE) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosRuntimetypehandleGetHashCode(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_GETHASHCODE_SYSTEM_OBJECT) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosRuntimeHelpersGetHashCode(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_GET_LENGTH) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            chaos_string_get_length(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_GENERICPARAMETERPOSITION) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosReflectionGetGenericParamPos(args...));
    }
    else if constexpr (S == SHAPE_VOLATILE_READ_SYSTEM_INT32_) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            ChaosVolatileRead(args...));
    }
    else {
        static_assert(S != S, "Unhandled shape ID in DispatchInt32");
        return 0;
    }
}

// ---- Runtime dispatch table (HotUpdate registration) ----
using ShapeRuntimeDispatchFn = void* (*)(void** args, uint32_t argc);

struct ShapeRuntimeEntry {
    ShapeId id;
    ShapeRuntimeDispatchFn fn;
};

constexpr uint32_t kMaxRuntimeShapeEntries = 32;
extern ShapeRuntimeEntry g_runtime_shape_entries[kMaxRuntimeShapeEntries];
extern uint32_t g_runtime_shape_count;

// ---- Compile-time completeness verification ----
static_assert(SHAPE_COUNT == 150u,
    "Number of registered shapes changed. Regenerate this header from RuntimeHelperShapeRegistry.");

#pragma pack(pop)
}}}  // namespace chaos::il2cpp::shape_dispatch

#endif // CHAOS_IL2CPP_RUNTIME_HELPER_SHAPES_H_